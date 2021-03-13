#include <tdme/engine/subsystems/lighting/LightingShaderTerrainEditorImplementation.h>

#include <string>

#include <tdme/engine/subsystems/lighting/LightingShaderTerrainImplementation.h>
#include <tdme/engine/subsystems/lighting/LightingShaderConstants.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>

using std::string;
using std::to_string;

using tdme::engine::subsystems::lighting::LightingShaderConstants;
using tdme::engine::subsystems::lighting::LightingShaderTerrainEditorImplementation;
using tdme::engine::subsystems::lighting::LightingShaderTerrainImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;

LightingShaderTerrainEditorImplementation::LightingShaderTerrainEditorImplementation(Renderer* renderer): LightingShaderTerrainImplementation(renderer)
{
	additionalDefinitions = "\n#define HAVE_TERRAIN_SHADER_EDITOR";
}

const string LightingShaderTerrainEditorImplementation::getId() {
	return "terraineditor";
}

void LightingShaderTerrainEditorImplementation::initialize()
{
	LightingShaderTerrainImplementation::initialize();

	if (initialized == false) return;

	//
	initialized = false;

	uniformBrushTextureUnit = renderer->getProgramUniformLocation(programId, "brushTextureUnit");
	if (uniformBrushTextureUnit == -1) return;

	uniformBrushPosition = renderer->getProgramUniformLocation(programId, "brushPosition");
	if (uniformBrushPosition == -1) return;

	uniformBrushTextureDimension = renderer->getProgramUniformLocation(programId, "brushTextureDimension");
	if (uniformBrushTextureDimension == -1) return;

	//
	initialized = true;
}

void LightingShaderTerrainEditorImplementation::registerShader() {
	Engine::registerShader(
		Engine::ShaderType::SHADERTYPE_OBJECT3D,
		getId(),
		{
			{ "brushDimension", ShaderParameter(Vector2(0.0f, 0.0f)) },
			{ "brushTexture", ShaderParameter(0) },
			{ "brushPosition", ShaderParameter(Vector2(0.0f, 0.0f)) }
		}
	);
}

void LightingShaderTerrainEditorImplementation::useProgram(Engine* engine, void* context) {
	LightingShaderTerrainImplementation::useProgram(engine, context);

	//
	auto currentTextureUnit = renderer->getTextureUnit(context);
	renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_BRUSH);
	renderer->bindTexture(context, engine->getShaderParameter(getId(), "brushTexture").getIntValue());
	renderer->setTextureUnit(context, currentTextureUnit);

	//
	renderer->setProgramUniformInteger(context, uniformBrushTextureUnit, LightingShaderConstants::SPECULAR_TEXTUREUNIT_TERRAIN_BRUSH);
	renderer->setProgramUniformFloatVec2(context, uniformBrushTextureDimension, engine->getShaderParameter(getId(), "brushDimension").getVector2Value().getArray());
	renderer->setProgramUniformFloatVec2(context, uniformBrushPosition, engine->getShaderParameter(getId(), "brushPosition").getVector2Value().getArray());
}
