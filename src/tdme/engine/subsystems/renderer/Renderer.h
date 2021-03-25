#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/EntityShaderParameters.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix2D3x3.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/fwd-tdme.h>

using std::array;
using std::map;
using std::string;
using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::EntityShaderParameters;
using tdme::engine::FrameBuffer;
using tdme::math::Matrix2D3x3;
using tdme::math::Matrix4x4;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::IntBuffer;
using tdme::utilities::ShortBuffer;

/**
 * Renderer interface
 * @author Andreas Drewke
 * @ersion $Id$
 */
class tdme::engine::subsystems::renderer::Renderer
{
public:

	/**
	 * Bean holding light properties
	 */
	struct Renderer_Light
	{
		int32_t enabled { 0 };
		array<float, 4> ambient {{ 0.0f, 0.0f, 0.0f, 1.0f }};
		array<float, 4> diffuse {{ 1.0f, 1.0f, 1.0f, 1.0f }};
		array<float, 4> specular {{ 1.0f, 1.0f, 1.0f, 1.0f }};
		array<float, 4> position {{ 0.0f, 0.0f, 0.0f, 0.0f }};
		array<float, 3> spotDirection {{ 0.0f, 0.0f, -1.0f }};
		float spotExponent { 0.0f };
		float spotCosCutoff { 0.0f };
		float constantAttenuation { 1.0f };
		float linearAttenuation { 0.0f };
		float quadraticAttenuation { 0.0f };
	};

	/**
	 * Bean holding PBR material properties
	 */
	struct Renderer_PBRMaterial
	{
		array<float, 4> baseColorFactor {{ 1.0f, 1.0f, 1.0f, 1.0f }};
		float metallicFactor { 1.0f };
		float roughnessFactor { 1.0f };
		float normalScale { 1.0f };
		float exposure { 1.0f };
		int baseColorTextureMaskedTransparency { 0 };
		float baseColorTextureMaskedTransparencyThreshold { 0.0f };
	};

	/**
	 *  Bean holding specular material properties
	 */
	struct Renderer_SpecularMaterial
	{
		array<float, 4> ambient {{ 0.2f, 0.2f, 0.2f, 1.0f }};
		array<float, 4> diffuse {{ 0.8f, 0.8f, 0.8f, 1.0f }};
		array<float, 4> specular {{ 0.0f, 0.0f, 0.0f, 1.0f }};
		array<float, 4> emission {{ 0.0f, 0.0f, 0.0f, 1.0f }};
		float shininess { 0.0f };
		float reflection { 0.0f };
		int diffuseTextureMaskedTransparency { 0 };
		float diffuseTextureMaskedTransparencyThreshold { 0.0f };
		int textureAtlasSize { 1 };
		array<float, 2>  textureAtlasPixelDimension { 0.0f, 0.0f };
	};

	/**
	 * Bean holding light properties
	 */
	struct Renderer_Statistics
	{
		int64_t time { -1LL };
		int64_t memoryUsageGPU { -1LL };
		int64_t memoryUsageShared { -1LL };
		uint32_t clearCalls { 0 };
		uint32_t renderCalls { 0 };
		uint32_t computeCalls { 0 };
		uint32_t triangles { 0 };
		uint32_t points { 0 };
		uint32_t linePoints { 0 };
		uint32_t bufferUploads { 0 };
		uint32_t textureUploads { 0 };
		uint32_t renderPasses { 0 };
		uint32_t drawCommands { 0 };
		uint32_t submits { 0 };
	};

