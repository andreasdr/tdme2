/**
 * Vulkan renderer
 * based on
 * 	https://github.com/glfw/glfw/blob/master/tests/vulkan.c and util.c from Vulkan samples
 * 	https://vulkan-tutorial.com
 * 	https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
 * 	https://github.com/SaschaWillems/Vulkan
 */

#include <tdme/engine/subsystems/renderer/VKRenderer.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <ext/vulkan/glslang/Public/ShaderLang.h>
#include <ext/vulkan/OGLCompilersDLL/InitializeDll.h>
#include <ext/vulkan/spirv/GlslangToSpv.h>
#include <ext/vulkan/vma/src/VmaUsage.h>

#include <stdlib.h>
#include <string.h>

#include <array>
#include <cassert>
#include <iterator>
#include <map>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <tdme/application/Application.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/subsystems/manager/TextureManager.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/renderer/Renderer_PBRMaterial.h>
#include <tdme/engine/subsystems/renderer/Renderer_SpecularMaterial.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/os/threading/ReadWriteLock.h>
#include <tdme/utilities/Buffer.h>
#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/FloatBuffer.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/IntBuffer.h>
#include <tdme/utilities/ShortBuffer.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>

using std::to_string;
using std::floor;
using std::log2;
using std::max;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define ERR_EXIT(err_msg, err_class)                                           \
    do {                                                                       \
        printf(err_msg);                                                       \
        fflush(stdout);                                                        \
        Application::exit(1);                                                  \
    } while (0)

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)														\
    {																											\
        fp##entrypoint = (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint);				\
        if (fp##entrypoint == nullptr) {																	\
            ERR_EXIT("vkGetInstanceProcAddr failed to find vk" #entrypoint, "vkGetInstanceProcAddr Failure");	\
        }                                                                      									\
    }

#define GET_DEVICE_PROC_ADDR(dev, entrypoint)																	\
    {																											\
        fp##entrypoint = (PFN_vk##entrypoint)vkGetDeviceProcAddr(dev, "vk" #entrypoint);				\
        if (fp##entrypoint == nullptr) {																	\
            ERR_EXIT("vkGetDeviceProcAddr failed to find vk" #entrypoint, "vkGetDeviceProcAddr Failure");		\
        }																										\
    }

using std::array;
using std::iterator;
using std::map;
using std::stack;
using std::string;
using std::to_string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

using tdme::engine::subsystems::renderer::VKRenderer;
using tdme::application::Application;
using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::renderer::Renderer_PBRMaterial;
using tdme::engine::subsystems::renderer::Renderer_SpecularMaterial;
using tdme::math::Matrix4x4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::os::threading::ReadWriteLock;
using tdme::os::threading::Mutex;
using tdme::utilities::ByteBuffer;
using tdme::utilities::Buffer;
using tdme::utilities::Console;
using tdme::utilities::FloatBuffer;
using tdme::utilities::Integer;
using tdme::utilities::IntBuffer;
using tdme::utilities::ShortBuffer;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;

VKRenderer::VKRenderer():
	queue_mutex("queue-mutex"),
	buffers_rwlock("buffers_rwlock"),
	textures_rwlock("textures_rwlock"),
	delete_mutex("delete_mutex"),
	pipeline_rwlock("pipeline_rwlock")
{
	// setup consts
	ID_NONE = 0;
	CLEAR_DEPTH_BUFFER_BIT = 2;
	CLEAR_COLOR_BUFFER_BIT = 1;
	CULLFACE_FRONT = VK_CULL_MODE_FRONT_BIT;
	CULLFACE_BACK = VK_CULL_MODE_BACK_BIT;
	FRONTFACE_CW = VK_FRONT_FACE_CLOCKWISE + 1;
	FRONTFACE_CCW = VK_FRONT_FACE_COUNTER_CLOCKWISE + 1;
	SHADER_FRAGMENT_SHADER = VK_SHADER_STAGE_FRAGMENT_BIT;
	SHADER_VERTEX_SHADER = VK_SHADER_STAGE_VERTEX_BIT;
	SHADER_COMPUTE_SHADER = VK_SHADER_STAGE_COMPUTE_BIT;
	SHADER_GEOMETRY_SHADER = VK_SHADER_STAGE_GEOMETRY_BIT;
	DEPTHFUNCTION_ALWAYS = VK_COMPARE_OP_ALWAYS;
	DEPTHFUNCTION_EQUAL = VK_COMPARE_OP_EQUAL;
	DEPTHFUNCTION_LESSEQUAL = VK_COMPARE_OP_LESS_OR_EQUAL;
	DEPTHFUNCTION_GREATEREQUAL = VK_COMPARE_OP_GREATER_OR_EQUAL;
	FRAMEBUFFER_DEFAULT = 0;
}

inline VkBool32 VKRenderer::checkLayers(uint32_t check_count, const char **check_names, uint32_t layer_count, VkLayerProperties *layers) {
	uint32_t i, j;
	for (i = 0; i < check_count; i++) {
		VkBool32 found = 0;
		for (j = 0; j < layer_count; j++) {
			if (!strcmp(check_names[i], layers[j].layerName)) {
				found = 1;
				break;
			}
		}
		if (!found) {
			fprintf(stderr, "Cannot find layer: %s\n", check_names[i]);
			return 0;
		}
	}
	return 1;
}

inline void VKRenderer::prepareSetupCommandBuffer(int contextIdx) {
	auto& context = contexts[contextIdx];
	if (context.setup_cmd_inuse == VK_NULL_HANDLE) {
		context.setup_cmd_inuse = context.setup_cmd;

		//
		const VkCommandBufferBeginInfo cmd_buf_info = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			.pInheritanceInfo = nullptr
		};

		VkResult err;
		err = vkBeginCommandBuffer(context.setup_cmd_inuse, &cmd_buf_info);
		assert(!err);
	}
}

inline void VKRenderer::finishSetupCommandBuffer(int contextIdx) {
	auto& context = contexts[contextIdx];

	//
	if (context.setup_cmd_inuse != VK_NULL_HANDLE) {
		VkResult err;

		//
		err = vkEndCommandBuffer(context.setup_cmd_inuse);
		assert(!err);

		const VkCommandBuffer cmd_bufs[] = { context.setup_cmd_inuse };
		VkFence nullFence = { VK_NULL_HANDLE };
		VkSubmitInfo submit_info = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.pWaitDstStageMask = nullptr,
			.commandBufferCount = 1,
			.pCommandBuffers = cmd_bufs,
			.signalSemaphoreCount = 0,
			.pSignalSemaphores = nullptr
		};

		queue_mutex.lock();

		err = vkQueueSubmit(queue, 1, &submit_info, context.setup_fence);
		assert(!err);

		//
		queue_mutex.unlock();

		//
		VkResult fence_result;
		do {
			fence_result = vkWaitForFences(device, 1, &context.setup_fence, VK_TRUE, 100000000);
		} while (fence_result == VK_TIMEOUT);
		vkResetFences(device, 1, &context.setup_fence);

		//
		context.setup_cmd_inuse = VK_NULL_HANDLE;
	}
}

inline bool VKRenderer::beginDrawCommandBuffer(int contextIdx, int bufferId) {
	auto& context = contexts[contextIdx];

	//
	if (bufferId == -1) bufferId = context.draw_cmd_current;

	//
	if (context.draw_cmd_started[bufferId][context.front_face_index] == true) return false;

	//
	VkResult fence_result;
	do {
		fence_result = vkWaitForFences(device, 1, &context.draw_fences[bufferId][context.front_face_index], VK_TRUE, 100000000);
	} while (fence_result == VK_TIMEOUT);
	vkResetFences(device, 1, &context.draw_fences[bufferId][context.front_face_index]);

	//
	const VkCommandBufferBeginInfo cmd_buf_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		.pInheritanceInfo = nullptr
	};

	//
	VkResult err;
	err = vkBeginCommandBuffer(context.draw_cmds[bufferId][context.front_face_index], &cmd_buf_info);
	assert(!err);

	//
	// We can use LAYOUT_UNDEFINED as a wildcard here because we don't care what
	// happens to the previous contents of the image
	VkImageMemoryBarrier image_memory_barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = swapchain_buffers[current_buffer].image,
		.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
	};
	vkCmdPipelineBarrier(context.draw_cmds[bufferId][context.front_face_index], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

	//
	context.draw_cmd_started[bufferId][context.front_face_index] = true;

	//
	return true;
}

inline array<VkCommandBuffer, 3> VKRenderer::endDrawCommandBuffer(int contextIdx, int bufferId, bool cycleBuffers) {
	auto& context = contexts[contextIdx];

	array<VkCommandBuffer, 3> commandBuffers;

	//
	if (bufferId == -1) bufferId = context.draw_cmd_current;

	for (auto i = 0; i < 3; i++) {
		context.pipeline[i] = VK_NULL_HANDLE;

		//
		if (context.draw_cmd_started[bufferId][i] == false) {
			commandBuffers[i] = VK_NULL_HANDLE;
			continue;
		}

		//
		VkResult err;
		err = vkEndCommandBuffer(context.draw_cmds[bufferId][i]);
		assert(!err);

		commandBuffers[i] = context.draw_cmds[bufferId][i];

		//
		context.draw_cmd_started[bufferId][i] = false;
	}

	//
	if (cycleBuffers == true) context.draw_cmd_current = (context.draw_cmd_current + 1) % DRAW_COMMANDBUFFER_MAX;

	//
	return commandBuffers;
}

inline void VKRenderer::recreateContextFences(int contextIdx) {
	auto& context = contexts[contextIdx];
	if (context.draw_fence == VK_NULL_HANDLE) {
		VkFenceCreateInfo fence_create_info_unsignaled = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};
		vkCreateFence(device, &fence_create_info_unsignaled, nullptr, &context.draw_fence);
	}
	VkFenceCreateInfo fence_create_info_signaled = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};
	for (auto i = 0; i < DRAW_COMMANDBUFFER_MAX; i++) {
		for (auto j = 0; j < 3; j++) {
			if (context.draw_fences[i][j] != VK_NULL_HANDLE) vkDestroyFence(device, context.draw_fences[i][j], nullptr);
			vkCreateFence(device, &fence_create_info_signaled, nullptr, &context.draw_fences[i][j]);
		}
	}

}

inline void VKRenderer::submitDrawCommandBuffers(int commandBufferCount, VkCommandBuffer* commandBuffers, VkFence& fence, bool waitUntilSubmitted, bool resetFence) {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(commandBufferCount));

	//
	VkResult err;
	VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	VkSubmitInfo submit_info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = &pipe_stage_flags,
		.commandBufferCount = static_cast<uint32_t>(commandBufferCount),
		.pCommandBuffers = commandBuffers,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr
	};

	//
	queue_mutex.lock();

	err = vkQueueSubmit(queue, 1, &submit_info, fence);
	assert(!err);

	queue_mutex.unlock();

	//
	if (waitUntilSubmitted == true) {
		//
		VkResult fence_result;
		do {
			fence_result = vkWaitForFences(device, 1, &fence, VK_TRUE, 100000000);
		} while (fence_result == VK_TIMEOUT);
		if (resetFence == true) vkResetFences(device, 1, &fence);
	}
}

inline void VKRenderer::finishSetupCommandBuffers() {
	for (auto contextIdx = 0; contextIdx < Engine::getThreadCount(); contextIdx++) finishSetupCommandBuffer(contextIdx);
}

inline void VKRenderer::setImageLayout(int contextIdx, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout, VkImageLayout new_image_layout, VkAccessFlagBits srcAccessMask, uint32_t baseLevel, uint32_t levelCount) {
	auto& context = contexts[contextIdx];

	//
	prepareSetupCommandBuffer(contextIdx);

	//
	VkResult err;
	VkImageMemoryBarrier image_memory_barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = nullptr,
		.srcAccessMask = srcAccessMask,
		.dstAccessMask = 0,
		.oldLayout = old_image_layout,
		.newLayout = new_image_layout,
	    .srcQueueFamilyIndex = 0,
	    .dstQueueFamilyIndex = 0,
		.image = image,
		.subresourceRange = {
			.aspectMask = aspectMask,
			.baseMipLevel = baseLevel,
			.levelCount = levelCount,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}
	if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}
	if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	}
	if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	}

	//
	vkCmdPipelineBarrier(context.setup_cmd_inuse, VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
}

inline uint32_t VKRenderer::getMipLevels(int32_t textureWidth, int32_t textureHeight) {
	return static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;
}

inline void VKRenderer::prepareTextureImage(int contextIdx, struct texture_object *tex_obj, VkImageTiling tiling, VkImageUsageFlags usage, VkFlags required_props, Texture* texture, VkImageLayout image_layout, bool disableMipMaps) {
	const VkFormat tex_format = texture->getHeight() == 32?VK_FORMAT_R8G8B8A8_UNORM:VK_FORMAT_R8G8B8A8_UNORM;
	VkResult err;
	bool pass;

	auto textureWidth = static_cast<uint32_t>(texture->getTextureWidth());
	auto textureHeight = static_cast<uint32_t>(texture->getTextureHeight());

	const VkImageCreateInfo image_create_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = tex_format,
		.extent = {
			.width = textureWidth,
			.height = textureHeight,
			.depth = 1
		},
		.mipLevels = disableMipMaps == false && texture->isUseMipMap() == true?getMipLevels(textureWidth, textureHeight):1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	    .queueFamilyIndexCount = 0,
	    .pQueueFamilyIndices = 0,
		.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED
	};

	VmaAllocationCreateInfo image_alloc_create_info = {};
	image_alloc_create_info.usage = VMA_MEMORY_USAGE_UNKNOWN;
	image_alloc_create_info.requiredFlags = required_props;


	VmaAllocationInfo allocation_info = {};
	err = vmaCreateImage(allocator, &image_create_info, &image_alloc_create_info, &tex_obj->image, &tex_obj->allocation, &allocation_info);
	assert(!err);

	if ((required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
		const VkImageSubresource subres = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.arrayLayer = 0,
		};
		VkSubresourceLayout layout;
		vkGetImageSubresourceLayout(device, tex_obj->image, &subres, &layout);

		void* data;
		err = vmaMapMemory(allocator, tex_obj->allocation, &data);
		assert(!err);

		auto bytesPerPixel = texture->getDepth() / 8;
		auto textureBuffer = texture->getTextureData();
		for (auto y = 0; y < textureHeight; y++) {
			char* row = (char*)((char*)data + layout.offset + layout.rowPitch * y);
			for (auto x = 0; x < textureWidth; x++) {
				row[x * 4 + 0] = textureBuffer->get((y * textureWidth * bytesPerPixel) + (x * bytesPerPixel) + 0);
				row[x * 4 + 1] = textureBuffer->get((y * textureWidth * bytesPerPixel) + (x * bytesPerPixel) + 1);
				row[x * 4 + 2] = textureBuffer->get((y * textureWidth * bytesPerPixel) + (x * bytesPerPixel) + 2);
				row[x * 4 + 3] = bytesPerPixel == 4?textureBuffer->get((y * textureWidth * bytesPerPixel) + (x * bytesPerPixel) + 3):0xff;
			}
		}
		vmaFlushAllocation(allocator, tex_obj->allocation, 0, VK_WHOLE_SIZE);
		vmaUnmapMemory(allocator, tex_obj->allocation);
	}

	//
	tex_obj->image_layout = image_layout;
	setImageLayout(
		contextIdx,
		tex_obj->image,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_PREINITIALIZED,
		tex_obj->image_layout,
		VK_ACCESS_HOST_WRITE_BIT
	);
}

void VKRenderer::shaderInitResources(TBuiltInResource &resources) {
    resources.maxLights = 32;
    resources.maxClipPlanes = 6;
    resources.maxTextureUnits = 32;
    resources.maxTextureCoords = 32;
    resources.maxVertexAttribs = 64;
    resources.maxVertexUniformComponents = 4096;
    resources.maxVaryingFloats = 64;
    resources.maxVertexTextureImageUnits = 32;
    resources.maxCombinedTextureImageUnits = 80;
    resources.maxTextureImageUnits = 32;
    resources.maxFragmentUniformComponents = 4096;
    resources.maxDrawBuffers = 32;
    resources.maxVertexUniformVectors = 128;
    resources.maxVaryingVectors = 8;
    resources.maxFragmentUniformVectors = 16;
    resources.maxVertexOutputVectors = 16;
    resources.maxFragmentInputVectors = 15;
    resources.minProgramTexelOffset = -8;
    resources.maxProgramTexelOffset = 7;
    resources.maxClipDistances = 8;
    resources.maxComputeWorkGroupCountX = 65535;
    resources.maxComputeWorkGroupCountY = 65535;
    resources.maxComputeWorkGroupCountZ = 65535;
    resources.maxComputeWorkGroupSizeX = 1024;
    resources.maxComputeWorkGroupSizeY = 1024;
    resources.maxComputeWorkGroupSizeZ = 64;
    resources.maxComputeUniformComponents = 1024;
    resources.maxComputeTextureImageUnits = 16;
    resources.maxComputeImageUniforms = 8;
    resources.maxComputeAtomicCounters = 8;
    resources.maxComputeAtomicCounterBuffers = 1;
    resources.maxVaryingComponents = 60;
    resources.maxVertexOutputComponents = 64;
    resources.maxGeometryInputComponents = 64;
    resources.maxGeometryOutputComponents = 128;
    resources.maxFragmentInputComponents = 128;
    resources.maxImageUnits = 8;
    resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    resources.maxCombinedShaderOutputResources = 8;
    resources.maxImageSamples = 0;
    resources.maxVertexImageUniforms = 0;
    resources.maxTessControlImageUniforms = 0;
    resources.maxTessEvaluationImageUniforms = 0;
    resources.maxGeometryImageUniforms = 0;
    resources.maxFragmentImageUniforms = 8;
    resources.maxCombinedImageUniforms = 8;
    resources.maxGeometryTextureImageUnits = 16;
    resources.maxGeometryOutputVertices = 256;
    resources.maxGeometryTotalOutputComponents = 1024;
    resources.maxGeometryUniformComponents = 1024;
    resources.maxGeometryVaryingComponents = 64;
    resources.maxTessControlInputComponents = 128;
    resources.maxTessControlOutputComponents = 128;
    resources.maxTessControlTextureImageUnits = 16;
    resources.maxTessControlUniformComponents = 1024;
    resources.maxTessControlTotalOutputComponents = 4096;
    resources.maxTessEvaluationInputComponents = 128;
    resources.maxTessEvaluationOutputComponents = 128;
    resources.maxTessEvaluationTextureImageUnits = 16;
    resources.maxTessEvaluationUniformComponents = 1024;
    resources.maxTessPatchComponents = 120;
    resources.maxPatchVertices = 32;
    resources.maxTessGenLevel = 64;
    resources.maxViewports = 16;
    resources.maxVertexAtomicCounters = 0;
    resources.maxTessControlAtomicCounters = 0;
    resources.maxTessEvaluationAtomicCounters = 0;
    resources.maxGeometryAtomicCounters = 0;
    resources.maxFragmentAtomicCounters = 8;
    resources.maxCombinedAtomicCounters = 8;
    resources.maxAtomicCounterBindings = 1;
    resources.maxVertexAtomicCounterBuffers = 0;
    resources.maxTessControlAtomicCounterBuffers = 0;
    resources.maxTessEvaluationAtomicCounterBuffers = 0;
    resources.maxGeometryAtomicCounterBuffers = 0;
    resources.maxFragmentAtomicCounterBuffers = 1;
    resources.maxCombinedAtomicCounterBuffers = 1;
    resources.maxAtomicCounterBufferSize = 16384;
    resources.maxTransformFeedbackBuffers = 4;
    resources.maxTransformFeedbackInterleavedComponents = 64;
    resources.maxCullDistances = 8;
    resources.maxCombinedClipAndCullDistances = 8;
    resources.maxSamples = 4;
    resources.maxMeshOutputVerticesNV = 256;
    resources.maxMeshOutputPrimitivesNV = 512;
    resources.maxMeshWorkGroupSizeX_NV = 32;
    resources.maxMeshWorkGroupSizeY_NV = 1;
    resources.maxMeshWorkGroupSizeZ_NV = 1;
    resources.maxTaskWorkGroupSizeX_NV = 32;
    resources.maxTaskWorkGroupSizeY_NV = 1;
    resources.maxTaskWorkGroupSizeZ_NV = 1;
    resources.maxMeshViewCountNV = 4;
    resources.limits.nonInductiveForLoops = 1;
    resources.limits.whileLoops = 1;
    resources.limits.doWhileLoops = 1;
    resources.limits.generalUniformIndexing = 1;
    resources.limits.generalAttributeMatrixVectorIndexing = 1;
    resources.limits.generalVaryingIndexing = 1;
    resources.limits.generalSamplerIndexing = 1;
    resources.limits.generalVariableIndexing = 1;
    resources.limits.generalConstantMatrixVectorIndexing = 1;
}

EShLanguage VKRenderer::shaderFindLanguage(const VkShaderStageFlagBits shaderType) {
    switch (shaderType) {
        case VK_SHADER_STAGE_VERTEX_BIT:
            return EShLangVertex;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return EShLangTessControl;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return EShLangTessEvaluation;
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return EShLangGeometry;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return EShLangFragment;
        case VK_SHADER_STAGE_COMPUTE_BIT:
            return EShLangCompute;
        default:
            return EShLangVertex;
    }
}

void VKRenderer::initializeSwapChain() {

	VkResult err;
	VkSwapchainKHR oldSwapchain = swapchain;

	// Check the surface capabilities and formats
	VkSurfaceCapabilitiesKHR surfCapabilities;
	err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &surfCapabilities);
	assert(err == VK_SUCCESS);

	uint32_t presentModeCount;
	err = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, nullptr);
	assert(err == VK_SUCCESS);
	VkPresentModeKHR *presentModes = (VkPresentModeKHR *) malloc(presentModeCount * sizeof(VkPresentModeKHR));
	assert(presentModes);
	err = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, presentModes);
	assert(err == VK_SUCCESS);

	VkExtent2D swapchainExtent;
	// width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
	if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
		// If the surface size is undefined, the size is set to the size
		// of the images requested, which must fit within the minimum and
		// maximum values.
		swapchainExtent.width = width;
		swapchainExtent.height = height;

		if (swapchainExtent.width < surfCapabilities.minImageExtent.width) {
			swapchainExtent.width = surfCapabilities.minImageExtent.width;
		} else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width) {
			swapchainExtent.width = surfCapabilities.maxImageExtent.width;
		}

		if (swapchainExtent.height < surfCapabilities.minImageExtent.height) {
			swapchainExtent.height = surfCapabilities.minImageExtent.height;
		} else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height) {
			swapchainExtent.height = surfCapabilities.maxImageExtent.height;
		}
	} else {
		// If the surface size is defined, the swap chain size must match
		swapchainExtent = surfCapabilities.currentExtent;
		width = surfCapabilities.currentExtent.width;
		height = surfCapabilities.currentExtent.height;
	}

	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; // no vsync, VK_PRESENT_MODE_FIFO_KHR: vsync

	// Determine the number of VkImage's to use in the swap chain.
	// Application desires to only acquire 1 image at a time (which is
	// "surfCapabilities.minImageCount").
	uint32_t desiredNumOfSwapchainImages = surfCapabilities.minImageCount;
	// If maxImageCount is 0, we can ask for as many images as we want;
	// otherwise we're limited to maxImageCount
	if ((surfCapabilities.maxImageCount > 0) && (desiredNumOfSwapchainImages > surfCapabilities.maxImageCount)) {
		// Application must settle for fewer images than desired:
		desiredNumOfSwapchainImages = surfCapabilities.maxImageCount;
	}

	VkSurfaceTransformFlagsKHR preTransform;
	if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	} else {
		preTransform = surfCapabilities.currentTransform;
	}

	const VkSwapchainCreateInfoKHR swapchainCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.surface = surface,
		.minImageCount = desiredNumOfSwapchainImages,
		.imageFormat = format,
		.imageColorSpace = color_space,
		.imageExtent = {
			.width = swapchainExtent.width,
			.height = swapchainExtent.height
		},
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = 0,
		.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform, /// TODO = a.drewke, ???
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = swapchainPresentMode,
		.clipped = true,
		.oldSwapchain = oldSwapchain,
	};
	uint32_t i;

	err = fpCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);
	assert(!err);

	// If we just re-created an existing swapchain, we should destroy the old
	// swapchain at this point.
	// Note: destroying the swapchain also cleans up all its associated
	// presentable images once the platform is done with them.
	if (oldSwapchain != VK_NULL_HANDLE) {
		fpDestroySwapchainKHR(device, oldSwapchain, nullptr);
	}

	err = fpGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr);
	assert(err == VK_SUCCESS);

	VkImage* swapchainImages = (VkImage*)malloc(swapchain_image_count * sizeof(VkImage));
	assert(swapchainImages != nullptr);
	err = fpGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchainImages);
	assert(err == VK_SUCCESS);

	swapchain_buffers = (swapchain_buffer_type*)malloc(sizeof(swapchain_buffer_type) * swapchain_image_count);
	assert(swapchain_buffers != nullptr);

	for (i = 0; i < swapchain_image_count; i++) {
		VkImageViewCreateInfo color_attachment_view = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = swapchainImages[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = format,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_R,
				.g = VK_COMPONENT_SWIZZLE_G,
				.b = VK_COMPONENT_SWIZZLE_B,
				.a = VK_COMPONENT_SWIZZLE_A
			},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		swapchain_buffers[i].image = swapchainImages[i];
		err = vkCreateImageView(device, &color_attachment_view, nullptr, &swapchain_buffers[i].view);
		assert(err == VK_SUCCESS);
	}

	current_buffer = 0;

	if (nullptr != presentModes) free(presentModes);
}

