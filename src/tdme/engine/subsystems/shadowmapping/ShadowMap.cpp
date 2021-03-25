#include <tdme/engine/subsystems/shadowmapping/ShadowMap.h>

#include <vector>

#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapping.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityHierarchy.h>
#include <tdme/engine/FrameBuffer.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Object3DRenderGroup.h>
#include <tdme/engine/ObjectParticleSystem.h>
#include <tdme/engine/ParticleSystemGroup.h>
#include <tdme/engine/Partition.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using std::vector;

using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::shadowmapping::ShadowMap;
using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityHierarchy;
using tdme::engine::FrameBuffer;
using tdme::engine::Light;
using tdme::engine::LODObject3D;
using tdme::engine::Object3D;
using tdme::engine::Object3DRenderGroup;
using tdme::engine::ObjectParticleSystem;
using tdme::engine::Partition;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

ShadowMap::ShadowMap(ShadowMapping* shadowMapping, int32_t width, int32_t height)
{
	this->shadowMapping = shadowMapping;
	lightCamera = new Camera(shadowMapping->renderer);
	lightCamera->setCameraMode(Camera::CAMERAMODE_NONE);
	frameBuffer = new FrameBuffer(width, height, FrameBuffer::FRAMEBUFFER_DEPTHBUFFER);
	#if defined(VULKAN)
		biasMatrix.set(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f
		);
	#else
		biasMatrix.set(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
		);
	#endif
	depthBiasMVPMatrix.identity();
}

ShadowMap::~ShadowMap() {
	delete lightCamera;
	delete frameBuffer;
}

int32_t ShadowMap::getWidth()
{
	return frameBuffer->getWidth();
}

int32_t ShadowMap::getHeight()
{
	return frameBuffer->getHeight();
}

void ShadowMap::initialize()
{
	frameBuffer->initialize();
}

void ShadowMap::reshape(int32_t width, int32_t height)
{
}

void ShadowMap::dispose()
{
	frameBuffer->dispose();
}

void ShadowMap::bindDepthBufferTexture(void* context)
{
	frameBuffer->bindDepthBufferTexture(context);
}

Camera* ShadowMap::getCamera()
{
	return lightCamera;
}