	int32_t ID_NONE;
	int32_t CLEAR_DEPTH_BUFFER_BIT;
	int32_t CLEAR_COLOR_BUFFER_BIT;
	int32_t CULLFACE_FRONT;
	int32_t CULLFACE_BACK;
	int32_t TEXTUREUNITS_MAX;
	int32_t PROGRAM_OBJECTS;
	int32_t PROGRAM_POINTS;
	int32_t PROGRAM_LINES;
	int32_t PROGRAM_COMPUTE;
	int32_t SHADER_FRAGMENT_SHADER;
	int32_t SHADER_VERTEX_SHADER;
	int32_t SHADER_GEOMETRY_SHADER;
	int32_t SHADER_COMPUTE_SHADER;
	int32_t DEPTHFUNCTION_ALWAYS;
	int32_t DEPTHFUNCTION_EQUAL;
	int32_t DEPTHFUNCTION_LESSEQUAL;
	int32_t DEPTHFUNCTION_GREATEREQUAL;
	int32_t FRAMEBUFFER_DEFAULT;
	int32_t FRONTFACE_CW;
	int32_t FRONTFACE_CCW;
	int32_t CUBEMAPTEXTUREINDEX_NEGATIVE_X;
	int32_t CUBEMAPTEXTUREINDEX_POSITIVE_X;
	int32_t CUBEMAPTEXTUREINDEX_POSITIVE_Y;
	int32_t CUBEMAPTEXTUREINDEX_NEGATIVE_Y;
	int32_t CUBEMAPTEXTUREINDEX_POSITIVE_Z;
	int32_t CUBEMAPTEXTUREINDEX_NEGATIVE_Z;

	int32_t LIGHTING_NONE;
	int32_t LIGHTING_SPECULAR;
	int32_t LIGHTING_PBR;

	int32_t UNIFORM_CL_SKINNING_VERTEX_COUNT;
	int32_t UNIFORM_CL_SKINNING_MATRIX_COUNT;
	int32_t UNIFORM_CL_SKINNING_INSTANCE_COUNT;
protected:
	int32_t viewPortX;
	int32_t viewPortY;
	int32_t viewPortWidth;
	int32_t viewPortHeight;
	Renderer_Statistics statistics;
private:
	Vector3 cameraPosition;
	Matrix4x4 projectionMatrix;
	Matrix4x4 cameraMatrix;
	Matrix4x4 modelViewMatrix;
	Matrix4x4 viewportMatrix;
	int32_t effectPass;
	string shaderPrefix;
public:
	/**
	 * Public constructor
	 */
	Renderer();

	/**
	 * Destructor
	 */
	virtual ~Renderer();

	/**
	 * @return default context
	 */
	virtual void* getDefaultContext();

	/**
	 * @return context by index
	 */
	virtual void* getContext(int contextIdx);

	/**
	 * Retrieve context index from given context
	 * @return context index
	 */
	virtual int getContextIndex(void* context);

	/**
	 * Initialize renderer
	 */
	virtual void initialize() = 0;

	/**
	 * Pre Frame Initialization
	 */
	virtual void initializeFrame();

	/**
	 * Finish frame
	 */
	virtual void finishFrame() = 0;

	/**
	 * @return shader version e.g. gl2, gl3 or gles2
	 */
	virtual const string getShaderVersion() = 0;

	/**
	 * @return if renderer is supporting multi threaded rendering
	 */
	virtual bool isSupportingMultithreadedRendering() = 0;

	/**
	 * @return if renderer is supporting multiple render queues
	 */
	virtual bool isSupportingMultipleRenderQueues() = 0;

	/**
	 * @return if renderer supports vertex arrays
	 */
	virtual bool isSupportingVertexArrays() = 0;

	/**
	 * Checks if buffer objects is available
	 * @return buffer objects availability
	 */
	virtual bool isBufferObjectsAvailable() = 0;

	/**
	 * Checks if depth texture is available
	 * @return depth texture is available
	 */
	virtual bool isDepthTextureAvailable() = 0;

	/**
	 * @return requires program attribute location
	 */
	virtual bool isUsingProgramAttributeLocation() = 0;

	/**
	 * @return is supporting integer program attributes
	 */
	virtual bool isSupportingIntegerProgramAttributes() = 0;

	/**
	 * @return if specular mapping is supported
	 */
	virtual bool isSpecularMappingAvailable() = 0;

