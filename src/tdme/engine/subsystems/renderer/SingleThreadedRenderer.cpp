#include <tdme/engine/subsystems/renderer/SingleThreadedRenderer.h>

#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/Time.h>

using tdme::engine::subsystems::renderer::SingleThreadedRenderer;

using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::utilities::Time;

SingleThreadedRenderer::SingleThreadedRenderer()
{
	// set up lights
	for (auto i = 0; i < lights.size(); i++) {
		lights[i].spotCosCutoff = static_cast< float >(Math::cos(Math::PI / 180.0f * 180.0f));
	}
}

SingleThreadedRenderer::~SingleThreadedRenderer() {
}

int32_t SingleThreadedRenderer::getLighting(void* context) {
	return lighting;
}

void SingleThreadedRenderer::setLighting(void* context, int32_t lighting) {
	this->lighting = lighting;
}

Matrix2D3x3& SingleThreadedRenderer::getTextureMatrix(void* context) {
	return textureMatrix;
}

Renderer::Renderer_Light& SingleThreadedRenderer::getLight(void* context, int32_t lightId) {
	return lights[lightId];
}

array<float, 4>& SingleThreadedRenderer::getEffectColorMul(void* context) {
	return effectColorMul;
}

array<float, 4>& SingleThreadedRenderer::getEffectColorAdd(void* context) {
	return effectColorAdd;
}

Renderer::Renderer_SpecularMaterial& SingleThreadedRenderer::getSpecularMaterial(void* context) {
	return specularMaterial;
}

Renderer::Renderer_PBRMaterial& SingleThreadedRenderer::getPBRMaterial(void* context) {
	return pbrMaterial;
}

const string& SingleThreadedRenderer::getShader(void* context) {
	return shader;
}

void SingleThreadedRenderer::setShader(void* context, const string& id) {
	shader = id;
}

const string& SingleThreadedRenderer::getShaderParametersHash(void* context) {
	return shaderParametersHash;
}

const map<string, string>& SingleThreadedRenderer::getShaderParameters(void* context) {
	return shaderParameters;
}

void SingleThreadedRenderer::setShaderParameters(void* context, const map<string, string>& parameters) {
	shaderParameters = parameters;
}

float SingleThreadedRenderer::getMaskMaxValue(void* context) {
	return maskMaxValue;
}

void SingleThreadedRenderer::setMaskMaxValue(void* context, float maskMaxValue) {
	this->maskMaxValue = maskMaxValue;
}

array<float, 3>& SingleThreadedRenderer::getEnvironmentMappingCubeMapPosition(void* context) {
	return environmentMappingCubeMapPosition;
}

void SingleThreadedRenderer::setEnvironmentMappingCubeMapPosition(void* context, array<float, 3>& position) {
	this->environmentMappingCubeMapPosition = position;
}

const Renderer::Renderer_Statistics SingleThreadedRenderer::getStatistics() {
	auto stats = statistics;
	statistics.time = Time::getCurrentMillis();
	statistics.memoryUsageGPU = -1LL;
	statistics.memoryUsageShared = -1LL;
	statistics.clearCalls = 0;
	statistics.renderCalls = 0;
	statistics.computeCalls = 0;
	statistics.triangles = 0;
	statistics.points = 0;
	statistics.linePoints = 0;
	statistics.bufferUploads = 0;
	statistics.textureUploads = 0;
	statistics.renderPasses = 0;
	statistics.drawCommands = 0;
	statistics.submits = 0;
	return stats;
}
