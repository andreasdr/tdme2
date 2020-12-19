#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <ext/vulkan/spirv/GlslangToSpv.h>
#include <ext/vulkan/svs/thsvs_simpler_vulkan_synchronization.h>
#include <ext/vulkan/vma/src/VmaUsage.h>

#include <array>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/os/threading/ReadWriteLock.h>
#include <tdme/os/threading/SpinLock.h>
#include <tdme/utilities/fwd-tdme.h>

using std::array;
using std::list;
using std::map;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::engine::FrameBuffer;
using tdme::math::Matrix2D3x3;
using tdme::math::Matrix4x4;
using tdme::os::threading::Mutex;
using tdme::os::threading::ReadWriteLock;
using tdme::os::threading::SpinLock;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::IntBuffer;
using tdme::utilities::ShortBuffer;

/**
 * Vulkan renderer
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::renderer::VKRenderer
	: public Renderer
{
private:
	static constexpr bool VERBOSE { false };
	static constexpr int DRAW_COMMANDBUFFER_MAX { 3 };
	static constexpr int COMMANDS_MAX_GRAPHICS { 16 }; // TODO: make this variable
	static constexpr int COMMANDS_MAX_COMPUTE { 5 }; // TODO: make this variable
	static constexpr int DESC_MAX { 4096 };

	struct delete_buffer_type {
		VkBuffer buffer;
		VmaAllocation allocation;
	};

	struct delete_image_type {
		VkImage image;
		VmaAllocation allocation;
		VkImageView image_view;
		VkSampler sampler;
	};

	struct buffer_object_type {
		struct reusable_buffer {
			bool memoryMappable { false };
			int64_t frame_used_last { -1 };
			VkBuffer buf { VK_NULL_HANDLE };
			VmaAllocation allocation { VK_NULL_HANDLE };
			uint32_t size { 0 };
			void* data { nullptr };
		};
		int32_t id { 0 };
		bool useGPUMemory { false };
		bool shared { false };
		list<reusable_buffer> buffers;
		uint32_t buffer_count { 0 };
		int64_t frame_cleaned_last { 0 };
		reusable_buffer* current_buffer { nullptr };
		volatile bool uploading { false };
	};

	struct shader_type {
		struct uniform_type {
			enum uniform_type_enum { TYPE_NONE, TYPE_UNIFORM, TYPE_SAMPLER2D };
			string name;
			string newName;
			uniform_type_enum type;
			int32_t position;
			uint32_t size;
			int32_t texture_unit;
		};
		unordered_map<string, uniform_type*> uniforms;
		vector<uniform_type*> uniformList;
		vector<uniform_type*> sampler2DUniformList;
		uint32_t ubo_size { 0 };
		uint32_t samplers { 0 };
		int32_t binding_max { -1 };
		vector<int32_t> ubo_ids;
		vector<buffer_object_type*> ubo;
		int32_t ubo_binding_idx { -1 };
		string definitions;
 		string source;
 		string file;
		vector<unsigned int> spirv;
		int32_t id { 0 };
		VkShaderStageFlagBits type;
		VkShaderModule module { VK_NULL_HANDLE };
	};

	struct pipeline_type {
		string id;
		VkPipelineCache pipelineCache { VK_NULL_HANDLE };
		VkPipeline pipeline { VK_NULL_HANDLE };
	};

	struct program_type {
		int type { 0 };
		unordered_map<string, pipeline_type*> pipelines;
		vector<int32_t> shader_ids;
		vector<shader_type*> shaders;
		unordered_map<int32_t, string> uniforms;
		vector<int32_t> uniform_buffers;
		vector<bool> uniform_buffers_stored;
		vector<array<vector<uint8_t>, 4>> uniform_buffers_last;
		vector<array<bool, 4>> uniform_buffers_changed_last;
		uint32_t layout_bindings { 0 };
		VkPipelineLayout pipeline_layout { VK_NULL_HANDLE };
		vector<array<VkDescriptorSet, DESC_MAX>> desc_sets;
		VkDescriptorSetLayout desc_layout { VK_NULL_HANDLE };
		vector<uint32_t> desc_idxs;
		int32_t id { 0 };
	};

	struct texture_type {
		enum type { TYPE_NONE, TYPE_TEXTURE, TYPE_FRAMEBUFFER_COLORBUFFER, TYPE_FRAMEBUFFER_DEPTHBUFFER };
		volatile bool uploaded { false };
		type type { TYPE_NONE };
		int32_t id { 0 };
		int32_t frame_buffer_object_id { 0 };
		uint32_t width { 0 };
		uint32_t height { 0 };
		VkFormat format { VK_FORMAT_UNDEFINED };
		VkSampler sampler { VK_NULL_HANDLE };
		VkImage image { VK_NULL_HANDLE };
		VkImageAspectFlags aspect_mask { 0 };
		array<ThsvsAccessType, 2> access_types { THSVS_ACCESS_NONE, THSVS_ACCESS_NONE };
		ThsvsImageLayout svsLayout { THSVS_IMAGE_LAYOUT_OPTIMAL };
		VkImageLayout vkLayout { VK_IMAGE_LAYOUT_UNDEFINED };
		VmaAllocation allocation { VK_NULL_HANDLE };
		VkImageView view { VK_NULL_HANDLE };
	};

	struct framebuffer_object_type {
		int32_t id { 0 };
		int32_t depth_texture_id { 0 };
		int32_t color_texture_id { 0 };
		VkFramebuffer frame_buffer { VK_NULL_HANDLE };
		VkRenderPass render_pass { VK_NULL_HANDLE };
	};

	struct swapchain_buffer_type {
		array<ThsvsAccessType, 2> access_types { THSVS_ACCESS_NONE, THSVS_ACCESS_NONE };
		ThsvsImageLayout svsLayout { THSVS_IMAGE_LAYOUT_OPTIMAL };
		VkImage image { VK_NULL_HANDLE };
		VkImageView view { VK_NULL_HANDLE };
	};

	struct context_type {
		int32_t idx { 0 };

		vector<pipeline_type*> pipelineVector;
		vector<buffer_object_type*> bufferVector;
		vector<texture_type*> textureVector;

		VkCommandPool cmd_setup_pool;
		VkCommandBuffer setup_cmd_inuse;
		VkCommandBuffer setup_cmd;
		VkFence setup_fence;

		array<bool, 3> render_pass_started;

		VkCommandPool cmd_draw_pool;
		uint32_t draw_cmd_current;
		array<array<VkCommandBuffer, 3>, DRAW_COMMANDBUFFER_MAX> draw_cmds;
		array<array<VkFence, 3>, DRAW_COMMANDBUFFER_MAX> draw_fences;
		VkFence draw_fence { VK_NULL_HANDLE };
		array<array<bool, 3>, DRAW_COMMANDBUFFER_MAX> draw_cmd_started;

		program_type* program { nullptr };

		array<string, 3> pipeline_id;
		array<VkPipeline, 3> pipeline;

		int32_t bound_indices_buffer { 0 };
		array<int32_t, 10> bound_buffers { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		array<vector<uint8_t>, 4> uniform_buffers;
		array<bool, 4> uniform_buffers_changed;
		int32_t texture_unit_active { 0 };
		array<int32_t, 16> bound_textures { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		struct objects_render_command {
			struct texture {
				VkSampler sampler { VK_NULL_HANDLE };
				VkImageView view { VK_NULL_HANDLE };
				VkImageLayout layout { VK_IMAGE_LAYOUT_UNDEFINED };
			};
			VkBuffer indices_buffer { VK_NULL_HANDLE };
			array<VkBuffer, 10> vertex_buffers = {
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE,
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE,
				VK_NULL_HANDLE, VK_NULL_HANDLE
			};
			array<VkBuffer, 4> ubo_buffers = {
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE
			};

			array<texture, 16> textures;
			int32_t count { 0 };
			int32_t offset { 0 };
			int32_t instances { 0 };
		};

		struct points_render_command {
			struct texture {
				VkSampler sampler { VK_NULL_HANDLE };
				VkImageView view { VK_NULL_HANDLE };
				VkImageLayout layout { VK_IMAGE_LAYOUT_UNDEFINED };
			};
			array<VkBuffer, 10> vertex_buffers = {
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE,
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE,
				VK_NULL_HANDLE, VK_NULL_HANDLE
			};
			array<VkBuffer, 4> ubo_buffers = {
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE
			};

			array<texture, 16> textures;
			int32_t count { 0 };
			int32_t offset { 0 };
		};

		struct lines_render_command {
			struct texture {
				VkSampler sampler { VK_NULL_HANDLE };
				VkImageView view { VK_NULL_HANDLE };
				VkImageLayout layout { VK_IMAGE_LAYOUT_UNDEFINED };
			};
			array<VkBuffer, 4> vertex_buffers = {
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE
			};
			array<VkBuffer, 4> ubo_buffers = {
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE
			};

			array<texture, 16> textures;
			int32_t count { 0 };
			int32_t offset { 0 };
		};

		struct compute_command {
			array<VkBuffer, 8> storage_buffers = {
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE,
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE,
			};
			array<uint32_t, 8> storage_buffer_sizes = {
				0, 0, 0, 0,
				0, 0, 0, 0,
			};
			array<VkBuffer, 4> ubo_buffers = {
				VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE
			};

			int32_t num_groups_x { 0 };
			int32_t num_groups_y { 0 };
			int32_t num_groups_z { 0 };
		};

		enum command_type { COMMAND_NONE, COMMAND_OBJECTS, COMMAND_POINTS, COMMAND_LINES, COMMAND_COMPUTE };
		command_type command_type { COMMAND_NONE };
		objects_render_command objects_render_command;
		points_render_command points_render_command;
		lines_render_command lines_render_command;
		compute_command compute_command;
		array<uint32_t, 3> command_count { 0 };

		string shader;
		string shaderParametersHash;
		map<string, string> shaderParameters;
		array<float, 4> effect_color_mul {{ 1.0f, 1.0f, 1.0f, 1.0f }};
		array<float, 4> effect_color_add {{ 0.0f, 0.0f, 0.0f, 0.0f }};
		Renderer_PBRMaterial pbrMaterial;
		Renderer_SpecularMaterial specularMaterial;
		array<Renderer_Light, 8> lights;
		Matrix2D3x3 texture_matrix;

		bool culling_enabled { true };
		int front_face { VK_FRONT_FACE_COUNTER_CLOCKWISE + 1 };
		int front_face_index { VK_FRONT_FACE_COUNTER_CLOCKWISE + 1 };

		int32_t program_id { 0 };

		float maskMaxValue { 1.0f };

		int32_t lighting { 0 };
	};

	VkSurfaceKHR surface { VK_NULL_HANDLE };

	VkInstance inst { VK_NULL_HANDLE };
	VkPhysicalDevice gpu { VK_NULL_HANDLE };
	VkDevice device { VK_NULL_HANDLE };
	SpinLock queue_spinlock;
	VkQueue queue { VK_NULL_HANDLE };
	VkPhysicalDeviceProperties gpu_props;
	VkPhysicalDeviceFeatures gpu_features;
	VkQueueFamilyProperties *queue_props { nullptr };
	VkPhysicalDeviceMemoryProperties memory_properties;
	uint32_t graphics_queue_node_index { 0 };

	PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR { nullptr };
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR { nullptr };
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR { nullptr };
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR { nullptr };
	PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR { nullptr };
	PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR { nullptr };
	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR { nullptr };
	PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR { nullptr };
	PFN_vkQueuePresentKHR fpQueuePresentKHR { nullptr };

	uint32_t swapchain_image_count { 0 };
	VkSwapchainKHR swapchain { VK_NULL_HANDLE };
	swapchain_buffer_type* swapchain_buffers { nullptr };
	VkFramebuffer* window_framebuffers { nullptr };

	VkFence memorybarrier_fence { VK_NULL_HANDLE };

	ReadWriteLock pipeline_rwlock;

	VkRenderPass render_pass { VK_NULL_HANDLE };

	int32_t shader_idx { 1 };
	int32_t buffer_idx { 1 };
	int32_t texture_idx { 1 };
	vector<program_type*> programList { nullptr };
	unordered_map<int32_t, shader_type*> shaders;
	unordered_map<int32_t, buffer_object_type*> buffers;
	unordered_map<int32_t, texture_type*> textures;
	vector<int32_t> free_texture_ids;
	vector<int32_t> free_buffer_ids;
	vector<framebuffer_object_type*> framebuffers { nullptr };

	ReadWriteLock buffers_rwlock;
	ReadWriteLock textures_rwlock;

	uint32_t width { 0 };
	uint32_t height { 0 };
	VkFormat format { VK_FORMAT_UNDEFINED };
	VkColorSpaceKHR color_space { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	buffer_object_type* empty_vertex_buffer { nullptr };
	int empty_vertex_buffer_id { 0 };
	int depth_buffer_default { 0 };
	int white_texture_default_id { 0 };
	texture_type* white_texture_default { nullptr };

	VkDescriptorPool desc_pool { VK_NULL_HANDLE };

	// enable validation layers
	bool validate { false };

	uint32_t current_buffer { 0 };
	uint32_t queue_count { 0 };

	VkSemaphore image_acquired_semaphore { VK_NULL_HANDLE };
	VkSemaphore draw_complete_semaphore { VK_NULL_HANDLE };

	float clear_red { 0.0f };
	float clear_green { 0.0f };
	float clear_blue { 0.0f };
	float clear_alpha { 1.0f };

	VkViewport viewport;
	VkRect2D scissor;

	int32_t bound_frame_buffer { 0 };

	bool blending_enabled { true };
	VkCullModeFlagBits cull_mode { VK_CULL_MODE_FRONT_BIT};
	bool depth_buffer_writing { true };
	bool depth_buffer_testing { true };
	int depth_function { VK_COMPARE_OP_LESS_OR_EQUAL };
	float line_width { 1.0f };
	int64_t frame { 0 };

	Mutex delete_mutex;
	vector<delete_buffer_type> delete_buffers;
	vector<delete_image_type> delete_images;

	Mutex dispose_mutex;
	vector<int32_t> dispose_textures;
	vector<int32_t> dispose_buffers;

	vector<context_type> contexts;
	VmaAllocator allocator { VK_NULL_HANDLE };

	VkPresentModeKHR swapchainPresentMode = { VK_PRESENT_MODE_IMMEDIATE_KHR };

	//
	VkBool32 checkLayers(uint32_t check_count, const char **check_names, uint32_t layer_count, VkLayerProperties *layers);
	void setImageLayout(int contextIdx, texture_type* textureObject, const array<ThsvsAccessType,2>& nextAccessTypes, ThsvsImageLayout nextLayout, bool discardContent, uint32_t baseLevel = 0, uint32_t levelCount = 1);
	uint32_t getMipLevels(Texture* texture);
	void prepareTextureImage(int contextIdx, struct texture_type* textureObject, VkImageTiling tiling, VkImageUsageFlags usage, VkFlags requiredFlags, Texture* texture, const array<ThsvsAccessType,2>& nextAccesses, ThsvsImageLayout imageLayout, bool disableMipMaps = true, uint32_t baseLevel = 0, uint32_t levelCount = 1);
	VkBuffer getBufferObjectInternal(int contextIdx,  int32_t bufferObjectId, uint32_t& size);
	VkBuffer getBufferObjectInternal(buffer_object_type* bufferObject, uint32_t& size);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VmaAllocation& allocation, VmaAllocationInfo& allocationInfo);
	buffer_object_type* getBufferObjectInternal(int contextIdx,  int32_t bufferObjectId);
	void uploadBufferObjectInternal(int contextIdx,  buffer_object_type* buffer, int32_t size, const uint8_t* data, VkBufferUsageFlagBits usage);
	void uploadBufferObjectInternal(int contextIdx, int32_t bufferObjectId, int32_t size, const uint8_t* data, VkBufferUsageFlagBits usage);
	texture_type* getTextureInternal(int contextIdx, int32_t textureId);
	pipeline_type* getPipelineInternal(int contextIdx, program_type* program, const string& pipelineId);
	void setProgramUniformInternal(void* context, int32_t uniformId, uint8_t* data, int32_t size);
	void shaderInitResources(TBuiltInResource &resources);
	EShLanguage shaderFindLanguage(const VkShaderStageFlagBits shaderType);
	void initializeSwapChain();
	void initializeFrameBuffers();
	void endDrawCommands(int contextIdx);
	void endDrawCommandsAllContexts();
	void executeCommand(int contextIdx);
	void initializeRenderPass();
	void startRenderPass(int contextIdx);
	void endRenderPass(int contextIdx);
	void preparePipeline(int contextIdx, program_type* program);
	void createObjectsRenderingProgram(program_type* program);
	pipeline_type* createObjectsRenderingPipeline(int contextIdx, program_type* program);
	void setupObjectsRenderingPipeline(int contextIdx, program_type* program);
	void createPointsRenderingProgram(program_type* program);
	pipeline_type* createPointsRenderingPipeline(int contextIdx, program_type* program);
	void setupPointsRenderingPipeline(int contextIdx, program_type* program);
	void createLinesRenderingProgram(program_type* program);
	pipeline_type* createLinesRenderingPipeline(int contextIdx, program_type* program);
	void setupLinesRenderingPipeline(int contextIdx, program_type* program);
	void createSkinningComputingProgram(program_type* program);
	pipeline_type* createSkinningComputingPipeline(int contextIdx, program_type* program);
	void setupSkinningComputingPipeline(int contextIdx, program_type* program);
	void finishPipeline(int contextIdx);
	void prepareSetupCommandBuffer(int contextIdx);
	void finishSetupCommandBuffer(int contextIdx);
	void finishSetupCommandBuffers();
	void reshape();
	int determineAlignment(const unordered_map<string, vector<string>>& structs, const vector<string>& uniforms);
	int align(int alignment, int offset);
	bool addToShaderUniformBufferObject(
		shader_type& shader,
		const unordered_map<string, string>& definitionValues,
		const unordered_map<string, vector<string>>& structs,
		const vector<string>& uniforms,
		const string& prefix,
		unordered_set<string>& uniformArrays,
		string& uniformsBlock
	);
	void createRasterizationStateCreateInfo(int contextIdx, VkPipelineRasterizationStateCreateInfo& rs);
	void createColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& att_state);
	void createDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo& ds);
	const string createPipelineId(program_type* program, int contextIdx);
	void createDepthBufferTexture(int32_t textureId, int32_t width, int32_t height);
	void createColorBufferTexture(int32_t textureId, int32_t width, int32_t height);
	void drawInstancedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset, uint32_t indicesBuffer, int32_t instances);
	void createFramebufferObject(int32_t frameBufferId);
	bool beginDrawCommandBuffer(int contextIdx, int bufferId = -1);
	array<VkCommandBuffer, 3> endDrawCommandBuffer(int contextIdx, int bufferId = -1, bool cycleBuffers = true);
	void submitDrawCommandBuffers(int commandBufferCount, VkCommandBuffer* commandBuffers, VkFence& fence, bool waitUntilSubmitted = false, bool resetFence = true);
	void recreateContextFences(int contextIdx);
protected:
	/**
	 * Protected constructor
	 */
	VKRenderer();