	/**
	 * @return if normal mapping is supported
	 */
	virtual bool isNormalMappingAvailable() = 0;

	/**
	 * Checks if instanced rendering is available
	 * @return instance rendering availability
	 */
	virtual bool isInstancedRenderingAvailable() = 0;

	/**
	 * @return if PBR lighting is supported
	 */
	virtual bool isPBRAvailable() = 0;

	/**
	 * @return if compute shaders are available
	 */
	virtual bool isComputeShaderAvailable() = 0;

	/**
	 * @return if OpenGL+CL is available
	 */
	virtual bool isGLCLAvailable() = 0;

	/**
	 * @return Returns if renderer is using short indices, otherwise it uses int indices
	 */
	virtual bool isUsingShortIndices() = 0;

	/**
	 * @return number of texture units
	 */
	virtual int32_t getTextureUnits() = 0;

	/**
	 * @return viewport width
	 */
	virtual int32_t getViewPortWidth();

	/**
	 * @return viewport height
	 */
	virtual int32_t getViewPortHeight();

	/**
	 * Loads a shader into open gl stack
	 * @param type type
	 * @param pathName path name
	 * @param fileName file name
	 * @param definitions preprocessor definitions
	 * @param functions included functions
	 * @return shader handle
	 */
	virtual int32_t loadShader(int32_t type, const string& pathName, const string& fileName, const string& definitions = string(), const string& functions = string()) = 0;

	/**
	 * Use shader program
	 * @param context context
	 * @param programId programId
	 */
	virtual void useProgram(void* context, int32_t programId) = 0;

	/**
	 * Creates a shader program
	 * @param type type
	 * @return int
	 */
	virtual int32_t createProgram(int type) = 0;

	/**
	 * Attaches a shader to a program
	 * @param programId program id
	 * @param shaderId shader id
	 */
	virtual void attachShaderToProgram(int32_t programId, int32_t shaderId) = 0;

	/**
	 * Links attached shaders to a program
	 * @param programId program id
	 * @return success
	 */
	virtual bool linkProgram(int32_t programId) = 0;

	/**
	 * Returns location of given uniform variable
	 * @param programId program id
	 * @param name uniform name
	 * @return
	 */
	virtual int32_t getProgramUniformLocation(int32_t programId, const string& name) = 0;

	/**
	 * Set up a integer uniform value
	 * @param context context
	 * @param uniformId uniform id
	 * @param value value
	 */
	virtual void setProgramUniformInteger(void* context, int32_t uniformId, int32_t value) = 0;

	/**
	 * Set up a float uniform value
	 * @param context context
	 * @param uniformId uniform id
	 * @param value value
	 */
	virtual void setProgramUniformFloat(void* context, int32_t uniformId, float value) = 0;

	/**
	 * Set up a float matrix 3x3 uniform value
	 * @param context context
	 * @param uniformId uniform id
	 * @param value value
	 */
	virtual void setProgramUniformFloatMatrix3x3(void* context, int32_t uniformId, const array<float, 9>& value) = 0;

	/**
	 * Set up a float matrix 4x4 uniform value
	 * @param context context
	 * @param uniformId uniform id
	 * @param value value
	 */
	virtual void setProgramUniformFloatMatrix4x4(void* context, int32_t uniformId, const array<float, 16>& value) = 0;

	/**
	 * Set up a float matrices 4x4 uniform values
	 * @param context context
	 * @param uniformId uniform id
	 * @param count count
	 * @param data data
	 */
	virtual void setProgramUniformFloatMatrices4x4(void* context, int32_t uniformId, int32_t count, FloatBuffer* data) = 0;

	/**
	 * Set up a float vec4 uniform value
	 * @param context context
	 * @param uniformId uniform id
	 * @param data data
	 */
	virtual void setProgramUniformFloatVec4(void* context, int32_t uniformId, const array<float, 4>& data) = 0;