const string VKRenderer::getShaderVersion()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return "gl3";
}

void* VKRenderer::getDefaultContext() {
	return &contexts[0];
}

void* VKRenderer::getContext(int contextIdx) {
	return &contexts[contextIdx];
}

int VKRenderer::getContextIndex(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.idx;
}

bool VKRenderer::isSupportingMultithreadedRendering() {
	return true;
}

bool VKRenderer::isSupportingMultipleRenderQueues() {
	return false;
}

bool VKRenderer::isSupportingVertexArrays() {
	return false;
}

void VKRenderer::initialize()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	//
	glfwGetWindowSize(Application::glfwWindow, (int32_t*)&width, (int32_t*)&height);

	//
	glslang::InitProcess();
	glslang::InitThread();
	ShInitialize();

	VkResult err;
	uint32_t i = 0;
	uint32_t required_extension_count = 0;
	uint32_t instance_extension_count = 0;
	uint32_t instance_layer_count = 0;
	uint32_t validation_layer_count = 0;
	const char **required_extensions = nullptr;
	const char **instance_validation_layers = nullptr;

	uint32_t enabled_extension_count = 0;
	uint32_t enabled_layer_count = 0;
	const char *extension_names[64];
	const char *enabled_layers[64];

	char* instance_validation_layers_alt1[] = {
		"VK_LAYER_LUNARG_standard_validation"
	};
	char* instance_validation_layers_alt2[] = {
		"VK_LAYER_GOOGLE_threading",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_image",
		"VK_LAYER_LUNARG_core_validation",
		"VK_LAYER_LUNARG_swapchain",
		"VK_LAYER_GOOGLE_unique_objects"
	};

	// Look for validation layers
	if (validate == true) {
		VkBool32 validation_found = 0;
		err = vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr);
		assert(!err);

		instance_validation_layers = (const char**) instance_validation_layers_alt1;
		if (instance_layer_count > 0) {
			VkLayerProperties* instance_layers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * instance_layer_count);
			err = vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layers);
			assert(!err);

			validation_found = checkLayers(
				ARRAY_SIZE(instance_validation_layers_alt1),
				instance_validation_layers,
				instance_layer_count,
				instance_layers
			);
			if (validation_found) {
				enabled_layer_count = ARRAY_SIZE(instance_validation_layers_alt1);
				enabled_layers[0] = "VK_LAYER_LUNARG_standard_validation";
				validation_layer_count = 1;
			} else {
				// use alternative set of validation layers
				instance_validation_layers = (const char**) instance_validation_layers_alt2;
				enabled_layer_count = ARRAY_SIZE(instance_validation_layers_alt2);
				validation_found = checkLayers(
					ARRAY_SIZE(instance_validation_layers_alt2),
					instance_validation_layers,
					instance_layer_count,
					instance_layers
				);
				validation_layer_count = ARRAY_SIZE(instance_validation_layers_alt2);
				for (i = 0; i < validation_layer_count; i++) {
					enabled_layers[i] = instance_validation_layers[i];
				}
			}
			free(instance_layers);
		}

		if (!validation_found) {
			ERR_EXIT("vkEnumerateInstanceLayerProperties failed to find "
					"required validation layer.\n\n"
					"Please look at the Getting Started guide for additional "
					"information.\n", "vkCreateInstance Failure");
		}
	}

	// Look for instance extensions
	required_extensions = glfwGetRequiredInstanceExtensions(&required_extension_count);
	if (!required_extensions) {
		ERR_EXIT("glfwGetRequiredInstanceExtensions failed to find the "
			"platform surface extensions.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n", "vkCreateInstance Failure"
		);
	}

	for (i = 0; i < required_extension_count; i++) {
		extension_names[enabled_extension_count++] = required_extensions[i];
		assert(enabled_extension_count < 64);
	}

	err = vkEnumerateInstanceExtensionProperties(
	nullptr, &instance_extension_count, nullptr);
	assert(!err);

	if (instance_extension_count > 0) {
		VkExtensionProperties* instance_extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * instance_extension_count);
		err = vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, instance_extensions);
		assert(!err);
		for (i = 0; i < instance_extension_count; i++) {
			if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, instance_extensions[i].extensionName)) {
				if (validate == true) {
					extension_names[enabled_extension_count++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
				}
			}
			assert(enabled_extension_count < 64);
		}
		free(instance_extensions);
	}

	const VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "TDME2 based application",
		.applicationVersion = 0,
		.pEngineName = "TDME2",
		.engineVersion = 0,
		.apiVersion = VK_API_VERSION_1_0,
	};
	VkInstanceCreateInfo inst_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &app,
		.enabledLayerCount = enabled_layer_count,
		.ppEnabledLayerNames = (const char * const *)instance_validation_layers,
		.enabledExtensionCount = enabled_extension_count,
		.ppEnabledExtensionNames = (const char * const *)extension_names,
	};
	uint32_t gpu_count;

	err = vkCreateInstance(&inst_info, nullptr, &inst);
	if (err == VK_ERROR_INCOMPATIBLE_DRIVER) {
		ERR_EXIT("Cannot find a compatible Vulkan installable client driver "
				"(ICD).\n\nPlease look at the Getting Started guide for "
				"additional information.\n", "vkCreateInstance Failure");
	} else
	if (err == VK_ERROR_EXTENSION_NOT_PRESENT) {
		ERR_EXIT("Cannot find a specified extension library"
				".\nMake sure your layers path is set appropriately\n",
				"vkCreateInstance Failure");
	} else
	if (err) {
		ERR_EXIT("vkCreateInstance failed.\n\nDo you have a compatible Vulkan "
				"installable client driver (ICD) installed?\nPlease look at "
				"the Getting Started guide for additional information.\n",
				"vkCreateInstance Failure");
	}

	// Make initial call to query gpu_count, then second call for gpu info
	err = vkEnumeratePhysicalDevices(inst, &gpu_count, nullptr);
	assert(!err && gpu_count > 0);

	if (gpu_count > 0) {
		VkPhysicalDevice* physical_devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
		err = vkEnumeratePhysicalDevices(inst, &gpu_count, physical_devices);
		assert(!err);
		// For tri demo we just grab the first physical device
		gpu = physical_devices[0];
		free(physical_devices);
	} else {
		ERR_EXIT(
			"vkEnumeratePhysicalDevices reported zero accessible devices."
			"\n\nDo you have a compatible Vulkan installable client"
			" driver (ICD) installed?\nPlease look at the Getting Started"
			" guide for additional information.\n",
			"vkEnumeratePhysicalDevices Failure"
		);
	}

	// Look for device extensions
	uint32_t device_extension_count = 0;
	VkBool32 swapchainExtFound = 0;
	enabled_extension_count = 0;

	err = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &device_extension_count, nullptr);
	assert(!err);

	if (device_extension_count > 0) {
		VkExtensionProperties* device_extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * device_extension_count);
		err = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &device_extension_count, device_extensions);
		assert(!err);

		for (i = 0; i < device_extension_count; i++) {
			if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, device_extensions[i].extensionName)) {
				swapchainExtFound = 1;
				extension_names[enabled_extension_count++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
			}
			assert(enabled_extension_count < 64);
		}

		free(device_extensions);
	}

	if (!swapchainExtFound) {
		ERR_EXIT(
			"vkEnumerateDeviceExtensionProperties failed to find "
			"the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n", "vkCreateInstance Failure"
		);
	}

	// Having these GIPA queries of device extension entry points both
	// BEFORE and AFTER vkCreateDevice is a good test for the loader
	GET_INSTANCE_PROC_ADDR(inst, GetPhysicalDeviceSurfaceCapabilitiesKHR);
	GET_INSTANCE_PROC_ADDR(inst, GetPhysicalDeviceSurfaceFormatsKHR);
	GET_INSTANCE_PROC_ADDR(inst, GetPhysicalDeviceSurfacePresentModesKHR);
	GET_INSTANCE_PROC_ADDR(inst, GetPhysicalDeviceSurfaceSupportKHR);

	vkGetPhysicalDeviceProperties(gpu, &gpu_props);

	// Query with nullptr data to get count
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, nullptr);

	queue_props = (VkQueueFamilyProperties *) malloc(queue_count * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, queue_props);
	assert(queue_count >= 1);

	vkGetPhysicalDeviceFeatures(gpu, &gpu_features);

	// Create a WSI surface for the window:
	err = glfwCreateWindowSurface(inst, Application::glfwWindow, nullptr, &surface);
	assert(!err);

	// Iterate over each queue to learn whether it supports presenting:
	VkBool32 *supportsPresent = (VkBool32 *) malloc(queue_count * sizeof(VkBool32));
	for (i = 0; i < queue_count; i++) {
		fpGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &supportsPresent[i]);
	}

	// Search for a graphics and a present queue in the array of queue
	// families, try to find one that supports both
	uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	uint32_t presentQueueNodeIndex = UINT32_MAX;
	for (i = 0; i < queue_count; i++) {
		if ((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
			if (graphicsQueueNodeIndex == UINT32_MAX) {
				graphicsQueueNodeIndex = i;
			}

			if (supportsPresent[i] == VK_TRUE) {
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	if (presentQueueNodeIndex == UINT32_MAX) {
		// If didn't find a queue that supports both graphics and present, then
		// find a separate present queue.
		for (i = 0; i < queue_count; ++i) {
			if (supportsPresent[i] == VK_TRUE) {
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	free(supportsPresent);

	// Generate error if could not find both a graphics and a present queue
	if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) {
		ERR_EXIT(
			"Could not find a graphics and a present queue\n",
			"Swapchain Initialization Failure"
		);
	}

	// TODO: Add support for separate queues, including presentation,
	//       synchronization, and appropriate tracking for QueueSubmit.
	// NOTE: While it is possible for an application to use a separate graphics
	//       and a present queues, this demo program assumes it is only using
	//       one:
	if (graphicsQueueNodeIndex != presentQueueNodeIndex) {
		ERR_EXIT(
			"Could not find a common graphics and a present queue\n",
			"Swapchain Initialization Failure"
		);
	}

	graphics_queue_node_index = graphicsQueueNodeIndex;

	// init_device
	float queue_priorities[1] = { 0.0f };
	const VkDeviceQueueCreateInfo queueCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = graphics_queue_node_index,
		.queueCount = 1,
		.pQueuePriorities = queue_priorities
	};

	VkPhysicalDeviceFeatures features;
	memset(&features, 0, sizeof(features));
	if (gpu_features.shaderClipDistance) features.shaderClipDistance = VK_TRUE;
	if (gpu_features.wideLines) features.wideLines = VK_TRUE; // TODO: a.drewke, store enabled GPU features and check them on rendering if available

	VkDeviceCreateInfo deviceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queueCreateInfo,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = enabled_extension_count,
		.ppEnabledExtensionNames = (const char * const *) extension_names,
		.pEnabledFeatures = &features
	};

	err = vkCreateDevice(gpu, &deviceCreateInfo, nullptr, &device);
	assert(!err);

	GET_DEVICE_PROC_ADDR(device, CreateSwapchainKHR);
	GET_DEVICE_PROC_ADDR(device, DestroySwapchainKHR);
	GET_DEVICE_PROC_ADDR(device, GetSwapchainImagesKHR);
	GET_DEVICE_PROC_ADDR(device, AcquireNextImageKHR);
	GET_DEVICE_PROC_ADDR(device, QueuePresentKHR);

	vkGetDeviceQueue(device, graphics_queue_node_index, 0, &queue);

	// Get the list of VkFormat's that are supported:
	uint32_t formatCount;
	err = fpGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);
	assert(!err);
	VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *) malloc(formatCount * sizeof(VkSurfaceFormatKHR));
	err = fpGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, surfFormats);

	assert(!err);
	// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
	// the surface has no preferred format.  Otherwise, at least one
	// supported format will be returned.
	if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {
		format = VK_FORMAT_B8G8R8A8_UNORM;
	} else {
		assert(formatCount >= 1);
		format = surfFormats[0].format;
	}
	color_space = surfFormats[0].colorSpace;

	// Get Memory information and properties
	vkGetPhysicalDeviceMemoryProperties(gpu, &memory_properties);

	// initialize allocator
	VmaAllocatorCreateInfo allocator_info = {};
	allocator_info.physicalDevice = gpu;
	allocator_info.device = device;
	allocator_info.instance = inst;
	allocator_info.vulkanApiVersion = VK_API_VERSION_1_0;

    //
	vmaCreateAllocator(&allocator_info, &allocator);

	// swap chain
	initializeSwapChain();

	// create descriptor pool
	const VkDescriptorPoolSize types_count[3] = {
		[0] = {
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = static_cast<uint32_t>(32 * 4 * DESC_MAX * Engine::getThreadCount()) // 32 shader * 4 image sampler
		},
		[1] = {
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = static_cast<uint32_t>(32 * 4 * DESC_MAX * Engine::getThreadCount()) // 32 shader * 4 uniform buffer
		},
		[2] = {
			.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = static_cast<uint32_t>(32 * 10 * DESC_MAX * Engine::getThreadCount()) // 32 shader * 10 storage buffer
		}
	};
	const VkDescriptorPoolCreateInfo descriptor_pool = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.maxSets = static_cast<uint32_t>(DESC_MAX * Engine::getThreadCount() * 32), // 32 shader
		.poolSizeCount = 3,
		.pPoolSizes = types_count,
	};

	err = vkCreateDescriptorPool(device, &descriptor_pool, nullptr, &desc_pool);
	assert(!err);

	contexts.resize(Engine::getThreadCount());
	// create set up command buffers
	for (auto contextIdx = 0; contextIdx < Engine::getThreadCount(); contextIdx++) {
		auto& context = contexts[contextIdx];
		{
			// command pool
			const VkCommandPoolCreateInfo cmd_pool_info = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.pNext = nullptr,
				.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				.queueFamilyIndex = graphics_queue_node_index
			};
			err = vkCreateCommandPool(device, &cmd_pool_info, nullptr, &context.cmd_setup_pool);
			assert(!err);

			// command buffer
			const VkCommandBufferAllocateInfo cmd = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.pNext = nullptr,
				.commandPool = context.cmd_setup_pool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1
			};
			err = vkAllocateCommandBuffers(device, &cmd, &context.setup_cmd);
			assert(!err);
		}

		{
			// draw command pool
			const VkCommandPoolCreateInfo cmd_pool_info = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.pNext = nullptr,
				.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				.queueFamilyIndex = graphics_queue_node_index
			};
			err = vkCreateCommandPool(device, &cmd_pool_info, nullptr, &context.cmd_draw_pool);
			assert(!err);

			// command buffer
			const VkCommandBufferAllocateInfo cmd = {
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.pNext = nullptr,
				.commandPool = context.cmd_draw_pool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1
			};
			for (auto j = 0; j < DRAW_COMMANDBUFFER_MAX; j++) {
				for (auto k = 0; k < 3; k++) {
					err = vkAllocateCommandBuffers(device, &cmd, &context.draw_cmds[j][k]);
					assert(!err);
				}
			}
		}

		{
			VkFenceCreateInfo fence_create_info = {
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0
			};
			vkCreateFence(device, &fence_create_info, nullptr, &context.setup_fence);
		}

		//
		context.idx = contextIdx;
		context.setup_cmd_inuse = VK_NULL_HANDLE;
		context.draw_cmd_current = 0;
		context.pipeline.fill(VK_NULL_HANDLE);
		context.draw_cmd_started.fill({false, false, false});
		context.render_pass_started.fill(false);

		// set up lights
		for (auto i = 0; i < context.lights.size(); i++) {
			context.lights[i].spotCosCutoff = static_cast< float >(Math::cos(Math::PI / 180.0f * 180.0f));
		}
		context.texture_matrix.identity();

		//
		context.draw_fences.fill({VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE});
		recreateContextFences(context.idx);
	}

	// memory barrier fence
	{
		VkFenceCreateInfo fence_create_info = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};
		vkCreateFence(device, &fence_create_info, nullptr, &memorybarrier_fence);
	}

	//
	initializeRenderPass();
	initializeFrameBuffers();

	//
	empty_vertex_buffer_id = createBufferObjects(1, true, true)[0];
	empty_vertex_buffer = getBufferObjectInternal(empty_vertex_buffer_id);
	array<float, 16> bogusVertexBuffer = {{
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
	}};
	uploadBufferObjectInternal(0, empty_vertex_buffer, bogusVertexBuffer.size() * sizeof(float), (uint8_t*)bogusVertexBuffer.data(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));

	//
	white_texture_default_id = Engine::getInstance()->getTextureManager()->addTexture(TextureReader::read("resources/engine/textures", "transparent_pixel.png"), getDefaultContext());
	white_texture_default = &textures.find(white_texture_default_id)->second;
}

void VKRenderer::initializeRenderPass() {
	VkResult err;

	//
	if (render_pass != VK_NULL_HANDLE) vkDestroyRenderPass(device, render_pass, nullptr);

	// depth buffer
	if (depth_buffer_default != 0) disposeTexture(depth_buffer_default);
	depth_buffer_default = createDepthBufferTexture(width, height);

	// render pass
	const VkAttachmentDescription attachments[2] = {
		[0] = {
			.flags = 0,
			.format = format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			// TODO: a.drewke, was: VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, check me later, something with changing image layouts is wrong here sometimes
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		},
		[1] = {
			.flags = 0,
			.format = VK_FORMAT_D32_SFLOAT,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
			.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		},
	};
	const VkAttachmentReference color_reference = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};
	const VkAttachmentReference depth_reference = {
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};
	const VkSubpassDescription subpass = {
		.flags = 0,
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount = 0,
		.pInputAttachments = nullptr,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_reference,
		.pResolveAttachments = nullptr,
		.pDepthStencilAttachment = &depth_reference,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = nullptr
	};
	const VkRenderPassCreateInfo rp_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 0,
		.pDependencies = nullptr
	};
	err = vkCreateRenderPass(device, &rp_info, nullptr, &render_pass);
	assert(!err);
}

inline void VKRenderer::startRenderPass(int contextIdx, int line) {
	auto& context = contexts[contextIdx];

	if (context.render_pass_started[context.front_face_index] == true) return;
	context.render_pass_started[context.front_face_index] = true;

	auto frameBuffer = window_framebuffers[current_buffer];
	auto renderPass = render_pass;
	if (bound_frame_buffer != 0) {
		auto frameBufferIt = framebuffers.find(bound_frame_buffer);
		if (frameBufferIt == framebuffers.end()) {
			Console::println("VKRenderer::" + string(__FUNCTION__) + "(): framebuffer not found: " + to_string(bound_frame_buffer));
		} else {
			frameBuffer = frameBufferIt->second.frame_buffer;
			renderPass = frameBufferIt->second.render_pass;
		}
	}

	const VkRenderPassBeginInfo rp_begin = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = renderPass,
		.framebuffer = frameBuffer,
		.renderArea = scissor,
		.clearValueCount = 0,
		.pClearValues = nullptr
	};
	vkCmdBeginRenderPass(context.draw_cmds[context.draw_cmd_current][context.front_face_index], &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
}

inline void VKRenderer::endRenderPass(int contextIdx, int line) {
	auto& context = contexts[contextIdx];
	for (auto i = 0; i < 3; i++) {
		if (context.render_pass_started[i] == false) continue;
		context.render_pass_started[i] = false;
		vkCmdEndRenderPass(context.draw_cmds[context.draw_cmd_current][i]);
	}
}

void VKRenderer::initializeFrameBuffers() {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	VkImageView attachments[2];
	auto depthBufferIt = textures.find(depth_buffer_default);
	assert(depthBufferIt != textures.end());
	attachments[1] = depthBufferIt->second.view;

	const VkFramebufferCreateInfo fb_info = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.renderPass = render_pass,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.width = width,
		.height = height,
		.layers = 1
	};

	VkResult err;
	uint32_t i;

	window_framebuffers = (VkFramebuffer*)malloc(swapchain_image_count * sizeof(VkFramebuffer));
	assert(window_framebuffers);

	for (i = 0; i < swapchain_image_count; i++) {
		attachments[0] = swapchain_buffers[i].view;
		err = vkCreateFramebuffer(device, &fb_info, nullptr, &window_framebuffers[i]);
		assert(!err);
	}
}

void VKRenderer::reshape() {
	// TODO: this crashes MoltenVK currently, need to fix
	#if defined(__APPLE__)
		return;
	#endif

	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	// new dimensions
	glfwGetWindowSize(Application::glfwWindow, (int32_t*)&width, (int32_t*)&height);

	//
	Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(width) + " x " + to_string(height));

	//
	auto frame_buffers_last = window_framebuffers;

	// reinit swapchain, renderpass and framebuffers
	initializeSwapChain();
	initializeRenderPass();
	initializeFrameBuffers();
	current_buffer = 0;

	// dispose old frame buffers
	for (auto i = 0; i < swapchain_image_count; i++) vkDestroyFramebuffer(device, frame_buffers_last[i], nullptr);
	delete [] frame_buffers_last;

	//
	Engine::getInstance()->reshape(width, height);
}

void VKRenderer::initializeFrame()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	// work around for AMD drivers not telling if window needs to be reshaped
	{
		int32_t currentWidth;
		int32_t currentHeight;
		glfwGetWindowSize(Application::glfwWindow, &currentWidth, &currentHeight);
		auto needsReshape = currentWidth > 0 && currentHeight > 0 && (currentWidth != width || currentHeight != height);
		if (needsReshape == true) reshape();
	}

	//
	Renderer::initializeFrame();

	//
	VkResult err;
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};

	err = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &image_acquired_semaphore);
	assert(!err);

	err = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &draw_complete_semaphore);
	assert(!err);

	// get the index of the next available swapchain image:
	err = fpAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_acquired_semaphore, (VkFence)0, &current_buffer);

	//
	if (err == VK_ERROR_OUT_OF_DATE_KHR) {
		// TODO: a.drewke
		//
		finishSetupCommandBuffers();
		for (auto i = 0; i < Engine::getThreadCount(); i++) endRenderPass(i, __LINE__);
		vkDestroySemaphore(device, image_acquired_semaphore, nullptr);
		vkDestroySemaphore(device, draw_complete_semaphore, nullptr);

		//
		reshape();

		// recreate semaphores
		err = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &image_acquired_semaphore);
		assert(!err);

		err = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &draw_complete_semaphore);
		assert(!err);
	} else
	if (err == VK_SUBOPTIMAL_KHR) {
		// demo->swapchain is not as optimal as it could be, but the platform's
		// presentation engine will still present the image correctly.
	} else {
		assert(!err);
	}

	//
	for (auto i = 0; i < contexts.size(); i++) {
		contexts[i].command_count.fill(0);
	}
}

void VKRenderer::finishFrame()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	//
	endDrawCommandsAllContexts();

	// flush command buffers
	for (auto& context: contexts) {
		finishPipeline(context.idx);
		for (auto& ubo: context.uniform_buffers) ubo.clear();
		context.program = nullptr;
		context.program_id = 0;
	}

	// transitition to present
	{
		prepareSetupCommandBuffer(0);
		VkImageMemoryBarrier image_memory_barrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = 0,
			.dstAccessMask = 0,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.srcQueueFamilyIndex = 0,
			.dstQueueFamilyIndex = 0,
			.image = swapchain_buffers[current_buffer].image,
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		vkCmdPipelineBarrier(
			contexts[0].setup_cmd_inuse,
			VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
			VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&image_memory_barrier
		);
		finishSetupCommandBuffer(0);
	}

	//
	VkPresentInfoKHR present = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &current_buffer
	};

	VkResult err;
	err = fpQueuePresentKHR(queue, &present);
	auto needsReshape = false;
	if (err == VK_ERROR_OUT_OF_DATE_KHR) {
		needsReshape = true;
	} else
	if (err == VK_SUBOPTIMAL_KHR) {
		// swapchain is not as optimal as it could be, but the platform's
		// presentation engine will still present the image correctly.
		needsReshape = true;
	} else {
		assert(!err);
	}

	//
	vkDestroySemaphore(device, image_acquired_semaphore, nullptr);
	vkDestroySemaphore(device, draw_complete_semaphore, nullptr);

	// remove marked vulkan resources
	delete_mutex.lock();
	for (auto& delete_buffer: delete_buffers) {
		vmaUnmapMemory(allocator, delete_buffer.allocation);
		vmaDestroyBuffer(allocator, delete_buffer.buffer, delete_buffer.allocation);
	}
	for (auto& delete_image: delete_images) vmaDestroyImage(allocator, delete_image.image, delete_image.allocation);
	delete_buffers.clear();
	delete_images.clear();
	delete_mutex.unlock();

	// reset desc index
	for (auto& programIt: programs) {
		for (auto i = 0; i < programIt.second.desc_idxs.size(); i++) programIt.second.desc_idxs[i] = 0;
	}

	//
	frame++;
}