public:
	const string getShaderVersion() override;
	void* getDefaultContext() override;
	void* getContext(int contextIdx) override;
	int getContextIndex(void* context) override;
	void initialize() override;
	void initializeFrame() override;
	void finishFrame() override;
	bool isSupportingMultithreadedRendering() override;
	bool isSupportingMultipleRenderQueues() override;
	bool isSupportingVertexArrays() override;
	bool isBufferObjectsAvailable() override;
	bool isDepthTextureAvailable() override;
	bool isUsingProgramAttributeLocation() override;
	bool isSupportingIntegerProgramAttributes() override;
	bool isSpecularMappingAvailable() override;
	bool isNormalMappingAvailable() override;
	bool isInstancedRenderingAvailable() override;
	bool isPBRAvailable() override;
	bool isComputeShaderAvailable() override;
	bool isGLCLAvailable() override;
	bool isUsingShortIndices() override;
	int32_t getTextureUnits() override;
	int32_t loadShader(int32_t type, const string& pathName, const string& fileName, const string& definitions = string(), const string& functions = string()) override;
	void useProgram(void* context, int32_t programId) override;
	int32_t createProgram(int type) override;
	void attachShaderToProgram(int32_t programId, int32_t shaderId) override;
	bool linkProgram(int32_t programId) override;
	int32_t getProgramUniformLocation(int32_t programId, const string& name) override;
	void setProgramUniformInteger(void* context, int32_t uniformId, int32_t value) override;
	void setProgramUniformFloat(void* context, int32_t uniformId, float value) override;
	void setProgramUniformFloatMatrix3x3(void* context, int32_t uniformId, const array<float, 9>& data) override;
	void setProgramUniformFloatMatrix4x4(void* context, int32_t uniformId, const array<float, 16>& data) override;
	void setProgramUniformFloatMatrices4x4(void* context, int32_t uniformId, int32_t count, FloatBuffer* data) override;
	void setProgramUniformFloatVec4(void* context, int32_t uniformId, const array<float, 4>& data) override;
	void setProgramUniformFloatVec3(void* context, int32_t uniformId, const array<float, 3>& data) override;
	void setProgramUniformFloatVec2(void* context, int32_t uniformId, const array<float, 2>& data) override;
	void setProgramAttributeLocation(int32_t programId, int32_t location, const string& name) override;
	int32_t getLighting(void* context) override;
	void setLighting(void* context, int32_t lighting) override;
	void setViewPort(int32_t x, int32_t y, int32_t width, int32_t height) override;
	void updateViewPort() override;
	void setClearColor(float red, float green, float blue, float alpha) override;
	void enableCulling(void* context) override;
	void disableCulling(void* context) override;
	void setFrontFace(void* context, int32_t frontFace) override;
	void setCullFace(int32_t cullFace) override;
	void enableBlending() override;
	void enableAdditionBlending() override;
	void disableBlending() override;
	void enableDepthBufferWriting() override;
	void disableDepthBufferWriting() override;
	void disableDepthBufferTest() override;
	void enableDepthBufferTest() override;
	void setDepthFunction(int32_t depthFunction) override;
	void setColorMask(bool red, bool green, bool blue, bool alpha) override;
	void clear(int32_t mask) override;
	int32_t createTexture() override;
	int32_t createDepthBufferTexture(int32_t width, int32_t height) override;
	int32_t createColorBufferTexture(int32_t width, int32_t height) override;
	void uploadTexture(void* context, Texture* texture) override;
	void uploadCubeMapTexture(void* context, Texture* textureLeft, Texture* textureRight, Texture* textureTop, Texture* textureBottom, Texture* textureFront, Texture* textureBack) override;
	int32_t createCubeMapTexture(void* context, int32_t width, int32_t height) override;
	void resizeDepthBufferTexture(int32_t textureId, int32_t width, int32_t height) override;
	void resizeColorBufferTexture(int32_t textureId, int32_t width, int32_t height) override;
	void bindTexture(void* context, int32_t textureId) override;
	void bindCubeMapTexture(void* context, int32_t textureId) override;
	void disposeTexture(int32_t textureId) override;
	int32_t createFramebufferObject(int32_t depthBufferTextureGlId, int32_t colorBufferTextureGlId, int32_t cubeMapTextureId = 0, int32_t cubeMapTextureIndex = 0) override;
	void bindFrameBuffer(int32_t frameBufferId) override;
	void disposeFrameBufferObject(int32_t frameBufferId) override;
	vector<int32_t> createBufferObjects(int32_t bufferCount, bool useGPUMemory, bool shared) override;
	void uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, FloatBuffer* data) override;
	void uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, ShortBuffer* data) override;
	void uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data) override;
	void uploadIndicesBufferObject(void* context, int32_t bufferObjectId, int32_t size, ShortBuffer* data) override;
	void uploadIndicesBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data) override;
	void bindIndicesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindTextureCoordinatesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindVerticesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindNormalsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindColorsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindTangentsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindBitangentsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindModelMatricesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindEffectColorMulsBufferObject(void* context, int32_t bufferObjectId, int32_t divisor) override;
	void bindEffectColorAddsBufferObject(void* context, int32_t bufferObjectIdd, int32_t divisor) override;
	void bindOriginsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindTextureSpriteIndicesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindPointSizesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindSpriteSheetDimensionBufferObject(void* context, int32_t bufferObjectId) override;
	void drawInstancedIndexedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset, int32_t instances) override;
	void drawIndexedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset) override;
	void drawInstancedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset, int32_t instances) override;
	void drawTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset) override;
	void drawPointsFromBufferObjects(void* context, int32_t points, int32_t pointsOffset) override;
	void setLineWidth(float lineWidth) override;
	void drawLinesFromBufferObjects(void* context, int32_t points, int32_t pointsOffset) override;
	void unbindBufferObjects(void* context) override;
	void disposeBufferObjects(vector<int32_t>& bufferObjectIds) override;
	float readPixelDepth(int32_t x, int32_t y) override;
	ByteBuffer* readPixels(int32_t x, int32_t y, int32_t width, int32_t height) override;
	void initGuiMode() override;
	void doneGuiMode() override;

	// overridden methods for skinning on GPU via compute shader
	void dispatchCompute(void* context, int32_t numNodesX, int32_t numNodesY, int32_t numNodesZ) override;
	void memoryBarrier() override;
	void uploadSkinningBufferObject(void* context, int32_t bufferObjectId, int32_t size, FloatBuffer* data) override;
	void uploadSkinningBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data) override;
	void bindSkinningVerticesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindSkinningNormalsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindSkinningVertexJointsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindSkinningVertexJointIdxsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindSkinningVertexJointWeightsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindSkinningVerticesResultBufferObject(void* context, int32_t bufferObjectId) override;
	void bindSkinningNormalsResultBufferObject(void* context, int32_t bufferObjectId) override;
	void bindSkinningMatricesBufferObject(void* context, int32_t bufferObjectId) override;

	//
	int32_t createVertexArrayObject() override;
	void disposeVertexArrayObject(int32_t vertexArrayObjectId) override;
	void bindVertexArrayObject(int32_t vertexArrayObjectId) override;

	//
	int32_t getTextureUnit(void* context) override;
	void setTextureUnit(void* context, int32_t textureUnit) override;
	Matrix2D3x3& getTextureMatrix(void* context) override;
	Renderer_Light& getLight(void* context, int32_t lightId) override;
	array<float, 4>& getEffectColorMul(void* context) override;
	array<float, 4>& getEffectColorAdd(void* context) override;
	Renderer_SpecularMaterial& getSpecularMaterial(void* context) override;
	Renderer_PBRMaterial& getPBRMaterial(void* context) override;
	const string& getShader(void* context) override;
	void setShader(void* context, const string& id) override;
	const string& getShaderParametersHash(void* context) override;
	const map<string, string>& getShaderParameters(void* context) override;
	void setShaderParameters(void* context, const map<string, string>& parameters) override;
	float getMaskMaxValue(void* context) override;
	void setMaskMaxValue(void* context, float maskMaxValue) override;
	const Renderer_Statistics getStatistics() override;

	/**
	 * Enable/Disable v-sync
	 * @param vSync V-sync enabled
	 */
	void setVSyncEnabled(bool vSync);

};