	/**
	 * Set up a float vec3 uniform value
	 * @param context context
	 * @param uniformId uniform id
	 * @param data data
	 */
	virtual void setProgramUniformFloatVec3(void* context, int32_t uniformId, const array<float, 3>& data) = 0;

	/**
	 * Set up a float vec2 uniform value
	 * @param context context
	 * @param uniformId uniform id
	 * @param data data
	 */
	virtual void setProgramUniformFloatVec2(void* context, int32_t uniformId, const array<float, 2>& data) = 0;

	/**
	 * Bind attribute to a input location
	 * @param programId program id
	 * @param location location
	 * @param name attribute name
	 */
	virtual void setProgramAttributeLocation(int32_t programId, int32_t location, const string& name) = 0;

	/**
	 * Get effect pass
	 * @return effect pass
	 */
	virtual int32_t getEffectPass();

	/**
	 * Set effect pass
	 * @param effectPass effect pass
	 */
	virtual void setEffectPass(int32_t effectPass);

	/**
	 * Get shader prefix
	 * @return shader prefix
	 */
	virtual const string& getShaderPrefix();

	/**
	 * Set shader prefix
	 * @param shaderPrefix shader prefix
	 */
	virtual void setShaderPrefix(const string& shaderPrefix);

	/**
	 * Get current lighting model
	 * @param context context
	 * @return lighting, see LIGHTING_*
	 */
	virtual int32_t getLighting(void* context) = 0;

	/**
	 * Set current lighting model
	 * @param context context
	 * @param lighting lighting, see LIGHTING_*
	 */
	virtual void setLighting(void* context, int32_t lighting) = 0;

	/**
	 * @return camera position
	 */
	virtual Vector3& getCameraPosition();

	/**
	 * Set up viewport parameter
	 * @param x x
	 * @param y y
	 * @param width width
	 * @param height height
	 */
	virtual void setViewPort(int32_t x, int32_t y, int32_t width, int32_t height) = 0;

	/**
	 * Update viewport
	 */
	virtual void updateViewPort() = 0;

	/**
	 * @return projection matrix
	 */
	virtual Matrix4x4& getProjectionMatrix();

	/**
	 * Update projection matrix event
	 * @param context context
	 */
	virtual void onUpdateProjectionMatrix(void* context) = 0;

	/**
	 * @return camera matrix
	 */
	virtual Matrix4x4& getCameraMatrix();

	/**
	 * Update camera matrix event
	 * @param context context
	 */
	virtual void onUpdateCameraMatrix(void* context) = 0;

	/**
	 * @return model view matrix or in some cases the model matrix
	 */
	virtual Matrix4x4& getModelViewMatrix();

	/**
	 * Update model view matrix event
	 * @param context context
	 */
	virtual void onUpdateModelViewMatrix(void* context) = 0;

	/**
	 * @return view port matrix
	 */
	virtual Matrix4x4& getViewportMatrix();

	/**
	 * Get texture matrix
	 * @param context context
	 * @return texture matrix
	 */
	virtual Matrix2D3x3& getTextureMatrix(void* context) = 0;

	/**
	 * Update texture matrix for active texture unit event
	 * @param context context
	 */
	virtual void onUpdateTextureMatrix(void* context) = 0;

	/**
	 * Set up clear color
	 * @param red red
	 * @param green green
	 * @param blue blue
	 * @param alpha alpha
	 */
	virtual void setClearColor(float red, float green, float blue, float alpha) = 0;

	/**
	 * Enable culling
	 * @param context context
	 */
	virtual void enableCulling(void* context) = 0;

	/**
	 * Disable culling
	 * @param context context
	 */
	virtual void disableCulling(void* context) = 0;

	/**
	 * Set up clock wise or counter clock wise faces as front face
	 * @param context context
	 * @param frontFace frontFace
	 */
	virtual void setFrontFace(void* context, int32_t frontFace) = 0;