bool VKRenderer::isBufferObjectsAvailable()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return true;
}

bool VKRenderer::isDepthTextureAvailable()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return true;
}

bool VKRenderer::isUsingProgramAttributeLocation()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return false;
}

bool VKRenderer::isSpecularMappingAvailable()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return true;
}

bool VKRenderer::isNormalMappingAvailable()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return true;
}

bool VKRenderer::isInstancedRenderingAvailable() {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return true;
}

bool VKRenderer::isPBRAvailable()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return true;
}

bool VKRenderer::isComputeShaderAvailable() {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return true;
}

bool VKRenderer::isGLCLAvailable() {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return false;
}

bool VKRenderer::isUsingShortIndices() {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return false;
}

bool VKRenderer::isGeometryShaderAvailable() {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return false;
}

int32_t VKRenderer::getTextureUnits()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return -1;
}

int VKRenderer::determineAlignment(const unordered_map<string, vector<string>>& structs, const vector<string>& uniforms) {
	StringTokenizer t;
	auto alignmentMax = 0;
	for (auto uniform: uniforms) {
		t.tokenize(uniform, "\t ;");
		string uniformType;
		string uniformName;
		auto isArray = false;
		auto arraySize = 1;
		if (t.hasMoreTokens() == true) uniformType = t.nextToken();
		while (t.hasMoreTokens() == true) uniformName = t.nextToken();
		if (uniformName.find('[') != -1 && uniformName.find(']') != -1) {
			uniformName = StringTools::substring(uniformName, 0, uniformName.find('['));
		}
		if (uniformType == "int") {
			uint32_t size = sizeof(int32_t);
			alignmentMax = Math::max(alignmentMax, size);
		} else
		if (uniformType == "float") {
			uint32_t size = sizeof(float);
			alignmentMax = Math::max(alignmentMax, size);
		} else
		if (uniformType == "vec2") {
			uint32_t size = sizeof(float) * 2;
			alignmentMax = Math::max(alignmentMax, size);
		} else
		if (uniformType == "vec3") {
			uint32_t size = sizeof(float) * 3;
			alignmentMax = Math::max(alignmentMax, size);
		} else
		if (uniformType == "vec4") {
			uint32_t size = sizeof(float) * 4;
			alignmentMax = Math::max(alignmentMax, size);
		} else
		if (uniformType == "mat3") {
			uint32_t size = sizeof(float) * 3;
			alignmentMax = Math::max(alignmentMax, size);
		} else
		if (uniformType == "mat4") {
			uint32_t size = sizeof(float) * 4;
			alignmentMax = Math::max(alignmentMax, size);
		} else
		if (uniformType == "sampler2D") {
			// no op
		} else {
			if (structs.find(uniformType) != structs.end()) {
				uint32_t size = determineAlignment(structs, structs.find(uniformType)->second);
				alignmentMax = Math::max(alignmentMax, size);
			} else {
				return false;
			}
		}
	}
	return alignmentMax;
}

int VKRenderer::align(int alignment, int offset) {
	auto alignRemainder = offset % alignment;
	return alignRemainder == 0?offset:offset + (alignment - alignRemainder);
}

bool VKRenderer::addToShaderUniformBufferObject(shader_type& shader, const unordered_map<string, string>& definitionValues, const unordered_map<string, vector<string>>& structs, const vector<string>& uniforms, const string& prefix, unordered_set<string>& uniformArrays, string& uniformsBlock) {
	StringTokenizer t;
	for (auto uniform: uniforms) {
		t.tokenize(uniform, "\t ;");
		string uniformType;
		string uniformName;
		auto isArray = false;
		auto arraySize = 1;
		if (t.hasMoreTokens() == true) uniformType = t.nextToken();
		while (t.hasMoreTokens() == true) uniformName = t.nextToken();
		if (uniformName.find('[') != -1 && uniformName.find(']') != -1) {
			isArray = true;
			auto arraySizeString = StringTools::substring(uniformName, uniformName.find('[') + 1, uniformName.find(']'));
			for (auto definitionValueIt: definitionValues) arraySizeString = StringTools::replace(arraySizeString, definitionValueIt.first, definitionValueIt.second);
			arraySize = Integer::parseInt(arraySizeString);
			uniformName = StringTools::substring(uniformName, 0, uniformName.find('['));
			uniformArrays.insert(uniformName);
		}
		if (uniformType == "int") {
			for (auto i = 0; i < arraySize; i++) {
				auto suffix = isArray == true?"[" + to_string(i) + "]":"";
				uint32_t size = sizeof(int32_t);
				auto position = align(size, shader.ubo_size);
				shader.uniforms[prefix + uniformName + suffix] = {.name = prefix + uniformName + suffix, .type = shader_type::uniform_type::UNIFORM, .position = position, .size = size, .texture_unit = -1};
				shader.ubo_size = position + size;
			}
		} else
		if (uniformType == "float") {
			for (auto i = 0; i < arraySize; i++) {
				auto suffix = isArray == true?"[" + to_string(i) + "]":"";
				uint32_t size = sizeof(float);
				auto position = align(size, shader.ubo_size);
				shader.uniforms[prefix + uniformName + suffix] = {.name = prefix + uniformName + suffix, .type = shader_type::uniform_type::UNIFORM, .position = position, .size = size, .texture_unit = -1};
				shader.ubo_size = position + size;
			}
		} else
		if (uniformType == "vec2") {
			for (auto i = 0; i < arraySize; i++) {
				auto suffix = isArray == true?"[" + to_string(i) + "]":"";
				uint32_t size = sizeof(float) * 2;
				auto position = align(sizeof(float) * 4, shader.ubo_size);
				shader.uniforms[prefix + uniformName + suffix] = {.name = prefix + uniformName + suffix, .type = shader_type::uniform_type::UNIFORM, .position = position, .size = size, .texture_unit = -1};
				shader.ubo_size = position + size;
			}
		} else
		if (uniformType == "vec3") {
			for (auto i = 0; i < arraySize; i++) {
				auto suffix = isArray == true?"[" + to_string(i) + "]":"";
				uint32_t size = sizeof(float) * 3;
				auto position = align(sizeof(float) * 4, shader.ubo_size);
				shader.uniforms[prefix + uniformName + suffix] = {.name = prefix + uniformName + suffix, .type = shader_type::uniform_type::UNIFORM, .position = position, .size = size, .texture_unit = -1};
				shader.ubo_size = position + size;
			}
		} else
		if (uniformType == "vec4") {
			for (auto i = 0; i < arraySize; i++) {
				auto suffix = isArray == true?"[" + to_string(i) + "]":"";
				uint32_t size = sizeof(float) * 4;
				auto position = align(size, shader.ubo_size);
				shader.uniforms[prefix + uniformName + suffix] = {.name = prefix + uniformName + suffix, .type = shader_type::uniform_type::UNIFORM, .position = position, .size = size, .texture_unit = -1};
				shader.ubo_size = position + size;
			}
		} else
		if (uniformType == "mat3") {
			for (auto i = 0; i < arraySize; i++) {
				auto suffix = isArray == true?"[" + to_string(i) + "]":"";
				uint32_t size = sizeof(float) * 12;
				auto position = align(sizeof(float) * 4, shader.ubo_size);
				shader.uniforms[prefix + uniformName + suffix] = {.name = prefix + uniformName + suffix, .type = shader_type::uniform_type::UNIFORM, .position = position, .size = size, .texture_unit = -1};
				shader.ubo_size = position + size;
			}
		} else
		if (uniformType == "mat4") {
			for (auto i = 0; i < arraySize; i++) {
				auto suffix = isArray == true?"[" + to_string(i) + "]":"";
				uint32_t size = sizeof(float) * 16;
				auto position = align(sizeof(float) * 4, shader.ubo_size);
				shader.uniforms[prefix + uniformName + suffix] = {.name = prefix + uniformName + suffix, .type = shader_type::uniform_type::UNIFORM, .position = position, .size = size, .texture_unit = -1};
				shader.ubo_size = position + size;
			}
		} else
		if (uniformType == "sampler2D") {
			shader.uniforms[uniformName] = {.name = uniformName, .type = shader_type::uniform_type::SAMPLER2D, .position = -1, .size = 0, .texture_unit = -1};
			continue;
		} else {
			if (structs.find(uniformType) != structs.end()) {
				for (auto i = 0; i < arraySize; i++) {
					auto structPrefix = prefix + uniformName + (isArray == true?"[" + to_string(i) + "]":"") + ".";
					string uniformsBlockIgnore;
					auto alignment = determineAlignment(structs, structs.find(uniformType)->second);
					shader.ubo_size = align(alignment, shader.ubo_size);
					auto success = addToShaderUniformBufferObject(shader, definitionValues, structs, structs.find(uniformType)->second, structPrefix, uniformArrays, uniformsBlockIgnore);
					shader.ubo_size = align(alignment, shader.ubo_size);
					if (success == false) return false;
				}
			} else {
				Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Unknown uniform type: " + uniformType);
				shaders.erase(shader.id);
				return false;
			}
		}
		uniformsBlock+= uniform + "\n";
	}
	return true;
}

int32_t VKRenderer::loadShader(int32_t type, const string& pathName, const string& fileName, const string& definitions, const string& functions)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): INIT: " + pathName + "/" + fileName + ": " + definitions);

	auto& shaderStruct = shaders[shader_idx];
	shaderStruct.type = (VkShaderStageFlagBits)type;
	shaderStruct.id = shader_idx++;
	shaderStruct.file = pathName + "/" + fileName;

	// shader source
	auto shaderSource = StringTools::replace(
		StringTools::replace(
			FileSystem::getInstance()->getContentAsString(pathName, fileName),
			"{$DEFINITIONS}",
			definitions + "\n\n"
		),
		"{$FUNCTIONS}",
		functions + "\n\n"
	);

	// do some shader adjustments
	{
		// pre parse shader code
		vector<string> newShaderSourceLines;
		vector<string> uniforms;
		shaderSource = StringTools::replace(shaderSource, "\r", "");
		shaderSource = StringTools::replace(shaderSource, "\t", " ");
		shaderSource = StringTools::replace(shaderSource, "#version 330", "#version 430\n#extension GL_EXT_scalar_block_layout: require\n\n");
		shaderSource = StringTools::replace(shaderSource, "#version 430 core", "#version 430\n#extension GL_EXT_scalar_block_layout: require\n\n");
		StringTokenizer t;
		t.tokenize(shaderSource, "\n");
		StringTokenizer t2;
		vector<string> definitions;
		unordered_map<string, string> definitionValues;
		unordered_map<string, vector<string>> structs;
		string currentStruct;
		stack<string> testedDefinitions;
		vector<bool> matchedDefinitions;
		vector<bool> hadMatchedDefinitions;
		auto inLocationCount = 0;
		auto outLocationCount = 0;
		auto uboUniformCount = 0;
		auto multiLineComment = false;
		while (t.hasMoreTokens() == true) {
			auto matchedAllDefinitions = true;
			for (auto matchedDefinition: matchedDefinitions) matchedAllDefinitions&= matchedDefinition;
			auto line = StringTools::trim(t.nextToken());
			if (StringTools::startsWith(line, "//") == true) continue;
			auto position = -1;
			if (StringTools::startsWith(line, "/*") == true) {
				multiLineComment = true;
			} else
			if (multiLineComment == true) {
				if (StringTools::endsWith(line, "*/") == true) multiLineComment = false;
			} else
			// TODO: a.drewke, #elif
			if (StringTools::startsWith(line, "#if defined(") == true) {
				auto definition = StringTools::trim(StringTools::substring(line, string("#if defined(").size(), (position = line.find(")")) != -1?position:line.size()));
				if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have preprocessor test begin: " + definition);
				testedDefinitions.push(definition);
				bool matched = false;
				for (auto availableDefinition: definitions) {
					if (definition == availableDefinition) {
						matched = true;
						break;
					}
				}
				if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have preprocessor test begin: " + definition + ": " + to_string(matched));
				matchedDefinitions.push_back(matched);
				hadMatchedDefinitions.push_back(matched);
				newShaderSourceLines.push_back("// " + line);
			} else
			if (StringTools::startsWith(line, "#elif defined(") == true) {
				// remove old test from stack
				if (testedDefinitions.size() == 0) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have preprocessor else end: invalid depth"); else {
					testedDefinitions.pop();
					matchedDefinitions.pop_back();
				}
				newShaderSourceLines.push_back("// " + line);
				// do new test
				auto definition = StringTools::trim(StringTools::substring(line, string("#elif defined(").size(), (position = line.find(")")) != -1?position:line.size()));
				if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have preprocessor test else if: " + definition);
				testedDefinitions.push(definition);
				bool matched = false;
				for (auto availableDefinition: definitions) {
					if (definition == availableDefinition) {
						matched = true;
						break;
					}
				}
				if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have preprocessor else test begin: " + definition + ": " + to_string(matched));
				matchedDefinitions.push_back(matched);
				if (matched == true) hadMatchedDefinitions[hadMatchedDefinitions.size() - 1] = matched;
			} else
			if (StringTools::startsWith(line, "#define ") == true) {
				auto definition = StringTools::trim(StringTools::substring(line, string("#define ").size()));
				if (definition.find(' ') != -1 || definition.find('\t') != -1) {
					t2.tokenize(definition, "\t ");
					definition = t2.nextToken();
					string value;
					while (t2.hasMoreTokens() == true) value+= t2.nextToken();
					definitionValues[definition] = value;
					newShaderSourceLines.push_back((matchedAllDefinitions == true?"":"// ") + line);
					if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have define with value: " + definition + " --> " + value);
				} else {
					definitions.push_back(definition);
					newShaderSourceLines.push_back((matchedAllDefinitions == true?"":"// ") + line);
					if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have define: " + definition);
				}
			} else
			if (StringTools::startsWith(line, "#else") == true) {
				if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have preprocessor else: " + line);
				matchedDefinitions[matchedDefinitions.size() - 1] = !matchedDefinitions[matchedDefinitions.size() - 1] && hadMatchedDefinitions[matchedDefinitions.size() - 1] == false;
				newShaderSourceLines.push_back("// " + line);
				matchedAllDefinitions = true;
				for (auto matchedDefinition: matchedDefinitions) matchedAllDefinitions&= matchedDefinition;
			} else
			if (StringTools::startsWith(line, "#endif") == true) {
				if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have preprocessor test end: " + line);
				if (testedDefinitions.size() == 0) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have preprocessor test end: invalid depth"); else {
					testedDefinitions.pop();
					matchedDefinitions.pop_back();
					hadMatchedDefinitions.pop_back();
				}
				newShaderSourceLines.push_back("// " + line);
			} else
			if (matchedAllDefinitions == true) {
				if (StringTools::startsWith(line, "struct ") == true) {
					currentStruct = StringTools::trim(StringTools::substring(line, string("struct ").size(), line.find('{')));
				} else
				if (currentStruct.size() > 0) {
					if (line == "};") {
						currentStruct.clear();
					} else {
						structs[currentStruct].push_back(line);
					}
				} else
				if (currentStruct.size() > 0) {

				}
				if ((StringTools::startsWith(line, "uniform ")) == true) {
					string uniform;
					if (line.find("sampler2D") != -1) {
						uniform = StringTools::substring(line, string("uniform").size() + 1);
						t2.tokenize(uniform, "\t ;");
						string uniformType;
						string uniformName;
						if (t2.hasMoreTokens() == true) uniformType = t2.nextToken();
						while (t2.hasMoreTokens() == true) uniformName = t2.nextToken();
						newShaderSourceLines.push_back("layout(binding = {$SAMPLER2D_BINDING_" + uniformName + "_IDX}) " + line);
						shaderStruct.samplers++;
					} else {
						uniform = StringTools::substring(line, string("uniform").size() + 1);
						uboUniformCount++;
					}
					uniforms.push_back(uniform);
					newShaderSourceLines.push_back("// " + line);
					if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have uniform: " + uniform);
				} else
				if (StringTools::startsWith(line, "out ") == true || StringTools::startsWith(line, "flat out ") == true) {
					newShaderSourceLines.push_back("layout (location = " + to_string(outLocationCount) + ") " + line);
					if (VERBOSE == true) Console::println("layout (location = " + to_string(outLocationCount) + ") " + line);
					outLocationCount++;
				} else
				if (StringTools::startsWith(line, "in ") == true || StringTools::startsWith(line, "flat in ") == true) {
					newShaderSourceLines.push_back("layout (location = " + to_string(inLocationCount) + ") " + line);
					if (VERBOSE == true) Console::println("layout (location = " + to_string(inLocationCount) + ") " + line);
					inLocationCount++;
				} else
				if (StringTools::startsWith(line, "layout") == true && line.find("binding=") != -1) {
					if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Have layout with binding: " + line);
					t2.tokenize(line, "(,)= \t");
					while (t2.hasMoreTokens() == true) {
						auto token = t2.nextToken();
						if (token == "binding" && t2.hasMoreTokens() == true) {
							auto nextToken = t2.nextToken();
							shaderStruct.binding_max = Math::max(Integer::parseInt(nextToken), shaderStruct.binding_max);
							break;
						}
					}
					newShaderSourceLines.push_back(line);
				} else {
					newShaderSourceLines.push_back(line);
				}
			} else {
				newShaderSourceLines.push_back("// " + line);
			}
		}

		// generate new uniform block
		unordered_set<string> uniformArrays;
		string uniformsBlock = "";

		// replace uniforms to use ubo
		if (uniforms.size() > 0) {
			if (uboUniformCount > 0) {
				uniformsBlock+= "layout(std430, column_major, binding={$UBO_BINDING_IDX}) uniform UniformBufferObject\n";
				uniformsBlock+= "{\n";
			}
			string uniformsBlockIgnore;
			addToShaderUniformBufferObject(shaderStruct, definitionValues, structs, uniforms, "", uniformArrays, uboUniformCount > 0?uniformsBlock:uniformsBlockIgnore);
			if (uboUniformCount > 0) uniformsBlock+= "} ubo_generated;\n";
			if (VERBOSE == true) Console::println("Shader UBO size: " + to_string(shaderStruct.ubo_size));
		}

		// construct new shader from vector and flip y, also inject uniforms
		shaderSource.clear();
		auto injectedUniformsAt = -1;
		auto injectedYFlip = false;
		// inject uniform before first method
		for (auto i = 0; i < newShaderSourceLines.size(); i++) {
			auto line = newShaderSourceLines[i];
			if (line.find('(') != -1 && line.find(')') != -1 && StringTools::startsWith(line, "layout") == false && line.find("defined(") == -1 && line.find("#define") == -1) {
				injectedUniformsAt = i;
				break;
			}
		}
		for (int i = newShaderSourceLines.size() - 1; i >= 0; i--) {
			auto line = newShaderSourceLines[i] + "\n";
			if (i == injectedUniformsAt) {
				shaderSource = uniformsBlock + line + shaderSource;
			} else
			if (StringTools::startsWith(line, "//") == true) {
				shaderSource = line + shaderSource;
				// rename uniforms to ubo uniforms
			} else {
				for (auto& uniformIt: shaderStruct.uniforms) {
					if (uniformIt.second.type == shader_type::uniform_type::SAMPLER2D) continue;
					auto uniformName = uniformIt.second.name;
					line = StringTools::regexReplace(
						line,
						"(\\b)" + uniformName + "(\\b)",
						"$1ubo_generated." + uniformName + "$2"
					);
				}
				// rename arrays to ubo uniforms
				for (auto& uniformName: uniformArrays) {
					line = StringTools::regexReplace(
						line,
						"(\\b)" + uniformName + "(\\b)",
						"$1ubo_generated." + uniformName + "$2"
					);
				}
				// inject gl_Position flip before last } from main
				if (type == SHADER_VERTEX_SHADER && injectedYFlip == false && StringTools::startsWith(line, "}") == true) {
					shaderSource =
						"gl_Position.y*= -1.0;\n" +
						line +
						shaderSource;
					injectedYFlip = true;
				} else  {
					shaderSource = line + shaderSource;
				}
			}
		}

		// debug uniforms
		for (auto& uniformIt: shaderStruct.uniforms) {
			if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Uniform: " + uniformIt.second.name + ": " + to_string(uniformIt.second.position) + " / " + to_string(uniformIt.second.size));
		}
	}

	shaderStruct.definitions = definitions;
	shaderStruct.source = shaderSource;

    //
	return shaderStruct.id;
}

inline void VKRenderer::preparePipeline(int contextIdx, program_type* program) {
	auto& context = contexts[contextIdx];
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(contextIdx) + ": " + to_string(context.program_id));
	if (program->uniform_buffers_stored[context.idx] == true) {
		context.uniform_buffers = program->uniform_buffers_last[context.idx];
		context.uniform_buffers_changed = program->uniform_buffers_changed_last[context.idx];
	} else {
		auto shaderIdx = 0;
		for (auto shader: program->shaders) {
			if (shader->ubo_binding_idx == -1) {
				context.uniform_buffers[shaderIdx].resize(0);
				context.uniform_buffers_changed[shaderIdx] = false;
				shaderIdx++;
				continue;
			}
			context.uniform_buffers[shaderIdx].resize(shader->ubo_size);
			context.uniform_buffers_changed[shaderIdx] = true;
			shaderIdx++;
		}
	}
}

inline void VKRenderer::finishPipeline(int contextIdx) {
	auto& context = contexts[contextIdx];
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(contextIdx) + ": " + to_string(context.program_id));

	// TODO: check if pipeline was bound maybe
	if (context.program != nullptr) {
		context.program->uniform_buffers_stored[contextIdx] = true;
		context.program->uniform_buffers_last[contextIdx] = context.uniform_buffers;
		context.program->uniform_buffers_changed_last[contextIdx] = context.uniform_buffers_changed;
	}

	//
	context.pipeline_id.fill(string());
	context.pipeline.fill(VK_NULL_HANDLE);
}

inline void VKRenderer::createRasterizationStateCreateInfo(int contextIdx, VkPipelineRasterizationStateCreateInfo& rs) {
	memset(&rs, 0, sizeof(rs));
	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.polygonMode = VK_POLYGON_MODE_FILL;
	rs.cullMode = contexts[contextIdx].culling_enabled == true?cull_mode:VK_CULL_MODE_NONE;
	rs.frontFace = (VkFrontFace)(contexts[contextIdx].front_face - 1);
	rs.depthClampEnable = VK_FALSE;
	rs.rasterizerDiscardEnable = VK_FALSE;
	rs.depthBiasEnable = VK_FALSE;
	rs.lineWidth = 1.0f;
}

inline void VKRenderer::createColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& att_state) {
	memset(&att_state, 0, sizeof(att_state));
	att_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	att_state.blendEnable = blending_enabled = true?VK_TRUE:VK_FALSE;
	att_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	att_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	att_state.colorBlendOp = VK_BLEND_OP_ADD;
	att_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	att_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	att_state.alphaBlendOp = VK_BLEND_OP_ADD;
}

inline void VKRenderer::createDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo& ds) {
	memset(&ds, 0, sizeof(ds));
	ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	ds.depthTestEnable = depth_buffer_testing == true?VK_TRUE:VK_FALSE;
	ds.depthWriteEnable = depth_buffer_writing == true?VK_TRUE:VK_FALSE;
	ds.depthCompareOp = (VkCompareOp)depth_function;
	ds.back.failOp = VK_STENCIL_OP_KEEP;
	ds.back.passOp = VK_STENCIL_OP_KEEP;
	ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
	ds.stencilTestEnable = VK_FALSE;
	ds.front = ds.back;
	ds.depthBoundsTestEnable = VK_FALSE;
	ds.minDepthBounds = 0.0f;
	ds.maxDepthBounds = 1.0f;
}

