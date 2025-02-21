#include <agui/gui/renderer/GL2Renderer.h>

#if defined(__APPLE__)
        #include <OpenGL/gl.h>
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__linux__) || defined(_WIN32) || defined(__HAIKU__)
	#define GLEW_NO_GLU
	#include <GL/glew.h>
#endif

#include <string.h>

#include <array>
#include <map>
#include <string>
#include <vector>

#include <agui/agui.h>
#include <agui/gui/renderer/GUIRendererBackend.h>
#include <agui/gui/textures/GUITexture.h>
#include <agui/math/Math.h>
#include <agui/math/Matrix4x4.h>
#include <agui/os/filesystem/FileSystem.h>
#include <agui/os/filesystem/FileSystemInterface.h>
#include <agui/utilities/Buffer.h>
#include <agui/utilities/ByteBuffer.h>
#include <agui/utilities/Console.h>
#include <agui/utilities/FloatBuffer.h>
#include <agui/utilities/IntBuffer.h>
#include <agui/utilities/ShortBuffer.h>
#include <agui/utilities/StringTokenizer.h>
#include <agui/utilities/StringTools.h>
#include <agui/utilities/Time.h>

using std::array;
using std::map;
using std::string;
using std::to_string;
using std::vector;

using agui::gui::renderer::GL2Renderer;

using agui::gui::renderer::GUIRendererBackend;
using agui::gui::textures::GUITexture;
using agui::math::Math;
using agui::math::Matrix4x4;
using agui::os::filesystem::FileSystem;
using agui::os::filesystem::FileSystemInterface;
using agui::utilities::Buffer;
using agui::utilities::ByteBuffer;
using agui::utilities::Console;
using agui::utilities::FloatBuffer;
using agui::utilities::IntBuffer;
using agui::utilities::ShortBuffer;
using agui::utilities::StringTokenizer;
using agui::utilities::StringTools;
using agui::utilities::Time;

GL2Renderer::GL2Renderer()
{
	// setup GL2 consts
	rendererType = RENDERERTYPE_OPENGL;
	ID_NONE = 0;
	CLEAR_DEPTH_BUFFER_BIT = GL_DEPTH_BUFFER_BIT;
	CLEAR_COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT;
	SHADER_FRAGMENT_SHADER = GL_FRAGMENT_SHADER;
	SHADER_VERTEX_SHADER = GL_VERTEX_SHADER;
	SHADER_COMPUTE_SHADER = -1;
	activeTextureUnit = 0;
}

const string GL2Renderer::getVendor() {
	auto vendor = glGetString(GL_VENDOR);
	return string((char*)vendor);
}

const string GL2Renderer::getRenderer() {
	auto renderer = glGetString(GL_RENDERER);
	return string((char*)renderer) + " [GL2/3]";
}

const string GL2Renderer::getShaderVersion()
{
	return "gl2";
}

bool GL2Renderer::isTextureCompressionAvailable()
{
	return false;
}

void GL2Renderer::initialize()
{
	glGetError();
	// get default framebuffer
	FRAMEBUFFER_DEFAULT = 0; // getContext()->getDefaultDrawFramebuffer();
	// setup open gl
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendEquation(GL_FUNC_ADD);
	glDisable(GL_BLEND);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
	setTextureUnit(CONTEXTINDEX_DEFAULT, 0);
	// renderer contexts
	rendererContexts.resize(1);
	for (auto& rendererContext: rendererContexts) {
		rendererContext.textureMatrix.identity();
	}
}

void GL2Renderer::initializeFrame()
{
}

void GL2Renderer::finishFrame()
{
}

bool GL2Renderer::isUsingProgramAttributeLocation()
{
	return true;
}

bool GL2Renderer::isSupportingIntegerProgramAttributes() {
	return false;
}


bool GL2Renderer::isUsingShortIndices() {
	return false;
}

int32_t GL2Renderer::getTextureUnits()
{
	return -1;
}