	/**
	 * Sets up which face will be culled
	 * @param cullFace cull face
	 */
	virtual void setCullFace(int32_t cullFace) = 0;

	/**
	 * Enables blending
	 */
	virtual void enableBlending() = 0;

	/**
	 * Enable blending with c = a + b
	 */
	virtual void enableAdditionBlending() = 0;

	/**
	 * Disables blending
	 */
	virtual void disableBlending() = 0;

	/**
	 * Enable depth buffer writing
	 */
	virtual void enableDepthBufferWriting() = 0;

	/**
	 * Disable depth buffer writing
	 */
	virtual void disableDepthBufferWriting() = 0;

	/**
	 * Disable depth buffer test
	 */
	virtual void disableDepthBufferTest() = 0;

	/**
	 * Enable depth buffer test
	 */
	virtual void enableDepthBufferTest() = 0;

	/**
	 * Set up depth function
	 * @param depthFunction depth function
	 */
	virtual void setDepthFunction(int32_t depthFunction) = 0;

	/**
	 * Set up GL rendering colormask
	 * @param red red
	 * @param green green
	 * @param blue blue
	 * @param alpha alpha
	 */
	virtual void setColorMask(bool red, bool green, bool blue, bool alpha) = 0;

	/**
	 * Clear render buffer with given mask
	 * @param mask mask
	 */
	virtual void clear(int32_t mask) = 0;

	/**
	 * Creates a texture
	 * @return texture id
	 */
	virtual int32_t createTexture() = 0;

	/**
	 * Creates a depth buffer texture
	 * @param width width
	 * @param height height
	 * @oaram cubeMapTextureId cube map texture id
	 * @param cubeMapTextureIndex cube map texture index
	 * @return depth texture id
	 */
	virtual int32_t createDepthBufferTexture(int32_t width, int32_t height, int32_t cubeMapTextureId, int32_t cubeMapTextureIndex) = 0;

	/**
	 * Creates a color buffer texture
	 * @param width width
	 * @param height height
	 * @oaram cubeMapTextureId cube map texture id
	 * @param cubeMapTextureIndex cube map texture index
	 * @return color buffer texture id
	 */
	virtual int32_t createColorBufferTexture(int32_t width, int32_t height, int32_t cubeMapTextureId, int32_t cubeMapTextureIndex) = 0;

	/**
	 * Uploads texture data to current bound texture
	 * @param context context
	 * @param texture texture
	 */
	virtual void uploadTexture(void* context, Texture* texture) = 0;

	/**
	 * Uploads cube map texture data to current bound texture
	 * @param context context
	 * @param textureLeft texture left
	 * @param textureRight texture right
	 * @param textureTop texture top
	 * @param textureBottom texture bottom
	 * @param textureFront texture front
	 * @param textureBack texture back
	 */
	virtual void uploadCubeMapTexture(void* context, Texture* textureLeft, Texture* textureRight, Texture* textureTop, Texture* textureBottom, Texture* textureFront, Texture* textureBack) = 0;

	/**
	 * Create cube map texture from frame buffers
	 * @param context context
	 * @param width width
	 * @param height height
	 * @return texture id
	 */
	virtual int32_t createCubeMapTexture(void* context, int32_t width, int32_t height) = 0;

	/**
	 * Resizes a depth texture
	 * @param textureId texture id
	 * @param width width
	 * @param height height
	 */
	virtual void resizeDepthBufferTexture(int32_t textureId, int32_t width, int32_t height) = 0;

	/**
	 * Resize color buffer texture
	 * @param textureId texture id
	 * @param width width
	 * @param height height
	 */
	virtual void resizeColorBufferTexture(int32_t textureId, int32_t width, int32_t height) = 0;

	/**
	 * Binds a texture with given id or unbinds when using ID_NONE
	 * @param context context
	 * @param textureId textureId
	 */
	virtual void bindTexture(void* context, int32_t textureId) = 0;

