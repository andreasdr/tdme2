#include <tdme/engine/subsystems/renderer/GL2Renderer.h>

#if defined(__APPLE__)
        #include <OpenGL/gl.h>
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__linux__) || defined(_WIN32) || defined(__HAIKU__)
	#define GLEW_NO_GLU
	#include <GL/glew.h>
#endif

#include <string.h>

#include <array>
#include <map>
#include <vector>
#include <string>

#include <tdme/utilities/Buffer.h>
#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/FloatBuffer.h>
#include <tdme/utilities/IntBuffer.h>
#include <tdme/utilities/ShortBuffer.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>

using std::array;
using std::map;
using std::vector;
using std::string;
using std::to_string;

using tdme::engine::subsystems::renderer::GL2Renderer;
using tdme::utilities::Buffer;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::IntBuffer;
using tdme::utilities::ShortBuffer;
using tdme::engine::Engine;
using tdme::engine::fileio::textures::Texture;
using tdme::math::Matrix4x4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;

GL2Renderer::GL2Renderer()
{
	// setup GL2 consts
	ID_NONE = 0;
	CLEAR_DEPTH_BUFFER_BIT = GL_DEPTH_BUFFER_BIT;
	CLEAR_COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT;
	CULLFACE_FRONT = GL_FRONT;
	CULLFACE_BACK = GL_BACK;
	FRONTFACE_CW = GL_CW;
	FRONTFACE_CCW = GL_CCW;
	SHADER_FRAGMENT_SHADER = GL_FRAGMENT_SHADER;
	SHADER_VERTEX_SHADER = GL_VERTEX_SHADER;
	SHADER_GEOMETRY_SHADER = -1;
	SHADER_COMPUTE_SHADER = -1;
	DEPTHFUNCTION_ALWAYS = GL_ALWAYS;
	DEPTHFUNCTION_EQUAL = GL_EQUAL;
	DEPTHFUNCTION_LESSEQUAL = GL_LEQUAL;
	DEPTHFUNCTION_GREATEREQUAL = GL_GEQUAL;
	// TODO: buffer objects available
	activeTextureUnit = 0;
	bufferObjectsAvailable = true;
}

const string GL2Renderer::getShaderVersion()
{
	return "gl2";
}

bool GL2Renderer::isSupportingMultithreadedRendering() {
	return false;
}

bool GL2Renderer::isSupportingMultipleRenderQueues() {
	return false;
}

bool GL2Renderer::isSupportingVertexArrays() {
	return false;
}

bool GL2Renderer::checkBufferObjectsAvailable()
{
	// see: http://www.felixgers.de/teaching/jogl/vertexBufferObject.html
	// Check if extension is available.
	auto extensionOK = true; // isExtensionAvailable(u"GL_ARB_vertex_buffer_object"_j);
	// Check for VBO functions.
	auto functionsOK = true; // isFunctionAvailable(u"glGenBuffersARB"_j) && isFunctionAvailable(u"glBindBufferARB"_j) && isFunctionAvailable(u"glBufferDataARB"_j)&& isFunctionAvailable(u"glDeleteBuffersARB"_j);
	return extensionOK == true && functionsOK == true;
}

bool GL2Renderer::isDepthTextureAvailable()
{
	return true;
}

void GL2Renderer::initialize()
{
	glGetError();
	// get default framebuffer
	FRAMEBUFFER_DEFAULT = 0; // getContext()->getDefaultDrawFramebuffer();
	// check VBO
	bufferObjectsAvailable = checkBufferObjectsAvailable();
	// setup open gl
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glDisable(GL_BLEND);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
	setTextureUnit(nullptr, 0);
}

void GL2Renderer::initializeFrame()
{
	Renderer::initializeFrame();
}

void GL2Renderer::finishFrame()
{
}

bool GL2Renderer::isBufferObjectsAvailable()
{
	return bufferObjectsAvailable;
}

bool GL2Renderer::isUsingProgramAttributeLocation()
{
	return true;
}

bool GL2Renderer::isSupportingIntegerProgramAttributes() {
	return false;
}

bool GL2Renderer::isSpecularMappingAvailable()
{
	return false;
}