inline const string VKRenderer::createPipelineId(int contextIdx) {
	string result;
	result.reserve(
		sizeof(contexts[contextIdx].front_face_index) +
		sizeof(cull_mode) +
		sizeof(blending_enabled) +
		sizeof(depth_buffer_testing) +
		sizeof(depth_buffer_writing) +
		sizeof(depth_function) +
		sizeof(bound_frame_buffer)
	);
	result.append((char*)&contexts[contextIdx].front_face_index, sizeof(contexts[contextIdx].front_face_index));
	result.append((char*)&cull_mode, sizeof(cull_mode));
	result.append((char*)&blending_enabled, sizeof(blending_enabled));
	result.append((char*)&depth_buffer_testing, sizeof(depth_buffer_testing));
	result.append((char*)&depth_buffer_writing, sizeof(depth_buffer_writing));
	result.append((char*)&depth_function, sizeof(depth_function));
	result.append((char*)&bound_frame_buffer, sizeof(bound_frame_buffer));
	return result;
}

void VKRenderer::createObjectsRenderingProgram(program_type* program) {
	VkResult err;

	//
	VkDescriptorSetLayoutBinding layout_bindings[program->layout_bindings];
	memset(layout_bindings, 0, sizeof(layout_bindings));

	// ubos, samplers
	auto shaderIdx = 0;
	for (auto shader: program->shaders) {
		if (shader->ubo_binding_idx != -1) {
			layout_bindings[shader->ubo_binding_idx] = {
				.binding = static_cast<uint32_t>(shader->ubo_binding_idx),
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = shader->type,
				.pImmutableSamplers = nullptr
			};
		}
		for (auto uniformIt: shader->uniforms) {
			auto& uniform = uniformIt.second;
			if (uniform.type == shader_type::uniform_type::SAMPLER2D) {
				layout_bindings[uniform.position] = {
					.binding = static_cast<uint32_t>(uniform.position),
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = shader->type,
					.pImmutableSamplers = nullptr
				};
			}
		}
		shaderIdx++;
	}

	const VkDescriptorSetLayoutCreateInfo descriptor_layout = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = program->layout_bindings,
		.pBindings = layout_bindings,
	};

	err = vkCreateDescriptorSetLayout(device, &descriptor_layout, nullptr, &program->desc_layout);
	assert(!err);

	const VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = 1,
		.pSetLayouts = &program->desc_layout,
	};

	VkDescriptorSetLayout desc_layouts[DESC_MAX];
	for (auto i = 0; i < DESC_MAX; i++) desc_layouts[i] = program->desc_layout;

	//
	VkDescriptorSetAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = desc_pool,
		.descriptorSetCount = DESC_MAX,
		.pSetLayouts = desc_layouts
	};
	for (auto& context: contexts) {
		err = vkAllocateDescriptorSets(device, &alloc_info, program->desc_sets[context.idx].data());
		assert(!err);
	}

	//
	err = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &program->pipeline_layout);
	assert(!err);
}

void VKRenderer::createObjectsRenderingPipeline(int contextIdx, program_type* program) {
	auto& context = contexts[contextIdx];
	auto pipelinesIt = program->pipelines.find(context.pipeline_id[context.front_face_index]);
	if (pipelinesIt == program->pipelines.end()) {
		auto& programPipeline = program->pipelines[context.pipeline_id[context.front_face_index]];

		VkRenderPass renderPass = render_pass;
		auto haveDepthBuffer = true;
		auto haveColorBuffer = true;
		if (bound_frame_buffer != 0) {
			auto frameBufferIt = framebuffers.find(bound_frame_buffer);
			auto& frameBufferStruct = frameBufferIt->second;
			haveDepthBuffer = frameBufferStruct.depth_texture_id != 0;
			haveColorBuffer = frameBufferStruct.color_texture_id != 0;
			renderPass = frameBufferStruct.render_pass;
		}

		//
		VkResult err;

		//
		VkGraphicsPipelineCreateInfo pipeline;
		memset(&pipeline, 0, sizeof(pipeline));

		// create pipepine
		VkPipelineCacheCreateInfo pipelineCache;

		VkPipelineVertexInputStateCreateInfo vi;
		VkPipelineInputAssemblyStateCreateInfo ia;
		VkPipelineRasterizationStateCreateInfo rs;
		VkPipelineColorBlendStateCreateInfo cb;
		VkPipelineDepthStencilStateCreateInfo ds;
		VkPipelineViewportStateCreateInfo vp;
		VkPipelineMultisampleStateCreateInfo ms;
		VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
		VkPipelineDynamicStateCreateInfo dynamicState;

		createRasterizationStateCreateInfo(contextIdx, rs);
		createDepthStencilStateCreateInfo(ds);

		VkPipelineShaderStageCreateInfo shaderStages[program->shaders.size()];
		memset(shaderStages, 0, program->shaders.size() * sizeof(VkPipelineShaderStageCreateInfo));

		// shader stages
		auto shaderIdx = 0;
		for (auto shader: program->shaders) {
			shaderStages[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[shaderIdx].stage = shader->type;
			shaderStages[shaderIdx].module = shader->module;
			shaderStages[shaderIdx].pName = "main";
			shaderIdx++;
		}

		memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
		memset(&dynamicState, 0, sizeof dynamicState);
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pDynamicStates = dynamicStateEnables;

		pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline.stageCount = program->shaders.size();
		pipeline.layout = program->pipeline_layout;

		memset(&ia, 0, sizeof(ia));
		ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkPipelineColorBlendAttachmentState att_state;
		createColorBlendAttachmentState(att_state);

		memset(&cb, 0, sizeof(cb));
		cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		cb.logicOpEnable = VK_FALSE;
		cb.attachmentCount = haveColorBuffer == true?1:0;
		cb.pAttachments = haveColorBuffer == true?&att_state:nullptr;

		memset(&vp, 0, sizeof(vp));
		vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vp.viewportCount = 1;
		dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
		vp.scissorCount = 1;
		dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;

		memset(&ms, 0, sizeof(ms));
		ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		ms.pSampleMask = nullptr;
		ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkVertexInputBindingDescription vi_bindings[10];
		memset(vi_bindings, 0, sizeof(vi_bindings));
		VkVertexInputAttributeDescription vi_attrs[13];
		memset(vi_attrs, 0, sizeof(vi_attrs));

		// vertices
		vi_bindings[0].binding = 0;
		vi_bindings[0].stride = sizeof(float) * 3;
		vi_bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[0].binding = 0;
		vi_attrs[0].location = 0;
		vi_attrs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vi_attrs[0].offset = 0;

		// normals
		vi_bindings[1].binding = 1;
		vi_bindings[1].stride = sizeof(float) * 3;
		vi_bindings[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[1].binding = 1;
		vi_attrs[1].location = 1;
		vi_attrs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vi_attrs[1].offset = 0;

		// texture coordinates
		vi_bindings[2].binding = 2;
		vi_bindings[2].stride = sizeof(float) * 2;
		vi_bindings[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[2].binding = 2;
		vi_attrs[2].location = 2;
		vi_attrs[2].format = VK_FORMAT_R32G32_SFLOAT;
		vi_attrs[2].offset = 0;

		// colors
		vi_bindings[3].binding = 3;
		vi_bindings[3].stride = sizeof(float) * 4;
		vi_bindings[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[3].binding = 3;
		vi_attrs[3].location = 3;
		vi_attrs[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vi_attrs[3].offset = 0;

		// tangents
		vi_bindings[4].binding = 4;
		vi_bindings[4].stride = sizeof(float) * 3;
		vi_bindings[4].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[4].binding = 4;
		vi_attrs[4].location = 4;
		vi_attrs[4].format = VK_FORMAT_R32G32B32_SFLOAT;
		vi_attrs[4].offset = 0;

		// bitangents
		vi_bindings[5].binding = 5;
		vi_bindings[5].stride = sizeof(float) * 3;
		vi_bindings[5].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[5].binding = 5;
		vi_attrs[5].location = 5;
		vi_attrs[5].format = VK_FORMAT_R32G32B32_SFLOAT;
		vi_attrs[5].offset = 0;

		// model matrices 1
		vi_bindings[6].binding = 6;
		vi_bindings[6].stride = sizeof(float) * 4 * 4;
		vi_bindings[6].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		vi_attrs[6].binding = 6;
		vi_attrs[6].location = 6;
		vi_attrs[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vi_attrs[6].offset = sizeof(float) * 4 * 0;

		// model matrices 2
		vi_attrs[7].binding = 6;
		vi_attrs[7].location = 7;
		vi_attrs[7].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vi_attrs[7].offset = sizeof(float) * 4 * 1;

		// model matrices 3
		vi_attrs[8].binding = 6;
		vi_attrs[8].location = 8;
		vi_attrs[8].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vi_attrs[8].offset = sizeof(float) * 4 * 2;

		// model matrices 4
		vi_attrs[9].binding = 6;
		vi_attrs[9].location = 9;
		vi_attrs[9].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vi_attrs[9].offset = sizeof(float) * 4 * 3;

		// effect color mul
		vi_bindings[7].binding = 7;
		vi_bindings[7].stride = sizeof(float) * 4;
		vi_bindings[7].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		vi_attrs[10].binding = 7;
		vi_attrs[10].location = 10;
		vi_attrs[10].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vi_attrs[10].offset = 0;

		// effect color add
		vi_bindings[8].binding = 8;
		vi_bindings[8].stride = sizeof(float) * 4;
		vi_bindings[8].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		vi_attrs[11].binding = 8;
		vi_attrs[11].location = 11;
		vi_attrs[11].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vi_attrs[11].offset = 0;

		// vertices
		vi_bindings[9].binding = 9;
		vi_bindings[9].stride = sizeof(float) * 3;
		vi_bindings[9].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[12].binding = 9;
		vi_attrs[12].location = 12;
		vi_attrs[12].format = VK_FORMAT_R32G32B32_SFLOAT;
		vi_attrs[12].offset = 0;

		memset(&vi, 0, sizeof(vi));
		vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vi.pNext = nullptr;
		vi.vertexBindingDescriptionCount = 10;
		vi.pVertexBindingDescriptions = vi_bindings;
		vi.vertexAttributeDescriptionCount = 13;
		vi.pVertexAttributeDescriptions = vi_attrs;

		pipeline.pVertexInputState = &vi;
		pipeline.pInputAssemblyState = &ia;
		pipeline.pRasterizationState = &rs;
		pipeline.pColorBlendState = haveColorBuffer == true?&cb:nullptr;
		pipeline.pMultisampleState = &ms;
		pipeline.pViewportState = &vp;
		pipeline.pDepthStencilState = haveDepthBuffer == true?&ds:nullptr;
		pipeline.pStages = shaderStages;
		pipeline.renderPass = renderPass;
		pipeline.pDynamicState = &dynamicState;

		memset(&pipelineCache, 0, sizeof(pipelineCache));
		pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		err = vkCreatePipelineCache(device, &pipelineCache, nullptr, &programPipeline.pipelineCache);
		assert(!err);

		err = vkCreateGraphicsPipelines(device, programPipeline.pipelineCache, 1, &pipeline, nullptr, &programPipeline.pipeline);
		assert(!err);

		vkDestroyPipelineCache(device, programPipeline.pipelineCache, nullptr);
	}

}

inline void VKRenderer::setupObjectsRenderingPipeline(int contextIdx, program_type* program) {
	auto& context = contexts[contextIdx];
	if (context.pipeline_id[context.front_face_index].empty() == true || context.pipeline[context.front_face_index] == VK_NULL_HANDLE) {
		if (context.pipeline_id[context.front_face_index].empty() == true) context.pipeline_id[context.front_face_index] = createPipelineId(contextIdx);
		pipeline_rwlock.readLock();
		auto pipelinesIt = program->pipelines.find(context.pipeline_id[context.front_face_index]);
		pipeline_rwlock.unlock();
		if (pipelinesIt == program->pipelines.end()) {
			pipeline_rwlock.writeLock();
			createObjectsRenderingPipeline(contextIdx, program);
			pipeline_rwlock.unlock();
		}

		//
		pipeline_rwlock.readLock();
		pipelinesIt = program->pipelines.find(context.pipeline_id[context.front_face_index]);
		auto newPipeline = pipelinesIt->second.pipeline;
		pipeline_rwlock.unlock();
		if (newPipeline != context.pipeline[context.front_face_index]) {
			vkCmdBindPipeline(context.draw_cmds[context.draw_cmd_current][context.front_face_index], VK_PIPELINE_BIND_POINT_GRAPHICS, newPipeline);
			vkCmdSetViewport(context.draw_cmds[context.draw_cmd_current][context.front_face_index], 0, 1, &viewport);
			vkCmdSetScissor(context.draw_cmds[context.draw_cmd_current][context.front_face_index], 0, 1, &scissor);
			context.pipeline[context.front_face_index] = newPipeline;
		}
	}
}

void VKRenderer::createPointsRenderingProgram(program_type* program) {
	VkResult err;

	//
	VkDescriptorSetLayoutBinding layout_bindings[program->layout_bindings];
	memset(layout_bindings, 0, sizeof(layout_bindings));

	// ubos, samplers
	auto shaderIdx = 0;
	for (auto shader: program->shaders) {
		if (shader->ubo_binding_idx != -1) {
			layout_bindings[shader->ubo_binding_idx] = {
				.binding = static_cast<uint32_t>(shader->ubo_binding_idx),
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = shader->type,
				.pImmutableSamplers = nullptr
			};
		}
		for (auto uniformIt: shader->uniforms) {
			auto& uniform = uniformIt.second;
			if (uniform.type == shader_type::uniform_type::SAMPLER2D) {
				layout_bindings[uniform.position] = {
					.binding = static_cast<uint32_t>(uniform.position),
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = shader->type,
					.pImmutableSamplers = nullptr
				};
			}
		}
		shaderIdx++;
	}

	const VkDescriptorSetLayoutCreateInfo descriptor_layout = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = program->layout_bindings,
		.pBindings = layout_bindings,
	};

	err = vkCreateDescriptorSetLayout(device, &descriptor_layout, nullptr, &program->desc_layout);
	assert(!err);

	const VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = 1,
		.pSetLayouts = &program->desc_layout,
	};

	VkDescriptorSetLayout desc_layouts[DESC_MAX];
	for (auto i = 0; i < DESC_MAX; i++) desc_layouts[i] = program->desc_layout;

	//
	VkDescriptorSetAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = desc_pool,
		.descriptorSetCount = DESC_MAX,
		.pSetLayouts = desc_layouts
	};
	for (auto& context: contexts) {
		err = vkAllocateDescriptorSets(device, &alloc_info, program->desc_sets[context.idx].data());
		assert(!err);
	}

	//
	err = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &program->pipeline_layout);
	assert(!err);
}

void VKRenderer::createPointsRenderingPipeline(int contextIdx, program_type* program) {
	auto& context = contexts[contextIdx];
	auto pipelinesIt = program->pipelines.find(context.pipeline_id[context.front_face_index]);
	if (pipelinesIt == program->pipelines.end()) {
		auto& programPipeline = program->pipelines[context.pipeline_id[context.front_face_index]];

		VkRenderPass renderPass = render_pass;
		auto haveDepthBuffer = true;
		auto haveColorBuffer = true;
		if (bound_frame_buffer != 0) {
			auto frameBufferIt = framebuffers.find(bound_frame_buffer);
			auto& frameBufferStruct = frameBufferIt->second;
			haveDepthBuffer = frameBufferStruct.depth_texture_id != 0;
			haveColorBuffer = frameBufferStruct.color_texture_id != 0;
			renderPass = frameBufferStruct.render_pass;
		}

		//
		VkResult err;

		//
		VkGraphicsPipelineCreateInfo pipeline;
		memset(&pipeline, 0, sizeof(pipeline));

		// Stages
		VkPipelineShaderStageCreateInfo shaderStages[program->shaders.size()];
		memset(shaderStages, 0, program->shaders.size() * sizeof(VkPipelineShaderStageCreateInfo));

		// shader stages
		auto shaderIdx = 0;
		for (auto shader: program->shaders) {
			shaderStages[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[shaderIdx].stage = shader->type;
			shaderStages[shaderIdx].module = shader->module;
			shaderStages[shaderIdx].pName = "main";
			shaderIdx++;
		}

		// create pipepine
		VkPipelineCacheCreateInfo pipelineCache;

		VkPipelineVertexInputStateCreateInfo vi;
		VkPipelineInputAssemblyStateCreateInfo ia;
		VkPipelineRasterizationStateCreateInfo rs;
		VkPipelineColorBlendStateCreateInfo cb;
		VkPipelineDepthStencilStateCreateInfo ds;
		VkPipelineViewportStateCreateInfo vp;
		VkPipelineMultisampleStateCreateInfo ms;
		VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
		VkPipelineDynamicStateCreateInfo dynamicState;

		createRasterizationStateCreateInfo(contextIdx, rs);
		createDepthStencilStateCreateInfo(ds);

		memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
		memset(&dynamicState, 0, sizeof dynamicState);
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pDynamicStates = dynamicStateEnables;

		pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline.stageCount = program->shaders.size();
		pipeline.layout = program->pipeline_layout;

		memset(&ia, 0, sizeof(ia));
		ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		ia.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

		VkPipelineColorBlendAttachmentState att_state;
		createColorBlendAttachmentState(att_state);

		memset(&cb, 0, sizeof(cb));
		cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		cb.logicOpEnable = VK_FALSE;
		cb.attachmentCount = haveColorBuffer == true?1:0;
		cb.pAttachments = haveColorBuffer == true?&att_state:nullptr;

		memset(&vp, 0, sizeof(vp));
		vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vp.viewportCount = 1;
		dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
		vp.scissorCount = 1;
		dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;

		memset(&ms, 0, sizeof(ms));
		ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		ms.pSampleMask = nullptr;
		ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkVertexInputBindingDescription vi_bindings[4];
		memset(vi_bindings, 0, sizeof(vi_bindings));
		VkVertexInputAttributeDescription vi_attrs[4];
		memset(vi_attrs, 0, sizeof(vi_attrs));

		// vertices
		vi_bindings[0].binding = 0;
		vi_bindings[0].stride = sizeof(float) * 3;
		vi_bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[0].binding = 0;
		vi_attrs[0].location = 0;
		vi_attrs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vi_attrs[0].offset = 0;

		// sprite indices
		vi_bindings[1].binding = 1;
		vi_bindings[1].stride = sizeof(uint16_t);
		vi_bindings[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[1].binding = 1;
		vi_attrs[1].location = 1;
		vi_attrs[1].format = VK_FORMAT_R16_UINT;
		vi_attrs[1].offset = 0;

		// texture coordinates
		vi_bindings[2].binding = 2;
		vi_bindings[2].stride = sizeof(float) * 2;
		vi_bindings[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[2].binding = 2;
		vi_attrs[2].location = 2;
		vi_attrs[2].format = VK_FORMAT_R32G32_SFLOAT;
		vi_attrs[2].offset = 0;

		// colors
		vi_bindings[3].binding = 3;
		vi_bindings[3].stride = sizeof(float) * 4;
		vi_bindings[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[3].binding = 3;
		vi_attrs[3].location = 3;
		vi_attrs[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vi_attrs[3].offset = 0;


		memset(&vi, 0, sizeof(vi));
		vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vi.pNext = nullptr;
		vi.vertexBindingDescriptionCount = 4;
		vi.pVertexBindingDescriptions = vi_bindings;
		vi.vertexAttributeDescriptionCount = 4;
		vi.pVertexAttributeDescriptions = vi_attrs;

		pipeline.pVertexInputState = &vi;
		pipeline.pInputAssemblyState = &ia;
		pipeline.pRasterizationState = &rs;
		pipeline.pColorBlendState = haveColorBuffer == true?&cb:nullptr;
		pipeline.pMultisampleState = &ms;
		pipeline.pViewportState = &vp;
		pipeline.pDepthStencilState = haveDepthBuffer == true?&ds:nullptr;
		pipeline.pStages = shaderStages;
		pipeline.renderPass = renderPass;
		pipeline.pDynamicState = &dynamicState;

		memset(&pipelineCache, 0, sizeof(pipelineCache));
		pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		err = vkCreatePipelineCache(device, &pipelineCache, nullptr, &programPipeline.pipelineCache);
		assert(!err);

		err = vkCreateGraphicsPipelines(device, programPipeline.pipelineCache, 1, &pipeline, nullptr, &programPipeline.pipeline);
		assert(!err);

		//
		vkDestroyPipelineCache(device, programPipeline.pipelineCache, nullptr);
	}
}

inline void VKRenderer::setupPointsRenderingPipeline(int contextIdx, program_type* program) {
	auto& context = contexts[contextIdx];
	if (context.pipeline_id[context.front_face_index].empty() == true || context.pipeline[context.front_face_index] == VK_NULL_HANDLE) {
		if (context.pipeline_id[context.front_face_index].empty() == true) context.pipeline_id[context.front_face_index] = createPipelineId(contextIdx);
		pipeline_rwlock.readLock();
		auto pipelinesIt = program->pipelines.find(context.pipeline_id[context.front_face_index]);
		pipeline_rwlock.unlock();
		if (pipelinesIt == program->pipelines.end()) {
			pipeline_rwlock.writeLock();
			createPointsRenderingPipeline(contextIdx, program);
			pipeline_rwlock.unlock();
		}

		//
		pipeline_rwlock.readLock();
		pipelinesIt = program->pipelines.find(context.pipeline_id[context.front_face_index]);
		pipeline_rwlock.unlock();
		auto newPipeline = pipelinesIt->second.pipeline;
		if (newPipeline != context.pipeline[context.front_face_index]) {
			vkCmdBindPipeline(context.draw_cmds[context.draw_cmd_current][context.front_face_index], VK_PIPELINE_BIND_POINT_GRAPHICS, newPipeline);
			vkCmdSetViewport(context.draw_cmds[context.draw_cmd_current][context.front_face_index], 0, 1, &viewport);
			vkCmdSetScissor(context.draw_cmds[context.draw_cmd_current][context.front_face_index], 0, 1, &scissor);
			context.pipeline[context.front_face_index] = newPipeline;
		}
	}
}

void VKRenderer::createLinesRenderingProgram(program_type* program) {
	VkResult err;

	//
	VkDescriptorSetLayoutBinding layout_bindings[program->layout_bindings];
	memset(layout_bindings, 0, sizeof(layout_bindings));

	// ubos, samplers
	auto shaderIdx = 0;
	for (auto shader: program->shaders) {
		if (shader->ubo_binding_idx != -1) {
			layout_bindings[shader->ubo_binding_idx] = {
				.binding = static_cast<uint32_t>(shader->ubo_binding_idx),
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = shader->type,
				.pImmutableSamplers = nullptr
			};
		}
		for (auto uniformIt: shader->uniforms) {
			auto& uniform = uniformIt.second;
			if (uniform.type == shader_type::uniform_type::SAMPLER2D) {
				layout_bindings[uniform.position] = {
					.binding = static_cast<uint32_t>(uniform.position),
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = shader->type,
					.pImmutableSamplers = nullptr
				};
			}
		}
		shaderIdx++;
	}

	const VkDescriptorSetLayoutCreateInfo descriptor_layout = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = program->layout_bindings,
		.pBindings = layout_bindings,
	};

	err = vkCreateDescriptorSetLayout(device, &descriptor_layout, nullptr, &program->desc_layout);
	assert(!err);

	const VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = 1,
		.pSetLayouts = &program->desc_layout,
	};

	VkDescriptorSetLayout desc_layouts[DESC_MAX];
	for (auto i = 0; i < DESC_MAX; i++) desc_layouts[i] = program->desc_layout;

	//
	VkDescriptorSetAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = desc_pool,
		.descriptorSetCount = DESC_MAX,
		.pSetLayouts = desc_layouts
	};
	for (auto& context: contexts) {
		err = vkAllocateDescriptorSets(device, &alloc_info, program->desc_sets[context.idx].data());
		assert(!err);
	}

	//
	err = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &program->pipeline_layout);
	assert(!err);
}

void VKRenderer::createLinesRenderingPipeline(int contextIdx, program_type* program) {
	auto& context = contexts[contextIdx];
	auto pipelinesIt = program->pipelines.find(context.pipeline_id[context.front_face_index]);
	if (pipelinesIt == program->pipelines.end()) {
		auto& programPipeline = program->pipelines[context.pipeline_id[context.front_face_index]];

		VkRenderPass renderPass = render_pass;
		auto haveDepthBuffer = true;
		auto haveColorBuffer = true;
		if (bound_frame_buffer != 0) {
			auto frameBufferIt = framebuffers.find(bound_frame_buffer);
			auto& frameBufferStruct = frameBufferIt->second;
			haveDepthBuffer = frameBufferStruct.depth_texture_id != 0;
			haveColorBuffer = frameBufferStruct.color_texture_id != 0;
			renderPass = frameBufferStruct.render_pass;
		}

		//
		VkResult err;

		//
		VkGraphicsPipelineCreateInfo pipeline;
		memset(&pipeline, 0, sizeof(pipeline));

		// Stages
		VkPipelineShaderStageCreateInfo shaderStages[program->shaders.size()];
		memset(shaderStages, 0, program->shaders.size() * sizeof(VkPipelineShaderStageCreateInfo));

		// shader stages
		auto shaderIdx = 0;
		for (auto shader: program->shaders) {
			shaderStages[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[shaderIdx].stage = shader->type;
			shaderStages[shaderIdx].module = shader->module;
			shaderStages[shaderIdx].pName = "main";
			shaderIdx++;
		}

		// create pipepine
		VkPipelineCacheCreateInfo pipelineCache;

		VkPipelineVertexInputStateCreateInfo vi;
		VkPipelineInputAssemblyStateCreateInfo ia;
		VkPipelineRasterizationStateCreateInfo rs;
		VkPipelineColorBlendStateCreateInfo cb;
		VkPipelineDepthStencilStateCreateInfo ds;
		VkPipelineViewportStateCreateInfo vp;
		VkPipelineMultisampleStateCreateInfo ms;
		VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
		VkPipelineDynamicStateCreateInfo dynamicState;

		createRasterizationStateCreateInfo(contextIdx, rs);
		createDepthStencilStateCreateInfo(ds);

		memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
		memset(&dynamicState, 0, sizeof dynamicState);
		dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_LINE_WIDTH;
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pDynamicStates = dynamicStateEnables;


		pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline.stageCount = program->shaders.size();
		pipeline.layout = program->pipeline_layout;

		memset(&ia, 0, sizeof(ia));
		ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		ia.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

		VkPipelineColorBlendAttachmentState att_state;
		createColorBlendAttachmentState(att_state);

		memset(&cb, 0, sizeof(cb));
		cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		cb.logicOpEnable = VK_FALSE;
		cb.attachmentCount = haveColorBuffer == true?1:0;
		cb.pAttachments = haveColorBuffer == true?&att_state:nullptr;

		memset(&vp, 0, sizeof(vp));
		vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vp.viewportCount = 1;
		dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
		vp.scissorCount = 1;
		dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;

		memset(&ms, 0, sizeof(ms));
		ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		ms.pSampleMask = nullptr;
		ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkVertexInputBindingDescription vi_bindings[4];
		memset(vi_bindings, 0, sizeof(vi_bindings));
		VkVertexInputAttributeDescription vi_attrs[4];
		memset(vi_attrs, 0, sizeof(vi_attrs));

		// vertices
		vi_bindings[0].binding = 0;
		vi_bindings[0].stride = sizeof(float) * 3;
		vi_bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[0].binding = 0;
		vi_attrs[0].location = 0;
		vi_attrs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vi_attrs[0].offset = 0;

		// normals
		vi_bindings[1].binding = 1;
		vi_bindings[1].stride = sizeof(float) * 3;
		vi_bindings[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[1].binding = 1;
		vi_attrs[1].location = 1;
		vi_attrs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vi_attrs[1].offset = 0;

		// texture coordinates
		vi_bindings[2].binding = 2;
		vi_bindings[2].stride = sizeof(float) * 2;
		vi_bindings[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[2].binding = 2;
		vi_attrs[2].location = 2;
		vi_attrs[2].format = VK_FORMAT_R32G32_SFLOAT;
		vi_attrs[2].offset = 0;

		// colors
		vi_bindings[3].binding = 3;
		vi_bindings[3].stride = sizeof(float) * 4;
		vi_bindings[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vi_attrs[3].binding = 3;
		vi_attrs[3].location = 3;
		vi_attrs[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vi_attrs[3].offset = 0;


		memset(&vi, 0, sizeof(vi));
		vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vi.pNext = nullptr;
		vi.vertexBindingDescriptionCount = 4;
		vi.pVertexBindingDescriptions = vi_bindings;
		vi.vertexAttributeDescriptionCount = 4;
		vi.pVertexAttributeDescriptions = vi_attrs;

		pipeline.pVertexInputState = &vi;
		pipeline.pInputAssemblyState = &ia;
		pipeline.pRasterizationState = &rs;
		pipeline.pColorBlendState = haveColorBuffer == true?&cb:nullptr;
		pipeline.pMultisampleState = &ms;
		pipeline.pViewportState = &vp;
		pipeline.pDepthStencilState = haveDepthBuffer == true?&ds:nullptr;
		pipeline.pStages = shaderStages;
		pipeline.renderPass = renderPass;
		pipeline.pDynamicState = &dynamicState;

		memset(&pipelineCache, 0, sizeof(pipelineCache));
		pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		err = vkCreatePipelineCache(device, &pipelineCache, nullptr, &programPipeline.pipelineCache);
		assert(!err);

		err = vkCreateGraphicsPipelines(device, programPipeline.pipelineCache, 1, &pipeline, nullptr, &programPipeline.pipeline);
		assert(!err);

		//
		vkDestroyPipelineCache(device, programPipeline.pipelineCache, nullptr);
	}
}

inline void VKRenderer::setupLinesRenderingPipeline(int contextIdx, program_type* program) {
	auto& context = contexts[contextIdx];
	if (context.pipeline_id[context.front_face_index].empty() == true || context.pipeline[context.front_face_index] == VK_NULL_HANDLE) {
		if (context.pipeline_id[context.front_face_index].empty() == true) context.pipeline_id[context.front_face_index] = createPipelineId(contextIdx);
		pipeline_rwlock.readLock();
		auto pipelinesIt = program->pipelines.find(context.pipeline_id[context.front_face_index]);
		pipeline_rwlock.unlock();
		if (pipelinesIt == program->pipelines.end()) {
			pipeline_rwlock.writeLock();
			createLinesRenderingPipeline(contextIdx, program);
			pipeline_rwlock.unlock();
		}

		//
		pipeline_rwlock.readLock();
		pipelinesIt = program->pipelines.find(context.pipeline_id[context.front_face_index]);
		pipeline_rwlock.unlock();
		auto newPipeline = pipelinesIt->second.pipeline;
		if (newPipeline != context.pipeline[context.front_face_index]) {
			vkCmdBindPipeline(context.draw_cmds[context.draw_cmd_current][context.front_face_index], VK_PIPELINE_BIND_POINT_GRAPHICS, newPipeline);
			vkCmdSetViewport(context.draw_cmds[context.draw_cmd_current][context.front_face_index], 0, 1, &viewport);
			vkCmdSetScissor(context.draw_cmds[context.draw_cmd_current][context.front_face_index], 0, 1, &scissor);
			context.pipeline[context.front_face_index] = newPipeline;
		}
	}
}

inline void VKRenderer::createSkinningComputingProgram(program_type* program) {
	VkResult err;

	auto& programPipeline = program->pipelines["default"];

	//
	VkDescriptorSetLayoutBinding layout_bindings[program->layout_bindings];
	memset(layout_bindings, 0, sizeof(layout_bindings));

	// Stages
	VkPipelineShaderStageCreateInfo shaderStages[program->shaders.size()];
	memset(shaderStages, 0, program->shaders.size() * sizeof(VkPipelineShaderStageCreateInfo));

	auto shaderIdx = 0;
	for (auto shader: program->shaders) {
		shaderStages[shaderIdx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[shaderIdx].stage = shader->type;
		shaderStages[shaderIdx].module = shader->module;
		shaderStages[shaderIdx].pName = "main";

		for (int i = 0; i <= shader->binding_max; i++) {
			layout_bindings[i] = {
				.binding = static_cast<uint32_t>(i),
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = shader->type,
				.pImmutableSamplers = nullptr
			};
		}

		if (shader->ubo_binding_idx != -1) {
			layout_bindings[shader->ubo_binding_idx] = {
				.binding = static_cast<uint32_t>(shader->ubo_binding_idx),
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = shader->type,
				.pImmutableSamplers = nullptr
			};
		}
		shaderIdx++;
	}
	const VkDescriptorSetLayoutCreateInfo descriptor_layout = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.bindingCount = program->layout_bindings,
		.pBindings = layout_bindings,
	};

	err = vkCreateDescriptorSetLayout(device, &descriptor_layout, nullptr, &program->desc_layout);
	assert(!err);

	const VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = 1,
		.pSetLayouts = &program->desc_layout,
	};

	VkDescriptorSetLayout desc_layouts[DESC_MAX];
	for (auto i = 0; i < DESC_MAX; i++) desc_layouts[i] = program->desc_layout;

	//
	VkDescriptorSetAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = desc_pool,
		.descriptorSetCount = DESC_MAX,
		.pSetLayouts = desc_layouts
	};
	for (auto& context: contexts) {
		err = vkAllocateDescriptorSets(device, &alloc_info, program->desc_sets[context.idx].data());
		assert(!err);
	}

	//
	err = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &program->pipeline_layout);
	assert(!err);

	// create pipepine
	VkPipelineCacheCreateInfo pipelineCache = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.initialDataSize = 0,
		.pInitialData = nullptr
	};

	err = vkCreatePipelineCache(device, &pipelineCache, nullptr, &programPipeline.pipelineCache);
	assert(!err);

	// create pipepine
	VkComputePipelineCreateInfo pipeline = {
		.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.stage = shaderStages[0],
		.layout = program->pipeline_layout,
		.basePipelineHandle = nullptr,
		.basePipelineIndex = 0

	};

	err = vkCreateComputePipelines(device, programPipeline.pipelineCache, 1, &pipeline, nullptr, &programPipeline.pipeline);
	assert(!err);

	vkDestroyPipelineCache(device, programPipeline.pipelineCache, nullptr);
}

inline void VKRenderer::createSkinningComputingPipeline(int contextIdx, program_type* program) {
}

inline void VKRenderer::setupSkinningComputingPipeline(int contextIdx, program_type* program) {
	auto& context = contexts[contextIdx];
	if (context.pipeline_id[context.front_face_index].empty() == true || context.pipeline[context.front_face_index] == VK_NULL_HANDLE) {
		if (context.pipeline_id[context.front_face_index].empty() == true) context.pipeline_id[context.front_face_index] = "default";
		pipeline_rwlock.readLock();
		auto pipelineIt = program->pipelines.find(context.pipeline_id[context.front_face_index]);
		pipeline_rwlock.unlock();
		if (pipelineIt == program->pipelines.end()) {
			pipeline_rwlock.writeLock();
			createSkinningComputingPipeline(contextIdx, program);
			pipeline_rwlock.unlock();
		}

		//
		pipeline_rwlock.readLock();
		auto newPipeline = program->pipelines.find(context.pipeline_id[context.front_face_index])->second.pipeline;
		pipeline_rwlock.unlock();
		if (newPipeline != context.pipeline[context.front_face_index]) {
			vkCmdBindPipeline(context.draw_cmds[context.draw_cmd_current][context.front_face_index], VK_PIPELINE_BIND_POINT_COMPUTE, newPipeline);
			context.pipeline[context.front_face_index] = newPipeline;
		}
	}
}

void VKRenderer::useProgram(void* context, int32_t programId)
{
	auto& contextTyped = *static_cast<context_type*>(context);

	//
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(contextTyped.idx) + ": " + to_string(programId));

	// if unsetting program flush command buffers
	if (contextTyped.program_id != 0) {
		endRenderPass(contextTyped.idx, __LINE__);
		auto currentBufferIdx = contextTyped.draw_cmd_current;
		auto commandBuffers = endDrawCommandBuffer(contextTyped.idx, -1, true);
		for (auto commandBufferIdx = 0; commandBufferIdx < commandBuffers.size(); commandBufferIdx++) {
			if (commandBuffers[commandBufferIdx] != VK_NULL_HANDLE) {
				submitDrawCommandBuffers(1, &commandBuffers[commandBufferIdx], contextTyped.draw_fences[currentBufferIdx][commandBufferIdx], false, false);
			}
		}
		finishPipeline(contextTyped.idx);
		for (auto& ubo: contextTyped.uniform_buffers) ubo.clear();
	}

	//
	contextTyped.program_id = 0;
	contextTyped.program = nullptr;
	if (programId == 0) return;

	//
	auto programIt = programs.find(programId);
	if (programIt == programs.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): program does not exist: " + to_string(programId));
		return;
	}

	//
	auto program = &programIt->second;
	preparePipeline(contextTyped.idx, program);
	contextTyped.program_id = programId;
	contextTyped.program = program;
}

int32_t VKRenderer::createProgram(int type)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	auto& programStruct = programs[program_idx];
	programStruct.type = type;
	programStruct.id = program_idx++;
	programStruct.uniform_buffers_stored.resize(Engine::getThreadCount());
	for (auto i = 0; i < programStruct.uniform_buffers_stored.size(); i++) programStruct.uniform_buffers_stored[i] = false;
	programStruct.uniform_buffers_last.resize(Engine::getThreadCount());
	programStruct.uniform_buffers_changed_last.resize(Engine::getThreadCount());
	programStruct.desc_sets.resize(Engine::getThreadCount());
	programStruct.desc_idxs.resize(Engine::getThreadCount());
	for (auto i = 0; i < programStruct.desc_idxs.size(); i++) programStruct.desc_idxs[i] = 0;
	return programStruct.id;
}

void VKRenderer::attachShaderToProgram(int32_t programId, int32_t shaderId)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	auto shaderIt = shaders.find(shaderId);
	if (shaderIt == shaders.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): shader does not exist");
		return;
	}
	auto programIt = programs.find(programId);
	if (programIt == programs.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): program does not exist");
		return;
	}
	programIt->second.shader_ids.push_back(shaderId);
	programIt->second.shaders.push_back(&shaderIt->second);
}