int32_t GL2Renderer::loadShader(int32_t type, const string& pathName, const string& fileName, const string& definitions, const string& functions)
{
	// create shader
	int32_t shaderId = glCreateShader(type);
	// exit if no handle returned
	if (shaderId == 0) return 0;
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
	// some adjustments to have GLES2 shader working with GL2
	shaderSource = StringTools::replace(shaderSource, "#version 100", "#version 120");
	shaderSource = StringTools::replace(shaderSource, "\r", "");
	{
		StringTokenizer t;
		t.tokenize(shaderSource, "\n");
		shaderSource.clear();
		while (t.hasMoreTokens() == true) {
			auto line = t.nextToken();
			if (StringTools::startsWith(line, "precision") == true) continue;
			shaderSource+= line + "\n";
		}
	}
	auto shaderSourceNullTerminated = shaderSource + "\0";
	// load source
	array<const char*, 1> shaderSourceNullTerminatedArray = { shaderSourceNullTerminated.data() };
	glShaderSource(shaderId, 1, shaderSourceNullTerminatedArray.data(), nullptr);
	// compile
	glCompileShader(shaderId);
	// check state
	int32_t compileStatus;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == 0) {
		// get error
		int32_t infoLogLength;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
		string infoLog(infoLogLength, static_cast<char>(0));
		glGetShaderInfoLog(shaderId, infoLogLength, &infoLogLength, infoLog.data());
		// be verbose
		Console::printLine(
			string(
				string("GL2Renderer::loadShader") +
				string("[") +
				to_string(shaderId) +
				string("]") +
				pathName +
				string("/") +
				fileName +
				string(": failed: ") +
				infoLog
			 )
		 );
		Console::printLine(shaderSource);
		// remove shader
		glDeleteShader(shaderId);
		//
		return 0;
	}
	//
	return shaderId;
}

void GL2Renderer::useProgram(int contextIdx, int32_t programId)
{
	glUseProgram(programId);
}

int32_t GL2Renderer::createProgram(int type)
{
	return glCreateProgram();
}

void GL2Renderer::attachShaderToProgram(int32_t programId, int32_t shaderId)
{
	glAttachShader(programId, shaderId);
}

bool GL2Renderer::linkProgram(int32_t programId)
{
	glLinkProgram(programId);
	// check state
	int32_t linkStatus;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == 0) {
		// get error
		int32_t infoLogLength;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		string infoLog(infoLogLength, static_cast<char>(0));
		glGetProgramInfoLog(programId, infoLogLength, &infoLogLength, infoLog.data());
		// be verbose
		Console::printLine(
			string(
				string("GL2Renderer::linkProgram") +
				"[" +
				to_string(programId) +
				"]: failed: " +
				infoLog
			 )
		);
		//
		return false;
	}
	//
	return true;
}

int32_t GL2Renderer::getProgramUniformLocation(int32_t programId, const string& name)
{
	auto uniformLocation = glGetUniformLocation(programId, (name).c_str());
	return uniformLocation;
}

void GL2Renderer::setProgramUniformInteger(int contextIdx, int32_t uniformId, int32_t value)
{
	glUniform1i(uniformId, value);
}

void GL2Renderer::setProgramUniformFloat(int contextIdx, int32_t uniformId, float value)
{
	glUniform1f(uniformId, value);
}

void GL2Renderer::setProgramUniformFloatMatrices4x4(int contextIdx, int32_t uniformId, int32_t count, FloatBuffer* data)
{
	glUniformMatrix4fv(uniformId, count, false, (float*)data->getBuffer());
}

void GL2Renderer::setProgramUniformFloatMatrix3x3(int contextIdx, int32_t uniformId, const array<float, 9>& data)
{
	glUniformMatrix3fv(uniformId, 1, false, data.data());
}

void GL2Renderer::setProgramUniformFloatMatrix4x4(int contextIdx, int32_t uniformId, const array<float, 16>& data)
{
	glUniformMatrix4fv(uniformId, 1, false, data.data());
}

void GL2Renderer::setProgramUniformFloatVec4(int contextIdx, int32_t uniformId, const array<float, 4>& data)
{
	glUniform4fv(uniformId, 1, data.data());
}

void GL2Renderer::setProgramUniformFloatVec3(int contextIdx, int32_t uniformId, const array<float, 3>& data)
{
	glUniform3fv(uniformId, 1, data.data());
}

void GL2Renderer::setProgramUniformFloatVec2(int contextIdx, int32_t uniformId, const array<float, 2>& data)
{
	glUniform2fv(uniformId, 1, data.data());
}

void GL2Renderer::setProgramAttributeLocation(int32_t programId, int32_t location, const string& name)
{
	glBindAttribLocation(programId, location, (name).c_str());
}

void GL2Renderer::setViewPort(int32_t width, int32_t height)
{
	this->viewPortWidth = width;
	this->viewPortHeight = height;
}

void GL2Renderer::updateViewPort()
{
	glViewport(0, 0, viewPortWidth, viewPortHeight);
}

void GL2Renderer::setClearColor(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
}

void GL2Renderer::clear(int32_t mask)
{
	glClear(mask);
	statistics.clearCalls++;
}

