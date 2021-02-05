#include <tdme/engine/Light.h>

#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/subsystems/manager/TextureManager.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/Engine.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>

using tdme::engine::fileio::textures::Texture;
using tdme::engine::model::Color4;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::math::Vector4;

Light::Light()
{
	this->renderer = nullptr;
	this->id = -1;
	enabled = false;
	ambient.set(0.0f, 0.0f, 0.0f, 1.0f);
	diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
	specular.set(1.0f, 1.0f, 1.0f, 1.0f);
	position.set(0.0f, 0.0f, 0.0f, 0.0f);
	spotDirection.set(0.0f, 0.0f, -1.0f);
	spotExponent = 0.0f;
	spotCutOff = 180.0f;
	constantAttenuation = 1.0f;
	linearAttenuation = 0.0f;
	quadraticAttenuation = 0.0f;
	renderLightSource = false;
	lightSourceSize = 0.25f;
	lightSourceTextureId = 0;
}

Light::Light(Renderer* renderer, int32_t id)
{
	this->renderer = renderer;
	this->id = id;
	enabled = false;
	ambient.set(0.0f, 0.0f, 0.0f, 1.0f);
	diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
	specular.set(1.0f, 1.0f, 1.0f, 1.0f);
	position.set(0.0f, 0.0f, 0.0f, 0.0f);
	spotDirection.set(0.0f, 0.0f, -1.0f);
	spotExponent = 0.0f;
	spotCutOff = 180.0f;
	constantAttenuation = 1.0f;
	linearAttenuation = 0.0f;
	quadraticAttenuation = 0.0f;
	renderLightSource = false;
	lightSourceSize = 0.25f;
	lightSourceTextureId = 0;
}

void Light::setLightSourceTexture(Texture* texture) {
	if (lightSourceTexture == texture) return;
	if (lightSourceTexture != nullptr) lightSourceTexture->releaseReference();
	lightSourceTexture = texture;
	lightSourceTextureId = Engine::getInstance()->getTextureManager()->addTexture(lightSourceTexture);
}

void Light::dispose() {
	if (lightSourceTexture != nullptr) lightSourceTexture->releaseReference();
}

void Light::update(void* context) {
	if (enabled == true) {
		Vector4 lightPositionTransformed;
		Vector3 tmpVector3;
		Vector4 spotDirection4;
		Vector4 spotDirection4Transformed;
		auto& light = renderer->getLight(context, id);
		light.enabled = 1;
		light.ambient = ambient.getArray();
		light.diffuse = diffuse.getArray();
		light.position = position.getArray();
		light.spotDirection = spotDirection.getArray();
		light.spotExponent = spotExponent;
		light.spotCosCutoff = static_cast<float>(Math::cos(Math::PI / 180.0f * spotCutOff));
		light.constantAttenuation = constantAttenuation;
		light.linearAttenuation = linearAttenuation;
		light.quadraticAttenuation = quadraticAttenuation;
		renderer->onUpdateLight(context, id);
	} else {
		auto& light = renderer->getLight(context, id);
		light.enabled = 0;
		renderer->onUpdateLight(context, id);
	}
}
