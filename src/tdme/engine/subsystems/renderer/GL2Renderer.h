#pragma once

#include <array>
#include <map>
#include <vector>
#include <string>

#include <tdme/tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/SingleThreadedRenderer.h>

using std::array;
using std::map;
using std::vector;
using std::string;

using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::ShortBuffer;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::subsystems::renderer::SingleThreadedRenderer;
using tdme::math::Matrix4x4;

/** 
 * OpenGL 2 renderer
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::renderer::GL2Renderer: public SingleThreadedRenderer
{
private:
	bool bufferObjectsAvailable;
	map<uint32_t, int32_t> vbosUsage;
	int activeTextureUnit;

private:

	/** 
	 * Checks if VBO is available
	 * @return vbo availability
	 */
	bool checkBufferObjectsAvailable();

public:
	/**
	 * Public constructor
	 */
	GL2Renderer();

	bool isDepthTextureAvailable() override;
	void initialize() override;
	void initializeFrame() override;
	void finishFrame() override;
	const string getShaderVersion() override;
	bool isSupportingMultithreadedRendering() override;
	bool isSupportingMultipleRenderQueues() override;
	bool isSupportingVertexArrays() override;
	bool isBufferObjectsAvailable() override;
	bool isUsingProgramAttributeLocation() override;
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
	void setProgramUniformFloatMatrices4x4(void* context, int32_t uniformId, int32_t count, FloatBuffer* data) override;
	void setProgramUniformFloatMatrix3x3(void* context, int32_t uniformId, const array<float, 9>& data) override;
	void setProgramUniformFloatMatrix4x4(void* context, int32_t uniformId, const array<float, 16>& data) override;
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
	void resizeDepthBufferTexture(int32_t textureId, int32_t width, int32_t height) override;
	void resizeColorBufferTexture(int32_t textureId, int32_t width, int32_t height) override;
	void bindTexture(void* context, int32_t textureId) override;
	void bindCubeMapTexture(void* context, int32_t textureId) override;
	void disposeTexture(int32_t textureId) override;
	int32_t createFramebufferObject(int32_t depthBufferTextureGlId, int32_t colorBufferTextureGlId) override;
	void bindFrameBuffer(int32_t frameBufferId) override;
	void disposeFrameBufferObject(int32_t frameBufferId) override;
	vector<int32_t> createBufferObjects(int32_t buffers, bool useGPUMemory, bool shared) override;
	void uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, FloatBuffer* data) override;
	void uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data) override;
	void uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, ShortBuffer* data) override;
	void uploadIndicesBufferObject(void* context, int32_t bufferObjectId, int32_t size, ShortBuffer* data) override;
	void uploadIndicesBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data) override;
	void bindIndicesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindTextureCoordinatesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindVerticesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindNormalsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindSpriteIndicesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindColorsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindTangentsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindBitangentsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindModelMatricesBufferObject(void* context, int32_t bufferObjectId) override;
	void bindEffectColorMulsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindEffectColorAddsBufferObject(void* context, int32_t bufferObjectId) override;
	void bindOrigins(void* context, int32_t bufferObjectId) override;
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

	// overriden methods for skinning on GPU via compute shader
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
	void bindSkinningMatricesBufferObject(void* context, 	int32_t bufferObjectId) override;

	//
	int32_t createVertexArrayObject() override;
	void disposeVertexArrayObject(int32_t vertexArrayObjectId) override;
	void bindVertexArrayObject(int32_t vertexArrayObjectId) override;

};