int32_t GL2Renderer::createTexture()
{
	uint32_t textureId;
	glGenTextures(1, &textureId);
	return textureId;
}

int32_t GL2Renderer::createColorBufferTexture(int32_t width, int32_t height, int32_t cubeMapTextureId, int32_t cubeMapTextureIndex)
{
	uint32_t colorBufferTextureId;
	// create color texture
	glGenTextures(1, &colorBufferTextureId);
	glBindTexture(GL_TEXTURE_2D, colorBufferTextureId);
	// create color texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<Buffer*>(nullptr));
	// color texture parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// unbind, return
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	return colorBufferTextureId;
}

void GL2Renderer::uploadTexture(int contextIdx, GUITexture* texture)
{
	//
	auto textureTextureData = texture->getRGBTextureData();
	//
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		texture->getRGBDepthBitsPerPixel() == 32?GL_RGBA:GL_RGB,
		texture->getTextureWidth(),
		texture->getTextureHeight(),
		0,
		texture->getRGBDepthBitsPerPixel() == 32?GL_RGBA:GL_RGB,
		GL_UNSIGNED_BYTE,
		textureTextureData.getBuffer()
	);
	if (texture->getAtlasSize() > 1) {
		if (texture->isUseMipMap() == true) {
			float maxLodBias;
			glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &maxLodBias);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -Math::clamp(static_cast<float>(texture->getAtlasSize()) * 0.125f, 0.0f, maxLodBias));
			auto borderSize = 32;
			auto maxLevel = 0;
			while (borderSize > 4) {
				maxLevel++;
				borderSize/= 2;
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel - 1);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	} else {
		if (texture->isUseMipMap() == true) glGenerateMipmap(GL_TEXTURE_2D);
		if (texture->isRepeat() == true) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		} else {
			float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
			if (texture->getClampMode() == GUITexture::CLAMPMODE_TRANSPARENTPIXEL) glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->getClampMode() == GUITexture::CLAMPMODE_EDGE?GL_CLAMP_TO_EDGE:GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->getClampMode() == GUITexture::CLAMPMODE_EDGE?GL_CLAMP_TO_EDGE:GL_CLAMP_TO_BORDER);
		}
	}
	switch (texture->getMinFilter()) {
		case GUITexture::TEXTUREFILTER_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); break;
		case GUITexture::TEXTUREFILTER_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
		case GUITexture::TEXTUREFILTER_NEAREST_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->isUseMipMap() == true?GL_NEAREST_MIPMAP_NEAREST:GL_NEAREST); break;
		case GUITexture::TEXTUREFILTER_LINEAR_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->isUseMipMap() == true?GL_LINEAR_MIPMAP_NEAREST:GL_NEAREST); break;
		case GUITexture::TEXTUREFILTER_NEAREST_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->isUseMipMap() == true?GL_NEAREST_MIPMAP_LINEAR:GL_LINEAR); break;
		case GUITexture::TEXTUREFILTER_LINEAR_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->isUseMipMap() == true?GL_LINEAR_MIPMAP_LINEAR:GL_LINEAR); break;
	}
	switch (texture->getMagFilter()) {
		case GUITexture::TEXTUREFILTER_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); break;
		case GUITexture::TEXTUREFILTER_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); break;
		case GUITexture::TEXTUREFILTER_NEAREST_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->isUseMipMap() == true?GL_NEAREST_MIPMAP_NEAREST:GL_NEAREST); break;
		case GUITexture::TEXTUREFILTER_LINEAR_MIPMAP_NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->isUseMipMap() == true?GL_LINEAR_MIPMAP_NEAREST:GL_NEAREST); break;
		case GUITexture::TEXTUREFILTER_NEAREST_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->isUseMipMap() == true?GL_NEAREST_MIPMAP_LINEAR:GL_LINEAR); break;
		case GUITexture::TEXTUREFILTER_LINEAR_MIPMAP_LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->isUseMipMap() == true?GL_LINEAR_MIPMAP_LINEAR:GL_LINEAR); break;
	}
	statistics.textureUploads++;
}

void GL2Renderer::resizeColorBufferTexture(int32_t textureId, int32_t width, int32_t height)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GL2Renderer::bindTexture(int contextIdx, int32_t textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	onBindTexture(contextIdx, textureId);
}

void GL2Renderer::disposeTexture(int32_t textureId)
{
	glDeleteTextures(1, (const uint32_t*)&textureId);
	statistics.disposedTextures++;
}