bool VKRenderer::linkProgram(int32_t programId)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(programId));
	auto programIt = programs.find(programId);
	if (programIt == programs.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): program does not exist");
		return false;
	}

	map<string, int32_t> uniformsByName;
	auto bindingIdx = 0;
	for (auto shader: programIt->second.shaders) {
		//
		bindingIdx = Math::max(shader->binding_max + 1, bindingIdx);
	}

	auto uniformIdx = 1;
	for (auto shader: programIt->second.shaders) {
		// do we need a uniform buffer object for this shader stage?
		if (shader->ubo_size > 0) {
			shader->ubo_ids.resize(Engine::getThreadCount());
			shader->ubo.resize(Engine::getThreadCount());
			for (auto& context: contexts) {
				shader->ubo_ids[context.idx] = createBufferObjects(1, false, false)[0];
				shader->ubo[context.idx] = getBufferObjectInternal(shader->ubo_ids[context.idx]);
			};
			// yep, inject UBO index
			shader->ubo_binding_idx = bindingIdx;
			shader->source = StringTools::replace(shader->source, "{$UBO_BINDING_IDX}", to_string(bindingIdx));
			bindingIdx++;
		}

	}

	// bind samplers, compile shaders
	for (auto shader: programIt->second.shaders) {
		auto shaderSamplerIdx = 0;

		//
		for (auto& uniformIt: shader->uniforms) {
			auto& uniform = uniformIt.second;
			//
			if (uniform.type == shader_type::uniform_type::SAMPLER2D) {
				shader->source = StringTools::replace(shader->source, "{$SAMPLER2D_BINDING_" + uniform.name + "_IDX}", to_string(bindingIdx));
				uniform.position = bindingIdx++;
			}
			uniformsByName[uniform.name] = uniformIdx++;
		}

		// compile shader
		EShLanguage stage = shaderFindLanguage(shader->type);
		glslang::TShader glslShader(stage);
		glslang::TProgram glslProgram;
		const char *shaderStrings[1];
		TBuiltInResource resources;
		shaderInitResources(resources);

		// Enable SPIR-V and Vulkan rules when parsing GLSL
		EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

		shaderStrings[0] = shader->source.c_str();
		glslShader.setStrings(shaderStrings, 1);

		if (!glslShader.parse(&resources, 100, false, messages)) {
			// be verbose
			Console::println(
				string(
					string("VKRenderer::") +
					string(__FUNCTION__) +
					string("[") +
					to_string(shader->id) +
					string("]") +
					string(": parsing failed: ") +
					glslShader.getInfoLog() + ": " +
					glslShader.getInfoDebugLog()
				 )
			);
			Console::println(shader->source);
			return false;
		}

		glslProgram.addShader(&glslShader);
		if (glslProgram.link(messages) == false) {
			// be verbose
			Console::println(
				string(
					string("VKRenderer::") +
					string(__FUNCTION__) +
					string("[") +
					to_string(shader->id) +
					string("]") +
					string(": linking failed: ") +
					glslShader.getInfoLog() + ": " +
					glslShader.getInfoDebugLog()
				)
			);
			Console::println(shader->source);
			return false;
		}

		/*
		Console::println("definitions: " + shader->definitions);
		Console::println("source:\n" + shader->source);
		*/

		glslang::GlslangToSpv(*glslProgram.getIntermediate(stage), shader->spirv);

		// create shader module
		{
			VkResult err;
			VkShaderModuleCreateInfo moduleCreateInfo;
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.pNext = nullptr;
			moduleCreateInfo.codeSize = shader->spirv.size() * sizeof(uint32_t);
			moduleCreateInfo.pCode = shader->spirv.data();
			moduleCreateInfo.flags = 0;
			err = vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shader->module);
			if (err == VK_SUCCESS) {
				if (VERBOSE == true) {
					Console::println(
						string(
							string("GL3Renderer::") +
							string(__FUNCTION__) +
							string("[") +
							to_string(shader->id) +
							string("]") +
							string(": SUCCESS")
						 )
					);
				}
			} else {
				Console::println(
					string(
						string("GL3Renderer::") +
						string(__FUNCTION__) +
						string("[") +
						to_string(shader->id) +
						string("]") +
						string(": FAILED")
					 )
				);
				Console::println(shader->source);
				return false;
			}
	    }
	}

	//
	for (auto& uniformIt: uniformsByName) {
		programIt->second.uniforms[uniformIt.second] = uniformIt.first;
	}

	// total bindings of program
	programIt->second.layout_bindings = bindingIdx;

	// create programs in terms of ubos and so on
	if (programIt->second.type == PROGRAM_OBJECTS) {
		createObjectsRenderingProgram(&programIt->second);
	} else
	if (programIt->second.type == PROGRAM_POINTS) {
		createPointsRenderingProgram(&programIt->second);
	} else
	if (programIt->second.type == PROGRAM_LINES) {
		createLinesRenderingProgram(&programIt->second);
	} else
	if (programIt->second.type == PROGRAM_COMPUTE) {
		createSkinningComputingProgram(&programIt->second);
	} else {
		Console::println(
			string("VKRenderer::") +
			string(__FUNCTION__) +
			string("[") +
			to_string(programId) +
			string("]") +
			string(": unknown program: ") +
			to_string(programIt->second.type)
		);
	}

	//
	return true;
}

int32_t VKRenderer::getProgramUniformLocation(int32_t programId, const string& name)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + name);
	auto programIt = programs.find(programId);
	if (programIt == programs.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): program does not exist");
		return -1;
	}
	for (auto& uniformIt: programIt->second.uniforms) {
		if (uniformIt.second == name) {
			if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + name + " -- > " + to_string(uniformIt.first));
			return uniformIt.first;
		}
	}
	Console::println("VKRenderer::" + string(__FUNCTION__) + "(): uniform not found: '" + name + "'");
	return -1;
}

inline void VKRenderer::setProgramUniformInternal(void* context, int32_t uniformId, uint8_t* data, int32_t size) {
	auto& contextTyped = *static_cast<context_type*>(context);
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(contextTyped.idx) + ": " + to_string(contextTyped.program_id) + ": " + to_string(uniformId));
	auto uniformIt = contextTyped.program->uniforms.find(uniformId);
	if (uniformIt == contextTyped.program->uniforms.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): uniform does not exist: " + to_string(uniformId));
		return;
	}

	//
	auto changedUniforms = 0;
	auto shaderIdx = 0;
	for (auto shader: contextTyped.program->shaders) {
		auto shaderUniformIt = shader->uniforms.find(uniformIt->second);
		if (shaderUniformIt == shader->uniforms.end()) {
			shaderIdx++;
			continue;
		}
		auto& shaderUniform = shaderUniformIt->second;
		if (shaderUniform.type == shader_type::uniform_type::SAMPLER2D) {
			shaderUniform.texture_unit = *((int32_t*)data);
		} else {
			if (contextTyped.uniform_buffers[shaderIdx].size() < shaderUniform.position + size) {
				Console::println(
					"VKRenderer::" +
					string(__FUNCTION__) +
					"(): program: uniform buffer is too small: " +
					to_string(contextTyped.idx) + ": " +
					to_string(contextTyped.program_id) + ": " +
					to_string(shaderIdx) + "; " +
					to_string(contextTyped.uniform_buffers[shaderIdx].size()) + "; " +
					to_string(shaderUniform.position + size) + ": " +
					shaderUniform.name + ": " +
					to_string(shaderUniform.position + size) + " / " +
					to_string(contextTyped.uniform_buffers[shaderIdx].size())
				);
				shaderIdx++;
				continue;
			}
			auto uniformNoChange = true;
			auto byteChanged = false;
			for (auto i = 0; i < size; i++) {
				byteChanged = contextTyped.uniform_buffers[shaderIdx][shaderUniform.position + i] != data[i];
				if (byteChanged == true) contextTyped.uniform_buffers[shaderIdx][shaderUniform.position + i] = data[i];
				uniformNoChange&= !byteChanged;
			}
			if (uniformNoChange == false) contextTyped.uniform_buffers_changed[shaderIdx] = true;
		}
		changedUniforms++;
		shaderIdx++;
	}
	if (changedUniforms == 0) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): program: no uniform: " + uniformIt->second);
	}
}

void VKRenderer::setProgramUniformInteger(void* context, int32_t uniformId, int32_t value)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	setProgramUniformInternal(context, uniformId, (uint8_t*)&value, sizeof(int32_t));
}

void VKRenderer::setProgramUniformFloat(void* context, int32_t uniformId, float value)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	setProgramUniformInternal(context, uniformId, (uint8_t*)&value, sizeof(float));
}

void VKRenderer::setProgramUniformFloatMatrix3x3(void* context, int32_t uniformId, const array<float, 9>& data)
{
	if (VERBOSE == true) if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	array<float, 12> _data = {
		data[0],
		data[1],
		data[2],
		0.0f,
		data[3],
		data[4],
		data[5],
		0.0f,
		data[6],
		data[7],
		data[8],
		0.0f
	};
	setProgramUniformInternal(context, uniformId, (uint8_t*)_data.data(), _data.size() * sizeof(float));
}

void VKRenderer::setProgramUniformFloatMatrix4x4(void* context, int32_t uniformId, const array<float, 16>& data)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	setProgramUniformInternal(context, uniformId, (uint8_t*)data.data(), data.size() * sizeof(float));
}

void VKRenderer::setProgramUniformFloatMatrices4x4(void* context, int32_t uniformId, int32_t count, FloatBuffer* data)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	setProgramUniformInternal(context, uniformId, (uint8_t*)data->getBuffer(), count * sizeof(float) * 16);
}

void VKRenderer::setProgramUniformFloatVec4(void* context, int32_t uniformId, const array<float, 4>& data)
{
	auto& contextTyped = *static_cast<context_type*>(context);
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	setProgramUniformInternal(context, uniformId, (uint8_t*)data.data(), data.size() * sizeof(float));
}

void VKRenderer::setProgramUniformFloatVec3(void* context, int32_t uniformId, const array<float, 3>& data)
{
	auto& contextTyped = *static_cast<context_type*>(context);
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	setProgramUniformInternal(context, uniformId, (uint8_t*)data.data(), data.size() * sizeof(float));
}

void VKRenderer::setProgramUniformFloatVec2(void* context, int32_t uniformId, const array<float, 2>& data)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	setProgramUniformInternal(context, uniformId, (uint8_t*)data.data(), data.size() * sizeof(float));
}

void VKRenderer::setProgramAttributeLocation(int32_t programId, int32_t location, const string& name)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
}

int32_t VKRenderer::getLighting(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.lighting;
}

void VKRenderer::setLighting(void* context, int32_t lighting) {
	auto& contextTyped = *static_cast<context_type*>(context);
	contextTyped.lighting = lighting;
}

void VKRenderer::setViewPort(int32_t x, int32_t y, int32_t width, int32_t height)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(x) + ", " + to_string(y) + "; " + to_string(width) + ", " + to_string(height));

	//
	memset(&viewport, 0, sizeof(viewport));
	viewport.width = (float)width;
	viewport.height = (float)height;
	viewport.x = (float)x;
	viewport.y = (float)y;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	memset(&scissor, 0, sizeof(scissor));
	scissor.extent.width = width;
	scissor.extent.height = height;
	scissor.offset.x = x;
	scissor.offset.y = y;

	//
	this->pointSize = width > height ? width / 120.0f : height / 120.0f * 16.0f / 9.0f;
}

void VKRenderer::updateViewPort()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
}

void VKRenderer::setClearColor(float red, float green, float blue, float alpha)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	//
	clear_red = red;
	clear_green = green;
	clear_blue = blue;
	clear_alpha = alpha;
}

void VKRenderer::enableCulling(void* context)
{
	auto& contextTyped = *static_cast<context_type*>(context);
	if (contextTyped.culling_enabled == true) return;

	contextTyped.culling_enabled = true;
	contextTyped.front_face_index = contextTyped.front_face;
}

