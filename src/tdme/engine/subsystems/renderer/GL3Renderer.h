#pragma once

#if defined (__APPLE__)
	#include <OpenCL/opencl.h>
#endif

#include <array>
#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/SingleThreadedRenderer.h>
#include <tdme/utilities/fwd-tdme.h>

using std::array;
using std::map;
using std::string;
using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::subsystems::renderer::SingleThreadedRenderer;
using tdme::engine::FrameBuffer;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::IntBuffer;
using tdme::utilities::ShortBuffer;

/**
 * OpenGL 3 renderer
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::renderer::GL3Renderer: public SingleThreadedRenderer
{
private:
	uint32_t engineVAO;
	map<uint32_t, int32_t> vbosUsage;
	int activeTextureUnit;
	#if defined (__APPLE__)
		struct CLSkinningParameters {
			array<int32_t, 8> boundGLBuffers { -1, -1, -1, -1, -1, -1, -1, -1 };
			array<bool, 8> boundGLBuffersWrite { false, false, false, false, false, false, false, false };
			int32_t matrixCount { 0 };
			int32_t instanceCount { 0 };
			int32_t vertexCount { 0 };
			int32_t numGroupsX { 0 };
			int32_t numGroupsY { 0 };
		};
		cl_context clContext;
		cl_program clSkinningKernelProgram;
		cl_kernel clSkinningKernel;
		cl_command_queue clCommandQueue;
		CLSkinningParameters clSkinningParameters;

		/**
		 * OpenCL bind GL buffer
		 * @param idx argument index
		 * @param bufferObjectId OpenGL buffer object id
		 * @param write write
		 */
		void clBindGLBuffer(int32_t idx, int32_t bufferObjectId, bool write);

		/**
		 * OpenCL error callback
		 * @param errorInfo error info
		 * @param privateInfo private info
		 * @param cb cb?
		 * @param userData user data
		 */
		static void clErrorCallback(const char* errorInfo, const void* privateInfo, size_t cb, void* userData);
	#endif

public:
	/**
	 * Public constructor
	 */
	GL3Renderer();

	// overridden methods
	void initialize() override;
	void initializeFrame() override;
	void finishFrame() override;
	const string getShaderVersion() override;
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
	int32_t createDepthBufferTexture(int32_t width, int32_t height, int32_t cubeMapTextureId, int32_t cubeMapTextureIndex) override;
	int32_t createColorBufferTexture(int32_t width, int32_t height, int32_t cubeMapTextureId, int32_t cubeMapTextureIndex) override;
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
	vector<int32_t> createBufferObjects(int32_t buffers, bool useGPUMemory, bool shared) override;
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
	void bindEffectColorAddsBufferObject(void* context, int32_t bufferObjectId, int32_t divisor) override;
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
	int32_t getTextureUnit(void* context) override;
	void setTextureUnit(void* context, int32_t textureUnit) override;
	float readPixelDepth(int32_t x, int32_t y) override;
	ByteBuffer* readPixels(int32_t x, int32_t y, int32_t width, int32_t height) override;
	void initGuiMode() override;
	void doneGuiMode() override;
	void dispatchCompute(void* context, int32_t numGroupsX, int32_t numGroupsY, int32_t numGroupsZ) override;
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
	int32_t createVertexArrayObject() override;
	void disposeVertexArrayObject(int32_t vertexArrayObjectId) override;
	void bindVertexArrayObject(int32_t vertexArrayObjectId) override;
private:

	/**
	 * Checks if GL error did occour
	 */
	void checkGLError(int line);
};
