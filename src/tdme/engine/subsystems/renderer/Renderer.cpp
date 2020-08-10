#include <tdme/engine/subsystems/renderer/Renderer.h>

#include <tdme/math/Math.h>
#include <tdme/engine/subsystems/renderer/Renderer_Light.h>
#include <tdme/engine/subsystems/renderer/Renderer_SpecularMaterial.h>
#include <tdme/math/Matrix4x4.h>

using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Math;
using tdme::utils::FloatBuffer;
using tdme::engine::subsystems::renderer::Renderer_Light;
using tdme::engine::subsystems::renderer::Renderer_SpecularMaterial;
using tdme::math::Matrix4x4;

Renderer::Renderer()
{
	ID_NONE = -1;
	CLEAR_DEPTH_BUFFER_BIT = -1;
	CLEAR_COLOR_BUFFER_BIT = -1;
	CULLFACE_FRONT = -1;
	CULLFACE_BACK = -1;
	TEXTUREUNITS_MAX = -1;
	PROGRAM_OBJECTS = 1;
	PROGRAM_POINTS = 2;
	PROGRAM_LINES = 3;
	PROGRAM_COMPUTE = 4;
	SHADER_FRAGMENT_SHADER = -1;
	SHADER_VERTEX_SHADER = -1;
	SHADER_GEOMETRY_SHADER = -1;
	SHADER_COMPUTE_SHADER = -1;
	DEPTHFUNCTION_ALWAYS = -1;
	DEPTHFUNCTION_EQUAL = -1;
	DEPTHFUNCTION_LESSEQUAL = -1;
	DEPTHFUNCTION_GREATEREQUAL = -1;
	FRAMEBUFFER_DEFAULT = -1;
	FRONTFACE_CW = -1;
	FRONTFACE_CCW = -1;
	LIGHTING_NONE = 0;
	LIGHTING_SPECULAR = 1;
	LIGHTING_PBR = 2;
	UNIFORM_CL_SKINNING_VERTEX_COUNT = -1;
	UNIFORM_CL_SKINNING_MATRIX_COUNT = -1;
	UNIFORM_CL_SKINNING_INSTANCE_COUNT = -1;
	viewPortX = 0;
	viewPortY = 0;
	viewPortWidth = 0;
	viewPortHeight = 0;
	TEXTUREUNITS_MAX = 2;
	projectionMatrix.identity();
	cameraMatrix.identity();
	modelViewMatrix.identity();
	viewportMatrix.identity();
	effectPass = 0;
}

Renderer::~Renderer() {
}

void* Renderer::getDefaultContext() {
	return nullptr;
}

void* Renderer::getContext(int contextIdx) {
	return nullptr;
}

int Renderer::getContextIndex(void* context) {
	return 0;
}

Vector3& Renderer::getCameraPosition() {
	return cameraPosition;
}

Matrix4x4& Renderer::getProjectionMatrix()
{
	return projectionMatrix;
}

Matrix4x4& Renderer::getCameraMatrix()
{
	return cameraMatrix;
}

Matrix4x4& Renderer::getModelViewMatrix()
{
	return modelViewMatrix;
}

Matrix4x4& Renderer::getViewportMatrix()
{
	return viewportMatrix;
}

int32_t Renderer::getViewPortWidth() {
	return viewPortWidth;
}

int32_t Renderer::getViewPortHeight() {
	return viewPortHeight;
}

void Renderer::initializeFrame() {
}

int32_t Renderer::getEffectPass() {
	return effectPass;
}

void Renderer::setEffectPass(int32_t effectPass) {
	this->effectPass = effectPass;
}

const string& Renderer::getShaderPrefix() {
	return shaderPrefix;
}

void Renderer::setShaderPrefix(const string& shaderPrefix) {
	this->shaderPrefix = shaderPrefix;
}