bool GL2Renderer::isNormalMappingAvailable()
{
	return false;
}

bool GL2Renderer::isInstancedRenderingAvailable() {
	return false;
}

bool GL2Renderer::isPBRAvailable()
{
	return false;
}

bool GL2Renderer::isComputeShaderAvailable() {
	return false;
}

bool GL2Renderer::isGLCLAvailable() {
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
	int32_t handle = glCreateShader(type);
	// exit if no handle returned
	if (handle == 0) return 0;
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
	//
	string sourceString = (shaderSource);
	char* sourceHeap = new char[sourceString.length() + 1];
	strcpy(sourceHeap, sourceString.c_str());
	// load source
	glShaderSource(handle, 1, &sourceHeap, nullptr);
	// compile
	glCompileShader(handle);
	//
	delete [] sourceHeap;
	// check state
	int32_t compileStatus;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compileStatus);
	// get error
	if (compileStatus == 0) {
		int32_t infoLogLengthBuffer;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLengthBuffer);
		char* infoLogBuffer = new char[infoLogLengthBuffer];
		glGetShaderInfoLog(handle, infoLogLengthBuffer, &infoLogLengthBuffer, infoLogBuffer);
		auto infoLogString = (string(infoLogBuffer, infoLogLengthBuffer));
		// be verbose
		Console::println(
			string(
				string("GL2Renderer::loadShader") +
				string("[") +
				to_string(handle) +
				string("]") +
				pathName +
				"/" +
				fileName +
				string(": failed: ") +
				infoLogString
			 )
		 );
		//
		delete [] infoLogBuffer;
		// remove shader
		glDeleteShader(handle);
		//
		return 0;
	}

	//
	return handle;
}

void GL2Renderer::useProgram(void* context, int32_t programId)
{
	glUseProgram(programId);
}

int32_t GL2Renderer::createProgram(int type)
{
	auto programId = glCreateProgram();
	return programId;
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
		int32_t infoLogLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(programId, infoLogLength, &infoLogLength, infoLog);
		auto infoLogString = (string(infoLog, infoLogLength));
		// be verbose
		Console::println(
			string(
				"[" +
				to_string(programId) +
				"]: failed: " +
				infoLogString
			 )
		);
		delete [] infoLog;
		return false;
	}
	return true;
}

int32_t GL2Renderer::getProgramUniformLocation(int32_t programId, const string& name)
{
	auto uniformLocation = glGetUniformLocation(programId, (name).c_str());
	return uniformLocation;
}

void GL2Renderer::setProgramUniformInteger(void* context, int32_t uniformId, int32_t value)
{
	glUniform1i(uniformId, value);
}

void GL2Renderer::setProgramUniformFloat(void* context, int32_t uniformId, float value)
{
	glUniform1f(uniformId, value);
}

void GL2Renderer::setProgramUniformFloatMatrices4x4(void* context, int32_t uniformId, int32_t count, FloatBuffer* data)
{
	glUniformMatrix4fv(uniformId, count, false, (float*)data->getBuffer());
}

void GL2Renderer::setProgramUniformFloatMatrix3x3(void* context, int32_t uniformId, const array<float, 9>& data)
{
	glUniformMatrix3fv(uniformId, 1, false, data.data());
}

void GL2Renderer::setProgramUniformFloatMatrix4x4(void* context, int32_t uniformId, const array<float, 16>& data)
{
	glUniformMatrix4fv(uniformId, 1, false, data.data());
}

void GL2Renderer::setProgramUniformFloatVec4(void* context, int32_t uniformId, const array<float, 4>& data)
{
	glUniform4fv(uniformId, 1, data.data());
}

void GL2Renderer::setProgramUniformFloatVec3(void* context, int32_t uniformId, const array<float, 3>& data)
{
	glUniform3fv(uniformId, 1, data.data());
}

void GL2Renderer::setProgramUniformFloatVec2(void* context, int32_t uniformId, const array<float, 2>& data)
{
	glUniform2fv(uniformId, 1, data.data());
}