vector<int32_t> GL2Renderer::createBufferObjects(int32_t buffers, bool useGPUMemory, bool shared)
{
	vector<int32_t> bufferObjectIds;
	bufferObjectIds.resize(buffers);
	glGenBuffers(buffers, (uint32_t*)bufferObjectIds.data());
	for (auto bufferObjectId: bufferObjectIds) vbosUsage[bufferObjectId] = useGPUMemory == true?GL_STATIC_DRAW:GL_STREAM_DRAW;
	return bufferObjectIds;
}

void GL2Renderer::uploadBufferObject(int contextIdx, int32_t bufferObjectId, int32_t size, FloatBuffer* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, size, data->getBuffer(), vbosUsage[bufferObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, ID_NONE);
	statistics.bufferUploads++;
}

void GL2Renderer::uploadBufferObject(int contextIdx, int32_t bufferObjectId, int32_t size, IntBuffer* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, size, data->getBuffer(), vbosUsage[bufferObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, ID_NONE);
	statistics.bufferUploads++;
}

void GL2Renderer::uploadBufferObject(int contextIdx, int32_t bufferObjectId, int32_t size, ShortBuffer* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, size, data->getBuffer(), vbosUsage[bufferObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, ID_NONE);
	statistics.bufferUploads++;
}

void GL2Renderer::uploadIndicesBufferObject(int contextIdx, int32_t bufferObjectId, int32_t size, ShortBuffer* data)
{
	Console::printLine(string("GL2Renderer::uploadIndicesBufferObject()::not implemented yet"));
}

void GL2Renderer::uploadIndicesBufferObject(int contextIdx, int32_t bufferObjectId, int32_t size, IntBuffer* data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data->getBuffer(), vbosUsage[bufferObjectId]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID_NONE);
	statistics.bufferUploads++;
}

void GL2Renderer::bindIndicesBufferObject(int contextIdx, int32_t bufferObjectId)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjectId);
}

void GL2Renderer::bindSolidColorsBufferObject(int contextIdx, int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindTextureCoordinatesBufferObject(int contextIdx, int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindVertices2BufferObject(int contextIdx, int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindColorsBufferObject(int contextIdx, int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::drawIndexedTrianglesFromBufferObjects(int contextIdx, int32_t triangles, int32_t trianglesOffset)
{
	#define BUFFER_OFFSET(i) ((void*)(i))
	glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, BUFFER_OFFSET(static_cast<int64_t>(trianglesOffset) * 3LL * 4LL));
	statistics.renderCalls++;
	statistics.triangles+= triangles;
}

void GL2Renderer::unbindBufferObjects(int contextIdx)
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GL2Renderer::disposeBufferObjects(vector<int32_t>& bufferObjectIds)
{
	for (auto bufferObjectId: bufferObjectIds) vbosUsage.erase(bufferObjectId);
	glDeleteBuffers(bufferObjectIds.size(), (const uint32_t*)bufferObjectIds.data());
	statistics.disposedBuffers+= bufferObjectIds.size();
}

int32_t GL2Renderer::getTextureUnit(int contextIdx)
{
	return activeTextureUnit;
}

void GL2Renderer::setTextureUnit(int contextIdx, int32_t textureUnit)
{
	this->activeTextureUnit = textureUnit;
	glActiveTexture(GL_TEXTURE0 + textureUnit);
}

ByteBuffer* GL2Renderer::readPixels(int32_t x, int32_t y, int32_t width, int32_t height)
{
	auto pixelBuffer = ByteBuffer::allocate(width * height * 4);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer->getBuffer());
	return pixelBuffer;
}

void GL2Renderer::initGUIMode()
{
	setTextureUnit(CONTEXTINDEX_DEFAULT, 0);
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glGetError();
}

void GL2Renderer::doneGUIMode()
{
	glGetError();
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void GL2Renderer::setVSync(bool vSync) {
	// no op
}

const GUIRendererBackend::Renderer_Statistics GL2Renderer::getStatistics() {
	auto stats = statistics;
	statistics.time = Time::getCurrentMillis();
	statistics.memoryUsageGPU = -1LL;
	statistics.memoryUsageShared = -1LL;
	statistics.clearCalls = 0;
	statistics.renderCalls = 0;
	statistics.triangles = 0;
	statistics.bufferUploads = 0;
	statistics.textureUploads = 0;
	statistics.renderPasses = 0;
	statistics.drawCommands = 0;
	statistics.submits = 0;
	statistics.disposedTextures = 0;
	statistics.disposedBuffers = 0;
	return stats;
}
