#include <string>

#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderBaseImplementation.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShaderLightScatteringImplementation.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/ShaderParameter.h>
#include <tdme/math/Math.h>

using std::string;

using tdme::engine::subsystems::postprocessing::PostProcessingShaderLightScatteringImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::engine::ShaderParameter;
using tdme::math::Math;

bool PostProcessingShaderLightScatteringImplementation::isSupported(Renderer* renderer) {
	return renderer->getShaderVersion() == "gl3";
}

PostProcessingShaderLightScatteringImplementation::PostProcessingShaderLightScatteringImplementation(Renderer* renderer): PostProcessingShaderBaseImplementation(renderer)
{
}

void PostProcessingShaderLightScatteringImplementation::initialize()
{
	auto shaderVersion = renderer->getShaderVersion();

	//	fragment shader
	fragmentShaderId = renderer->loadShader(
		renderer->SHADER_FRAGMENT_SHADER,
		"shader/" + shaderVersion + "/postprocessing",
		"light_scattering_fragmentshader.frag"
	);
	if (fragmentShaderId == 0) return;

	//	vertex shader
	vertexShaderId = renderer->loadShader(
		renderer->SHADER_VERTEX_SHADER,
		"shader/" + shaderVersion + "/postprocessing",
		"light_scattering_vertexshader.vert"
	);
	if (vertexShaderId == 0) return;

	// create, attach and link program
	programId = renderer->createProgram(renderer->PROGRAM_OBJECTS);
	renderer->attachShaderToProgram(programId, vertexShaderId);
	renderer->attachShaderToProgram(programId, fragmentShaderId);

	//
	PostProcessingShaderBaseImplementation::initialize();

	// register shader
	if (initialized == false) return;

	//	lights
	for (auto i = 0; i < Engine::LIGHTS_MAX; i++) {
		uniformLightEnabled[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) +"].enabled");
		uniformLightPosition[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) +"].position");
		uniformLightIntensity[i] = renderer->getProgramUniformLocation(programId, "lights[" + to_string(i) +"].intensity");
	}

	//
	Engine::registerShader(
		Engine::ShaderType::SHADERTYPE_POSTPROCESSING,
		"light_scattering"
	);
}

void PostProcessingShaderLightScatteringImplementation::setShaderParameters(void* context, Engine* engine) {
	int _width = engine->getScaledWidth() != -1?engine->getScaledWidth():engine->getWidth();
	int _height = engine->getScaledHeight() != -1?engine->getScaledHeight():engine->getHeight();
	for (auto i = 0; i < engine->getLightCount(); i++) {
		auto light = engine->getLightAt(i);
		if (light->isEnabled() == false || light->isRenderLightSource() == false) {
			renderer->setProgramUniformInteger(context, uniformLightEnabled[i], 0);
			continue;
		}
		Vector2 lightSourcePosition2D;
		Vector3 lightSourcePosition = Vector3(light->getPosition().getX(), light->getPosition().getY(), light->getPosition().getZ());
		lightSourcePosition.scale(1.0f / light->getPosition().getW());
		engine->computeScreenCoordinateByWorldCoordinate(lightSourcePosition, lightSourcePosition2D);
		lightSourcePosition2D.setX(Math::clamp(lightSourcePosition2D.getX() / static_cast<float>(_width), 0.0f, 1.0f));
		lightSourcePosition2D.setY(Math::clamp(1.0f - (lightSourcePosition2D.getY() / static_cast<float>(_height)), 0.0f, 1.0f));
		float intensity = 1.0f;
		float _intensity = 1.0f;
		if (lightSourcePosition2D.getX() < 0.4f) _intensity = lightSourcePosition2D.getX() / 0.4f;
		if (_intensity < intensity) intensity = _intensity;
		if (lightSourcePosition2D.getX() > 0.6f) _intensity = (1.0f - lightSourcePosition2D.getX()) / 0.4f;
		if (_intensity < intensity) intensity = _intensity;
		if (lightSourcePosition2D.getY() < 0.4f) _intensity = lightSourcePosition2D.getY() / 0.4f;
		if (_intensity < intensity) intensity = _intensity;
		if (lightSourcePosition2D.getY() > 0.6f) _intensity = (1.0f - lightSourcePosition2D.getY()) / 0.4f;
		if (_intensity < intensity) intensity = _intensity;
		if (intensity < Math::EPSILON) {
			renderer->setProgramUniformInteger(context, uniformLightEnabled[i], 0);
		} else {
			#if defined(VULKAN)
				lightSourcePosition2D = Vector2(lightSourcePosition2D.getX(), 1.0f - lightSourcePosition2D.getY());
			#endif
			renderer->setProgramUniformInteger(context, uniformLightEnabled[i], 1);
			renderer->setProgramUniformFloatVec2(context, uniformLightPosition[i], lightSourcePosition2D.getArray());
			renderer->setProgramUniformFloat(context, uniformLightIntensity[i], intensity);
		}
	}
}