void GL2Renderer::setProgramAttributeLocation(int32_t programId, int32_t location, const string& name)
{
	glBindAttribLocation(programId, location, (name).c_str());
}

void GL2Renderer::setViewPort(int32_t x, int32_t y, int32_t width, int32_t height)
{
	this->viewPortX = x;
	this->viewPortY = y;
	this->viewPortWidth = width;
	this->viewPortHeight = height;
}

void GL2Renderer::updateViewPort()
{
	glViewport(viewPortX, viewPortY, viewPortWidth, viewPortHeight);
}

void GL2Renderer::setClearColor(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
}

void GL2Renderer::enableCulling(void* context)
{
	glEnable(GL_CULL_FACE);
}

void GL2Renderer::disableCulling(void* context)
{
	glDisable(GL_CULL_FACE);
}

void GL2Renderer::setFrontFace(void* context, int32_t frontFace)
{
	glFrontFace(frontFace);
}

void GL2Renderer::setCullFace(int32_t cullFace)
{
	glCullFace(cullFace);
}

void GL2Renderer::enableBlending()
{
	glEnable(GL_BLEND);
}

void GL2Renderer::disableBlending()
{
	glDisable(GL_BLEND);
}

void GL2Renderer::enableDepthBufferWriting()
{
	glDepthMask(true);
}

void GL2Renderer::disableDepthBufferWriting()
{
	glDepthMask(false);
}

void GL2Renderer::disableDepthBufferTest()
{
	glDisable(GL_DEPTH_TEST);
}

void GL2Renderer::enableDepthBufferTest()
{
	glEnable(GL_DEPTH_TEST);
}

void GL2Renderer::setDepthFunction(int32_t depthFunction)
{
	glDepthFunc(depthFunction);
}

void GL2Renderer::setColorMask(bool red, bool green, bool blue, bool alpha)
{
	glColorMask(red, green, blue, alpha);
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

int32_t GL2Renderer::createDepthBufferTexture(int32_t width, int32_t height)
{
	uint32_t depthTextureGlId;
	// create depth texture
	glGenTextures(1, &depthTextureGlId);
	glBindTexture(GL_TEXTURE_2D, depthTextureGlId);
	// create depth texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	// depth texture parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// unbind, return
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	return depthTextureGlId;
}

int32_t GL2Renderer::createColorBufferTexture(int32_t width, int32_t height)
{
	uint32_t colorBufferTextureGlId;
	// create color texture
	glGenTextures(1, &colorBufferTextureGlId);
	glBindTexture(GL_TEXTURE_2D, colorBufferTextureGlId);
	// create color texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast< Buffer* >(nullptr));
	// color texture parameter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// unbind, return
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	return colorBufferTextureGlId;
}

void GL2Renderer::uploadTexture(void* context, Texture* texture)
{
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		texture->getDepth() == 32?GL_RGBA:GL_RGB,
		texture->getTextureWidth(),
		texture->getTextureHeight(),
		0,
		texture->getDepth() == 32?GL_RGBA:GL_RGB,
		GL_UNSIGNED_BYTE,
		texture->getTextureData()->getBuffer()
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->isUseMipMap() == true?GL_LINEAR_MIPMAP_LINEAR:GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	statistics.textureUploads++;
}

void GL2Renderer::uploadCubeMapTexture(void* context, Texture* textureLeft, Texture* textureRight, Texture* textureTop, Texture* textureBottom, Texture* textureFront, Texture* textureBack) {
	glTexImage2D(
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		0,
		textureLeft->getDepth() == 32 ? GL_RGBA : GL_RGB,
		textureLeft->getTextureWidth(),
		textureLeft->getTextureHeight(),
		0,
		textureLeft->getDepth() == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
		textureLeft->getTextureData()->getBuffer()
	);
	glTexImage2D(
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		0,
		textureRight->getDepth() == 32 ? GL_RGBA : GL_RGB,
		textureRight->getTextureWidth(),
		textureRight->getTextureHeight(),
		0,
		textureRight->getDepth() == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
		textureRight->getTextureData()->getBuffer()
	);
	glTexImage2D(
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		0,
		textureTop->getDepth() == 32 ? GL_RGBA : GL_RGB,
		textureTop->getTextureWidth(),
		textureTop->getTextureHeight(),
		0,
		textureTop->getDepth() == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
		textureTop->getTextureData()->getBuffer()
	);
	glTexImage2D(
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		0,
		textureBottom->getDepth() == 32 ? GL_RGBA : GL_RGB,
		textureBottom->getTextureWidth(),
		textureBottom->getTextureHeight(),
		0,
		textureBottom->getDepth() == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
		textureBottom->getTextureData()->getBuffer()
	);
	glTexImage2D(
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		0,
		textureFront->getDepth() == 32 ? GL_RGBA : GL_RGB,
		textureFront->getTextureWidth(),
		textureFront->getTextureHeight(),
		0,
		textureFront->getDepth() == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
		textureFront->getTextureData()->getBuffer()
	);
	glTexImage2D(
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		0,
		textureBack->getDepth() == 32 ? GL_RGBA : GL_RGB,
		textureBack->getTextureWidth(),
		textureBack->getTextureHeight(),
		0,
		textureBack->getDepth() == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
		textureBack->getTextureData()->getBuffer()
	);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	statistics.textureUploads+= 6;
}

void GL2Renderer::resizeDepthBufferTexture(int32_t textureId, int32_t width, int32_t height)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GL2Renderer::resizeColorBufferTexture(int32_t textureId, int32_t width, int32_t height)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GL2Renderer::bindTexture(void* context, int32_t textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	onBindTexture(context, textureId);
}

void GL2Renderer::bindCubeMapTexture(void* context, int32_t textureId) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	onBindTexture(context, textureId);
}