	/**
	 * Binds a cube map texture with given id or unbinds when using ID_NONE
	 * @param context context
	 * @param textureId textureId
	 */
	virtual void bindCubeMapTexture(void* context, int32_t textureId) = 0;

	/**
	 * On bind texture event
	 * @param context context
	 * @param textureId textureId
	 */
	virtual void onBindTexture(void* context, int32_t textureId) = 0;

	/**
	 * Dispose a texture
	 * @param textureId texture id
	 */
	virtual void disposeTexture(int32_t textureId) = 0;

	/**
	 * Creates a frame buffer object with depth texture attached
	 * @param depthBufferTextureGlId colorBufferTextureGlId TODO
	 * @param colorBufferTextureGlId depth texture gl id
	 * @param cubeMapTextureId cube map texture id
	 * @param cubeMapTextureIndex cube map texture index
	 * @return frame buffer object id
	 */
	virtual int32_t createFramebufferObject(int32_t depthBufferTextureGlId, int32_t colorBufferTextureGlId, int32_t cubeMapTextureId = 0, int32_t cubeMapTextureIndex = 0) = 0;

	/**
	 * Enables a framebuffer to be rendered
	 * @param frameBufferId frameBufferId
	 */
	virtual void bindFrameBuffer(int32_t frameBufferId) = 0;

	/**
	 * Disposes a frame buffer object
	 * @param frameBufferId frame buffer id
	 */
	virtual void disposeFrameBufferObject(int32_t frameBufferId) = 0;

	/**
	 * Generate buffer objects for vertex data and such
	 * @param buffers buffers
	 * @param useGPUMemory use GPU memory
	 * @param shared shared between different threads
	 * @return ids
	 */
	virtual vector<int32_t> createBufferObjects(int32_t buffers, bool useGPUMemory, bool shared) = 0;