void VKRenderer::disableCulling(void* context)
{
	auto& contextTyped = *static_cast<context_type*>(context);
	if (contextTyped.culling_enabled == false) return;

	contextTyped.culling_enabled = false;
	contextTyped.front_face_index = 0;
}

void VKRenderer::setFrontFace(void* context, int32_t frontFace)
{
	auto& contextTyped = *static_cast<context_type*>(context);
	if (contextTyped.front_face == frontFace) return;

	contextTyped.front_face = frontFace;
	contextTyped.front_face_index = contextTyped.culling_enabled == true?frontFace:0;
}

void VKRenderer::setCullFace(int32_t cullFace)
{
	if (cull_mode == cullFace) return;
	endDrawCommandsAllContexts();
	cull_mode = (VkCullModeFlagBits)cullFace;
	for (auto i = 0; i < Engine::getThreadCount(); i++) contexts[i].pipeline_id.fill(string());
}

void VKRenderer::enableBlending()
{
	if (blending_enabled == true) return;
	endDrawCommandsAllContexts();
	blending_enabled = true;
	for (auto i = 0; i < Engine::getThreadCount(); i++) contexts[i].pipeline_id.fill(string());
}

void VKRenderer::disableBlending()
{
	if (blending_enabled == false) return;
	endDrawCommandsAllContexts();
	blending_enabled = false;
	for (auto i = 0; i < Engine::getThreadCount(); i++) contexts[i].pipeline_id.fill(string());
}

void VKRenderer::enableDepthBufferWriting()
{
	if (depth_buffer_writing == true) return;
	endDrawCommandsAllContexts();
	depth_buffer_writing = true;
	for (auto i = 0; i < Engine::getThreadCount(); i++) contexts[i].pipeline_id.fill(string());
}

void VKRenderer::disableDepthBufferWriting()
{
	if (depth_buffer_writing == false) return;
	endDrawCommandsAllContexts();
	depth_buffer_writing = false;
	for (auto i = 0; i < Engine::getThreadCount(); i++) contexts[i].pipeline_id.fill(string());
}

void VKRenderer::disableDepthBufferTest()
{
	if (depth_buffer_testing == false) return;
	endDrawCommandsAllContexts();
	depth_buffer_testing = false;
	for (auto i = 0; i < Engine::getThreadCount(); i++) contexts[i].pipeline_id.fill(string());
}

void VKRenderer::enableDepthBufferTest()
{
	if (depth_buffer_testing == true) return;
	endDrawCommandsAllContexts();
	depth_buffer_testing = true;
	for (auto i = 0; i < Engine::getThreadCount(); i++) contexts[i].pipeline_id.fill(string());
}

void VKRenderer::setDepthFunction(int32_t depthFunction)
{
	if (depth_function == depthFunction) return;
	endDrawCommandsAllContexts();
	depth_function = depthFunction;
	for (auto i = 0; i < Engine::getThreadCount(); i++) contexts[i].pipeline_id.fill(string());
}

void VKRenderer::setColorMask(bool red, bool green, bool blue, bool alpha)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
}

void VKRenderer::clear(int32_t mask)
{
	beginDrawCommandBuffer(0);
	startRenderPass(0, __LINE__);

	auto attachmentIdx = 0;
	VkClearAttachment attachments[2];
	if ((mask & CLEAR_COLOR_BUFFER_BIT) == CLEAR_COLOR_BUFFER_BIT) {
		attachments[attachmentIdx].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		attachments[attachmentIdx].colorAttachment = attachmentIdx;
		attachments[attachmentIdx].clearValue.color = { clear_red, clear_green, clear_blue, clear_alpha };
		attachmentIdx++;
	}
	if ((mask & CLEAR_DEPTH_BUFFER_BIT) == CLEAR_DEPTH_BUFFER_BIT) {
		attachments[attachmentIdx].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		attachments[attachmentIdx].colorAttachment = attachmentIdx;
		attachments[attachmentIdx].clearValue.depthStencil = { 1.0f, 0 };
		attachmentIdx++;
	}
	VkClearRect clearRect = {
		.rect = scissor,
		.baseArrayLayer = 0,
		.layerCount = 1
	};
	vkCmdClearAttachments(
		contexts[0].draw_cmds[contexts[0].draw_cmd_current][contexts[0].front_face_index],
		attachmentIdx,
		attachments,
		1,
		&clearRect
	);
	endRenderPass(0, __LINE__);
	auto currentBufferIdx = contexts[0].draw_cmd_current;
	auto commandBuffers = endDrawCommandBuffer(0, currentBufferIdx, true);
	for (auto commandBufferIdx = 0; commandBufferIdx < commandBuffers.size(); commandBufferIdx++) {
		if (commandBuffers[commandBufferIdx] != VK_NULL_HANDLE) {
			submitDrawCommandBuffers(1, &commandBuffers[commandBufferIdx], contexts[0].draw_fences[currentBufferIdx][commandBufferIdx], true, false);
		}
	}
}

int32_t VKRenderer::createTexture()
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	textures_rwlock.writeLock();
	auto& texture_object = textures[texture_idx];
	texture_object.id = texture_idx++;
	textures_rwlock.unlock();
	return texture_object.id;
}

int32_t VKRenderer::createDepthBufferTexture(int32_t width, int32_t height) {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	auto& depth_buffer_texture = textures[texture_idx];
	depth_buffer_texture.id = texture_idx++;
	createDepthBufferTexture(depth_buffer_texture.id, width, height);
	return depth_buffer_texture.id;
}

void VKRenderer::createDepthBufferTexture(int32_t textureId, int32_t width, int32_t height)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	auto& depth_buffer_texture = textures[textureId];
	depth_buffer_texture.format = VK_FORMAT_D32_SFLOAT;
	depth_buffer_texture.width = width;
	depth_buffer_texture.height = height;

	if (depth_buffer_texture.view != VK_NULL_HANDLE) vkDestroyImageView(device, depth_buffer_texture.view, nullptr);
	if (depth_buffer_texture.sampler != VK_NULL_HANDLE) vkDestroySampler(device, depth_buffer_texture.sampler, nullptr);
	if (depth_buffer_texture.image != VK_NULL_HANDLE &&
		depth_buffer_texture.allocation != VK_NULL_HANDLE) vmaDestroyImage(allocator, depth_buffer_texture.image, depth_buffer_texture.allocation);

	const VkImageCreateInfo image_create_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = depth_buffer_texture.format,
		.extent = {
			.width = depth_buffer_texture.width,
			.height = depth_buffer_texture.height,
			.depth = 1
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = 0,
		.initialLayout = (VkImageLayout)0,
	};

	//
	VkResult err;

	VmaAllocationCreateInfo image_alloc_create_info = {};
	image_alloc_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	VmaAllocationInfo allocation_info = {};
	err = vmaCreateImage(allocator, &image_create_info, &image_alloc_create_info, &depth_buffer_texture.image, &depth_buffer_texture.allocation, &allocation_info);
	assert(!err);

	//
	depth_buffer_texture.type = texture_object::TYPE_FRAMEBUFFER_DEPTHBUFFER;
	depth_buffer_texture.image_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//
	setImageLayout(
		0,
		depth_buffer_texture.image,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		depth_buffer_texture.image_layout,
		(VkAccessFlagBits)0
	);

	// create sampler
	const VkSamplerCreateInfo sampler = {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.magFilter = VK_FILTER_NEAREST,
		.minFilter = VK_FILTER_NEAREST,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 1,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_NEVER,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		.unnormalizedCoordinates = VK_FALSE,
	};
	err = vkCreateSampler(device, &sampler, nullptr, &depth_buffer_texture.sampler);
	assert(!err);

	// create image view
	VkImageViewCreateInfo view = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = depth_buffer_texture.image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = depth_buffer_texture.format,
		.components = VkComponentMapping(),
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
	};
	err = vkCreateImageView(device, &view, nullptr, &depth_buffer_texture.view);
	assert(!err);
}

int32_t VKRenderer::createColorBufferTexture(int32_t width, int32_t height) {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	auto& color_buffer_texture = textures[texture_idx];
	color_buffer_texture.id = texture_idx++;
	createColorBufferTexture(color_buffer_texture.id, width, height);
	return color_buffer_texture.id;
}

void VKRenderer::createColorBufferTexture(int32_t textureId, int32_t width, int32_t height)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	auto& color_buffer_texture = textures[textureId];
	color_buffer_texture.format = format;
	color_buffer_texture.width = width;
	color_buffer_texture.height = height;

	if (color_buffer_texture.view != VK_NULL_HANDLE) vkDestroyImageView(device, color_buffer_texture.view, nullptr);
	if (color_buffer_texture.sampler != VK_NULL_HANDLE) vkDestroySampler(device, color_buffer_texture.sampler, nullptr);
	if (color_buffer_texture.image != VK_NULL_HANDLE &&
		color_buffer_texture.allocation != VK_NULL_HANDLE) vmaDestroyImage(allocator, color_buffer_texture.image, color_buffer_texture.allocation);

	const VkImageCreateInfo image_create_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = color_buffer_texture.format,
		.extent = {
			.width = color_buffer_texture.width,
			.height = color_buffer_texture.height,
			.depth = 1
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = 0,
		.initialLayout = (VkImageLayout)0,
	};

	//
	VkResult err;

	VmaAllocationCreateInfo image_alloc_create_info = {};
	image_alloc_create_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	VmaAllocationInfo allocation_info = {};
	err = vmaCreateImage(allocator, &image_create_info, &image_alloc_create_info, &color_buffer_texture.image, &color_buffer_texture.allocation, &allocation_info);
	assert(!err);

	//
	color_buffer_texture.type = texture_object::TYPE_FRAMEBUFFER_COLORBUFFER;
	color_buffer_texture.image_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//
	setImageLayout(
		0,
		color_buffer_texture.image,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		color_buffer_texture.image_layout,
		(VkAccessFlagBits)0
	);

	// create sampler
	const VkSamplerCreateInfo sampler = {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 1,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_NEVER,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		.unnormalizedCoordinates = VK_FALSE,
	};
	err = vkCreateSampler(device, &sampler, nullptr, &color_buffer_texture.sampler);
	assert(!err);

	// create image view
	VkImageViewCreateInfo view = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = color_buffer_texture.image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = color_buffer_texture.format,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_R,
			.g = VK_COMPONENT_SWIZZLE_G,
			.b = VK_COMPONENT_SWIZZLE_B,
			.a = VK_COMPONENT_SWIZZLE_A,
		},
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};
	err = vkCreateImageView(device, &view, nullptr, &color_buffer_texture.view);
	assert(!err);
}

void VKRenderer::uploadCubeMapTexture(void* context, Texture* textureLeft, Texture* textureRight, Texture* textureTop, Texture* textureBottom, Texture* textureFront, Texture* textureBack) {
	Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Not yet implemented");
}

void VKRenderer::uploadTexture(void* context, Texture* texture)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	// have our context typed
	auto& contextTyped = *static_cast<context_type*>(context);

	//
	textures_rwlock.writeLock(); // TODO: have a more fine grained locking here
	auto textureObjectIt = textures.find(contextTyped.bound_textures[contextTyped.texture_unit_active]);
	if (textureObjectIt == textures.end()) {
		textures_rwlock.unlock();
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): texture not found: " + to_string(contextTyped.bound_textures[contextTyped.texture_unit_active]));
		return;
	}
	auto& texture_object = textureObjectIt->second;

	//
	uint32_t mipLevels = 1;
	texture_object.width = texture->getTextureWidth();
	texture_object.height = texture->getTextureHeight();

	if (texture_object.uploaded == true) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): texture already uploaded: " + to_string(contextTyped.bound_textures[contextTyped.texture_unit_active]));
		textures_rwlock.unlock();
		return;
	}

	const VkFormat tex_format = texture->getHeight() == 32?VK_FORMAT_R8G8B8A8_UNORM:VK_FORMAT_R8G8B8A8_UNORM;
	VkFormatProperties props;
	VkResult err;


	vkGetPhysicalDeviceFormatProperties(gpu, tex_format, &props);
	if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) == VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
		// we need a setup command buffer here
		prepareSetupCommandBuffer(contextTyped.idx);

		// Must use staging buffer to copy linear texture to optimized
		struct texture_object staging_texture;

		//
		memset(&staging_texture, 0, sizeof(staging_texture));
		prepareTextureImage(
			contextTyped.idx,
			&staging_texture,
			VK_IMAGE_TILING_LINEAR,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			texture,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
		);
		prepareTextureImage(
			contextTyped.idx,
			&texture_object,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			texture,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			false
		);
		setImageLayout(
			contextTyped.idx,
			staging_texture.image,
			VK_IMAGE_ASPECT_COLOR_BIT,
			staging_texture.image_layout,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			(VkAccessFlagBits)0
		);
		setImageLayout(
			contextTyped.idx,
			texture_object.image,
			VK_IMAGE_ASPECT_COLOR_BIT,
			texture_object.image_layout,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			(VkAccessFlagBits)0
		);
		texture_object.image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VkImageCopy copy_region = {
			.srcSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.srcOffset = {
				.x = 0,
				.y = 0,
				.z = 0
			},
			.dstSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.dstOffset = {
				.x = 0,
				.y = 0,
				.z = 0
			},
			.extent = {
				.width = texture_object.width,
				.height = texture_object.height,
				.depth = 1
			}
		};
		vkCmdCopyImage(
			contextTyped.setup_cmd_inuse,
			staging_texture.image,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			texture_object.image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&copy_region
		);

		if (texture->isUseMipMap() == true) {

			//
			auto textureWidth = texture->getTextureWidth();
			auto textureHeight = texture->getTextureHeight();
			mipLevels = getMipLevels(textureWidth, textureHeight);
			for (uint32_t i = 1; i < mipLevels; i++) {
				const VkImageBlit imageBlit = {
					.srcSubresource = {
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.mipLevel = 0,
						.baseArrayLayer = 0,
						.layerCount = 1
					},
					.srcOffsets = {
						[0] = {
							.x = 0,
							.y = 0,
							.z = 0
						},
						[1] = {
							.x = textureWidth,
							.y = textureHeight,
							.z = 1
						}
					},
					.dstSubresource = {
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.mipLevel = i,
						.baseArrayLayer = 0,
						.layerCount = 1
					},
					.dstOffsets = {
						[0] = {
							.x = 0,
							.y = 0,
							.z = 0
						},
						[1] = {
							.x = int32_t(textureWidth >> i),
							.y = int32_t(textureHeight >> i),
							.z = 1
						}
					}
				};
				setImageLayout(
					contextTyped.idx,
					texture_object.image,
					VK_IMAGE_ASPECT_COLOR_BIT,
					VK_IMAGE_LAYOUT_PREINITIALIZED,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					(VkAccessFlagBits)0,
					i,
					1
				);
				vkCmdBlitImage(
					contextTyped.setup_cmd_inuse,
					staging_texture.image,
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					texture_object.image,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&imageBlit,
					VK_FILTER_LINEAR
				);
			}
		}

		setImageLayout(
			contextTyped.idx,
			texture_object.image,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			texture_object.image_layout,
			(VkAccessFlagBits)0,
			0,
			mipLevels
		);

		// mark for deletion
		delete_mutex.lock();
		delete_images.push_back({.image = staging_texture.image, .allocation = staging_texture.allocation});
		delete_mutex.unlock();

		//
		finishSetupCommandBuffer(contextTyped.idx);
	} else
	if ((props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) == VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
		// Device can texture using linear textures
		prepareTextureImage(
			contextTyped.idx,
			&texture_object,
			VK_IMAGE_TILING_LINEAR,
			VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			texture,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		);
	} else {
		// Can't support VK_FORMAT_B8G8R8A8_UNORM !?
		assert(!"No support for B8G8R8A8_UNORM as texture image format");
	}

	const VkSamplerCreateInfo sampler = {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = texture->isRepeat() == true?VK_SAMPLER_ADDRESS_MODE_REPEAT:VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeV = texture->isRepeat() == true?VK_SAMPLER_ADDRESS_MODE_REPEAT:VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeW = texture->isRepeat() == true?VK_SAMPLER_ADDRESS_MODE_REPEAT:VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 1,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_NEVER,
		.minLod = 0.0f,
		.maxLod = texture->isUseMipMap() == true?static_cast<float>(mipLevels):0.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		.unnormalizedCoordinates = VK_FALSE,
	};
	VkImageViewCreateInfo view = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = texture_object.image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = tex_format,
		.components = {
			.r = VK_COMPONENT_SWIZZLE_R,
			.g = VK_COMPONENT_SWIZZLE_G,
			.b = VK_COMPONENT_SWIZZLE_B,
			.a = VK_COMPONENT_SWIZZLE_A,
		},
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = mipLevels,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	// create sampler
	err = vkCreateSampler(device, &sampler, nullptr, &texture_object.sampler);
	assert(!err);

	// create image view
	err = vkCreateImageView(device, &view, nullptr, &texture_object.view);
	assert(!err);

	//
	texture_object.type = texture_object::TYPE_TEXTURE;
	texture_object.uploaded = true;

	//
	textures_rwlock.unlock();
}

void VKRenderer::resizeDepthBufferTexture(int32_t textureId, int32_t width, int32_t height)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	auto textureIt = textures.find(textureId);
	if (textureIt == textures.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): texture not found: " + to_string(textureId));
		return;
	}
	auto texture = textureIt->second;
	createDepthBufferTexture(textureId, width, height);
	if (texture.frame_buffer_object_id != 0) createFramebufferObject(texture.frame_buffer_object_id);
}

void VKRenderer::resizeColorBufferTexture(int32_t textureId, int32_t width, int32_t height)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	auto textureIt = textures.find(textureId);
	if (textureIt == textures.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): texture not found: " + to_string(textureId));
		return;
	}
	auto texture = textureIt->second;
	createColorBufferTexture(textureId, width, height);
	if (texture.frame_buffer_object_id != 0) createFramebufferObject(texture.frame_buffer_object_id);
}

void VKRenderer::bindCubeMapTexture(void* context, int32_t textureId) {
	Console::println("VKRenderer::" + string(__FUNCTION__) + "(): Not yet implemented");
}

void VKRenderer::bindTexture(void* context, int32_t textureId)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	// have our context typed
	auto& contextTyped = *static_cast<context_type*>(context);

	//
	contextTyped.bound_textures[contextTyped.texture_unit_active] = 0;

	//
	textures_rwlock.readLock();
	auto textureObjectIt = textures.find(textureId);
	if (textureId != 0) {
		if (textureObjectIt == textures.end()) {
			textures_rwlock.unlock();
			Console::println("VKRenderer::" + string(__FUNCTION__) + "(): texture not found: " + to_string(contextTyped.bound_textures[contextTyped.texture_unit_active]));
			return;
		}
	}
	//
	textures_rwlock.unlock();

	// bin
	contextTyped.bound_textures[contextTyped.texture_unit_active] = textureId;
	if (textureId == 0) {
		onBindTexture(context, textureId);
		return;
	}

	auto& textureObject = textureObjectIt->second;

	//
	if (textureObject.type == texture_object::TYPE_FRAMEBUFFER_DEPTHBUFFER) {
		if (textureObject.image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			prepareSetupCommandBuffer(contextTyped.idx);
			setImageLayout(
				contextTyped.idx,
				textureObject.image,
				VK_IMAGE_ASPECT_DEPTH_BIT,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
				(VkAccessFlagBits)0
			);
			textureObject.image_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			finishSetupCommandBuffer(contextTyped.idx);
		}
	} else
	if (textureObject.type == texture_object::TYPE_FRAMEBUFFER_COLORBUFFER) {
		if (textureObject.image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			prepareSetupCommandBuffer(contextTyped.idx);
			setImageLayout(
				contextTyped.idx,
				textureObject.image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				(VkAccessFlagBits)0
			);
			textureObject.image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			finishSetupCommandBuffer(contextTyped.idx);
		}
	}

	// done
	onBindTexture(context, textureId);
}

void VKRenderer::disposeTexture(int32_t textureId)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	textures_rwlock.writeLock();
	auto textureObjectIt = textures.find(textureId);
	if (textureObjectIt == textures.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): texture not found: " + to_string(textureId));
		textures_rwlock.unlock();
		return;
	}

	auto& texture = textureObjectIt->second;

	textures.erase(textureObjectIt);
	textures_rwlock.unlock();

	if (texture.view != VK_NULL_HANDLE) vkDestroyImageView(device, texture.view, nullptr);
	if (texture.sampler != VK_NULL_HANDLE) vkDestroySampler(device, texture.sampler, nullptr);
	if (texture.image != VK_NULL_HANDLE &&
		texture.allocation != VK_NULL_HANDLE) vmaDestroyImage(allocator, texture.image, texture.allocation);
}

void VKRenderer::createFramebufferObject(int32_t frameBufferId) {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	auto frameBufferIt = framebuffers.find(frameBufferId);
	if (frameBufferIt == framebuffers.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): frame buffer not found: " + to_string(frameBufferId));
		return;
	}
	auto& frameBufferStruct = frameBufferIt->second;

	texture_object* depthBufferTexture = nullptr;
	texture_object* colorBufferTexture = nullptr;

	auto depthBufferTextureIt = textures.find(frameBufferStruct.depth_texture_id);
	if (depthBufferTextureIt == textures.end()) {
		if (frameBufferStruct.depth_texture_id != 0) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): depth buffer texture not found: " + to_string(frameBufferStruct.depth_texture_id));
	} else {
		depthBufferTexture = &depthBufferTextureIt->second;
	}
	auto colorBufferTextureIt = textures.find(frameBufferStruct.color_texture_id);
	if (colorBufferTextureIt == textures.end()) {
		if (frameBufferStruct.color_texture_id != 0) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): color buffer texture not found: " + to_string(frameBufferStruct.color_texture_id));
	} else {
		colorBufferTexture = &colorBufferTextureIt->second;
	}

	//
	if (depthBufferTexture != nullptr && colorBufferTexture != nullptr &&
		(depthBufferTexture->width != colorBufferTexture->width ||
		depthBufferTexture->height != colorBufferTexture->height)) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): attachments with different dimension found: Not creating!");
		return;
	}

	if (depthBufferTexture != nullptr) depthBufferTexture->frame_buffer_object_id = frameBufferStruct.id;
	if (colorBufferTexture != nullptr) colorBufferTexture->frame_buffer_object_id = frameBufferStruct.id;

	VkResult err;

	{
		if (frameBufferStruct.render_pass != VK_NULL_HANDLE) vkDestroyRenderPass(device, frameBufferStruct.render_pass, nullptr);

		auto attachmentIdx = 0;
		VkAttachmentDescription attachments[2];
		if (colorBufferTexture != nullptr) {
			attachments[attachmentIdx++] = {
				.flags = 0,
				.format = format,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			};
		}
		if (depthBufferTexture != nullptr) {
			attachments[attachmentIdx++] = {
				.flags = 0,
				.format = VK_FORMAT_D32_SFLOAT,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
				.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			};
		}
		const VkAttachmentReference color_reference = {
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		};
		const VkAttachmentReference depth_reference = {
			.attachment = static_cast<uint32_t>(colorBufferTexture != nullptr?1:0),
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		};
		const VkSubpassDescription subpass = {
			.flags = 0,
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = static_cast<uint32_t>(colorBufferTexture != nullptr?1:0),
			.pColorAttachments = colorBufferTexture != nullptr?&color_reference:nullptr,
			.pResolveAttachments = nullptr,
			.pDepthStencilAttachment = depthBufferTexture != nullptr?&depth_reference:nullptr,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr
		};
		const VkRenderPassCreateInfo rp_info = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = static_cast<uint32_t>(attachmentIdx),
			.pAttachments = attachments,
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 0,
			.pDependencies = nullptr
		};
		err = vkCreateRenderPass(device, &rp_info, nullptr, &frameBufferStruct.render_pass);
		assert(!err);
	}

	{
		if (frameBufferStruct.frame_buffer != VK_NULL_HANDLE) vkDestroyFramebuffer(device, frameBufferStruct.frame_buffer, nullptr);
		auto attachmentIdx = 0;
		VkImageView attachments[2];
		if (colorBufferTexture != nullptr) attachments[attachmentIdx++] = colorBufferTexture->view;
		if (depthBufferTexture != nullptr) attachments[attachmentIdx++] = depthBufferTexture->view;
		const VkFramebufferCreateInfo fb_info = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = frameBufferStruct.render_pass,
			.attachmentCount = static_cast<uint32_t>(attachmentIdx),
			.pAttachments = attachments,
			.width = colorBufferTexture != nullptr?colorBufferTexture->width:depthBufferTexture->width,
			.height = colorBufferTexture != nullptr?colorBufferTexture->height:depthBufferTexture->height,
			.layers = 1
		};
		err = vkCreateFramebuffer(device, &fb_info, nullptr, &frameBufferStruct.frame_buffer);
		assert(!err);
	}

}