void ShadowMap::createShadowMap(Light* light)
{
	// use default context
	// TODO: object->preRender only uses default context, lets see how to make this multithreaded
	auto context = shadowMapping->renderer->getDefaultContext();

	// clear visible objects
	visibleObjects.clear();

	// determine visible objects and objects that should generate a shadow
	for (auto entity: shadowMapping->engine->getPartition()->getVisibleEntities(shadowMapping->engine->getCamera()->getFrustum())) {
		switch (entity->getEntityType()) {
			case Entity::ENTITYTYPE_OBJECT3DRENDERGROUP:
				{
					auto org = static_cast<Object3DRenderGroup*>(entity);
					auto orgEntity = org->getEntity();
					if (orgEntity != nullptr) {
						if (orgEntity->isContributesShadows() == false) continue;
						switch(orgEntity->getEntityType()) {
							case Entity::ENTITYTYPE_OBJECT3D:
								{
									auto object = static_cast<Object3D*>(orgEntity);
									object->preRender(context);
									visibleObjects.push_back(object);
								}
								break;
							case Entity::ENTITYTYPE_LODOBJECT3D:
								{
									auto lodObject = static_cast<LODObject3D*>(orgEntity);
									if (lodObject->isContributesShadows() == false) continue;
									auto object = lodObject->getLODObject();
									if (object != nullptr) {
										object->preRender(context);
										visibleObjects.push_back(object);
									}
								}
								break;
							default:
								break;
						}
					}
				}
				break;
			case Entity::ENTITYTYPE_OBJECT3D:
				{
					auto object = static_cast<Object3D*>(entity);
					if (object->isContributesShadows() == false) continue;
					object->preRender(context);
					visibleObjects.push_back(object);
				}
				break;
			case Entity::ENTITYTYPE_LODOBJECT3D:
				{
					auto lodObject = static_cast<LODObject3D*>(entity);
					if (lodObject->isContributesShadows() == false) continue;
					auto object = lodObject->getLODObject();
					if (object != nullptr) {
						object->preRender(context);
						visibleObjects.push_back(object);
					}
				}
				break;
			case Entity::ENTITYTYPE_OBJECTPARTICLESYSTEM:
				{
					auto opse = static_cast<ObjectParticleSystem*>(entity);
					if (opse->isContributesShadows() == false) continue;
					for (auto object: opse->getEnabledObjects()) {
						object->preRender(context);
						visibleObjects.push_back(object);
					}
				}
				break;
			case Entity::ENTITYTYPE_PARTICLESYSTEMGROUP:
				{
					auto psg = static_cast<ParticleSystemGroup*>(entity);
					for (auto ps: psg->getParticleSystems()) {
						if (ps->getEntityType() != Entity::ENTITYTYPE_OBJECTPARTICLESYSTEM) continue;
						auto opse = static_cast<ObjectParticleSystem*>(ps);
						if (opse->isContributesShadows() == false) continue;
						for (auto object: opse->getEnabledObjects()) {
							object->preRender(context);
							visibleObjects.push_back(object);
						}
					}
				}
				break;
			case Entity::ENTITYTYPE_ENTITYHIERARCHY:
				{
					auto eh = static_cast<EntityHierarchy*>(entity);
					if (eh->isContributesShadows() == false) continue;
					for (auto entity: eh->getEntities()) {
						if (entity->getEntityType() != Entity::ENTITYTYPE_OBJECT3D) continue;
						auto object = static_cast<Object3D*>(entity);
						if (object->isEnabled() == false) continue;
						object->preRender(context);
						visibleObjects.push_back(object);
					}
				}
				break;
			default:
				break;
		}
	}

	//
	auto camera = shadowMapping->engine->getCamera();

	// try to determine light position

	// 	left
	Vector4 left;
	camera->getModelViewProjectionInvertedMatrix().multiply(
		Vector4(
			(2.0f * 0.0f) - 1.0f,
			1.0f - (2.0f * 0.5f),
			2.0f * 0.997f - 1.0f,
			1.0f
		),
		left
	);
	left.scale(1.0f / left.getW());

	//	right
	Vector4 right;
	camera->getModelViewProjectionInvertedMatrix().multiply(
		Vector4(
			(2.0f * 1.0f) - 1.0f,
			1.0f - (2.0f * 0.5f),
			2.0f * 0.997f - 1.0f,
			1.0f
		),
		right
	);
	right.scale(1.0f / right.getW());

	//	center
	Vector4 center4;
	camera->getModelViewProjectionInvertedMatrix().multiply(
		Vector4(
			(2.0f * 0.5f) - 1.0f,
			1.0f - (2.0f * 1.0f),
			2.0f * 0.5f - 1.0f,
			1.0f
		),
		center4
	);
	center4.scale(1.0f / center4.getW());

	// so we get some contraints for the shadow map camera, TODO: improve me
	Vector3 center(Vector3(center4.getX(), center4.getY(), center4.getZ()));
	auto width = Vector3(right.getX(), right.getY(), right.getZ()).sub(Vector3(left.getX(), left.getY(), left.getZ())).computeLength() * shadowMapping->engine->getShadowMapLightEyeDistanceScale();

	// viewers camera
	Vector3 lightDirection;
	Vector3 lightLookFrom;
	// compute camera from view of light
	lightDirection.set(light->getSpotDirection()).normalize();
	lightLookFrom
		.set(center)
		.sub(lightDirection.clone().scale(width * 1.25f));
	// set up light camera from view of light
	Vector3 lightCameraUpVector;
	Vector3 lightCameraSideVector;
	lightCamera->setZNear(camera->getZNear());
	lightCamera->setZFar(150.0f);
	lightCamera->setLookFrom(lightLookFrom);
	lightCamera->setForwardVector(lightDirection);
	lightCamera->setSideVector(Vector3(1.0f, 0.0f, 0.0f));
	// TODO: fix cross product NaN if side vector == forward vector
	Vector3::computeCrossProduct(lightCamera->getForwardVector(), lightCamera->getSideVector(), lightCameraUpVector);
	lightCamera->setUpVector(lightCameraUpVector);
	Vector3::computeCrossProduct(lightCamera->getForwardVector(), lightCamera->getUpVector(), lightCameraSideVector);
	lightCamera->setSideVector(lightCameraSideVector);
	lightCamera->setUpVector(lightCameraUpVector);
	lightCamera->update(context, frameBuffer->getWidth(), frameBuffer->getHeight());
	// bind frame buffer
	frameBuffer->enableFrameBuffer();
	// clear depth buffer
	shadowMapping->renderer->clear(shadowMapping->renderer->CLEAR_DEPTH_BUFFER_BIT);
	// generate shadow map texture matrix
	computeDepthBiasMVPMatrix();
	// only draw opaque face entities as shadows will not be produced from transparent objects
	for (auto i = 0; i < Entity::RENDERPASS_MAX; i++) {
		auto renderPass = static_cast<Entity::RenderPass>(Math::pow(2, i));
		shadowMapping->entityRenderer->render(
			renderPass,
			visibleObjects,
			false,
			EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY |
			EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY
		);
	}
}

void ShadowMap::computeDepthBiasMVPMatrix()
{
	// matrices
	auto modelViewMatrix = shadowMapping->renderer->getModelViewMatrix();
	auto projectionMatrix = shadowMapping->renderer->getProjectionMatrix();
	// compute shadow texture matrix
	depthBiasMVPMatrix.set(modelViewMatrix).multiply(projectionMatrix).multiply(biasMatrix);
}

void ShadowMap::updateDepthBiasMVPMatrix(void* context)
{
	shadowMapping->updateDepthBiasMVPMatrix(context, depthBiasMVPMatrix);
}

FrameBuffer* ShadowMap::getFrameBuffer() {
	return frameBuffer;
}
