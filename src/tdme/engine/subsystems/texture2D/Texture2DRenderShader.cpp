#include <tdme/engine/subsystems/texture2D/Texture2DRenderShader.h>

#include <tdme/engine/Engine.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/rendering/ObjectBuffer.h>
#include <tdme/engine/subsystems/manager/VBOManager.h>
#include <tdme/engine/subsystems/manager/VBOManager_VBOManaged.h>

#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/FloatBuffer.h>

using tdme::engine::subsystems::texture2D::Texture2DRenderShader;
using tdme::engine::Engine;
using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::manager::VBOManager_VBOManaged;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::ObjectBuffer;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;

Texture2DRenderShader::Texture2DRenderShader(Renderer* renderer)
{
	this->renderer = renderer;
	initialized = false;
	isRunning = false;
}

Texture2DRenderShader::~Texture2DRenderShader()
{
	Engine::getInstance()->getVBOManager()->removeVBO("texture2d_render_shader.vbo");
}

bool Texture2DRenderShader::isInitialized()
{
	return initialized;
}

void Texture2DRenderShader::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();
	vertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/texture2D",
		"render_vertexshader.vert"
	);
	if (vertexShaderId == 0) return;

	fragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/texture2D",
		"render_fragmentshader.frag"
	);
	if (fragmentShaderId == 0) return;

	programId = renderer->createProgram(renderer->PROGRAM_OBJECTS);
	renderer->attachShaderToProgram(programId, vertexShaderId);
	renderer->attachShaderToProgram(programId, fragmentShaderId);
	if (renderer->isUsingProgramAttributeLocation() == true) {
		renderer->setProgramAttributeLocation(programId, 0, "inVertex");
		renderer->setProgramAttributeLocation(programId, 2, "inTextureUV");
	}
	if (renderer->linkProgram(programId) == false) return;

	// uniforms
	uniformTextureUnit = renderer->getProgramUniformLocation(programId, "textureUnit");
	if (uniformTextureUnit == -1) return;

	// create vbos
	auto created = false;
	auto vboManaged = Engine::getInstance()->getVBOManager()->addVBO("texture2d_render_shader.vbo", 2, false, false, created);
	vboVertices = (*vboManaged->getVBOIds())[0];
	vboTextureCoordinates = (*vboManaged->getVBOIds())[1];

	//
	initialized = true;
}

void Texture2DRenderShader::useProgram()
{
	auto context = renderer->getDefaultContext();
	renderer->useProgram(context, programId);
	renderer->setLighting(context, renderer->LIGHTING_NONE);
	renderer->setProgramUniformInteger(context, uniformTextureUnit, 0);
	isRunning = true;
}

void Texture2DRenderShader::unUseProgram()
{
	isRunning = false;
}

void Texture2DRenderShader::renderTexture(Engine* engine, const Vector2& position, const Vector2& dimension, int textureId, int width, int height) {
		//
	auto context = renderer->getDefaultContext();

	//
	auto screenWidth = width != -1?width:(engine->getScaledWidth() == -1?engine->getWidth():engine->getScaledWidth());
	auto screenHeight = height != -1?height:(engine->getScaledHeight() == -1?engine->getHeight():engine->getScaledHeight());
	float textureLeft = position.getX();
	float textureTop = position.getY();
	float textureWidth = dimension.getX();
	float textureHeight = dimension.getY();

	//
	auto x0 = ((textureLeft) / (screenWidth / 2.0f)) - 1.0f;
	auto y0 = ((screenHeight - textureTop) / (screenHeight / 2.0f)) - 1.0f;
	auto x1 = ((textureLeft + textureWidth) / (screenWidth / 2.0f)) - 1.0f;
	auto y1 = ((screenHeight - textureTop) / (screenHeight / 2.0f)) - 1.0f;
	auto x2 = ((textureLeft + textureWidth) / (screenWidth / 2.0f)) - 1.0f;
	auto y2 = ((screenHeight - textureTop - textureHeight) / (screenHeight / 2.0f)) - 1.0f;
	auto x3 = ((textureLeft) / (screenWidth / 2.0f)) - 1.0f;
	auto y3 = ((screenHeight - textureTop - textureHeight) / (screenHeight / 2.0f)) - 1.0f;

	// texture coordinates
	{
		auto fbTextureCoordinates = ObjectBuffer::getByteBuffer(context, 6 * 2 * sizeof(float))->asFloatBuffer();

		#if defined(VULKAN)
			fbTextureCoordinates.put(+0.0f); fbTextureCoordinates.put(0.0f);
			fbTextureCoordinates.put(+1.0f); fbTextureCoordinates.put(0.0f);
			fbTextureCoordinates.put(+1.0f); fbTextureCoordinates.put(1.0f);

			fbTextureCoordinates.put(+1.0f); fbTextureCoordinates.put(1.0f);
			fbTextureCoordinates.put(+0.0f); fbTextureCoordinates.put(1.0f);
			fbTextureCoordinates.put(+0.0f); fbTextureCoordinates.put(0.0f);
		#else
			fbTextureCoordinates.put(+0.0f); fbTextureCoordinates.put(+1.0f);
			fbTextureCoordinates.put(+1.0f); fbTextureCoordinates.put(+1.0f);
			fbTextureCoordinates.put(+1.0f); fbTextureCoordinates.put(0.0f);

			fbTextureCoordinates.put(+1.0f); fbTextureCoordinates.put(0.0f);
			fbTextureCoordinates.put(+0.0f); fbTextureCoordinates.put(0.0f);
			fbTextureCoordinates.put(+0.0f); fbTextureCoordinates.put(+1.0f);
		#endif

		renderer->uploadBufferObject(context, vboTextureCoordinates, fbTextureCoordinates.getPosition() * sizeof(float), &fbTextureCoordinates);
	}

	// vertices
	{
		auto fbVertices = ObjectBuffer::getByteBuffer(context, 6 * 3 * sizeof(float))->asFloatBuffer();

		fbVertices.put(x0); fbVertices.put(y0); fbVertices.put(0.0f);
		fbVertices.put(x1); fbVertices.put(y1); fbVertices.put(0.0f);
		fbVertices.put(x2); fbVertices.put(y2); fbVertices.put(0.0f);

		fbVertices.put(x2); fbVertices.put(y2); fbVertices.put(0.0f);
		fbVertices.put(x3); fbVertices.put(y3); fbVertices.put(0.0f);
		fbVertices.put(x0); fbVertices.put(y0); fbVertices.put(0.0f);

		renderer->uploadBufferObject(context, vboVertices, fbVertices.getPosition() * sizeof(float), &fbVertices);
	}

	// disable culling
	renderer->enableBlending();
	renderer->disableCulling(context);

	// use program
	useProgram();

	// bind color buffer texture
	renderer->setTextureUnit(context, 0);
	renderer->bindTexture(context, textureId);

	//
	renderer->bindVerticesBufferObject(context, vboVertices);
	renderer->bindTextureCoordinatesBufferObject(context, vboTextureCoordinates);

	// draw
	renderer->drawTrianglesFromBufferObjects(context, 2, 0);

	// unbind buffers
	renderer->unbindBufferObjects(context);

	// unbind texture
	renderer->bindTexture(context, renderer->ID_NONE);

	// unuse program
	unUseProgram();

	// enabe culling
	renderer->enableCulling(context);
	renderer->disableBlending();

}