int32_t VKRenderer::createFramebufferObject(int32_t depthBufferTextureGlId, int32_t colorBufferTextureGlId)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(depthBufferTextureGlId) + ",  " + to_string(colorBufferTextureGlId));

	auto& frameBufferStruct = framebuffers[framebuffer_idx];
	frameBufferStruct.id = framebuffer_idx++;
	frameBufferStruct.depth_texture_id = depthBufferTextureGlId;
	frameBufferStruct.color_texture_id = colorBufferTextureGlId;

	//
	createFramebufferObject(frameBufferStruct.id);

	return frameBufferStruct.id;
}

void VKRenderer::bindFrameBuffer(int32_t frameBufferId)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(frameBufferId));

	// if unsetting program flush command buffers
	endDrawCommandsAllContexts();

	//
	if (frameBufferId != 0) {
		auto frameBufferIt = framebuffers.find(frameBufferId);
		if (frameBufferIt == framebuffers.end()) {
			Console::println("VKRenderer::" + string(__FUNCTION__) + "(): framebuffer not found: " + to_string(frameBufferId));
			frameBufferId = 0;
		}
	}

	//
	memoryBarrier();

	//
	bound_frame_buffer = frameBufferId;
	if (bound_frame_buffer != 0) {
		auto frameBufferIt = framebuffers.find(bound_frame_buffer);
		if (frameBufferIt == framebuffers.end()) {
			Console::println("VKRenderer::" + string(__FUNCTION__) + "(): framebuffer not found: " + to_string(bound_frame_buffer));
		} else {
			auto depthBufferTextureId = frameBufferIt->second.depth_texture_id;
			auto colorBufferTextureId = frameBufferIt->second.color_texture_id;
			prepareSetupCommandBuffer(0);
			if (depthBufferTextureId != 0) {
				auto& depth_buffer_texture = textures[depthBufferTextureId];
				if (depth_buffer_texture.image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
					setImageLayout(
						0,
						depth_buffer_texture.image,
						VK_IMAGE_ASPECT_DEPTH_BIT,
						VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
						VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
						(VkAccessFlagBits)0
					);
					depth_buffer_texture.image_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				}
			}
			if (colorBufferTextureId != 0) {
				auto& color_buffer_texture = textures[colorBufferTextureId];
				if (color_buffer_texture.image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
					setImageLayout(
						0,
						color_buffer_texture.image,
						VK_IMAGE_ASPECT_COLOR_BIT,
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
						(VkAccessFlagBits)0
					);
					color_buffer_texture.image_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}
			}
			finishSetupCommandBuffer(0);
		}
	}
}

void VKRenderer::disposeFrameBufferObject(int32_t frameBufferId)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(frameBufferId));
	auto frameBufferIt = framebuffers.find(frameBufferId);
	if (frameBufferIt == framebuffers.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): framebuffer not found: " + to_string(frameBufferId));
		return;
	}
	vkDestroyFramebuffer(device, frameBufferIt->second.frame_buffer, nullptr);
	framebuffers.erase(frameBufferIt);
}

vector<int32_t> VKRenderer::createBufferObjects(int32_t bufferCount, bool useGPUMemory, bool shared)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	vector<int32_t> bufferIds;
	buffers_rwlock.writeLock();
	for (auto i = 0; i < bufferCount; i++) {
		buffer_object& buffer = buffers[buffer_idx];
		buffer.id = buffer_idx++;
		#if defined(__APPLE__)
			// TODO: fix me, with my Intel Iris 655 GPU memory uploading is not working, however these cards have no GPU memory I guess
			buffer.useGPUMemory = false;
		#else
			buffer.useGPUMemory = useGPUMemory;
		#endif
		buffer.shared = shared;
		bufferIds.push_back(buffer.id);
	}
	buffers_rwlock.unlock();
	return bufferIds;
}

inline VkBuffer VKRenderer::getBufferObjectInternalNoLock(buffer_object* bufferObject, uint32_t& size) {
	auto buffer = bufferObject->current_buffer;
	size = buffer->size;
	buffer->frame_used_last = frame;
	return buffer->buf;
}

inline VkBuffer VKRenderer::getBufferObjectInternalNoLock(int32_t bufferObjectId, uint32_t& size) {
	auto bufferIt = buffers.find(bufferObjectId);
	if (bufferIt == buffers.end()) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): buffer with id " + to_string(bufferObjectId) + " does not exist");
		size = 0;
		return VK_NULL_HANDLE;
	}
	return getBufferObjectInternalNoLock(&bufferIt->second, size);
}

void VKRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VmaAllocation& allocation, VmaAllocationInfo& allocationInfo) {
	//
	VkResult err;

	const VkBufferCreateInfo buf_info = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.size = static_cast<uint32_t>(size),
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr
	};

	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = VMA_MEMORY_USAGE_UNKNOWN;
	alloc_info.requiredFlags = properties;

	//
	err = vmaCreateBuffer(allocator, &buf_info, &alloc_info, &buffer, &allocation, &allocationInfo);
	assert(!err);
}

inline VKRenderer::buffer_object* VKRenderer::getBufferObjectInternal(int32_t bufferObjectId) {
	auto bufferIt = buffers.find(bufferObjectId);
	if (bufferIt == buffers.end()) {
		return nullptr;
	}
	return &bufferIt->second;
}

inline void VKRenderer::uploadBufferObjectInternal(int contextIdx, int32_t bufferObjectId, int32_t size, const uint8_t* data, VkBufferUsageFlagBits usage) {
	if (size == 0) return;

	buffers_rwlock.readLock();
	auto buffer = getBufferObjectInternal(bufferObjectId);
	buffers_rwlock.unlock();
	if (buffer == nullptr) return;

	//
	if (buffer->shared == true) buffers_rwlock.writeLock();

	// do the work
	uploadBufferObjectInternal(contextIdx, buffer, size, data, usage);

	//
	if (buffer->shared == true) buffers_rwlock.unlock();
}

inline void VKRenderer::uploadBufferObjectInternal(int contextIdx, buffer_object* buffer, int32_t size, const uint8_t* data, VkBufferUsageFlagBits usage) {
	if (size == 0) return;

	//
	VkResult err;

	// find a reusable buffer
	buffer_object::reusable_buffer* reusableBuffer = nullptr;
	auto reusableBuffersIt = buffer->buffers.find(size);
	if (reusableBuffersIt != buffer->buffers.end()) {
		for (auto& reusableBufferCandidate: reusableBuffersIt->second) {
			if (reusableBufferCandidate.frame_used_last < frame) {
				reusableBuffer = &reusableBufferCandidate;
				break;
			}
		}
	}

	// nope check if to reuse a bigger one
	if (reusableBuffer == nullptr) {
		for (auto& bufferIt: buffer->buffers) {
			if (bufferIt.first >= size) {
				auto& buffers = bufferIt.second;
				for (auto& reusableBufferCandidate: buffers) {
					if (reusableBufferCandidate.frame_used_last < frame) {
						reusableBuffer = &reusableBufferCandidate;
						break;
					}
				}
			}
			if (reusableBuffer != nullptr) break;
		}
	}

	// nope, create one
	if (reusableBuffer == nullptr) {
		buffer->buffers[size].push_back(buffer_object::reusable_buffer());
		reusableBuffer = &buffer->buffers[size].back();
		buffer->buffer_count++;
	}

	// create buffer if not yet done
	if (reusableBuffer->size == 0) {
		VmaAllocationInfo allocation_info = {};
		createBuffer(size, usage, buffer->useGPUMemory == true?VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, reusableBuffer->buf, reusableBuffer->allocation, allocation_info);
		reusableBuffer->size = size;

		VkMemoryPropertyFlags mem_flags;
		vmaGetMemoryTypeProperties(allocator, allocation_info.memoryType, &mem_flags);
		reusableBuffer->memoryMappable = (mem_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		if (reusableBuffer->memoryMappable == true) {
			err = vmaMapMemory(allocator, reusableBuffer->allocation, &reusableBuffer->data);
			assert(!err);
		}
	}

	// create buffer
	if (reusableBuffer->memoryMappable == true) {
		// copy to buffer
		memcpy(reusableBuffer->data, data, size);

		//
		vmaFlushAllocation(allocator, reusableBuffer->allocation, 0, VK_WHOLE_SIZE);
	} else {
		prepareSetupCommandBuffer(contextIdx);

		void* stagingData;
		VkBuffer stagingBuffer;
		VmaAllocation stagingBufferAllocation;
		VkDeviceSize stagingBufferAllocationSize;
		VmaAllocationInfo stagingBufferAllocationInfo = {};
		createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, stagingBuffer, stagingBufferAllocation, stagingBufferAllocationInfo);
		// mark staging buffer for deletion when finishing frame
		delete_mutex.lock();
		delete_buffers.push_back({.buffer = stagingBuffer, .allocation = stagingBufferAllocation});
		delete_mutex.unlock();


		// map memory
		vmaMapMemory(allocator, stagingBufferAllocation, &stagingData);

		// copy to staging buffer
		memcpy(stagingData, data, size);

		// unmap
		vmaUnmapMemory(allocator, stagingBufferAllocation);

		//
		vmaFlushAllocation(allocator, stagingBufferAllocation, 0, VK_WHOLE_SIZE);

		// copy to GPU buffer
		VkBufferCopy copyRegion = {
			.srcOffset = 0,
			.dstOffset = 0,
			.size = static_cast<VkDeviceSize>(size)
		};
		vkCmdCopyBuffer(contexts[contextIdx].setup_cmd_inuse, stagingBuffer, reusableBuffer->buf, 1, &copyRegion);
	}

	// frame and current buffer
	reusableBuffer->frame_used_last = frame;
	buffer->current_buffer = reusableBuffer;

	// clean up
	vector<int> buffersToRemove;
	if (buffer->buffer_count > 1 && frame >= buffer->frame_cleaned_last + 60) {
		vector<int> bufferArraysToRemove;
		for (auto& reusableBuffersIt: buffer->buffers) {
			auto i = 0;
			buffersToRemove.clear();
			auto& bufferList = reusableBuffersIt.second;
			for (auto& reusableBufferCandidate: bufferList) {
				if (frame >= reusableBufferCandidate.frame_used_last + 60) {
					vmaUnmapMemory(allocator, reusableBufferCandidate.allocation);
					vmaDestroyBuffer(allocator, reusableBufferCandidate.buf, reusableBufferCandidate.allocation);
					buffersToRemove.push_back(i - buffersToRemove.size());
				}
				i++;
			}
			if (buffersToRemove.size() == bufferList.size()) {
				bufferArraysToRemove.push_back(reusableBuffersIt.first);
			} else {
				for (auto bufferToRemove: buffersToRemove) {
					auto listIt = bufferList.begin();
					advance(listIt, bufferToRemove);
					bufferList.erase(listIt);
					buffer->buffer_count--;
				}
			}
		}
		for (auto bufferArrayToRemove: bufferArraysToRemove) {
			buffer->buffer_count-= buffer->buffers.find(bufferArrayToRemove)->second.size();
			buffer->buffers.erase(bufferArrayToRemove);
		}
		buffer->frame_cleaned_last = frame;
	}
}

void VKRenderer::uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, FloatBuffer* data)
{
	auto& contextTyped = *static_cast<context_type*>(context);
	uploadBufferObjectInternal(contextTyped.idx, bufferObjectId, size, data->getBuffer(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
}

void VKRenderer::uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, ShortBuffer* data)
{
	auto& contextTyped = *static_cast<context_type*>(context);
	uploadBufferObjectInternal(contextTyped.idx, bufferObjectId, size, data->getBuffer(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
}

void VKRenderer::uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data)
{
	auto& contextTyped = *static_cast<context_type*>(context);
	uploadBufferObjectInternal(contextTyped.idx, bufferObjectId, size, data->getBuffer(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
}

void VKRenderer::uploadIndicesBufferObject(void* context, int32_t bufferObjectId, int32_t size, ShortBuffer* data)
{
	auto& contextTyped = *static_cast<context_type*>(context);
	uploadBufferObjectInternal(contextTyped.idx, bufferObjectId, size, data->getBuffer(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT));
}

void VKRenderer::uploadIndicesBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data)
{
	auto& contextTyped = *static_cast<context_type*>(context);
	uploadBufferObjectInternal(contextTyped.idx, bufferObjectId, size, data->getBuffer(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT));
}

void VKRenderer::bindIndicesBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_indices_buffer = bufferObjectId;
}

void VKRenderer::bindTextureCoordinatesBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_buffers[2] = bufferObjectId;
}

void VKRenderer::bindVerticesBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_buffers[0] = bufferObjectId;
}

void VKRenderer::bindNormalsBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_buffers[1] = bufferObjectId;
}

void VKRenderer::bindSpriteIndicesBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_buffers[1] = bufferObjectId;
}

void VKRenderer::bindColorsBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_buffers[3] = bufferObjectId;
}

void VKRenderer::bindTangentsBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_buffers[4] = bufferObjectId;
}

void VKRenderer::bindBitangentsBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_buffers[5] = bufferObjectId;
}

void VKRenderer::bindModelMatricesBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_buffers[6] = bufferObjectId;
}

void VKRenderer::bindEffectColorMulsBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_buffers[7] = bufferObjectId;
}

void VKRenderer::bindOrigins(void* context, int32_t bufferObjectId) {
	(*static_cast<context_type*>(context)).bound_buffers[9] = bufferObjectId;
}

void VKRenderer::bindEffectColorAddsBufferObject(void* context, int32_t bufferObjectId)
{
	(*static_cast<context_type*>(context)).bound_buffers[8] = bufferObjectId;
}

void VKRenderer::drawInstancedIndexedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset, int32_t instances) {
	drawInstancedTrianglesFromBufferObjects(context, triangles, trianglesOffset, (*static_cast<context_type*>(context)).bound_indices_buffer, instances);
}

inline void VKRenderer::drawInstancedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset, uint32_t indicesBuffer, int32_t instances)
{
	auto& contextTyped = *static_cast<context_type*>(context);

	//
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(contextTyped.program_id) + ": " + to_string(contextTyped.idx) + ": " + to_string(triangles) + " / " + to_string(trianglesOffset) + " / " + to_string(instances));

	// textures
	textures_rwlock.readLock();
	for (auto i = 0; i < contextTyped.bound_textures.size(); i++) {
		auto textureId = contextTyped.bound_textures[i];
		if (textureId == 0) {
			contextTyped.objects_render_command.textures.erase(i);
			continue;
		}
		auto textureObjectIt = textures.find(textureId);
		if (textureObjectIt == textures.end() ||
			textureObjectIt->second.type == texture_object::TYPE_NONE ||
			(textureObjectIt->second.type == texture_object::TYPE_TEXTURE && textureObjectIt->second.uploaded == false)) {
			Console::println("VKRenderer::" + string(__FUNCTION__) + "(): texture does not exist: " + to_string(contextTyped.bound_textures[i]));
			continue;
		}
		auto& texture_object = textureObjectIt->second;
		contextTyped.objects_render_command.textures[i] = {
			.sampler = texture_object.sampler,
			.view = texture_object.view,
			.image_layout = texture_object.image_layout
		};
	}
	textures_rwlock.unlock();

	// ubos
	auto shaderIdx = 0;
	for (auto shader: contextTyped.program->shaders) {
		if (shader->ubo_binding_idx == -1) {
			shaderIdx++;
			continue;
		}
		// upload
		if (contextTyped.uniform_buffers_changed[shaderIdx] == true) {
			uploadBufferObjectInternal(contextTyped.idx, shader->ubo[contextTyped.idx], contextTyped.uniform_buffers[shaderIdx].size(), contextTyped.uniform_buffers[shaderIdx].data(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
		}
		// get
		uint32_t uboBufferSize;
		contextTyped.objects_render_command.ubo_buffers[shader->ubo_binding_idx] = getBufferObjectInternalNoLock(shader->ubo[contextTyped.idx], uboBufferSize);
		shaderIdx++;
	}

	//
	uint32_t bufferSize;

	//
	buffers_rwlock.readLock();

	//
	contextTyped.objects_render_command.indices_buffer = indicesBuffer == 0?0:getBufferObjectInternalNoLock(indicesBuffer, bufferSize);
	for (auto i = 0; i < contextTyped.objects_render_command.vertex_buffers.size(); i++) {
		if (contextTyped.bound_buffers[i] != 0) contextTyped.objects_render_command.vertex_buffers[i] = getBufferObjectInternalNoLock(contextTyped.bound_buffers[i], bufferSize);
		if (contextTyped.objects_render_command.vertex_buffers[i] == VK_NULL_HANDLE) contextTyped.objects_render_command.vertex_buffers[i] = getBufferObjectInternalNoLock(empty_vertex_buffer, bufferSize);

	}
	contextTyped.objects_render_command.count = triangles;
	contextTyped.objects_render_command.offset = trianglesOffset;
	contextTyped.objects_render_command.instances = instances;

	//
	buffers_rwlock.unlock();

	//
	contextTyped.uniform_buffers_changed.fill(false);

	//
	contextTyped.command_type = context_type::COMMAND_OBJECTS;
	executeCommand(contextTyped.idx);
}

void VKRenderer::drawIndexedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	//
	drawInstancedIndexedTrianglesFromBufferObjects(context, triangles, trianglesOffset, 1);
}

inline void VKRenderer::endDrawCommandsAllContexts() {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	memoryBarrier();
}

inline void VKRenderer::endDrawCommands(int contextIdx) {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	VkResult err;

	// end render passes
	finishSetupCommandBuffer(contextIdx);
	endRenderPass(contextIdx, __LINE__);

	//
	auto submitted_command_buffers_count = 0;
	VkCommandBuffer submitted_command_buffers[DRAW_COMMANDBUFFER_MAX * 3];
	for (auto j = 0; j < DRAW_COMMANDBUFFER_MAX; j++) {
		auto command_buffers = endDrawCommandBuffer(contextIdx, j, false);
		for (auto command_buffer: command_buffers) {
			if (command_buffer != VK_NULL_HANDLE) {
				submitted_command_buffers[submitted_command_buffers_count++] = command_buffer;
			}
		}
	}
	if (submitted_command_buffers_count > 0) {
		submitDrawCommandBuffers(submitted_command_buffers_count, submitted_command_buffers, contexts[contextIdx].draw_fence, true, true);
		recreateContextFences(contextIdx);
	}
}

