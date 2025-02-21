#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

#include <agui/agui.h>
#include <agui/gui/fileio/fwd-agui.h>
#include <agui/gui/renderer/GUIRendererBackend.h>
#include <agui/gui/textures/fwd-agui.h>
#include <agui/utilities/fwd-agui.h>

using std::array;
using std::string;
using std::unordered_map;
using std::vector;

// namespaces
namespace agui {
namespace gui {
namespace renderer {
	using ::agui::gui::textures::GUITexture;
	using ::agui::utilities::ByteBuffer;
	using ::agui::utilities::FloatBuffer;
	using ::agui::utilities::IntBuffer;
	using ::agui::utilities::ShortBuffer;
}
}
}

/**
 * OpenGL 2 renderer backend
 * @author Andreas Drewke
 */
class agui::gui::renderer::GL2Renderer: public GUIRendererBackend
{
private:
	unordered_map<uint32_t, int32_t> vbosUsage;
	int activeTextureUnit;

public:
	// forbid class copy
	FORBID_CLASS_COPY(GL2Renderer)

	/**
	 * Public constructor
	 */
	GL2Renderer();

	// overridden methods
	void initialize() override;
	void initializeFrame() override;
	void finishFrame() override;
	const string getVendor() override;
	const string getRenderer() override;
	const string getShaderVersion() override;
	bool isTextureCompressionAvailable() override;
	bool isUsingProgramAttributeLocation() override;
	bool isSupportingIntegerProgramAttributes() override;
	bool isUsingShortIndices() override;
	int32_t getTextureUnits() override;
	int32_t loadShader(int32_t type, const string& pathName, const string& fileName, const string& definitions = string(), const string& functions = string()) override;
	void useProgram(int contextIdx, int32_t programId) override;
	int32_t createProgram(int type) override;
	void attachShaderToProgram(int32_t programId, int32_t shaderId) override;
	bool linkProgram(int32_t programId) override;
	int32_t getProgramUniformLocation(int32_t programId, const string& name) override;
	void setProgramUniformInteger(int contextIdx, int32_t uniformId, int32_t value) override;
	void setProgramUniformFloat(int contextIdx, int32_t uniformId, float value) override;
	void setProgramUniformFloatVec4(int contextIdx, int32_t uniformId, const array<float, 4>& data) override;
	void setProgramUniformFloatVec3(int contextIdx, int32_t uniformId, const array<float, 3>& data) override;
	void setProgramUniformFloatVec2(int contextIdx, int32_t uniformId, const array<float, 2>& data) override;
	void setProgramUniformFloatMatrix3x3(int contextIdx, int32_t uniformId, const array<float, 9>& value) override;
	void setProgramUniformFloatMatrix4x4(int contextIdx, int32_t uniformId, const array<float, 16>& value) override;
	void setProgramUniformFloatMatrices4x4(int contextIdx, int32_t uniformId, int32_t count, FloatBuffer* data) override;
	void setProgramAttributeLocation(int32_t programId, int32_t location, const string& name) override;
	void setViewPort(int32_t width, int32_t height) override;
	void updateViewPort() override;
	void setClearColor(float red, float green, float blue, float alpha) override;
	void clear(int32_t mask) override;
	int32_t createTexture() override;
	int32_t createColorBufferTexture(int32_t width, int32_t height, int32_t cubeMapTextureId, int32_t cubeMapTextureIndex) override;
	void uploadTexture(int contextIdx, GUITexture* texture) override;
	void resizeColorBufferTexture(int32_t textureId, int32_t width, int32_t height) override;
	void bindTexture(int contextIdx, int32_t textureId) override;
	void disposeTexture(int32_t textureId) override;
	vector<int32_t> createBufferObjects(int32_t buffers, bool useGPUMemory, bool shared) override;
	void uploadBufferObject(int contextIdx, int32_t bufferObjectId, int32_t size, FloatBuffer* data) override;
	void uploadBufferObject(int contextIdx, int32_t bufferObjectId, int32_t size, ShortBuffer* data) override;
	void uploadBufferObject(int contextIdx, int32_t bufferObjectId, int32_t size, IntBuffer* data) override;
	void uploadIndicesBufferObject(int contextIdx, int32_t bufferObjectId, int32_t size, ShortBuffer* data) override;
	void uploadIndicesBufferObject(int contextIdx, int32_t bufferObjectId, int32_t size, IntBuffer* data) override;
	void bindIndicesBufferObject(int contextIdx, int32_t bufferObjectId) override;
	void bindSolidColorsBufferObject(int contextIdx, int32_t bufferObjectId) override;
	void bindTextureCoordinatesBufferObject(int contextIdx, int32_t bufferObjectId) override;
	void bindVertices2BufferObject(int contextIdx, int32_t bufferObjectId) override;
	void bindColorsBufferObject(int contextIdx, int32_t bufferObjectId) override;
	void drawIndexedTrianglesFromBufferObjects(int contextIdx, int32_t triangles, int32_t trianglesOffset) override;
	void unbindBufferObjects(int contextIdx) override;
	void disposeBufferObjects(vector<int32_t>& bufferObjectIds) override;
	int32_t getTextureUnit(int contextIdx) override;
	void setTextureUnit(int contextIdx, int32_t textureUnit) override;
	ByteBuffer* readPixels(int32_t x, int32_t y, int32_t width, int32_t height) override;
	void initGUIMode() override;
	void doneGUIMode() override;
	void setVSync(bool vSync) override;
	const Renderer_Statistics getStatistics() override;
};