	/**
	 * Uploads buffer data to buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 * @param size size
	 * @param data data
	 */
	virtual void uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, FloatBuffer* data) = 0;

	/**
	 * Uploads buffer data to buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 * @param size size
	 * @param data data
	 */
	virtual void uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data) = 0;

	/**
	 * Uploads buffer data to buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 * @param size size
	 * @param data data
	 */
	virtual void uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, ShortBuffer* data) = 0;

	/**
	 * Uploads buffer data to buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 * @param size size
	 * @param data data
	 */
	virtual void uploadIndicesBufferObject(void* context, int32_t bufferObjectId, int32_t size, ShortBuffer* data) = 0;

	/**
	 * Uploads buffer data to buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 * @param size size
	 * @param data data
	 */
	virtual void uploadIndicesBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data) = 0;

	/**
	 * Bind indices buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindIndicesBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind texture coordinates buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindTextureCoordinatesBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind vertices buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindVerticesBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind normals buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindNormalsBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind colors buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindColorsBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind tangents buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindTangentsBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind bitangents buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindBitangentsBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind model matrices buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindModelMatricesBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind effect color muls buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 * @param divisor divisor
	 */
	virtual void bindEffectColorMulsBufferObject(void* context, int32_t bufferObjectId, int32_t divisor) = 0;

	/**
	 * Bind effect color adds buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 * @param divisor divisor
	 */
	virtual void bindEffectColorAddsBufferObject(void* context, int32_t bufferObjectId, int32_t divisor) = 0;

	/**
	 * Bind origins buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindOriginsBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind texture and sprite indices buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindTextureSpriteIndicesBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind point sizes buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindPointSizesBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind sprite sheet dimension buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindSpriteSheetDimensionBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Draw instanced indexed triangles from buffer objects
	 * @param context context
	 * @param triangles triangles
	 * @param trianglesOffset triangles offset
	 * @param instances instances
	 */
	virtual void drawInstancedIndexedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset, int32_t instances) = 0;

	/**
	 * Draw indexed triangles from buffer objects
	 * @param context context
	 * @param triangles triangles
	 * @param trianglesOffset triangles offset
	 */
	virtual void drawIndexedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset) = 0;

	/**
	 * Draw instanced triangles from buffer objects
	 * @param context context
	 * @param triangles triangles
	 * @param trianglesOffset triangles offset
	 * @param instances instances
	 */
	virtual void drawInstancedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset, int32_t instances) = 0;

	/**
	 * Draw triangles from buffer objects
	 * @param context context
	 * @param triangles triangles
	 * @param trianglesOffset triangles offset
	 */
	virtual void drawTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset) = 0;

	/**
	 * Draw points from buffer objects
	 * @param context context
	 * @param points points
	 * @param pointsOffset points offset
	 */
	virtual void drawPointsFromBufferObjects(void* context, int32_t points, int32_t pointsOffset) = 0;

	/**
	 * Set line width
	 * @param lineWidth line width
	 */
	virtual void setLineWidth(float lineWidth) = 0;

	/**
	 * Draw lines from buffer objects
	 * @param context context
	 * @param points points
	 * @param pointsOffset points offset
	 */
	virtual void drawLinesFromBufferObjects(void* context, int32_t points, int32_t pointsOffset) = 0;

	/**
	 * Unbind buffer objects
	 * @param context context
	 */
	virtual void unbindBufferObjects(void* context) = 0;

	/**
	 * Disposes a frame buffer object
	 * @param bufferObjectIds frame buffer id
	 */
	virtual void disposeBufferObjects(vector<int32_t>& bufferObjectIds) = 0;

	/**
	 * Get texture unit
	 * @param context context
	 * @return active texture unit
	 */
	virtual int32_t getTextureUnit(void* context) = 0;

	/**
	 * Sets up texture unit
	 * @param context context
	 * @param textureUnit texture unit
	 */
	virtual void setTextureUnit(void* context, int32_t textureUnit) = 0;

	/**
	 * Get light
	 * @param context context
	 * @param lightId light id
	 * @return light
	 */
	virtual Renderer_Light& getLight(void* context, int32_t lightId) = 0;

	/**
	 * Update light
	 * @param context context
	 * @param lightId light id
	 */
	virtual void onUpdateLight(void* context, int32_t lightId) = 0;

	/**
	 * Get effect color mul
	 * @param context
	 * @return effect color mul
	 */
	virtual array<float, 4>& getEffectColorMul(void* context) = 0;

	/**
	 * Get effect color add
	 * @param context
	 * @return effect color add
	 */
	virtual array<float, 4>& getEffectColorAdd(void* context) = 0;

	/**
	 * Update material
	 * @param context context
	 */
	virtual void onUpdateEffect(void* context) = 0;

	/**
	 * Get specular material
	 * @param context context
	 * @return material
	 */
	virtual Renderer_SpecularMaterial& getSpecularMaterial(void* context) = 0;

	/**
	 * Get PBR material
	 * @param context context
	 * @return material
	 */
	virtual Renderer_PBRMaterial& getPBRMaterial(void* context) = 0;

	/**
	 * On update material
	 * @param context context
	 */
	virtual void onUpdateMaterial(void* context) = 0;

	/**
	 * Get shader
	 * @param context context
	 */
	virtual const string& getShader(void* context) = 0;

	/**
	 * Set shader
	 * @param context context
	 * @param id shader id
	 * @param parameters parameters
	 */
	virtual void setShader(void* context, const string& id) = 0;

	/**
	 * On update shader
	 * @param context context
	 */
	virtual void onUpdateShader(void* context) = 0;

	/**
	 * Get shader parameters
	 * @param context context
	 * @return shader parameters
	 */
	virtual const EntityShaderParameters& getShaderParameters(void* context) = 0;

	/**
	 * Set shader parameters
	 * @param context context
	 * @param parameters shader parameters
	 */
	virtual void setShaderParameters(void* context, const EntityShaderParameters& parameters) = 0;

	/**
	 * On update shader parameters
	 * @param context context
	 */
	virtual void onUpdateShaderParameters(void* context) = 0;

	/**
	 * Reads a pixel depth
	 * @param x x
	 * @param y y
	 * @return depth 0.0f..1.0f
	 */
	virtual float readPixelDepth(int32_t x, int32_t y) = 0;

	/**
	 * Read pixels
	 * @param x x
	 * @param y y
	 * @param width width
	 * @param height height
	 * @return byte buffer
	 */
	virtual ByteBuffer* readPixels(int32_t x, int32_t y, int32_t width, int32_t height) = 0;

	/**
	 * Dispatch compute
	 * @param context context
	 * @param numGroupsX num groups x
	 * @param numGroupsY num groups y
	 * @param numGroupsZ num groups z
	 */
	virtual void dispatchCompute(void* context, int32_t numGroupsX, int32_t numGroupsY, int32_t numGroupsZ) = 0;

	/**
	 * Memory barrier
	 */
	virtual void memoryBarrier() = 0;

	/**
	 * Upload skinning buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 * @param size size
	 * @param data data
	 */
	virtual void uploadSkinningBufferObject(void* context, int32_t bufferObjectId, int32_t size, FloatBuffer* data) = 0;

	/**
	 * Upload skinning buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 * @param size size
	 * @param data data
	 */
	virtual void uploadSkinningBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data) = 0;

	/**
	 * Bind skinning vertices buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindSkinningVerticesBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning normal buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindSkinningNormalsBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning vertex joints buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindSkinningVertexJointsBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning vertex joint indices buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindSkinningVertexJointIdxsBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning vertex joint weights buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindSkinningVertexJointWeightsBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning vertices result buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindSkinningVerticesResultBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning normals result buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindSkinningNormalsResultBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Bind skinning matrices result buffer object
	 * @param context context
	 * @param bufferObjectId buffer object id
	 */
	virtual void bindSkinningMatricesBufferObject(void* context, int32_t bufferObjectId) = 0;

	/**
	 * Create a single vertex array object
	 */
	virtual int32_t createVertexArrayObject() = 0;

	/**
	 * Dispose a single vertex array object
	 * @param vertexArrayObjectId vertex array object id
	 */
	virtual void disposeVertexArrayObject(int32_t vertexArrayObjectId) = 0;

	/**
	 * Bind a single vertex array object
	 * @param vertexArrayObjectId vertex array object id
	 */
	virtual void bindVertexArrayObject(int32_t vertexArrayObjectId) = 0;

	/**
	 * Get mask max value
	 * @return mask max value
	 */
	virtual float getMaskMaxValue(void* context) = 0;

	/**
	 * Set mask max value
	 * @param context context
	 * @param maskMinValue mask mask value
	 */
	virtual void setMaskMaxValue(void* context, float maskMaxValue) = 0;

	/**
	 * Get environment mapping cube map position
	 * @param context context
	 * @return environment mapping position
	 */
	virtual array<float, 3>& getEnvironmentMappingCubeMapPosition(void* context) = 0;

	/**
	 * Set environment mapping cube map position
	 * @param context context
	 * @param position position
	 */
	virtual void setEnvironmentMappingCubeMapPosition(void* context, array<float, 3>& position) = 0;

	/**
	 * Set up renderer for GUI rendering
	 */
	virtual void initGuiMode() = 0;

	/**
	 * Set up renderer for 3d rendering
	 */
	virtual void doneGuiMode() = 0;

	/**
	 * @return renderer statistics
	 */
	virtual const Renderer_Statistics getStatistics() = 0;

	/**
	 * Generate mip map for atlas texture currently
	 * @param id id
	 * @param texture texture
	 * @param level level
	 * @param atlasBorderSize atlasBorderSize
	 */
	Texture* generateMipMap(const string& id, Texture* texture, int32_t level, int32_t atlasBorderSize);

};