void GL2Renderer::disposeTexture(int32_t textureId)
{
	glDeleteTextures(1, (const uint32_t*)&textureId);
}

int32_t GL2Renderer::createFramebufferObject(int32_t depthBufferTextureGlId, int32_t colorBufferTextureGlId)
{
	// create a frame buffer object
	uint32_t frameBufferGlId;
	glGenFramebuffers(1, &frameBufferGlId);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferGlId);
	// attach the depth buffer texture to FBO
	if (depthBufferTextureGlId != ID_NONE) {
		#ifdef __APPLE__
			glFramebufferTextureEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBufferTextureGlId, 0);
		#else
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBufferTextureGlId, 0);
		#endif
	}
	// attach the color buffer texture to FBO
	if (colorBufferTextureGlId != ID_NONE) {
		#ifdef __APPLE__
			glFramebufferTextureEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBufferTextureGlId, 0);
		#else
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferTextureGlId, 0);
		#endif
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
	} else {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	// check FBO status
	int32_t fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Console::println(string("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO: ") + to_string(fboStatus));
	}
	// switch back to window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return frameBufferGlId;
}

void GL2Renderer::bindFrameBuffer(int32_t frameBufferId)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
}

void GL2Renderer::disposeFrameBufferObject(int32_t frameBufferId)
{
	glDeleteFramebuffers(1, (uint32_t*)&frameBufferId);
}

vector<int32_t> GL2Renderer::createBufferObjects(int32_t buffers, bool useGPUMemory, bool shared)
{
	vector<int32_t> bufferObjectIds;
	bufferObjectIds.resize(buffers);
	glGenBuffers(buffers, (uint32_t*)bufferObjectIds.data());
	for (auto bufferObjectId: bufferObjectIds) vbosUsage[bufferObjectId] = useGPUMemory == true?GL_STATIC_DRAW:GL_STREAM_DRAW;
	return bufferObjectIds;
}

void GL2Renderer::uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, FloatBuffer* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, size, data->getBuffer(), vbosUsage[bufferObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, ID_NONE);
	statistics.bufferUploads++;
}

void GL2Renderer::uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, size, data->getBuffer(), vbosUsage[bufferObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, ID_NONE);
	statistics.bufferUploads++;
}

void GL2Renderer::uploadBufferObject(void* context, int32_t bufferObjectId, int32_t size, ShortBuffer* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, size, data->getBuffer(), vbosUsage[bufferObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, ID_NONE);
	statistics.bufferUploads++;
}

