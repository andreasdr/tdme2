#include <tdme/engine/subsystems/environmentmapping/EnvironmentMapping.h>

#include <vector>

#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer.h>
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
#include <tdme/utilities/Time.h>

using std::vector;

using tdme::engine::subsystems::environmentmapping::EnvironmentMapping;

using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::EntityRenderer;
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
using tdme::utilities::Time;

EnvironmentMapping::EnvironmentMapping(Engine* engine, int32_t width, int32_t height)
{
	this->engine = engine;
	this->width = width;
	this->height = height;
	camera = new Camera(engine->renderer);
	camera->setCameraMode(Camera::CAMERAMODE_NONE);
}

EnvironmentMapping::~EnvironmentMapping() {
	delete camera;
}

void EnvironmentMapping::initialize()
{
	cubeMapTextureId = engine->renderer->createCubeMapTexture(engine->renderer->getDefaultContext(), width, height);
	for (auto i = 0; i < frameBuffers.size(); i++) {
		frameBuffers[i] = new FrameBuffer(width, height, FrameBuffer::FRAMEBUFFER_COLORBUFFER | FrameBuffer::FRAMEBUFFER_DEPTHBUFFER, cubeMapTextureId, i + 1);
		frameBuffers[i]->initialize();
	}
}

void EnvironmentMapping::reshape(int32_t width, int32_t height)
{
}

void EnvironmentMapping::dispose()
{
	for (auto i = 0; i < frameBuffers.size(); i++) {
		frameBuffers[i]->dispose();
		delete frameBuffers[i];
	}
	engine->renderer->disposeTexture(cubeMapTextureId);
}

void EnvironmentMapping::render(const Vector3& position)
{
	auto now = Time::getCurrentMillis();
	if (timeRenderLast != -1LL && now - timeRenderLast < timeRenderUpdateFrequency) return;
	timeRenderLast = now;

	//
	auto engineCamera = engine->getCamera();

	//
	for (auto i = 0; i < frameBuffers.size(); i++) {
		// bind frame buffer
		frameBuffers[i]->enableFrameBuffer();

		// set up camera
		camera->setZNear(engineCamera->getZNear());
		camera->setZFar(engineCamera->getZFar());
		camera->setFovY(90.0f);
		camera->setLookFrom(position);
		camera->setForwardVector(forwardVectors[i]);
		camera->setSideVector(sideVectors[i]);
		camera->setUpVector(upVectors[i]);
		camera->update(engine->renderer->getDefaultContext(), width, height);

		// set up clear color
		Engine::renderer->setClearColor(
			engine->sceneColor.getRed(),
			engine->sceneColor.getGreen(),
			engine->sceneColor.getBlue(),
			engine->sceneColor.getAlpha()
		);

		// clear previous frame values
		Engine::renderer->clear(engine->renderer->CLEAR_DEPTH_BUFFER_BIT | engine->renderer->CLEAR_COLOR_BUFFER_BIT);

		//
		engine->computeTransformations(camera->getFrustum(), visibleDecomposedEntities, false, false);

		// do a render pass
		engine->render(
			visibleDecomposedEntities,
			Engine::EFFECTPASS_NONE,
			renderPassMask,
			string(),
			true,
			false,
			false,
			true,
			true,
			EntityRenderer::RENDERTYPE_NORMALS |
				EntityRenderer::RENDERTYPE_TEXTUREARRAYS |
				EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY |
				EntityRenderer::RENDERTYPE_EFFECTCOLORS |
				EntityRenderer::RENDERTYPE_MATERIALS |
				EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY |
				EntityRenderer::RENDERTYPE_TEXTURES |
				EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY |
				EntityRenderer::RENDERTYPE_LIGHTS
		);

		//
		engine->resetLists(visibleDecomposedEntities);
	}

	// unbind frame buffer
	FrameBuffer::disableFrameBuffer();
}