inline void VKRenderer::executeCommand(int contextIdx) {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "(): " + to_string(contextIdx));

	//
	finishSetupCommandBuffer(contextIdx);

	//
	auto& contextTyped = contexts[contextIdx];
	if (contextTyped.command_type == context_type::COMMAND_NONE) return;

	// check if desc left
	if (contextTyped.program->desc_idxs[contextTyped.idx] == DESC_MAX) {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): program.desc_idxs[" + to_string(contextTyped.idx) + "] == DESC_MAX: " + to_string(contextTyped.program->desc_idxs[contextTyped.idx]));
		return;
	}

	// start draw command buffer, it not yet done
	beginDrawCommandBuffer(contextTyped.idx);

	// create pipeline
	if (contextTyped.command_type == context_type::COMMAND_OBJECTS) {
		startRenderPass(contextTyped.idx, __LINE__);
		setupObjectsRenderingPipeline(contextTyped.idx, contextTyped.program);
	} else
	if (contextTyped.command_type == context_type::COMMAND_POINTS) {
		startRenderPass(contextTyped.idx, __LINE__);
		setupPointsRenderingPipeline(contextTyped.idx, contextTyped.program);
	} else
	if (contextTyped.command_type == context_type::COMMAND_LINES) {
		startRenderPass(contextTyped.idx, __LINE__);
		setupLinesRenderingPipeline(contextTyped.idx, contextTyped.program);
	} else
	if (contextTyped.command_type == context_type::COMMAND_COMPUTE) {
		endRenderPass(contextTyped.idx, __LINE__);
		setupSkinningComputingPipeline(contextTyped.idx, contextTyped.program);
	} else {
		Console::println("VKRenderer::" + string(__FUNCTION__) + "(): unknown pipeline: " + to_string(contextTyped.program_id));
		return;
	}

	VkDescriptorBufferInfo bufferInfos[contextTyped.program->layout_bindings];
	VkWriteDescriptorSet descriptorSetWrites[contextTyped.program->layout_bindings];
	VkDescriptorImageInfo texDescs[contextTyped.program->layout_bindings];

	// do object render command
	if (contextTyped.command_type == context_type::COMMAND_OBJECTS) {
		//
		auto samplerIdx = 0;
		for (auto shader: contextTyped.program->shaders) {
			// sampler2D
			for (auto uniformIt: shader->uniforms) {
				auto& uniform = uniformIt.second;
				if (uniform.type != shader_type::uniform_type::SAMPLER2D) continue;
				auto commandTextureIt = contextTyped.objects_render_command.textures.find(uniform.texture_unit);
				if (commandTextureIt == contextTyped.objects_render_command.textures.end()) {
					texDescs[samplerIdx] = {
						.sampler = white_texture_default->sampler,
						.imageView = white_texture_default->view,
						.imageLayout = white_texture_default->image_layout
					};
				} else {
					auto& texture = commandTextureIt->second;
					texDescs[samplerIdx] = {
						.sampler = texture.sampler,
						.imageView = texture.view,
						.imageLayout = texture.image_layout
					};
				}
				descriptorSetWrites[uniform.position] = {
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.pNext = nullptr,
					.dstSet = contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]],
					.dstBinding = static_cast<uint32_t>(uniform.position),
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.pImageInfo = &texDescs[samplerIdx],
					.pBufferInfo = VK_NULL_HANDLE,
					.pTexelBufferView = VK_NULL_HANDLE
				};
				samplerIdx++;
			}

			// uniform buffer
			if (shader->ubo_binding_idx == -1) {
				continue;
			}

			bufferInfos[shader->ubo_binding_idx] = {
				.buffer = contextTyped.objects_render_command.ubo_buffers[shader->ubo_binding_idx],
				.offset = 0,
				.range = shader->ubo_size
			};

			descriptorSetWrites[shader->ubo_binding_idx] = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]],
				.dstBinding = static_cast<uint32_t>(shader->ubo_binding_idx),
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pImageInfo = nullptr,
				.pBufferInfo = &bufferInfos[shader->ubo_binding_idx],
				.pTexelBufferView = nullptr
			};
		}

		//
		vkUpdateDescriptorSets(device, contextTyped.program->layout_bindings, descriptorSetWrites, 0, nullptr);

		//
		#define OBJECTSRENDERCOMMAND_VERTEX_BUFFER_COUNT	10
		VkBuffer vertexBuffersBuffer[OBJECTSRENDERCOMMAND_VERTEX_BUFFER_COUNT] = {
			contextTyped.objects_render_command.vertex_buffers[0],
			contextTyped.objects_render_command.vertex_buffers[1],
			contextTyped.objects_render_command.vertex_buffers[2],
			contextTyped.objects_render_command.vertex_buffers[3],
			contextTyped.objects_render_command.vertex_buffers[4],
			contextTyped.objects_render_command.vertex_buffers[5],
			contextTyped.objects_render_command.vertex_buffers[6],
			contextTyped.objects_render_command.vertex_buffers[7],
			contextTyped.objects_render_command.vertex_buffers[8],
			contextTyped.objects_render_command.vertex_buffers[9]
		};
		VkDeviceSize vertexBuffersOffsets[OBJECTSRENDERCOMMAND_VERTEX_BUFFER_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		//
		vkCmdBindDescriptorSets(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], VK_PIPELINE_BIND_POINT_GRAPHICS, contextTyped.program->pipeline_layout, 0, 1, &contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]], 0, nullptr);
		if (contextTyped.objects_render_command.indices_buffer != VK_NULL_HANDLE) vkCmdBindIndexBuffer(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], contextTyped.objects_render_command.indices_buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindVertexBuffers(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], 0, OBJECTSRENDERCOMMAND_VERTEX_BUFFER_COUNT, vertexBuffersBuffer, vertexBuffersOffsets);
		if (contextTyped.objects_render_command.indices_buffer != VK_NULL_HANDLE) {
			vkCmdDrawIndexed(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], contextTyped.objects_render_command.count * 3, contextTyped.objects_render_command.instances, contextTyped.objects_render_command.offset * 3, 0, 0);
		} else {
			vkCmdDraw(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], contextTyped.objects_render_command.count * 3, contextTyped.objects_render_command.instances, contextTyped.objects_render_command.offset * 3, 0);
		}

		//
		contextTyped.program->desc_idxs[contextTyped.idx]++;
		contextTyped.command_count[contextTyped.front_face_index]++;
	} else
	if (contextTyped.command_type == context_type::COMMAND_POINTS) {
		// do points render command
		auto samplerIdx = 0;
		for (auto shader: contextTyped.program->shaders) {
			// sampler2D
			for (auto uniformIt: shader->uniforms) {
				auto& uniform = uniformIt.second;
				if (uniform.type != shader_type::uniform_type::SAMPLER2D) continue;
				auto commandTextureIt = contextTyped.points_render_command.textures.find(uniform.texture_unit);
				if (commandTextureIt == contextTyped.points_render_command.textures.end()) {
					texDescs[samplerIdx] = {
						.sampler = white_texture_default->sampler,
						.imageView = white_texture_default->view,
						.imageLayout = white_texture_default->image_layout
					};
				} else {
					auto& texture = commandTextureIt->second;
					texDescs[samplerIdx] = {
						.sampler = texture.sampler,
						.imageView = texture.view,
						.imageLayout = texture.image_layout
					};
				}
				descriptorSetWrites[uniform.position] = {
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.pNext = nullptr,
					.dstSet = contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]],
					.dstBinding = static_cast<uint32_t>(uniform.position),
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.pImageInfo = &texDescs[samplerIdx],
					.pBufferInfo = VK_NULL_HANDLE,
					.pTexelBufferView = VK_NULL_HANDLE
				};
				samplerIdx++;
			}

			// uniform buffer
			if (shader->ubo_binding_idx == -1) {
				continue;
			}

			bufferInfos[shader->ubo_binding_idx] = {
				.buffer = contextTyped.points_render_command.ubo_buffers[shader->ubo_binding_idx],
				.offset = 0,
				.range = shader->ubo_size
			};

			descriptorSetWrites[shader->ubo_binding_idx] = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]],
				.dstBinding = static_cast<uint32_t>(shader->ubo_binding_idx),
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pImageInfo = nullptr,
				.pBufferInfo = &bufferInfos[shader->ubo_binding_idx],
				.pTexelBufferView = nullptr
			};
		}

		//
		vkUpdateDescriptorSets(device, contextTyped.program->layout_bindings, descriptorSetWrites, 0, nullptr);

		//
		#define POINTSRENDERCOMMAND_VERTEX_BUFFER_COUNT	4
		VkBuffer vertexBuffersBuffer[POINTSRENDERCOMMAND_VERTEX_BUFFER_COUNT] = {
			contextTyped.points_render_command.vertex_buffers[0],
			contextTyped.points_render_command.vertex_buffers[1],
			contextTyped.points_render_command.vertex_buffers[2],
			contextTyped.points_render_command.vertex_buffers[3]
		};

		//
		vkCmdBindDescriptorSets(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], VK_PIPELINE_BIND_POINT_GRAPHICS, contextTyped.program->pipeline_layout, 0, 1, &contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]], 0, nullptr);
		VkDeviceSize vertexBuffersOffsets[POINTSRENDERCOMMAND_VERTEX_BUFFER_COUNT] = { 0, 0, 0, 0 };
		vkCmdBindVertexBuffers(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], 0, POINTSRENDERCOMMAND_VERTEX_BUFFER_COUNT, vertexBuffersBuffer, vertexBuffersOffsets);
		vkCmdDraw(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], contextTyped.points_render_command.count, 1, contextTyped.points_render_command.offset, 0);

		//
		contextTyped.program->desc_idxs[contextTyped.idx]++;
		contextTyped.command_count[contextTyped.front_face_index]++;
	} else
	if (contextTyped.command_type == context_type::COMMAND_LINES) {
		// do points render command
		auto samplerIdx = 0;
		for (auto shader: contextTyped.program->shaders) {
			// sampler2D
			for (auto uniformIt: shader->uniforms) {
				auto& uniform = uniformIt.second;
				if (uniform.type != shader_type::uniform_type::SAMPLER2D) continue;
				auto commandTextureIt = contextTyped.lines_render_command.textures.find(uniform.texture_unit);
				if (commandTextureIt == contextTyped.lines_render_command.textures.end()) {
					texDescs[samplerIdx] = {
						.sampler = white_texture_default->sampler,
						.imageView = white_texture_default->view,
						.imageLayout = white_texture_default->image_layout
					};
				} else {
					auto& texture = commandTextureIt->second;
					texDescs[samplerIdx] = {
						.sampler = texture.sampler,
						.imageView = texture.view,
						.imageLayout = texture.image_layout
					};
				}
				descriptorSetWrites[uniform.position] = {
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.pNext = nullptr,
					.dstSet = contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]],
					.dstBinding = static_cast<uint32_t>(uniform.position),
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.pImageInfo = &texDescs[samplerIdx],
					.pBufferInfo = VK_NULL_HANDLE,
					.pTexelBufferView = VK_NULL_HANDLE
				};
				samplerIdx++;
			}

			// uniform buffer
			if (shader->ubo_binding_idx == -1) {
				continue;
			}

			bufferInfos[shader->ubo_binding_idx] = {
				.buffer = contextTyped.lines_render_command.ubo_buffers[shader->ubo_binding_idx],
				.offset = 0,
				.range = shader->ubo_size
			};

			descriptorSetWrites[shader->ubo_binding_idx] = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]],
				.dstBinding = static_cast<uint32_t>(shader->ubo_binding_idx),
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pImageInfo = nullptr,
				.pBufferInfo = &bufferInfos[shader->ubo_binding_idx],
				.pTexelBufferView = nullptr
			};
		}

		//
		vkUpdateDescriptorSets(device, contextTyped.program->layout_bindings, descriptorSetWrites, 0, nullptr);

		//
		#define LINESRENDERCOMMAND_VERTEX_BUFFER_COUNT	4
		VkBuffer vertexBuffersBuffer[LINESRENDERCOMMAND_VERTEX_BUFFER_COUNT] = {
			contextTyped.lines_render_command.vertex_buffers[0],
			contextTyped.lines_render_command.vertex_buffers[1],
			contextTyped.lines_render_command.vertex_buffers[2],
			contextTyped.lines_render_command.vertex_buffers[3]
		};

		//
		vkCmdBindDescriptorSets(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], VK_PIPELINE_BIND_POINT_GRAPHICS, contextTyped.program->pipeline_layout, 0, 1, &contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]], 0, nullptr);
		VkDeviceSize vertexBuffersOffsets[LINESRENDERCOMMAND_VERTEX_BUFFER_COUNT] = { 0, 0, 0, 0 };
		vkCmdBindVertexBuffers(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], 0, LINESRENDERCOMMAND_VERTEX_BUFFER_COUNT, vertexBuffersBuffer, vertexBuffersOffsets);
		vkCmdSetLineWidth(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], line_width);
		vkCmdDraw(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], contextTyped.lines_render_command.count, 1, contextTyped.lines_render_command.offset, 0);

		//
		contextTyped.program->desc_idxs[contextTyped.idx]++;
		contextTyped.command_count[contextTyped.front_face_index]++;
	} else
	if (contextTyped.command_type == context_type::COMMAND_COMPUTE) {
		// do compute command
		for (auto shader: contextTyped.program->shaders) {
			for (int i = 0; i <= shader->binding_max; i++) {
				bufferInfos[i] = {
					.buffer = contextTyped.compute_command.storage_buffers[i],
					.offset = 0,
					.range = contextTyped.compute_command.storage_buffer_sizes[i]
				};
				descriptorSetWrites[i] = {
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.pNext = nullptr,
					.dstSet = contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]],
					.dstBinding = static_cast<uint32_t>(i),
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
					.pImageInfo = nullptr,
					.pBufferInfo = &bufferInfos[i],
					.pTexelBufferView = nullptr
				};
			}

			// uniform buffer
			if (shader->ubo_binding_idx == -1) {
				continue;
			}

			bufferInfos[shader->ubo_binding_idx] = {
				.buffer = contextTyped.compute_command.ubo_buffers[0],
				.offset = 0,
				.range = shader->ubo_size
			};

			descriptorSetWrites[shader->ubo_binding_idx] = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]],
				.dstBinding = static_cast<uint32_t>(shader->ubo_binding_idx),
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pImageInfo = nullptr,
				.pBufferInfo = &bufferInfos[shader->ubo_binding_idx],
				.pTexelBufferView = nullptr,
			};
		}

		//
		vkUpdateDescriptorSets(device, contextTyped.program->layout_bindings, descriptorSetWrites, 0, nullptr);

		//
		vkCmdBindDescriptorSets(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], VK_PIPELINE_BIND_POINT_COMPUTE, contextTyped.program->pipeline_layout, 0, 1, &contextTyped.program->desc_sets[contextTyped.idx][contextTyped.program->desc_idxs[contextTyped.idx]], 0, nullptr);
		vkCmdDispatch(contextTyped.draw_cmds[contextTyped.draw_cmd_current][contextTyped.front_face_index], contextTyped.compute_command.num_groups_x, contextTyped.compute_command.num_groups_y, contextTyped.compute_command.num_groups_z);

		//
		contextTyped.program->desc_idxs[contextTyped.idx]++;
		contextTyped.command_count[contextTyped.front_face_index]++;
	}

	//
	auto commandsMax = contextTyped.command_type == context_type::COMMAND_COMPUTE?COMMANDS_MAX_COMPUTE:COMMANDS_MAX_GRAPHICS;
	contextTyped.command_type = context_type::COMMAND_NONE;
	auto haveOk = false;
	auto haveTooLess = false;
	auto haveTooMuch = false;
	for (auto count: contextTyped.command_count) {
		if (count > commandsMax) haveOk = true;
		if (count != 0 && count < commandsMax) haveTooLess = true;
		if (count > commandsMax * 3) haveTooMuch = true;
	};
	if ((haveTooLess == true && haveTooMuch == true) ||
		(haveTooLess == false && haveOk == true)) {
		endRenderPass(contextTyped.idx, __LINE__);
		auto currentBufferIdx = contextTyped.draw_cmd_current;
		auto commandBuffers = endDrawCommandBuffer(contextTyped.idx, -1, true);
		for (auto commandBufferIdx = 0; commandBufferIdx < commandBuffers.size(); commandBufferIdx++) {
			if (commandBuffers[commandBufferIdx] != VK_NULL_HANDLE) {
				submitDrawCommandBuffers(1, &commandBuffers[commandBufferIdx], contextTyped.draw_fences[currentBufferIdx][commandBufferIdx], false, false);
			}
		}
		contextTyped.command_count.fill(0);
	}
}

void VKRenderer::drawInstancedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset, int32_t instances)
{
	drawInstancedTrianglesFromBufferObjects(context, triangles, trianglesOffset, VK_NULL_HANDLE, instances);
}

void VKRenderer::drawTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset)
{
	drawInstancedTrianglesFromBufferObjects(context, triangles, trianglesOffset, VK_NULL_HANDLE, 1);
}

void VKRenderer::drawPointsFromBufferObjects(void* context, int32_t points, int32_t pointsOffset)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	// have our context typed
	auto& contextTyped = *static_cast<context_type*>(context);

	// textures
	textures_rwlock.readLock();
	for (auto i = 0; i < contextTyped.bound_textures.size(); i++) {
		auto textureId = contextTyped.bound_textures[i];
		if (textureId == 0) continue;
		auto textureObjectIt = textures.find(textureId);
		if (textureObjectIt == textures.end() || textureObjectIt->second.type == texture_object::TYPE_NONE || (textureObjectIt->second.type == texture_object::TYPE_TEXTURE && textureObjectIt->second.uploaded == false)) {
			Console::println("VKRenderer::" + string(__FUNCTION__) + "(): texture does not exist: " + to_string(contextTyped.bound_textures[i]));
			continue;
		}
		auto& texture_object = textureObjectIt->second;
		contextTyped.points_render_command.textures[i] = {
			.sampler = texture_object.sampler,
			.view = texture_object.view,
			.image_layout = texture_object.image_layout
		};
	}
	textures_rwlock.unlock();

	// ubos
	auto shaderIdx = 0;
	for (auto shader: contextTyped.program->shaders) {
		if (shader->ubo_binding_idx == -1) {
			shaderIdx++;
			continue;
		}
		// upload
		if (contextTyped.uniform_buffers_changed[shaderIdx] == true) {
			uploadBufferObjectInternal(contextTyped.idx, shader->ubo[contextTyped.idx], contextTyped.uniform_buffers[shaderIdx].size(), contextTyped.uniform_buffers[shaderIdx].data(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
		}
		// get
		uint32_t uboBufferSize;
		contextTyped.points_render_command.ubo_buffers[shader->ubo_binding_idx] = getBufferObjectInternalNoLock(shader->ubo[contextTyped.idx], uboBufferSize);
		shaderIdx++;
	}

	//
	uint32_t bufferSize;

	// buffers
	buffers_rwlock.readLock();

	//
	for (auto i = 0; i < contextTyped.points_render_command.vertex_buffers.size(); i++) {
		if (contextTyped.bound_buffers[i] != 0) contextTyped.points_render_command.vertex_buffers[i] = getBufferObjectInternalNoLock(contextTyped.bound_buffers[i], bufferSize);
		if (contextTyped.points_render_command.vertex_buffers[i] == VK_NULL_HANDLE) contextTyped.points_render_command.vertex_buffers[i] = getBufferObjectInternalNoLock(empty_vertex_buffer, bufferSize);
	}
	contextTyped.points_render_command.count = points;
	contextTyped.points_render_command.offset = pointsOffset;

	//
	buffers_rwlock.unlock();

	//
	contextTyped.uniform_buffers_changed.fill(false);

	//
	contextTyped.command_type = context_type::COMMAND_POINTS;
	executeCommand(contextTyped.idx);
}

void VKRenderer::setLineWidth(float lineWidth)
{
	line_width = lineWidth;
}

void VKRenderer::drawLinesFromBufferObjects(void* context, int32_t points, int32_t pointsOffset)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");

	//
	auto& contextTyped = *static_cast<context_type*>(context);

	// ubos
	auto shaderIdx = 0;
	for (auto shader: contextTyped.program->shaders) {
		if (shader->ubo_binding_idx == -1) {
			shaderIdx++;
			continue;
		}
		// upload
		if (contextTyped.uniform_buffers_changed[shaderIdx] == true) {
			uploadBufferObjectInternal(contextTyped.idx, shader->ubo[contextTyped.idx], contextTyped.uniform_buffers[shaderIdx].size(), contextTyped.uniform_buffers[shaderIdx].data(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
		}
		// get
		uint32_t uboBufferSize;
		contextTyped.lines_render_command.ubo_buffers[shader->ubo_binding_idx] = getBufferObjectInternalNoLock(shader->ubo[contextTyped.idx], uboBufferSize);
		shaderIdx++;
	}

	//
	uint32_t bufferSize;

	// buffers
	buffers_rwlock.readLock();

	//
	for (auto i = 0; i < contextTyped.lines_render_command.vertex_buffers.size(); i++) {
		if (contextTyped.bound_buffers[i] != 0) contextTyped.lines_render_command.vertex_buffers[i] = getBufferObjectInternalNoLock(contextTyped.bound_buffers[i], bufferSize);
		if (contextTyped.lines_render_command.vertex_buffers[i] == VK_NULL_HANDLE) contextTyped.lines_render_command.vertex_buffers[i] = getBufferObjectInternalNoLock(empty_vertex_buffer, bufferSize);
	}
	contextTyped.lines_render_command.count = points;
	contextTyped.lines_render_command.offset = pointsOffset;

	//
	buffers_rwlock.unlock();

	//
	contextTyped.uniform_buffers_changed.fill(false);

	//
	contextTyped.command_type = context_type::COMMAND_LINES;
	executeCommand(contextTyped.idx);
}

void VKRenderer::unbindBufferObjects(void* context)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	(*static_cast<context_type*>(context)).bound_indices_buffer = 0;
	(*static_cast<context_type*>(context)).bound_buffers.fill(0);
}

void VKRenderer::disposeBufferObjects(vector<int32_t>& bufferObjectIds)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	for (auto bufferObjectId: bufferObjectIds) {
		auto bufferIt = buffers.find(bufferObjectId);
		if (bufferIt == buffers.end()) {
			Console::println("VKRenderer::" + string(__FUNCTION__) + "(): buffer with id " + to_string(bufferObjectId) + " does not exist");
			continue;
		}
		auto& buffer = bufferIt->second;
		for (auto bufferIt: buffer.buffers) {
			for (auto& reusableBuffer: bufferIt.second) {
				if (reusableBuffer.size == 0) continue;
				vmaDestroyBuffer(allocator, reusableBuffer.buf, reusableBuffer.allocation);
			}
		}
		buffers.erase(bufferIt);
	}
}

int32_t VKRenderer::getTextureUnit(void* context)
{
	return (*static_cast<context_type*>(context)).texture_unit_active;
}

void VKRenderer::setTextureUnit(void* context, int32_t textureUnit)
{
	(*static_cast<context_type*>(context)).texture_unit_active = textureUnit;
}

float VKRenderer::readPixelDepth(int32_t x, int32_t y)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return 0.0f;
}

ByteBuffer* VKRenderer::readPixels(int32_t x, int32_t y, int32_t width, int32_t height)
{
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	return nullptr;
}

void VKRenderer::initGuiMode()
{
	disableCulling(&contexts[0]);
	disableDepthBufferTest();
	disableDepthBufferWriting();
}

void VKRenderer::doneGuiMode()
{
	enableDepthBufferWriting();
	enableDepthBufferTest();
	enableCulling(&contexts[0]);
}

void VKRenderer::dispatchCompute(void* context, int32_t numGroupsX, int32_t numGroupsY, int32_t numGroupsZ) {
	// have our context typed
	auto& contextTyped = *static_cast<context_type*>(context);

	// ubos
	auto shaderIdx = 0;
	for (auto shader: contextTyped.program->shaders) {
		if (shader->ubo_binding_idx == -1) {
			shaderIdx++;
			continue;
		}
		// upload
		if (contextTyped.uniform_buffers_changed[shaderIdx] == true) {
			uploadBufferObjectInternal(contextTyped.idx, shader->ubo[contextTyped.idx], contextTyped.uniform_buffers[shaderIdx].size(), contextTyped.uniform_buffers[shaderIdx].data(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
		}
		// get
		uint32_t uboBufferSize;
		contextTyped.compute_command.ubo_buffers[0] = getBufferObjectInternalNoLock(shader->ubo[contextTyped.idx], uboBufferSize); // TODO: do not use static 0 ubo buffer
		shaderIdx++;
	}

	//
	buffers_rwlock.readLock();

	// TODO: improve me to only use one buffer map lookup
	for (auto i = 0; i < contextTyped.compute_command.storage_buffers.size(); i++) {
		if (contextTyped.bound_buffers[i] != 0) contextTyped.compute_command.storage_buffers[i] = getBufferObjectInternalNoLock(contextTyped.bound_buffers[i], contextTyped.compute_command.storage_buffer_sizes[i]);
		if (contextTyped.compute_command.storage_buffers[i] == VK_NULL_HANDLE) contextTyped.compute_command.storage_buffers[i] = getBufferObjectInternalNoLock(empty_vertex_buffer, contextTyped.compute_command.storage_buffer_sizes[i]);
	}
	contextTyped.compute_command.num_groups_x = numGroupsX;
	contextTyped.compute_command.num_groups_y = numGroupsY;
	contextTyped.compute_command.num_groups_z = numGroupsZ;

	//
	buffers_rwlock.unlock();

	//
	contextTyped.uniform_buffers_changed.fill(false);

	//
	contextTyped.command_type = context_type::COMMAND_COMPUTE;
	executeCommand(contextTyped.idx);
}

void VKRenderer::memoryBarrier() {
	if (VERBOSE == true) Console::println("VKRenderer::" + string(__FUNCTION__) + "()");
	VkResult err;

	// end render passes
	auto submitted_command_buffers_count = 0;
	VkCommandBuffer submitted_command_buffers[Engine::getThreadCount() * DRAW_COMMANDBUFFER_MAX * 3];
	for (auto i = 0; i < Engine::getThreadCount(); i++) {
		finishSetupCommandBuffer(i);
		endRenderPass(i, __LINE__); // TODO: draw cmd cycling
		for (auto j = 0; j < DRAW_COMMANDBUFFER_MAX; j++) {
			auto command_buffers = endDrawCommandBuffer(i, j, false);
			for (auto command_buffer: command_buffers) {
				if (command_buffer != VK_NULL_HANDLE) {
					submitted_command_buffers[submitted_command_buffers_count++] = command_buffer;
				}
			}
		}
	}

	//
	if (submitted_command_buffers_count > 0) {
		submitDrawCommandBuffers(submitted_command_buffers_count, submitted_command_buffers, memorybarrier_fence, true, true);
		for (auto i = 0; i < Engine::getThreadCount(); i++) {
			recreateContextFences(i);
		}
	}
}

void VKRenderer::uploadSkinningBufferObject(void* context, int32_t bufferObjectId, int32_t size, FloatBuffer* data) {
	auto& contextTyped = *static_cast<context_type*>(context);
	uploadBufferObjectInternal(contextTyped.idx, bufferObjectId, size, data->getBuffer(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
}

void VKRenderer::uploadSkinningBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data) {
	auto& contextTyped = *static_cast<context_type*>(context);
	uploadBufferObjectInternal(contextTyped.idx, bufferObjectId, size, data->getBuffer(), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
}

void VKRenderer::bindSkinningVerticesBufferObject(void* context, int32_t bufferObjectId) {
	(*static_cast<context_type*>(context)).bound_buffers[0] = bufferObjectId;
}

void VKRenderer::bindSkinningNormalsBufferObject(void* context, int32_t bufferObjectId) {
	(*static_cast<context_type*>(context)).bound_buffers[1] = bufferObjectId;
}

void VKRenderer::bindSkinningVertexJointsBufferObject(void* context, int32_t bufferObjectId) {
	(*static_cast<context_type*>(context)).bound_buffers[2] = bufferObjectId;
}

void VKRenderer::bindSkinningVertexJointIdxsBufferObject(void* context, int32_t bufferObjectId) {
	(*static_cast<context_type*>(context)).bound_buffers[3] = bufferObjectId;
}

void VKRenderer::bindSkinningVertexJointWeightsBufferObject(void* context, int32_t bufferObjectId) {
	(*static_cast<context_type*>(context)).bound_buffers[4] = bufferObjectId;
}

void VKRenderer::bindSkinningVerticesResultBufferObject(void* context, int32_t bufferObjectId) {
	(*static_cast<context_type*>(context)).bound_buffers[5] = bufferObjectId;
}

void VKRenderer::bindSkinningNormalsResultBufferObject(void* context, int32_t bufferObjectId) {
	(*static_cast<context_type*>(context)).bound_buffers[6] = bufferObjectId;
}

void VKRenderer::bindSkinningMatricesBufferObject(void* context, int32_t bufferObjectId) {
	(*static_cast<context_type*>(context)).bound_buffers[7] = bufferObjectId;
}

int32_t VKRenderer::createVertexArrayObject() {
	Console::println("VKRenderer::createVertexArrayObject(): Not implemented");
	return -1;
}

void VKRenderer::disposeVertexArrayObject(int32_t vertexArrayObjectId) {
	Console::println("VKRenderer::disposeVertexArrayObject(): Not implemented");
}

void VKRenderer::bindVertexArrayObject(int32_t vertexArrayObjectId) {
	Console::println("VKRenderer::bindVertexArrayObject(): Not implemented");
}

Matrix2D3x3& VKRenderer::getTextureMatrix(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.texture_matrix;
}

Renderer_Light& VKRenderer::getLight(void* context, int32_t lightId) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.lights[lightId];
}

array<float, 4>& VKRenderer::getEffectColorMul(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.effect_color_mul;
}

array<float, 4>& VKRenderer::getEffectColorAdd(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.effect_color_add;
}

Renderer_SpecularMaterial& VKRenderer::getSpecularMaterial(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.specularMaterial;
}

Renderer_PBRMaterial& VKRenderer::getPBRMaterial(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.pbrMaterial;
}

const string& VKRenderer::getShader(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.shader;
}

void VKRenderer::setShader(void* context, const string& id) {
	auto& contextTyped = *static_cast<context_type*>(context);
	if (VERBOSE == true) Console::println("VKRenderer::setShader(): " + to_string(contextTyped.idx) + ": " + contextTyped.shader + " --> " + id);
	contextTyped.shader = id;
}

const string& VKRenderer::getShaderParametersHash(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.shaderParametersHash;
}

const map<string, string>& VKRenderer::getShaderParameters(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.shaderParameters;
}

void VKRenderer::setShaderParameters(void* context, const map<string, string>& parameters) {
	auto& contextTyped = *static_cast<context_type*>(context);
	contextTyped.shaderParameters = parameters;
}


float VKRenderer::getMaskMaxValue(void* context) {
	auto& contextTyped = *static_cast<context_type*>(context);
	return contextTyped.maskMaxValue;
}

void VKRenderer::setMaskMaxValue(void* context, float maskMaxValue) {
	auto& contextTyped = *static_cast<context_type*>(context);
	contextTyped.maskMaxValue = maskMaxValue;
}