void GL2Renderer::uploadIndicesBufferObject(void* context, int32_t bufferObjectId, int32_t size, ShortBuffer* data)
{
	Console::println(string("GL2Renderer::uploadIndicesBufferObject()::not implemented yet"));
}

void GL2Renderer::uploadIndicesBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjectId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data->getBuffer(), vbosUsage[bufferObjectId]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID_NONE);
	statistics.bufferUploads++;
}

void GL2Renderer::bindIndicesBufferObject(void* context, int32_t bufferObjectId)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjectId);
}

void GL2Renderer::bindTextureCoordinatesBufferObject(void* context, int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindVerticesBufferObject(void* context, int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindNormalsBufferObject(void* context, int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindColorsBufferObject(void* context, int32_t bufferObjectId)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindTangentsBufferObject(void* context, int32_t bufferObjectId)
{
	Console::println(string("GL2Renderer::bindTangentsBufferObject()::not implemented yet"));
}

void GL2Renderer::bindBitangentsBufferObject(void* context, int32_t bufferObjectId)
{
	Console::println(string("GL2Renderer::bindBitangentsBufferObject()::not implemented yet"));
}

void GL2Renderer::bindModelMatricesBufferObject(void* context, int32_t bufferObjectId) {
	Console::println(string("GL2Renderer::bindModelViewMatricesBufferObject()::not implemented yet"));
}

void GL2Renderer::bindEffectColorMulsBufferObject(void* context, int32_t bufferObjectId, int32_t divisor) {
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(10);
	glVertexAttribPointer(10, 4, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindEffectColorAddsBufferObject(void* context, int32_t bufferObjectId, int32_t divisor) {
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(11);
	glVertexAttribPointer(11, 4, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindOriginsBufferObject(void* context, int32_t bufferObjectId) {
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindTextureSpriteIndicesBufferObject(void* context, int32_t bufferObjectId) {
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindPointSizesBufferObject(void* context, int32_t bufferObjectId) {
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::bindSpriteSheetDimensionBufferObject(void* context, int32_t bufferObjectId) {
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjectId);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 2, GL_FLOAT, false, 0, 0LL);
}

void GL2Renderer::drawInstancedIndexedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset, int32_t instances)
{
	Console::println(string("GL2Renderer::drawInstancedIndexedTrianglesFromBufferObjects()::not implemented yet"));
}

void GL2Renderer::drawIndexedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset)
{
	#define BUFFER_OFFSET(i) ((void*)(i))
	glDrawElements(GL_TRIANGLES, triangles * 3, GL_UNSIGNED_INT, BUFFER_OFFSET(static_cast< int64_t >(trianglesOffset) * 3LL * 4LL));
	statistics.renderCalls++;
	statistics.triangles+= triangles;
}

void GL2Renderer::drawInstancedTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset, int32_t instances) {
	Console::println(string("GL2Renderer::drawInstancedTrianglesFromBufferObjects()::not implemented yet"));
}

void GL2Renderer::drawTrianglesFromBufferObjects(void* context, int32_t triangles, int32_t trianglesOffset)
{
	glDrawArrays(GL_TRIANGLES, trianglesOffset * 3, triangles * 3);
	statistics.renderCalls++;
	statistics.triangles+= triangles;
}

void GL2Renderer::drawPointsFromBufferObjects(void* context, int32_t points, int32_t pointsOffset)
{
	glDrawArrays(GL_POINTS, pointsOffset, points);
	statistics.renderCalls++;
	statistics.points+= points;
}

void GL2Renderer::setLineWidth(float lineWidth)
{
	glLineWidth(lineWidth);
}

void GL2Renderer::drawLinesFromBufferObjects(void* context, int32_t points, int32_t pointsOffset)
{
	glDrawArrays(GL_LINES, pointsOffset, points);
	statistics.renderCalls++;
	statistics.linePoints+= points;
}

void GL2Renderer::unbindBufferObjects(void* context)
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
	for (auto& bufferObjectId: bufferObjectIds) vbosUsage.erase(bufferObjectId);
	glDeleteBuffers(bufferObjectIds.size(), (const uint32_t*)bufferObjectIds.data());
}

int32_t GL2Renderer::getTextureUnit(void* context)
{
	return activeTextureUnit;
}

void GL2Renderer::setTextureUnit(void* context, int32_t textureUnit)
{
	this->activeTextureUnit = textureUnit;
	glActiveTexture(GL_TEXTURE0 + textureUnit);
}

float GL2Renderer::readPixelDepth(int32_t x, int32_t y)
{
	float depth;
	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	return depth;
}

ByteBuffer* GL2Renderer::readPixels(int32_t x, int32_t y, int32_t width, int32_t height)
{
	auto pixelBuffer = ByteBuffer::allocate(width * height * 4);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer->getBuffer());
	return pixelBuffer;
}

void GL2Renderer::initGuiMode()
{
	setTextureUnit(nullptr, 0);
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glGetError();
}

void GL2Renderer::doneGuiMode()
{
	glGetError();
	glBindTexture(GL_TEXTURE_2D, ID_NONE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void GL2Renderer::dispatchCompute(void* context, int32_t numGroupsX, int32_t numGroupsY, int32_t numGroupsZ) {
	Console::println("GL2Renderer::dispatchCompute(): Not implemented");
}

void GL2Renderer::memoryBarrier() {
	Console::println("GL2Renderer::memoryBarrier(): Not implemented");
}

void GL2Renderer::uploadSkinningBufferObject(void* context, int32_t bufferObjectId, int32_t size, FloatBuffer* data) {
	Console::println("GL2Renderer::uploadSkinningBufferObject(): Not implemented");
}

void GL2Renderer::uploadSkinningBufferObject(void* context, int32_t bufferObjectId, int32_t size, IntBuffer* data) {
	Console::println("GL2Renderer::uploadSkinningBufferObject(): Not implemented");
}

void GL2Renderer::bindSkinningVerticesBufferObject(void* context, int32_t bufferObjectId) {
	Console::println("GL2Renderer::bindSkinningVerticesBufferObject(): Not implemented");
}

void GL2Renderer::bindSkinningNormalsBufferObject(void* context, int32_t bufferObjectId) {
	Console::println("GL2Renderer::bindSkinningNormalsBufferObject(): Not implemented");
}

void GL2Renderer::bindSkinningVertexJointsBufferObject(void* context, int32_t bufferObjectId) {
	Console::println("GL2Renderer::bindSkinningVertexJointsBufferObject(): Not implemented");
}

void GL2Renderer::bindSkinningVertexJointIdxsBufferObject(void* context, int32_t bufferObjectId) {
	Console::println("GL2Renderer::bindSkinningVertexJointIdxsBufferObject(): Not implemented");
}

void GL2Renderer::bindSkinningVertexJointWeightsBufferObject(void* context, int32_t bufferObjectId) {
	Console::println("GL2Renderer::bindSkinningVertexJointWeightsBufferObject(): Not implemented");
}

void GL2Renderer::bindSkinningVerticesResultBufferObject(void* context, int32_t bufferObjectId) {
	Console::println("GL2Renderer::bindSkinningVerticesResultBufferObject(): Not implemented");
}

void GL2Renderer::bindSkinningNormalsResultBufferObject(void* context, int32_t bufferObjectId) {
	Console::println("GL2Renderer::bindSkinningNormalsResultBufferObject(): Not implemented");
}

void GL2Renderer::bindSkinningMatricesBufferObject(void* context, int32_t bufferObjectId) {
	Console::println("GL2Renderer::bindSkinningMatricesBufferObject(): Not implemented");
}

int32_t GL2Renderer::createVertexArrayObject() {
	Console::println("GL2Renderer::createVertexArrayObject(): Not implemented");
	return -1;
}

void GL2Renderer::disposeVertexArrayObject(int32_t vertexArrayObjectId) {
	Console::println("GL2Renderer::disposeVertexArrayObject(): Not implemented");
}

void GL2Renderer::bindVertexArrayObject(int32_t vertexArrayObjectId) {
	Console::println("GL2Renderer::bindVertexArrayObject(): Not implemented");
}
