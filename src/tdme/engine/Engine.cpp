#include <tdme/engine/Engine.h>

#if !defined(VULKAN)
	#if ((defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)) && !defined(GLES2)) || defined(_WIN32) || defined(__HAIKU__)
		#define GLEW_NO_GLU
		#include <GL/glew.h>
	#endif
#endif
#if defined(VULKAN) || defined(GLFW3)
	#if defined(VULKAN)
		#define GLFW_INCLUDE_VULKAN
	#else
		#define GLFW_INCLUDE_NONE
	#endif
	#include <GLFW/glfw3.h>
#endif

#include <algorithm>
#include <map>
#include <string>

#include <tdme/application/Application.h>
#if defined(VULKAN)
	#include <tdme/engine/EngineVKRenderer.h>
#else
	#include <tdme/engine/EngineGL2Renderer.h>
	#include <tdme/engine/EngineGL3Renderer.h>
	#include <tdme/engine/EngineGLES2Renderer.h>
#endif
#include <tdme/engine/fileio/textures/PNGTextureWriter.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/physics/CollisionDetection.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/LineSegment.h>
#include <tdme/engine/subsystems/earlyzrejection/EZRShader.h>
#include <tdme/engine/subsystems/environmentmapping/EnvironmentMapping.h>
#include <tdme/engine/subsystems/framebuffer/FrameBufferRenderShader.h>
#include <tdme/engine/subsystems/lighting/LightingShader.h>
#include <tdme/engine/subsystems/lines/LinesShader.h>
#include <tdme/engine/subsystems/manager/MeshManager.h>
#include <tdme/engine/subsystems/manager/TextureManager.h>
#include <tdme/engine/subsystems/manager/VBOManager.h>
#include <tdme/engine/subsystems/particlesystem/ParticlesShader.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessing.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingProgram.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingShader.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer_InstancedRenderFunctionParameters.h>
#include <tdme/engine/subsystems/rendering/Object3DBase_TransformedFacesIterator.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeMesh.h>
#include <tdme/engine/subsystems/rendering/ObjectBuffer.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesPool.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMap.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapping.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapCreationShader.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapRenderShader.h>
#include <tdme/engine/subsystems/skinning/SkinningShader.h>
#include <tdme/engine/subsystems/texture2D/Texture2DRenderShader.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityHierarchy.h>
#include <tdme/engine/EntityPickingFilter.h>
#include <tdme/engine/EnvironmentMapping.h>
#include <tdme/engine/FogParticleSystem.h>
#include <tdme/engine/FrameBuffer.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/LinesObject3D.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Object3DRenderGroup.h>
#include <tdme/engine/ObjectParticleSystem.h>
#include <tdme/engine/ParticleSystemEntity.h>
#include <tdme/engine/ParticleSystemGroup.h>
#include <tdme/engine/Partition.h>
#include <tdme/engine/PartitionOctTree.h>
#include <tdme/engine/PointsParticleSystem.h>
#include <tdme/engine/ShaderParameter.h>
#include <tdme/engine/Timing.h>
#include <tdme/gui/renderer/GUIRenderer.h>
#include <tdme/gui/renderer/GUIShader.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector2.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/VectorIteratorMultiple.h>

using std::map;
using std::remove;
using std::string;
using std::to_string;

using tdme::application::Application;
using tdme::engine::fileio::textures::PNGTextureWriter;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Node;
using tdme::engine::physics::CollisionDetection;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::LineSegment;
using tdme::engine::subsystems::earlyzrejection::EZRShader;
using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lines::LinesShader;
using tdme::engine::subsystems::manager::MeshManager;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::particlesystem::ParticlesShader;
using tdme::engine::subsystems::postprocessing::PostProcessing;
using tdme::engine::subsystems::postprocessing::PostProcessingProgram;
using tdme::engine::subsystems::postprocessing::PostProcessingShader;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::rendering::EntityRenderer_InstancedRenderFunctionParameters;
using tdme::engine::subsystems::rendering::Object3DBase_TransformedFacesIterator;
using tdme::engine::subsystems::rendering::ObjectBuffer;
using tdme::engine::subsystems::rendering::TransparentRenderFacesPool;
using tdme::engine::subsystems::shadowmapping::ShadowMap;
using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShader;
using tdme::engine::subsystems::shadowmapping::ShadowMapRenderShader;
using tdme::engine::subsystems::skinning::SkinningShader;
using tdme::engine::subsystems::texture2D::Texture2DRenderShader;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::EngineGL2Renderer;
using tdme::engine::EngineGL3Renderer;
using tdme::engine::EngineGLES2Renderer;
using tdme::engine::EngineVKRenderer;
using tdme::engine::Entity;
using tdme::engine::EntityHierarchy;
using tdme::engine::EntityPickingFilter;
using tdme::engine::FogParticleSystem;
using tdme::engine::FrameBuffer;
using tdme::engine::Light;
using tdme::engine::LinesObject3D;
using tdme::engine::LODObject3D;
using tdme::engine::Object3D;
using tdme::engine::Object3DRenderGroup;
using tdme::engine::ObjectParticleSystem;
using tdme::engine::ParticleSystemEntity;
using tdme::engine::ParticleSystemGroup;
using tdme::engine::Partition;
using tdme::engine::PartitionOctTree;
using tdme::engine::PointsParticleSystem;
using tdme::engine::ShaderParameter;
using tdme::engine::Timing;
using tdme::gui::renderer::GUIRenderer;
using tdme::gui::renderer::GUIShader;
using tdme::gui::GUI;
using tdme::gui::GUIParser;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector2;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::ByteBuffer;
using tdme::utilities::Console;
using tdme::utilities::Float;
using EnvironmentMappingEntity = tdme::engine::EnvironmentMapping;
using EnvironmentMappingImplementation = tdme::engine::subsystems::environmentmapping::EnvironmentMapping;

Engine* Engine::instance = nullptr;
Renderer* Engine::renderer = nullptr;
TextureManager* Engine::textureManager = nullptr;
VBOManager* Engine::vboManager = nullptr;
MeshManager* Engine::meshManager = nullptr;
GUIRenderer* Engine::guiRenderer = nullptr;
FrameBufferRenderShader* Engine::frameBufferRenderShader = nullptr;
PostProcessing* Engine::postProcessing = nullptr;
PostProcessingShader* Engine::postProcessingShader = nullptr;
Texture2DRenderShader* Engine::texture2DRenderShader = nullptr;
Engine::AnimationProcessingTarget Engine::animationProcessingTarget = Engine::AnimationProcessingTarget::CPU;
EZRShader* Engine::ezrShader = nullptr;
ShadowMapCreationShader* Engine::shadowMappingShaderPre = nullptr;
ShadowMapRenderShader* Engine::shadowMappingShaderRender = nullptr;
LightingShader* Engine::lightingShader = nullptr;
ParticlesShader* Engine::particlesShader = nullptr;
LinesShader* Engine::linesShader = nullptr;
SkinningShader* Engine::skinningShader = nullptr;
GUIShader* Engine::guiShader = nullptr;
Engine* Engine::currentEngine = nullptr;
bool Engine::skinningShaderEnabled = false;
int Engine::threadCount = 0;
bool Engine::have4K = false;
float Engine::animationBlendingTime = 250.0f;
int32_t Engine::shadowMapWidth = 0;
int32_t Engine::shadowMapHeight = 0;
int32_t Engine::shadowMapRenderLookUps = 0;
int32_t Engine::environmentMappingWidth = 512;
int32_t Engine::environmentMappingHeight = 512;
float Engine::transformationsComputingReduction1Distance = 25.0f;
float Engine::transformationsComputingReduction2Distance = 50.0f;
map<string, Engine::Shader> Engine::shaders;

vector<Engine::EngineThread*> Engine::engineThreads;

Engine::EngineThread::EngineThread(int idx, void* context):
	Thread("enginethread"),
	idx(idx),
	engine(nullptr),
	context(context) {
	//
	rendering.transparentRenderFacesPool = new TransparentRenderFacesPool();
}

void Engine::EngineThread::run() {
	Console::println("EngineThread::" + string(__FUNCTION__) + "()[" + to_string(idx) + "]: INIT");
	while (isStopRequested() == false) {
		switch(state) {
			case STATE_WAITING:
				while (state == STATE_WAITING) Thread::nanoSleep(100LL);
				break;
			case STATE_TRANSFORMATIONS:
				engine->computeTransformationsFunction(engine->visibleDecomposedEntities, threadCount, idx, transformations.computeTransformations);
				state = STATE_SPINNING;
				break;
			case STATE_RENDERING:
				rendering.transparentRenderFacesPool->reset();
				engine->entityRenderer->renderFunction(threadCount, idx, rendering.parameters.renderPass, rendering.parameters.objects, rendering.objectsByShadersAndModels, rendering.parameters.collectTransparentFaces, rendering.parameters.renderTypes, rendering.transparentRenderFacesPool);
				rendering.objectsByShadersAndModels.clear();
				state = STATE_SPINNING;
				break;
			case STATE_SPINNING:
				{
					volatile auto i = 0LL;
					while (state == STATE_SPINNING) i++;
				}
				break;
		}
	}
	Console::println("EngineThread::" + string(__FUNCTION__) + "()[" + to_string(idx) + "]: DONE");
}

Engine::Engine() {
	timing = new Timing();
	camera = nullptr;
	sceneColor.set(0.0f, 0.0f, 0.0f, 1.0f);
	frameBuffer = nullptr;
	// shadow mapping
	shadowMappingEnabled = false;
	shadowMapping = nullptr;
	// render process state
	renderingInitiated = false;
	renderingComputedTransformations = false;
	//
	initialized = false;
	// post processing frame buffers
	postProcessingFrameBuffer1 = nullptr;
	postProcessingFrameBuffer2 = nullptr;
	postProcessingTemporaryFrameBuffer = nullptr;
	//
	isUsingPostProcessingTemporaryFrameBuffer = false;
	//
	effectPassFrameBuffers.fill(nullptr);
	effectPassSkip.fill(false);
}

Engine::~Engine() {
	delete timing;
	delete camera;
	delete gui;
	delete partition;
	if (frameBuffer != nullptr) delete frameBuffer;
	if (postProcessingFrameBuffer1 != nullptr) delete postProcessingFrameBuffer1;
	if (postProcessingFrameBuffer2 != nullptr) delete postProcessingFrameBuffer2;
	if (postProcessingTemporaryFrameBuffer != nullptr) delete postProcessingTemporaryFrameBuffer;
	if (shadowMapping != nullptr) delete shadowMapping;
	delete entityRenderer;
	if (instance == this) {
		delete renderer;
		delete textureManager;
		delete vboManager;
		delete meshManager;
		delete guiRenderer;
		delete lightingShader;
		delete particlesShader;
		delete linesShader;
		delete frameBufferRenderShader;
		delete postProcessing;
		delete postProcessingShader;
		delete texture2DRenderShader;
		delete guiShader;
		delete ezrShader;
		delete shadowMappingShaderPre;
		delete shadowMappingShaderRender;
	}
	// set current engine
	if (currentEngine == this) currentEngine = nullptr;
}

Engine* Engine::getInstance()
{
	if (instance == nullptr) {
		instance = new Engine();
	}
	return instance;
}

Engine* Engine::createOffScreenInstance(int32_t width, int32_t height, bool enableShadowMapping)
{
	if (instance == nullptr || instance->initialized == false) {
		Console::println(string("Engine::createOffScreenInstance(): Engine not created or not initialized."));
		return nullptr;
	}
	// create off screen engine
	auto offScreenEngine = new Engine();
	offScreenEngine->initialized = true;
	// create GUI
	offScreenEngine->gui = new GUI(offScreenEngine, guiRenderer);
	// create object 3d vbo renderer
	offScreenEngine->entityRenderer = new EntityRenderer(offScreenEngine, renderer);
	offScreenEngine->entityRenderer->initialize();
	// create framebuffers
	offScreenEngine->frameBuffer = new FrameBuffer(width, height, FrameBuffer::FRAMEBUFFER_DEPTHBUFFER | FrameBuffer::FRAMEBUFFER_COLORBUFFER);
	offScreenEngine->frameBuffer->initialize();
	// create camera, frustum partition
	offScreenEngine->camera = new Camera(renderer);
	offScreenEngine->partition = new PartitionOctTree();
	// create lights
	for (auto i = 0; i < offScreenEngine->lights.size(); i++) {
		offScreenEngine->lights[i] = Light(renderer, i);
		offScreenEngine->lights[i].setLightSourceTexture(TextureReader::read("resources/engine/textures", "sun.png"));
	}
	// create shadow mapping
	if (instance->shadowMappingEnabled == true && enableShadowMapping == true) {
		offScreenEngine->shadowMapping = new ShadowMapping(offScreenEngine, renderer, offScreenEngine->entityRenderer);
	}
	//
	offScreenEngine->reshape(width, height);
	return offScreenEngine;
}

void Engine::setPartition(Partition* partition)
{
	if (this->partition != nullptr && this->partition != partition) delete this->partition;
	this->partition = partition;
}

void Engine::addEntity(Entity* entity)
{
	{
		auto entityByIdIt = entitiesById.find(entity->getId());
		if (entityByIdIt != entitiesById.end()) {
			// check if we want to add this entity a second time
			if (entity == entityByIdIt->second) {
				Console::println("Engine::addEntity(): " + entity->getId() + ": entity already added!");
				return;
			}
			// dispose old entity if any did exist in engine with same id
			removeEntity(entity->getId());
		}
	}

	// init entity
	entity->setEngine(this);
	entity->setRenderer(renderer);
	entity->initialize();
	entitiesById[entity->getId()] = entity;

	// add to partition if enabled and frustum culling requested
	if (entity->isFrustumCulling() == true && entity->isEnabled() == true) partition->addEntity(entity);

	// update
	registerEntity(entity);
}

void Engine::deregisterEntity(Entity* entity) {
	auto hierarchicalId = entity->getId();
	for (auto _entity = entity->getParentEntity(); _entity != nullptr; _entity = _entity->getParentEntity()) hierarchicalId = _entity->getId() + "." + hierarchicalId;

	//
	noFrustumCullingEntitiesById.erase(hierarchicalId);
	visibleDecomposedEntities.noFrustumCullingEntities.erase(
		remove(
			visibleDecomposedEntities.noFrustumCullingEntities.begin(),
			visibleDecomposedEntities.noFrustumCullingEntities.end(), entity
		),
		visibleDecomposedEntities.noFrustumCullingEntities.end());
	autoEmitParticleSystemEntities.erase(hierarchicalId);
}

void Engine::registerEntity(Entity* entity) {
	auto hierarchicalId = entity->getId();
	for (auto _entity = entity->getParentEntity(); _entity != nullptr; _entity = _entity->getParentEntity()) hierarchicalId = _entity->getId() + "." + hierarchicalId;

	//
	noFrustumCullingEntitiesById.erase(hierarchicalId);
	visibleDecomposedEntities.noFrustumCullingEntities.erase(
		remove(
			visibleDecomposedEntities.noFrustumCullingEntities.begin(),
			visibleDecomposedEntities.noFrustumCullingEntities.end(),
			entity
		),
		visibleDecomposedEntities.noFrustumCullingEntities.end()
	);
	autoEmitParticleSystemEntities.erase(hierarchicalId);

	// add to no frustum culling
	if (entity->isFrustumCulling() == false && entity->getParentEntity() == nullptr) {
		// otherwise add to no frustum culling entities
		noFrustumCullingEntitiesById[hierarchicalId] = entity;
		visibleDecomposedEntities.noFrustumCullingEntities.push_back(entity);
	}

	// add to auto emit particle system entities
	auto particleSystemEntity = dynamic_cast<ParticleSystemEntity*>(entity);
	if (particleSystemEntity != nullptr && particleSystemEntity->isAutoEmit() == true) {
		autoEmitParticleSystemEntities[hierarchicalId] = particleSystemEntity;
	}
}

void Engine::removeEntity(const string& id)
{
	// get entity and remove if we have any
	auto entityByIdIt = entitiesById.find(id);
	if (entityByIdIt == entitiesById.end()) return;

	//
	auto entity = entityByIdIt->second;

	//
	auto hierarchicalId = entity->getId();
	for (auto _entity = entity->getParentEntity(); _entity != nullptr; _entity = _entity->getParentEntity()) hierarchicalId = _entity->getId() + "." + hierarchicalId;

	//
	entitiesById.erase(entityByIdIt);
	autoEmitParticleSystemEntities.erase(hierarchicalId);
	noFrustumCullingEntitiesById.erase(hierarchicalId);

	// remove from partition if enabled and frustum culling requested
	if (entity->isFrustumCulling() == true && entity->isEnabled() == true) partition->removeEntity(entity);
	// dispose entity
	entity->setEngine(nullptr);
	entity->setRenderer(nullptr);
	entity->dispose();
	delete entity;

	// delete from lists
	visibleDecomposedEntities.objects.erase(
		remove(
			visibleDecomposedEntities.objects.begin(),
			visibleDecomposedEntities.objects.end(),
			entity
		),
		visibleDecomposedEntities.objects.end()
	);
	visibleDecomposedEntities.objectsPostPostProcessing.erase(
		remove(
			visibleDecomposedEntities.objectsPostPostProcessing.begin(),
			visibleDecomposedEntities.objectsPostPostProcessing.end(),
			entity
		),
		visibleDecomposedEntities.objectsPostPostProcessing.end()
	);
	visibleDecomposedEntities.objectsNoDepthTest.erase(
		remove(
			visibleDecomposedEntities.objectsNoDepthTest.begin(),
			visibleDecomposedEntities.objectsNoDepthTest.end(),
			entity
		),
		visibleDecomposedEntities.objectsNoDepthTest.end()
	);
	visibleDecomposedEntities.lodObjects.erase(
		remove(
			visibleDecomposedEntities.lodObjects.begin(),
			visibleDecomposedEntities.lodObjects.end(),
			entity
		),
		visibleDecomposedEntities.lodObjects.end()
	);
	visibleDecomposedEntities.opses.erase(
		remove(
			visibleDecomposedEntities.opses.begin(),
			visibleDecomposedEntities.opses.end(),
			entity
		),
		visibleDecomposedEntities.opses.end()
	);
	visibleDecomposedEntities.ppses.erase(
		remove(
			visibleDecomposedEntities.ppses.begin(),
			visibleDecomposedEntities.ppses.end(),
			entity
		),
		visibleDecomposedEntities.ppses.end()
	);
	visibleDecomposedEntities.psgs.erase(
		remove(
			visibleDecomposedEntities.psgs.begin(),
			visibleDecomposedEntities.psgs.end(),
			entity
		),
		visibleDecomposedEntities.psgs.end()
	);
	visibleDecomposedEntities.linesObjects.erase(
		remove(
			visibleDecomposedEntities.linesObjects.begin(),
			visibleDecomposedEntities.linesObjects.end(),
			entity
		),
		visibleDecomposedEntities.linesObjects.end()
	);
	visibleDecomposedEntities.objectRenderGroups.erase(
		remove(
			visibleDecomposedEntities.objectRenderGroups.begin(),
			visibleDecomposedEntities.objectRenderGroups.end(),
			entity
		),
		visibleDecomposedEntities.objectRenderGroups.end()
	);
	visibleDecomposedEntities.entityHierarchies.erase(
		remove(
			visibleDecomposedEntities.entityHierarchies.begin(),
			visibleDecomposedEntities.entityHierarchies.end(),
			entity
		),
		visibleDecomposedEntities.entityHierarchies.end()
	);
	visibleDecomposedEntities.ezrObjects.erase(
		remove(
			visibleDecomposedEntities.ezrObjects.begin(),
			visibleDecomposedEntities.ezrObjects.end(),
			entity
		),
		visibleDecomposedEntities.ezrObjects.end()
	);
	visibleDecomposedEntities.noFrustumCullingEntities.erase(
		remove(
			visibleDecomposedEntities.noFrustumCullingEntities.begin(),
			visibleDecomposedEntities.noFrustumCullingEntities.end(),
			entity
		),
		visibleDecomposedEntities.noFrustumCullingEntities.end()
	);
	visibleDecomposedEntities.environmentMappingEntities.erase(
		remove(
			visibleDecomposedEntities.environmentMappingEntities.begin(),
			visibleDecomposedEntities.environmentMappingEntities.end(),
			entity
		),
		visibleDecomposedEntities.environmentMappingEntities.end()
	);
}

void Engine::reset()
{
	vector<string> entitiesToRemove;
	for (auto it: entitiesById) {
		auto entityKey = it.first;
		entitiesToRemove.push_back(entityKey);
	}
	for (auto entityKey: entitiesToRemove) {
		removeEntity(entityKey);
	}
	partition->reset();
	entityRenderer->reset();
	if (skinningShaderEnabled == true) skinningShader->reset();
}

void Engine::initialize()
{
	// set current engine
	currentEngine = this;

	// exit if already initialized like a offscreen engine instance
	if (initialized == true)
		return;

	#if defined(VULKAN)
		renderer = new EngineVKRenderer(this);
		Console::println(string("TDME2::Using Vulkan"));
		// Console::println(string("TDME2::Extensions: ") + gl->glGetString(GL::GL_EXTENSIONS));
		shadowMappingEnabled = true;
		if (getShadowMapWidth() == 0 || getShadowMapHeight() == 0) setShadowMapSize(2048, 2048);
		if (getShadowMapRenderLookUps() == 0) setShadowMapRenderLookUps(8);
	#else
		// MacOSX, currently GL3 only
		#if defined(__APPLE__)
		{
			renderer = new EngineGL3Renderer(this);
			Console::println(string("TDME2::Using GL3+/CORE"));
			// Console::println(string("TDME2::Extensions: ") + gl->glGetString(GL::GL_EXTENSIONS));
			shadowMappingEnabled = true;
			if (getShadowMapWidth() == 0 || getShadowMapHeight() == 0) setShadowMapSize(2048, 2048);
			if (getShadowMapRenderLookUps() == 0) setShadowMapRenderLookUps(8);
		}
		// Linux/FreeBSD/NetBSD/Win32, GL2 or GL3 via GLEW
		#elif defined(_WIN32) || ((defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__linux__)) && !defined(GLES2)) || defined(__HAIKU__)
		{
			int glMajorVersion;
			int glMinorVersion;
			glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
			glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);
			if ((glMajorVersion == 3 && glMinorVersion >= 2) || glMajorVersion > 3) {
				Console::println(string("TDME2::Using GL3+/CORE(" + to_string(glMajorVersion) + "." + to_string(glMinorVersion) + ")"));
				renderer = new EngineGL3Renderer(this);
			} else {
				Console::println(string("TDME2::Using GL2(" + to_string(glMajorVersion) + "." + to_string(glMinorVersion) + ")"));
				renderer = new EngineGL2Renderer(this);
			}
			shadowMappingEnabled = true;
			if (getShadowMapWidth() == 0 || getShadowMapHeight() == 0) setShadowMapSize(2048, 2048);
			if (getShadowMapRenderLookUps() == 0) setShadowMapRenderLookUps(8);
		}
		// GLES2 on Linux
		#elif (defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)) && defined(GLES2)
		{
			renderer = new EngineGLES2Renderer(this);
			Console::println(string("TDME2::Using GLES2"));
			// Console::println(string("TDME2::Extensions: ") + gl->glGetString(GL::GL_EXTENSIONS));
			if (renderer->isBufferObjectsAvailable() == true && renderer->isDepthTextureAvailable() == true) {
				shadowMappingEnabled = true;
				animationProcessingTarget = Engine::AnimationProcessingTarget::CPU;
				if (getShadowMapWidth() == 0 || getShadowMapHeight() == 0) setShadowMapSize(1024, 1024);
				if (getShadowMapRenderLookUps() == 0) setShadowMapRenderLookUps(4);
			} else {
				shadowMappingEnabled = false;
				animationProcessingTarget = Engine::AnimationProcessingTarget::CPU;
			}
		}
		#else
			Console::println("Engine::initialize(): unsupported GL!");
			return;
		#endif
	#endif

	// determine if we have the skinning compute shader or OpenCL program
	skinningShaderEnabled = renderer->isComputeShaderAvailable() == true || renderer->isGLCLAvailable() == true;
	animationProcessingTarget = skinningShaderEnabled == true?Engine::AnimationProcessingTarget::GPU:Engine::AnimationProcessingTarget::CPU;

	// engine thread count
	if (renderer->isSupportingMultithreadedRendering() == true) {
		if (threadCount == 0) threadCount = Math::clamp(Thread::getHardwareThreadCount() == 0?2:Thread::getHardwareThreadCount() / 2, 2, 4);
	} else {
		threadCount = 1;
	}
	Console::println(string("TDME2::Thread count: ") + to_string(threadCount));

	// initialize object buffers
	ObjectBuffer::initialize();

	// create manager
	textureManager = new TextureManager(renderer);
	vboManager = new VBOManager(renderer);
	meshManager = new MeshManager();

	// init
	initialized = true;
	renderer->initialize();
	renderer->initializeFrame();

	// create object 3d renderer
	entityRenderer = new EntityRenderer(this, renderer);
	entityRenderer->initialize();
	GUIParser::initialize();

	// create GUI
	guiRenderer = new GUIRenderer(renderer);
	guiRenderer->initialize();
	gui = new GUI(this, guiRenderer);
	gui->initialize();

	// create camera
	camera = new Camera(renderer);

	// create lights
	for (auto i = 0; i < lights.size(); i++) {
		lights[i] = Light(renderer, i);
		lights[i].setLightSourceTexture(TextureReader::read("resources/engine/textures", "sun.png"));
	}

	// create partition
	partition = new PartitionOctTree();

	// create lighting shader
	lightingShader = new LightingShader(renderer);
	lightingShader->initialize();

	// create particles shader
	particlesShader = new ParticlesShader(this, renderer);
	particlesShader->initialize();

	// create particles shader
	linesShader = new LinesShader(this, renderer);
	linesShader->initialize();

	// create gui shader
	guiShader = new GUIShader(renderer);
	guiShader->initialize();

	// create frame buffer render shader
	frameBufferRenderShader = new FrameBufferRenderShader(renderer);
	frameBufferRenderShader->initialize();

	// create post processing shader
	postProcessingShader = new PostProcessingShader(renderer);
	postProcessingShader->initialize();

	// create post processing
	postProcessing = new PostProcessing();

	// create post processing shader
	texture2DRenderShader = new Texture2DRenderShader(renderer);
	texture2DRenderShader->initialize();

	// check if VBOs are available
	if (renderer->isBufferObjectsAvailable()) {
		Console::println(string("TDME2::VBOs are available."));
	} else {
		Console::println(string("TDME2::VBOs are not available! Engine will not work!"));
		initialized = false;
	}

	// check FBO support
	if (true == false/*glContext->hasBasicFBOSupport() == false*/) {
		Console::println(string("TDME2::Basic FBOs are not available!"));
		shadowMappingEnabled = false;
	} else {
		Console::println(string("TDME2::Basic FBOs are available."));
	}

	// TODO: make this configurable
	ezrShader = new EZRShader(renderer);
	ezrShader->initialize();

	// initialize shadow mapping
	if (shadowMappingEnabled == true) {
		Console::println(string("TDME2::Using shadow mapping"));
		shadowMappingShaderPre = new ShadowMapCreationShader(renderer);
		shadowMappingShaderPre->initialize();
		shadowMappingShaderRender = new ShadowMapRenderShader(renderer);
		shadowMappingShaderRender->initialize();
		shadowMapping = new ShadowMapping(this, renderer, entityRenderer);
	} else {
		Console::println(string("TDME2::Not using shadow mapping"));
	}

	// initialize skinning shader
	if (skinningShaderEnabled == true) {
		Console::println(string("TDME2::Using skinning compute shader"));
		skinningShader = new SkinningShader(renderer);
		skinningShader->initialize();
	} else {
		Console::println(string("TDME2::Not using skinning compute shader"));
	}

	#define CHECK_INITIALIZED(NAME, SHADER) if (SHADER != nullptr && SHADER->isInitialized() == false) Console::println(string("TDME: ") + NAME + ": Not initialized")

	CHECK_INITIALIZED("EZRShader", ezrShader);
	CHECK_INITIALIZED("ShadowMapCreationShader", shadowMappingShaderPre);
	CHECK_INITIALIZED("ShadowMappingShader", shadowMappingShaderRender);
	CHECK_INITIALIZED("LightingShader", lightingShader);
	CHECK_INITIALIZED("ParticlesShader", particlesShader);
	CHECK_INITIALIZED("LinesShader", linesShader);
	CHECK_INITIALIZED("GUIShader", guiShader);
	CHECK_INITIALIZED("FrameBufferRenderShader", frameBufferRenderShader);
	CHECK_INITIALIZED("PostProcessingShader", postProcessingShader);
	CHECK_INITIALIZED("Texture2DRenderShader", texture2DRenderShader);

	// check if initialized
	// initialized &= objectsFrameBuffer->isInitialized();
	initialized &= ezrShader == nullptr ? true : ezrShader->isInitialized();
	initialized &= shadowMappingShaderPre == nullptr ? true : shadowMappingShaderPre->isInitialized();
	initialized &= shadowMappingShaderRender == nullptr ? true : shadowMappingShaderRender->isInitialized();
	initialized &= lightingShader->isInitialized();
	initialized &= particlesShader->isInitialized();
	initialized &= linesShader->isInitialized();
	initialized &= guiShader->isInitialized();
	initialized &= frameBufferRenderShader->isInitialized();
	initialized &= postProcessingShader->isInitialized();
	initialized &= texture2DRenderShader->isInitialized();

	//
	if (renderer->isSupportingMultithreadedRendering() == true) {
		engineThreads.resize(threadCount - 1);
		for (auto i = 0; i < threadCount - 1; i++) {
			engineThreads[i] = new EngineThread(
				i + 1,
				renderer->getContext(i + 1)
			);
			engineThreads[i]->start();
		}
	}

	//
	Console::println(string("TDME2::initialized & ready: ") + to_string(initialized));

	// show registered shaders
	dumpShaders();
}

void Engine::reshape(int32_t width, int32_t height)
{
	// set current engine
	currentEngine = this;

	// update our width and height
	this->width = width;
	this->height = height;

	//
	int _width = scaledWidth != -1?scaledWidth:width;
	int _height = scaledHeight != -1?scaledHeight:height;

	// update frame buffer if we have one
	if (frameBuffer != nullptr) frameBuffer->reshape(_width, _height);

	// update post processing frame buffer if we have one
	if (postProcessingFrameBuffer1 != nullptr) postProcessingFrameBuffer1->reshape(_width, _height);
	if (postProcessingFrameBuffer2 != nullptr) postProcessingFrameBuffer2->reshape(_width, _height);
	if (postProcessingTemporaryFrameBuffer != nullptr) postProcessingTemporaryFrameBuffer->reshape(_width, _height);
	if (postProcessingTemporaryFrameBuffer != nullptr) postProcessingTemporaryFrameBuffer->reshape(_width, _height);
	if (postProcessingTemporaryFrameBuffer != nullptr) postProcessingTemporaryFrameBuffer->reshape(_width, _height);

	// update shadow mapping
	if (shadowMapping != nullptr) shadowMapping->reshape(width, height);

	// unset scaling frame buffer if width and height matches scaling
	if (this == Engine::instance && scaledWidth != -1 && scaledHeight != -1) {
		if (this->width == scaledWidth && this->height == scaledHeight) {
			if (frameBuffer != nullptr) frameBuffer->dispose();
			frameBuffer = nullptr;
		} else {
			if (frameBuffer == nullptr) {
				frameBuffer = new FrameBuffer(scaledWidth, scaledHeight, FrameBuffer::FRAMEBUFFER_DEPTHBUFFER | FrameBuffer::FRAMEBUFFER_COLORBUFFER);
				frameBuffer->initialize();
			} else {
				frameBuffer->reshape(scaledWidth, scaledHeight);
			}
		}
	}

	// update GUI system
	gui->reshape(width, height);
}

void Engine::scale(int32_t width, int32_t height)
{
	if (this != Engine::instance) return;
	scaledWidth = width;
	scaledHeight = height;
	if (frameBuffer != nullptr) frameBuffer->dispose();
	frameBuffer = nullptr;
	if (scaledWidth != this->width || scaledHeight != this->height) {
		frameBuffer = new FrameBuffer(scaledWidth, scaledHeight, FrameBuffer::FRAMEBUFFER_DEPTHBUFFER | FrameBuffer::FRAMEBUFFER_COLORBUFFER);
		frameBuffer->initialize();
	}
	reshape(this->width, this->height);
}

void Engine::unscale()
{
	scaledWidth = -1;
	scaledHeight = -1;
	if (frameBuffer != nullptr) frameBuffer->dispose();
	frameBuffer = nullptr;
	reshape(this->width, this->height);
}

void Engine::renderToScreen() {
	if (this == Engine::instance && frameBuffer != nullptr) frameBuffer->renderToScreen();
}

void Engine::resetLists(DecomposedEntities& decomposedEntites) {
	// clear lists of visible objects
	decomposedEntites.objects.clear();
	decomposedEntites.objectsPostPostProcessing.clear();
	decomposedEntites.objectsNoDepthTest.clear();
	decomposedEntites.lodObjects.clear();
	decomposedEntites.opses.clear();
	decomposedEntites.ppses.clear();
	decomposedEntites.psgs.clear();
	decomposedEntites.linesObjects.clear();
	decomposedEntites.objectRenderGroups.clear();
	decomposedEntites.entityHierarchies.clear();
	decomposedEntites.ezrObjects.clear();
	decomposedEntites.noFrustumCullingEntities.clear();
	decomposedEntites.environmentMappingEntities.clear();
}

void Engine::initRendering()
{
	// update timing
	timing->updateTiming();

	//
	resetLists(visibleDecomposedEntities);

	//
	renderingInitiated = true;
}

void Engine::computeTransformationsFunction(DecomposedEntities& decomposedEntites, int threadCount, int threadIdx, bool computeTransformations) {
	auto context = renderer->getContext(threadIdx);
	auto objectIdx = 0;
	for (auto object: decomposedEntites.objects) {
		if (threadCount > 1 && objectIdx % threadCount != threadIdx) {
			objectIdx++;
			continue;
		}
		object->preRender(context);
		if (computeTransformations == true) object->computeTransformations(context);
		objectIdx++;
	}
	for (auto object: decomposedEntites.objectsPostPostProcessing) {
		if (threadCount > 1 && objectIdx % threadCount != threadIdx) {
			objectIdx++;
			continue;
		}
		object->preRender(context);
		if (computeTransformations == true) object->computeTransformations(context);
		objectIdx++;
	}
	for (auto object: decomposedEntites.objectsNoDepthTest) {
		if (threadCount > 1 && objectIdx % threadCount != threadIdx) {
			objectIdx++;
			continue;
		}
		object->preRender(context);
		if (computeTransformations == true) object->computeTransformations(context);
		objectIdx++;
	}
}

inline void Engine::decomposeEntityType(Entity* entity, DecomposedEntities& decomposedEntities) {
	switch (entity->getEntityType()) {
		case Entity::ENTITY_OBJECT3D:
			{
				auto object = static_cast<Object3D*>(entity);
				if (object->isDisableDepthTest() == true) {
					decomposedEntities.objectsNoDepthTest.push_back(object);
				} else
				if (object->getRenderPass() == Entity::RENDERPASS_POST_POSTPROCESSING) {
					decomposedEntities.objectsPostPostProcessing.push_back(object);
				} else {
					decomposedEntities.objects.push_back(object);
				}
				if (object->isEnableEarlyZRejection() == true) {
					decomposedEntities.ezrObjects.push_back(object);
				};
			}
			break;
		case Entity::ENTITY_LODOBJECT3D:
			{
				auto lodObject = static_cast<LODObject3D*>(entity);
				auto object = lodObject->determineLODObject(camera); /* TODO: use a variable camera */
				if (object != nullptr) {
					decomposedEntities.lodObjects.push_back(lodObject);
					if (object->isDisableDepthTest() == true) {
						decomposedEntities.objectsNoDepthTest.push_back(object);
					} else
					if (object->getRenderPass() == Entity::RENDERPASS_POST_POSTPROCESSING) {
						decomposedEntities.objectsPostPostProcessing.push_back(object);
					} else {
						decomposedEntities.objects.push_back(object);
					}
					if (object->isEnableEarlyZRejection() == true) {
						decomposedEntities.ezrObjects.push_back(object);
					};
				}
			}
			break;
		case Entity::ENTITY_OBJECTPARTICLESYSTEM:
			{
				auto opse = static_cast<ObjectParticleSystem*>(entity);
				for (auto object: opse->getEnabledObjects()) {
					if (object->isDisableDepthTest() == true) {
						decomposedEntities.objectsNoDepthTest.push_back(object);
					} else
					if (object->getRenderPass() == Entity::RENDERPASS_POST_POSTPROCESSING) {
						decomposedEntities.objectsPostPostProcessing.push_back(object);
					} else {
						decomposedEntities.objects.push_back(object);
					}
				}
				decomposedEntities.opses.push_back(opse);
			}
			break;
		case Entity::ENTITY_POINTSPARTICLESYSTEM:
			{
				auto ppse = static_cast<PointsParticleSystem*>(entity);
				decomposedEntities.ppses.push_back(ppse);
			}
			break;
		case Entity::ENTITY_FOGPARTICLESYSTEM:
			{
				auto fpse = static_cast<FogParticleSystem*>(entity);
				decomposedEntities.ppses.push_back(fpse);
			}
			break;
		case Entity::ENTITY_LINESOBJECT3D:
			{
				auto lo = static_cast<LinesObject3D*>(entity);
				decomposedEntities.linesObjects.push_back(lo);
			}
			break;
		case Entity::ENTITY_ENVIRONMENTMAPPING:
			{
				auto eme = static_cast<EnvironmentMappingEntity*>(entity);
				decomposedEntities.environmentMappingEntities.push_back(eme);
			}
			break;
		default:
			break;
	}
}

inline void Engine::decomposeEntityTypes(const vector<Entity*>& entities, DecomposedEntities& decomposedEntities) {
	// add visible entities to related lists by querying frustum
	for (auto entity: entities) {
		switch (entity->getEntityType()) {
			case Entity::ENTITY_OBJECT3DRENDERGROUP:
				{
					auto org = static_cast<Object3DRenderGroup*>(entity);
					decomposedEntities.objectRenderGroups.push_back(org);
					auto subEntity = org->getEntity();
					if (subEntity != nullptr) decomposeEntityType(subEntity, decomposedEntities);
				}
				break;
			case Entity::ENTITY_PARTICLESYSTEMGROUP:
				{
					auto psg = static_cast<ParticleSystemGroup*>(entity);
					decomposedEntities.psgs.push_back(psg); \
					for (auto ps: psg->getParticleSystems()) decomposeEntityType(ps, decomposedEntities);
				}
				break;
			case Entity::ENTITY_ENTITYHIERARCHY:
				{
					auto eh = static_cast<EntityHierarchy*>(entity);
					decomposedEntities.entityHierarchies.push_back(eh);
					for (auto entityEh: eh->getEntities()) {
						if (entityEh->isEnabled() == false) continue;
						// compute transformations and add to lists
						switch (entityEh->getEntityType()) {
							case Entity::ENTITY_OBJECT3DRENDERGROUP:
								{
									auto org = static_cast<Object3DRenderGroup*>(entityEh);
									decomposedEntities.objectRenderGroups.push_back(org);
									auto subEntity = org->getEntity();
									if (subEntity != nullptr) decomposeEntityType(subEntity, decomposedEntities);
								}
								break;
							case Entity::ENTITY_PARTICLESYSTEMGROUP:
								{
									auto psg = static_cast<ParticleSystemGroup*>(entityEh);
									decomposedEntities.psgs.push_back(psg);
									for (auto ps: psg->getParticleSystems()) decomposeEntityType(ps, decomposedEntities);
								}
								break;
							default:
								decomposeEntityType(entityEh, decomposedEntities);
						}
					}
				}
				break;
			default:
				decomposeEntityType(entity, decomposedEntities);
		}
	}
}

void Engine::computeTransformations(Frustum* frustum, DecomposedEntities& decomposedEntities, bool autoEmit, bool computeTransformations)
{
	// do particle systems auto emit
	if (autoEmit == true) {
		for (auto it: autoEmitParticleSystemEntities) {
			auto pse = it.second;

			// skip on disabled entities
			if (pse->isEnabled() == false) continue;

			// do auto emit
			pse->emitParticles();
			pse->updateParticles();
		}
	}

	// determine entity types and store them
	decomposeEntityTypes(
		partition->getVisibleEntities(frustum),
		decomposedEntities
	);

	// collect entities that do not have frustum culling enabled
	for (auto it: noFrustumCullingEntitiesById) {
		auto entity = it.second;

		// skip on disabled entities
		if (entity->isEnabled() == false) continue;

		//
		decomposedEntities.noFrustumCullingEntities.push_back(entity);
	}

	// determine additional entity types for objects without frustum culling
	decomposeEntityTypes(
		decomposedEntities.noFrustumCullingEntities,
		decomposedEntities
	);

	//
	if (skinningShaderEnabled == true) skinningShader->useProgram();
	if (renderer->isSupportingMultithreadedRendering() == false) {
		computeTransformationsFunction(decomposedEntities, 1, 0, computeTransformations);
	} else {
		for (auto engineThread: engineThreads) engineThread->engine = this;
		for (auto engineThread: engineThreads) engineThread->transformations.computeTransformations = computeTransformations;
		for (auto engineThread: engineThreads) engineThread->state = EngineThread::STATE_TRANSFORMATIONS;
		computeTransformationsFunction(decomposedEntities, threadCount, 0, computeTransformations);
		for (auto engineThread: engineThreads) while (engineThread->state == EngineThread::STATE_TRANSFORMATIONS);
		for (auto engineThread: engineThreads) engineThread->state = EngineThread::STATE_SPINNING;
	}
	if (skinningShaderEnabled == true) {
		skinningShader->unUseProgram();
	}

	//
	renderingComputedTransformations = true;
}

void Engine::display()
{
	// finish last frame
	if (this == Engine::instance) Engine::renderer->finishFrame();

	// set current engine
	currentEngine = this;

	// init frame
	if (this == Engine::instance) Engine::renderer->initializeFrame();

	//
	initRendering();

	// default context
	auto context = Engine::renderer->getDefaultContext();
	auto _width = scaledWidth != -1?scaledWidth:width;
	auto _height = scaledHeight != -1?scaledHeight:height;

	// camera
	camera->update(context, _width, _height);

	// clear pre render states
	renderingComputedTransformations = false;

	// do pre rendering steps
	resetLists(visibleDecomposedEntities);
	computeTransformations(camera->getFrustum(), visibleDecomposedEntities, true, true);

	// create environment maps
	for (auto environmentMappingEntity: visibleDecomposedEntities.environmentMappingEntities) environmentMappingEntity->render();

	// camera
	camera->update(context, _width, _height);

	// create shadow maps
	if (shadowMapping != nullptr) shadowMapping->createShadowMaps();

	// do post processing programs effect passes
	array<bool, EFFECTPASS_COUNT - 1> effectPassFrameBuffersInUse;
	effectPassFrameBuffersInUse.fill(false);
	effectPassSkip.fill(false);
	for (auto programId: postProcessingPrograms) {
		auto program = postProcessing->getPostProcessingProgram(programId);
		if (program == nullptr) continue;
		for (auto& effectPass: program->getEffectPasses()) {
			auto effectPassIdx = effectPass.effectPassIdx;
			auto frameBufferIdx = effectPass.effectPassIdx - 1;
			auto frameBufferWidth = _width / effectPass.frameBufferWidthDivideFactor;
			auto frameBufferHeight = _height / effectPass.frameBufferHeightDivideFactor;
			if (effectPassFrameBuffers[frameBufferIdx] == nullptr) {
				effectPassFrameBuffers[frameBufferIdx] = new FrameBuffer(frameBufferWidth, frameBufferHeight, FrameBuffer::FRAMEBUFFER_COLORBUFFER); // TODO: types of buffers
				effectPassFrameBuffers[frameBufferIdx]->initialize();
			} else
			if (effectPassFrameBuffers[frameBufferIdx]->getWidth() != frameBufferWidth ||
				effectPassFrameBuffers[frameBufferIdx]->getHeight() != frameBufferHeight) {
				effectPassFrameBuffers[frameBufferIdx]->reshape(frameBufferWidth, frameBufferHeight);
			}
			effectPassFrameBuffersInUse[frameBufferIdx] = true;
			// enable
			effectPassFrameBuffers[frameBufferIdx]->enableFrameBuffer();
			// clear
			Engine::renderer->setClearColor(
				effectPass.clearColor.getRed(),
				effectPass.clearColor.getGreen(),
				effectPass.clearColor.getBlue(),
				effectPass.clearColor.getAlpha()
			);
			renderer->clear(renderer->CLEAR_COLOR_BUFFER_BIT);
			// camera
			camera->update(context, frameBufferWidth, frameBufferHeight);
			//
			auto lightSourceVisible = false;
			if (effectPass.renderLightSources == true) {
				lightSourceVisible = renderLightSources(frameBufferWidth, frameBufferHeight);
			}
			if (effectPass.skipOnLightSourceNotVisible == true && lightSourceVisible == false) {
				effectPassSkip[frameBufferIdx] = true;
			} else {
				// Do the effect render pass
				render(
					visibleDecomposedEntities,
					effectPassIdx,
					Entity::RENDERPASS_ALL,
					"ls_",
					false,
					false,
					false,
					false,
					false,
					EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY |
					EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY |
					EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY
				);
			}
		}
	}

	// dispose effect pass frame buffers that we do not use anymore
	for (auto i = 0; i < effectPassFrameBuffersInUse.size(); i++) {
		if (effectPassFrameBuffersInUse[i] == false && effectPassFrameBuffers[i] != nullptr) {
			effectPassFrameBuffers[i]->dispose();
			delete effectPassFrameBuffers[i];
			effectPassFrameBuffers[i] = nullptr;
		}
	}

	// set up clear color
	Engine::renderer->setClearColor(sceneColor.getRed(), sceneColor.getGreen(), sceneColor.getBlue(), sceneColor.getAlpha());

	// create post processing frame buffers if having post processing
	if (postProcessingPrograms.size() > 0) {
		if (postProcessingFrameBuffer1 == nullptr) {
			postProcessingFrameBuffer1 = new FrameBuffer(_width, _height, FrameBuffer::FRAMEBUFFER_DEPTHBUFFER | FrameBuffer::FRAMEBUFFER_COLORBUFFER);
			postProcessingFrameBuffer1->initialize();
		}
		if (postProcessingFrameBuffer2 == nullptr) {
			postProcessingFrameBuffer2 = new FrameBuffer(_width, _height, FrameBuffer::FRAMEBUFFER_DEPTHBUFFER | FrameBuffer::FRAMEBUFFER_COLORBUFFER);
			postProcessingFrameBuffer2->initialize();
		}
		postProcessingFrameBuffer1->enableFrameBuffer();
	} else {
		if (postProcessingFrameBuffer1 != nullptr) {
			postProcessingFrameBuffer1->dispose();
			delete postProcessingFrameBuffer1;
			postProcessingFrameBuffer1 = nullptr;
		}
		if (postProcessingFrameBuffer2 != nullptr) {
			postProcessingFrameBuffer2->dispose();
			delete postProcessingFrameBuffer2;
			postProcessingFrameBuffer2 = nullptr;
		}
		// render objects to target frame buffer or screen
		if (frameBuffer != nullptr) {
			frameBuffer->enableFrameBuffer();
		} else {
			FrameBuffer::disableFrameBuffer();
		}
	}

	// clear previous frame values
	renderer->clear(renderer->CLEAR_DEPTH_BUFFER_BIT | renderer->CLEAR_COLOR_BUFFER_BIT);

	// camera
	camera->update(context, _width, _height);

	// do rendering
	render(
		visibleDecomposedEntities,
		EFFECTPASS_NONE,
		Entity::RENDERPASS_ALL,
		string(),
		true,
		true,
		true,
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

	// delete post processing termporary buffer if not required anymore
	if (isUsingPostProcessingTemporaryFrameBuffer == false && postProcessingTemporaryFrameBuffer != nullptr) {
		postProcessingTemporaryFrameBuffer->dispose();
		delete postProcessingTemporaryFrameBuffer;
		postProcessingTemporaryFrameBuffer = nullptr;
	}

	// clear pre render states
	renderingInitiated = false;
	renderingComputedTransformations = false;

	//
	if (renderer->isSupportingMultithreadedRendering() == true) {
		for (auto engineThread: engineThreads) while (engineThread->state != EngineThread::STATE_SPINNING && engineThread->state != EngineThread::STATE_WAITING);
		for (auto engineThread: engineThreads) engineThread->state = EngineThread::STATE_WAITING;
	}

	//
	if (frameBuffer != nullptr) {
		FrameBuffer::disableFrameBuffer();
	}

	// camera
	camera->update(context, _width, _height);
}

void Engine::computeWorldCoordinateByMousePosition(int32_t mouseX, int32_t mouseY, float z, Vector3& worldCoordinate)
{
	auto scaleFactorWidth = static_cast<float>(scaledWidth != -1?scaledWidth:width) / static_cast<float>(width);
	auto scaleFactorHeight = static_cast<float>(scaledHeight != -1?scaledHeight:height) / static_cast<float>(height);
	// see: http://stackoverflow.com/questions/7692988/opengl-math-projecting-screen-space-to-world-space-coords-solved
	Vector4 worldCoordinate4;
	camera->getModelViewProjectionInvertedMatrix().multiply(
		Vector4(
			(2.0f * (mouseX * scaleFactorWidth - camera->getViewPortLeft()) / camera->getViewPortWidth()) - 1.0f,
			1.0f - (2.0f * (mouseY * scaleFactorHeight - camera->getViewPortTop()) / camera->getViewPortHeight()),
			2.0f * z - 1.0f,
			1.0f
		),
		worldCoordinate4
	);
	worldCoordinate4.scale(1.0f / worldCoordinate4.getW());
	worldCoordinate.set(
		worldCoordinate4.getX(),
		worldCoordinate4.getY(),
		worldCoordinate4.getZ()
	);
}

void Engine::computeWorldCoordinateByMousePosition(int32_t mouseX, int32_t mouseY, Vector3& worldCoordinate)
{
	// use framebuffer if we have one
	if (frameBuffer != nullptr)
		frameBuffer->enableFrameBuffer();

	//
	auto scaleFactorWidth = static_cast<float>(scaledWidth != -1?scaledWidth:width) / static_cast<float>(width);
	auto scaleFactorHeight = static_cast<float>(scaledHeight != -1?scaledHeight:height) / static_cast<float>(height);

	// see: http://stackoverflow.com/questions/7692988/opengl-math-projecting-screen-space-to-world-space-coords-solved
	auto z = renderer->readPixelDepth(mouseX * scaleFactorWidth, (height  - mouseY) * scaleFactorHeight);

	// unuse framebuffer if we have one
	if (frameBuffer != nullptr)
		FrameBuffer::disableFrameBuffer();

	//
	computeWorldCoordinateByMousePosition(mouseX, mouseY, z, worldCoordinate);
}

Entity* Engine::getEntityByMousePosition(
	DecomposedEntities& decomposedEntities,
	bool forcePicking,
	int32_t mouseX,
	int32_t mouseY,
	EntityPickingFilter* filter,
	Node** object3DNode,
	ParticleSystemEntity** particleSystemEntity
) {
	// get world position of mouse position at near and far plane
	Vector3 tmpVector3a;
	Vector3 tmpVector3b;
	Vector3 tmpVector3c;
	Vector3 tmpVector3d;
	Vector3 tmpVector3e;
	computeWorldCoordinateByMousePosition(mouseX, mouseY, 0.0f, tmpVector3a);
	computeWorldCoordinateByMousePosition(mouseX, mouseY, 1.0f, tmpVector3b);

	// selected entity
	auto selectedEntityDistance = Float::MAX_VALUE;
	Entity* selectedEntity = nullptr;
	Node* selectedObject3DNode = nullptr;
	ParticleSystemEntity* selectedParticleSystem = nullptr;

	// iterate visible objects that have no depth test, check if ray with given mouse position from near plane to far plane collides with each object's triangles
	for (auto entity: decomposedEntities.objectsNoDepthTest) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), tmpVector3a, tmpVector3b, tmpVector3c, tmpVector3d) == true) {
			for (auto it = entity->getTransformedFacesIterator()->iterator(); it->hasNext();) {
				auto& vertices = it->next();
				if (LineSegment::doesLineSegmentCollideWithTriangle(vertices[0], vertices[1], vertices[2], tmpVector3a, tmpVector3b, tmpVector3e) == true) {
					auto entityDistance = tmpVector3e.sub(tmpVector3a).computeLengthSquared();
					// check if match or better match
					if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
						selectedEntity = entity;
						selectedEntityDistance = entityDistance;
						selectedObject3DNode = it->getNode();
						selectedParticleSystem = nullptr;
					}
				}
			}
		}
	}
	// they have first priority right now
	if (selectedEntity != nullptr) {
		if (object3DNode != nullptr) *object3DNode = selectedObject3DNode;
		for (auto _entity = selectedEntity; _entity != nullptr; _entity = _entity->getParentEntity()) {
			if (_entity->getParentEntity() == nullptr) return _entity;
		}
		return nullptr;
	}

	// iterate visible object partition systems, check if ray with given mouse position from near plane to far plane collides with bounding volume
	for (auto entity: decomposedEntities.opses) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), tmpVector3a, tmpVector3b, tmpVector3c, tmpVector3d) == true) {
			auto entityDistance = tmpVector3e.set(entity->getBoundingBoxTransformed()->getCenter()).sub(tmpVector3a).computeLengthSquared();
			// check if match or better match
			if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
				selectedEntity = entity;
				selectedEntityDistance = entityDistance;
				selectedObject3DNode = nullptr;
				selectedParticleSystem = nullptr;
			}
		}
	}

	// iterate visible point partition systems, check if ray with given mouse position from near plane to far plane collides with bounding volume
	for (auto entity: decomposedEntities.ppses) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), tmpVector3a, tmpVector3b, tmpVector3c, tmpVector3d) == true) {
			auto entityDistance = tmpVector3e.set(entity->getBoundingBoxTransformed()->getCenter()).sub(tmpVector3a).computeLengthSquared();
			// check if match or better match
			if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
				selectedEntity = entity;
				selectedEntityDistance = entityDistance;
				selectedObject3DNode = nullptr;
				selectedParticleSystem = nullptr;
			}
		}
	}

	// iterate visible particle system nodes, check if ray with given mouse position from near plane to far plane collides with bounding volume
	for (auto entity: decomposedEntities.psgs) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), tmpVector3a, tmpVector3b, tmpVector3c, tmpVector3d) == true) {
			auto entityDistance = tmpVector3e.set(entity->getBoundingBoxTransformed()->getCenter()).sub(tmpVector3a).computeLengthSquared();
			// check if match or better match
			if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
				selectedEntity = entity;
				selectedEntityDistance = entityDistance;
				selectedObject3DNode = nullptr;
				selectedParticleSystem = nullptr;
				auto selectedSubEntityDistance = Float::MAX_VALUE;
				// iterate sub partition systems, check if ray with given mouse position from near plane to far plane collides with bounding volume
				for (auto subEntity: entity->getParticleSystems()) {
					if (LineSegment::doesBoundingBoxCollideWithLineSegment(subEntity->getBoundingBoxTransformed(), tmpVector3a, tmpVector3b, tmpVector3c, tmpVector3d) == true) {
						auto subEntityDistance = tmpVector3e.set(subEntity->getBoundingBoxTransformed()->getCenter()).sub(tmpVector3a).computeLengthSquared();
						// check if match or better match
						if (selectedParticleSystem == nullptr || subEntityDistance < selectedSubEntityDistance) {
							selectedSubEntityDistance = subEntityDistance;
							selectedParticleSystem = subEntity;
						}
					}
				}
			}
		}
	}

	// iterate visible line objects, check if ray with given mouse position from near plane to far plane collides with each object's triangles
	for (auto entity: decomposedEntities.linesObjects) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), tmpVector3a, tmpVector3b, tmpVector3c, tmpVector3d) == true) {
			auto entityDistance = tmpVector3e.set(entity->getBoundingBoxTransformed()->getCenter()).sub(tmpVector3a).computeLengthSquared();
			// check if match or better match
			if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
				selectedEntity = entity;
				selectedEntityDistance = entityDistance;
				selectedObject3DNode = nullptr;
				selectedParticleSystem = nullptr;
			}
		}
	}

	// iterate visible objects, check if ray with given mouse position from near plane to far plane collides with each object's triangles
	for (auto entity: decomposedEntities.objects) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), tmpVector3a, tmpVector3b, tmpVector3c, tmpVector3d) == true) {
			for (auto it = entity->getTransformedFacesIterator()->iterator(); it->hasNext();) {
				auto& vertices = it->next();
				if (LineSegment::doesLineSegmentCollideWithTriangle(vertices[0], vertices[1], vertices[2], tmpVector3a, tmpVector3b, tmpVector3e) == true) {
					auto entityDistance = tmpVector3e.sub(tmpVector3a).computeLengthSquared();
					// check if match or better match
					if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
						selectedEntity = entity;
						selectedEntityDistance = entityDistance;
						selectedObject3DNode = it->getNode();
						selectedParticleSystem = nullptr;
					}
				}
			}
		}
	}

	// iterate visible objects that have post post processing renderpass, check if ray with given mouse position from near plane to far plane collides with each object's triangles
	for (auto entity: decomposedEntities.objectsPostPostProcessing) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), tmpVector3a, tmpVector3b, tmpVector3c, tmpVector3d) == true) {
			for (auto it = entity->getTransformedFacesIterator()->iterator(); it->hasNext();) {
				auto& vertices = it->next();
				if (LineSegment::doesLineSegmentCollideWithTriangle(vertices[0], vertices[1], vertices[2], tmpVector3a, tmpVector3b, tmpVector3e) == true) {
					auto entityDistance = tmpVector3e.sub(tmpVector3a).computeLengthSquared();
					// check if match or better match
					if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
						selectedEntity = entity;
						selectedEntityDistance = entityDistance;
						selectedObject3DNode = it->getNode();
						selectedParticleSystem = nullptr;
					}
				}
			}
		}
	}

	// iterate visible LOD objects, check if ray with given mouse position from near plane to far plane collides with each object's triangles
	for (auto entity: decomposedEntities.lodObjects) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), tmpVector3a, tmpVector3b, tmpVector3c, tmpVector3d) == true) {
			auto object = entity->getLODObject();
			if (object != nullptr) {
				for (auto it = object->getTransformedFacesIterator()->iterator(); it->hasNext();) {
					auto& vertices = it->next();
					if (LineSegment::doesLineSegmentCollideWithTriangle(vertices[0], vertices[1], vertices[2], tmpVector3a, tmpVector3b, tmpVector3e) == true) {
						auto entityDistance = tmpVector3e.sub(tmpVector3a).computeLengthSquared();
						// check if match or better match
						if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
							selectedEntity = entity;
							selectedEntityDistance = entityDistance;
						}
					}
					selectedObject3DNode = it->getNode();
					selectedParticleSystem = nullptr;
				}
			}
		}
	}

	// iterate visible entity hierarches, check if ray with given mouse position from near plane to far plane collides with bounding volume
	for (auto entity: decomposedEntities.entityHierarchies) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), tmpVector3a, tmpVector3b, tmpVector3c, tmpVector3d) == true) {
			DecomposedEntities decomposedEntitiesEH;
			Node* object3DNodeEH = nullptr;
			ParticleSystemEntity* particleSystemEntityEH = nullptr;
			decomposeEntityTypes(
				entity->getEntities(),
				decomposedEntitiesEH
			);
			auto subEntity = getEntityByMousePosition(
				decomposedEntitiesEH,
				true,
				mouseX,
				mouseY,
				filter,
				&object3DNodeEH,
				&particleSystemEntityEH
			);
			if (subEntity != nullptr) {
				auto entityDistance = tmpVector3e.set(subEntity->getBoundingBoxTransformed()->getCenter()).sub(tmpVector3a).computeLengthSquared();
				// check if match or better match
				if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
					selectedEntity = entity;
					selectedEntityDistance = entityDistance;
					selectedObject3DNode = object3DNodeEH;
					selectedParticleSystem = particleSystemEntityEH;
				}
			}
		}
	}

	// store node
	if (object3DNode != nullptr) *object3DNode = selectedObject3DNode;

	// store particle system entity
	if (particleSystemEntity != nullptr) {
		*particleSystemEntity = selectedParticleSystem;
	}

	// return parent entity if entity is part of a hierarchy
	if (selectedEntity != nullptr) {
		for (auto _entity = selectedEntity; _entity != nullptr; _entity = _entity->getParentEntity()) {
			if (_entity->getParentEntity() == nullptr) return _entity;
		}
		return nullptr;
	} else {
		return nullptr;
	}
}

Entity* Engine::getEntityByMousePosition(int32_t mouseX, int32_t mouseY, Vector3& contactPoint, EntityPickingFilter* filter, Node** object3DNode, ParticleSystemEntity** particleSystemEntity) {
	// get world position of mouse position at near and far plane
	Vector3 startPoint;
	Vector3 endPoint;
	computeWorldCoordinateByMousePosition(mouseX, mouseY, 0.0f, startPoint);
	computeWorldCoordinateByMousePosition(mouseX, mouseY, 1.0f, endPoint);

	//
	return doRayCasting(startPoint, endPoint, contactPoint, filter);// TODO: object 3d node, particle system entity
}

Entity* Engine::doRayCasting(
	DecomposedEntities& decomposedEntities,
	bool forcePicking,
	const Vector3& startPoint,
	const Vector3& endPoint,
	Vector3& contactPoint,
	EntityPickingFilter* filter) {
	Vector3 tmpVector3c;
	Vector3 tmpVector3d;
	Vector3 tmpVector3e;

	// selected entity
	auto selectedEntityDistance = Float::MAX_VALUE;
	Entity* selectedEntity = nullptr;

	// iterate visible objects with no depth writing, check if ray with given mouse position from near plane to far plane collides with each object's triangles
	for (auto entity: decomposedEntities.objectsNoDepthTest) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), startPoint, endPoint, tmpVector3c, tmpVector3d) == true) {
			for (auto it = entity->getTransformedFacesIterator()->iterator(); it->hasNext();) {
				auto& vertices = it->next();
				if (LineSegment::doesLineSegmentCollideWithTriangle(vertices[0], vertices[1], vertices[2], startPoint, endPoint, tmpVector3e) == true) {
					auto entityDistance = tmpVector3e.clone().sub(startPoint).computeLengthSquared();
					// check if match or better match
					if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
						selectedEntity = entity;
						selectedEntityDistance = entityDistance;
						contactPoint = tmpVector3e;
					}
				}
			}
		}
	}
	// they have first priority right now
	if (selectedEntity != nullptr) {
		return selectedEntity;
	}

	// iterate visible objects, check if ray with given mouse position from near plane to far plane collides with each object's triangles
	for (auto entity: decomposedEntities.objects) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), startPoint, endPoint, tmpVector3c, tmpVector3d) == true) {
			for (auto it = entity->getTransformedFacesIterator()->iterator(); it->hasNext();) {
				auto& vertices = it->next();
				if (LineSegment::doesLineSegmentCollideWithTriangle(vertices[0], vertices[1], vertices[2], startPoint, endPoint, tmpVector3e) == true) {
					auto entityDistance = tmpVector3e.clone().sub(startPoint).computeLengthSquared();
					// check if match or better match
					if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
						selectedEntity = entity;
						selectedEntityDistance = entityDistance;
						contactPoint = tmpVector3e;
					}
				}
			}
		}
	}

	// iterate visible objects that have post post processing renderpass, check if ray with given mouse position from near plane to far plane collides with each object's triangles
	for (auto entity: decomposedEntities.objectsPostPostProcessing) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), startPoint, endPoint, tmpVector3c, tmpVector3d) == true) {
			for (auto it = entity->getTransformedFacesIterator()->iterator(); it->hasNext();) {
				auto& vertices = it->next();
				if (LineSegment::doesLineSegmentCollideWithTriangle(vertices[0], vertices[1], vertices[2], startPoint, endPoint, tmpVector3e) == true) {
					auto entityDistance = tmpVector3e.clone().sub(startPoint).computeLengthSquared();
					// check if match or better match
					if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
						selectedEntity = entity;
						selectedEntityDistance = entityDistance;
						contactPoint = tmpVector3e;
					}
				}
			}
		}
	}

	// iterate visible LOD objects, check if ray with given mouse position from near plane to far plane collides with each object's triangles
	for (auto entity: decomposedEntities.lodObjects) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), startPoint, endPoint, tmpVector3c, tmpVector3d) == true) {
			auto object = entity->getLODObject();
			if (object != nullptr) {
				for (auto it = object->getTransformedFacesIterator()->iterator(); it->hasNext();) {
					auto& vertices = it->next();
					if (LineSegment::doesLineSegmentCollideWithTriangle(vertices[0], vertices[1], vertices[2], startPoint, endPoint, tmpVector3e) == true) {
						auto entityDistance = tmpVector3e.sub(startPoint).computeLengthSquared();
						// check if match or better match
						if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
							selectedEntity = entity;
							selectedEntityDistance = entityDistance;
							contactPoint = tmpVector3e;
						}
					}
				}
			}
		}
	}

	// iterate visible entity hierarches, check if ray with given mouse position from near plane to far plane collides with bounding volume
	for (auto entity: decomposedEntities.entityHierarchies) {
		// skip if not pickable or ignored by filter
		if (forcePicking == false && entity->isPickable() == false) continue;
		if (filter != nullptr && filter->filterEntity(entity) == false) continue;
		// do the collision test
		if (LineSegment::doesBoundingBoxCollideWithLineSegment(entity->getBoundingBoxTransformed(), startPoint, endPoint, tmpVector3c, tmpVector3d) == true) {
			DecomposedEntities decomposedEntitiesEH;
			decomposeEntityTypes(
				entity->getEntities(),
				decomposedEntitiesEH
			);
			Vector3 contactPointEH;
			auto entity = doRayCasting(
				decomposedEntitiesEH,
				true,
				startPoint,
				endPoint,
				contactPointEH,
				filter
			);
			if (entity != nullptr) {
				auto entityDistance = tmpVector3e.sub(startPoint).computeLengthSquared();
				// check if match or better match
				if (selectedEntity == nullptr || entityDistance < selectedEntityDistance) {
					selectedEntity = entity;
					selectedEntityDistance = entityDistance;
					contactPoint = contactPointEH;
				}
			}
		}
	}

	//
	return selectedEntity;
}

bool Engine::computeScreenCoordinateByWorldCoordinate(const Vector3& worldCoordinate, Vector2& screenCoordinate, int width, int height)
{
	Vector3 screenCoordinate3;
	Vector4 screenCoordinate4;
	auto _width = width != -1?width:(scaledWidth != -1?scaledWidth:this->width);
	auto _height = height != -1?height:(scaledHeight != -1?scaledHeight:this->height);
	// convert to normalized device coordinates
	camera->getModelViewProjectionMatrix().multiply(Vector4(worldCoordinate, 1.0f), screenCoordinate4);
	screenCoordinate4.scale(1.0f / screenCoordinate4.getW());
	// convert to screen coordinate
	screenCoordinate.setX((screenCoordinate4[0] + 1.0f) * _width / 2.0f);
	screenCoordinate.setY(_height - ((screenCoordinate4[1] + 1.0f) * _height / 2.0f));
	return camera->getModelViewMatrix().multiply(worldCoordinate, screenCoordinate3).getZ() <= 0.0f;
}

void Engine::dispose()
{
	// finish last frame
	if (this == Engine::instance) Engine::renderer->finishFrame();

	// remove entities
	vector<string> entitiesToRemove;
	for (auto it: entitiesById) {
		auto entityKey = it.first;
		entitiesToRemove.push_back(entityKey);
	}
	for (auto entityKey: entitiesToRemove) {
		removeEntity(entityKey);
	}

	// dispose shadow mapping
	if (shadowMapping != nullptr) shadowMapping->dispose();

	// dispose frame buffers
	if (frameBuffer != nullptr) frameBuffer->dispose();
	if (postProcessingFrameBuffer1 != nullptr) postProcessingFrameBuffer1->dispose();
	if (postProcessingFrameBuffer2 != nullptr) postProcessingFrameBuffer2->dispose();
	if (postProcessingTemporaryFrameBuffer != nullptr) postProcessingTemporaryFrameBuffer->dispose();

	// dispose GUI
	gui->dispose();
	if (this == Engine::instance) {
		guiRenderer->dispose();
		GUIParser::dispose();
	}

	// dispose object 3d VBO renderer
	entityRenderer->dispose();

	// dispose object buffer if main engine
	if (this == Engine::instance) {
		ObjectBuffer::dispose();
	}

	// set current engine
	if (currentEngine == this) currentEngine = nullptr;
}

void Engine::initGUIMode()
{
	// use framebuffer if we have one
	if (frameBuffer != nullptr) {
		frameBuffer->enableFrameBuffer();
	} else {
		frameBuffer->disableFrameBuffer();
	}

	renderer->initGuiMode();
}

void Engine::doneGUIMode()
{
	renderer->doneGuiMode();

	// unuse framebuffer if we have one
	if (frameBuffer != nullptr)
		FrameBuffer::disableFrameBuffer();
}

bool Engine::makeScreenshot(const string& pathName, const string& fileName)
{
	// use framebuffer if we have one
	if (frameBuffer != nullptr) frameBuffer->enableFrameBuffer();

	// fetch pixel
	auto pixels = renderer->readPixels(0, 0, width, height);
	if (pixels == nullptr) {
		Console::println("Engine::makeScreenshot(): Failed to read pixels");
		return false;
	}

	// create texture, write and delete
	auto texture = new Texture(
		"tdme.engine.makescreenshot",
		32,
		width,
		height,
		width,
		height,
		pixels
	);
	texture->acquireReference();
	PNGTextureWriter::write(texture, pathName, fileName);
	texture->releaseReference();

	// unuse framebuffer if we have one
	if (frameBuffer != nullptr) FrameBuffer::disableFrameBuffer();

	//
	return true;
}

void Engine::resetPostProcessingPrograms() {
	postProcessingPrograms.clear();
}

void Engine::addPostProcessingProgram(const string& programId) {
	if (postProcessing->getPostProcessingProgram(programId) != nullptr) postProcessingPrograms.push_back(programId);
}

void Engine::doPostProcessing(PostProcessingProgram::RenderPass renderPass, array<FrameBuffer*, 2> postProcessingFrameBuffers, FrameBuffer* targetFrameBuffer) {
	auto postProcessingFrameBufferIdx = 0;
	for (auto programId: postProcessingPrograms) {
		auto program = postProcessing->getPostProcessingProgram(programId);
		if (program == nullptr) continue;
		if (program->getRenderPass() != renderPass) continue;
		auto effectPassSkipDetected = false;
		for (auto& effectPass: program->getEffectPasses()) {
			if (effectPassSkip[effectPass.effectPassIdx - 1] == true) effectPassSkipDetected = true;
		}
		if (effectPassSkipDetected == true) continue;
		for (auto& step: program->getPostProcessingSteps()) {
			auto shaderId = step.shaderId;
			FrameBuffer* blendToSource = nullptr;
			FrameBuffer* source = nullptr;
			FrameBuffer* target = nullptr;
			if (step.blendToSource == PostProcessingProgram::FRAMEBUFFERSOURCE_SCREEN) {
				blendToSource = postProcessingFrameBuffers[postProcessingFrameBufferIdx];
			}
			switch(step.source) {
				case PostProcessingProgram::FRAMEBUFFERSOURCE_NONE:
					break;
				case PostProcessingProgram::FRAMEBUFFERSOURCE_SCREEN:
					source = postProcessingFrameBuffers[postProcessingFrameBufferIdx];
					break;
				default:
					source = effectPassFrameBuffers[step.source - PostProcessingProgram::FRAMEBUFFERSOURCE_EFFECTPASS0];
					break;
			}
			switch(step.target) {
				case PostProcessingProgram::FRAMEBUFFERTARGET_SCREEN:
					target = postProcessingFrameBuffers[(postProcessingFrameBufferIdx + 1) % 2];
					break;
				case PostProcessingProgram::FRAMEBUFFERTARGET_TEMPORARY:
					isUsingPostProcessingTemporaryFrameBuffer = true;
					if (postProcessingTemporaryFrameBuffer == nullptr) {
						postProcessingTemporaryFrameBuffer = new FrameBuffer(width, height, FrameBuffer::FRAMEBUFFER_COLORBUFFER | FrameBuffer::FRAMEBUFFER_DEPTHBUFFER);
						postProcessingTemporaryFrameBuffer->initialize();
					}
					target = postProcessingTemporaryFrameBuffer;
					break;
			}
			FrameBuffer::doPostProcessing(this, target, source, programId, shaderId, step.bindTemporary == true?postProcessingTemporaryFrameBuffer:nullptr, blendToSource);
			switch(step.target) {
				case PostProcessingProgram::FRAMEBUFFERTARGET_SCREEN:
					postProcessingFrameBufferIdx = (postProcessingFrameBufferIdx + 1) % 2;
					break;
				case PostProcessingProgram::FRAMEBUFFERTARGET_TEMPORARY:
					break;
			}
		}
	}

	// render back to objects frame buffer
	if (postProcessingFrameBuffers[postProcessingFrameBufferIdx] != targetFrameBuffer) {
		if (targetFrameBuffer != nullptr) {
			targetFrameBuffer->enableFrameBuffer();
		} else {
			FrameBuffer::disableFrameBuffer();
		}
		postProcessingFrameBuffers[postProcessingFrameBufferIdx]->renderToScreen();
	}
}

const vector<string> Engine::getRegisteredShader(ShaderType type) {
	vector<string> result;
	for (auto shadersIt: shaders) {
		auto& shader = shadersIt.second;
		if (shader.type == type) {
			result.push_back(shader.id);
		}
	}
	return result;
}

void Engine::registerShader(ShaderType type, const string& shaderId, const map<string, ShaderParameter>& parameterDefaults) {
	if (shaders.find(shaderId) != shaders.end()) {
		Console::println("Engine::registerShader(): Shader already registered: " + shaderId);
		return;
	}
	shaders[shaderId] = {
		.type = type,
		.id = shaderId,
		.parameterDefaults = parameterDefaults
	};
}

const map<string, ShaderParameter> Engine::getShaderParameterDefaults(const string& shaderId) {
	auto shaderIt = shaders.find(shaderId);
	if (shaderIt == shaders.end()) {
		Console::println("Engine::getShaderParameterDefaults(): No registered shader: " + shaderId);
		return map<string, ShaderParameter>();
	}
	return shaderIt->second.parameterDefaults;
}

void Engine::render(DecomposedEntities& visibleDecomposedEntities, int32_t effectPass, int32_t renderPassMask, const string& shaderPrefix, bool useEZR, bool applyShadowMapping, bool applyPostProcessing, bool doRenderLightSource, bool doRenderParticleSystems, int32_t renderTypes) {
	//
	Engine::renderer->setEffectPass(effectPass);
	Engine::renderer->setShaderPrefix(shaderPrefix);

	// default context
	auto context = Engine::renderer->getDefaultContext();

	// render lines objects
	if (visibleDecomposedEntities.linesObjects.size() > 0) {
		// use particle shader
		if (linesShader != nullptr) linesShader->useProgram(context);

		// render points based particle systems
		for (auto i = 0; i < Entity::RENDERPASS_MAX; i++) {
			auto renderPass = static_cast<Entity::RenderPass>(Math::pow(2, i));
			if ((renderPassMask & renderPass) == renderPass) entityRenderer->render(renderPass, visibleDecomposedEntities.linesObjects);
		}

		// unuse particle shader
		if (linesShader != nullptr) linesShader->unUseProgram(context);
	}

	// do depth buffer writing aka early z rejection
	if (useEZR == true && ezrShader != nullptr && visibleDecomposedEntities.ezrObjects.size() > 0) {
		// disable color rendering, we only want to write to the Z-Buffer
		renderer->setColorMask(false, false, false, false);
		// render
		ezrShader->useProgram(this);
		// only draw opaque face entities of objects marked as EZR objects
		for (auto i = 0; i < Entity::RENDERPASS_MAX; i++) {
			auto renderPass = static_cast<Entity::RenderPass>(Math::pow(2, i));
			if ((renderPassMask & renderPass) == renderPass) {
				entityRenderer->render(
					renderPass,
					visibleDecomposedEntities.ezrObjects,
					false,
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY:0)
				);
			}
		}
		// done
		ezrShader->unUseProgram();
		// restore disable color rendering
		renderer->setColorMask(true, true, true, true);
	}

	// use lighting shader
	if (visibleDecomposedEntities.objects.size() > 0) {
		//
		if (lightingShader != nullptr) lightingShader->useProgram(this);

		// render objects
		for (auto i = 0; i < Entity::RENDERPASS_MAX; i++) {
			auto renderPass = static_cast<Entity::RenderPass>(Math::pow(2, i));
			if ((renderPassMask & renderPass) == renderPass) {
				entityRenderer->render(
					renderPass,
					visibleDecomposedEntities.objects,
					true,
					((renderTypes & EntityRenderer::RENDERTYPE_NORMALS) == EntityRenderer::RENDERTYPE_NORMALS?EntityRenderer::RENDERTYPE_NORMALS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTUREARRAYS) == EntityRenderer::RENDERTYPE_TEXTUREARRAYS?EntityRenderer::RENDERTYPE_TEXTUREARRAYS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_EFFECTCOLORS) == EntityRenderer::RENDERTYPE_EFFECTCOLORS?EntityRenderer::RENDERTYPE_EFFECTCOLORS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_MATERIALS) == EntityRenderer::RENDERTYPE_MATERIALS?EntityRenderer::RENDERTYPE_MATERIALS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTURES) == EntityRenderer::RENDERTYPE_TEXTURES?EntityRenderer::RENDERTYPE_TEXTURES:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY:0)|
					((renderTypes & EntityRenderer::RENDERTYPE_LIGHTS) == EntityRenderer::RENDERTYPE_LIGHTS?EntityRenderer::RENDERTYPE_LIGHTS:0)
				);
			}
		}

		// unuse lighting shader
		if (lightingShader != nullptr) lightingShader->unUseProgram();

		// render shadows if required
		if (applyShadowMapping == true && shadowMapping != nullptr) shadowMapping->renderShadowMaps(visibleDecomposedEntities.objects);
	}

	// do post processing
	if (applyPostProcessing == true) {
		isUsingPostProcessingTemporaryFrameBuffer = false;
		if (postProcessingPrograms.size() > 0) {
			doPostProcessing(PostProcessingProgram::RENDERPASS_OBJECTS, {{postProcessingFrameBuffer1, postProcessingFrameBuffer2 }}, postProcessingFrameBuffer1);
			postProcessingFrameBuffer1->enableFrameBuffer();
		}
	}

	// render point particle systems
	if (doRenderParticleSystems == true && visibleDecomposedEntities.ppses.size() > 0) {
		// use particle shader
		if (particlesShader != nullptr) particlesShader->useProgram(context);

		// render points based particle systems
		if (visibleDecomposedEntities.ppses.size() > 0) {
			for (auto i = 0; i < Entity::RENDERPASS_MAX; i++) {
				auto renderPass = static_cast<Entity::RenderPass>(Math::pow(2, i));
				if ((renderPassMask & renderPass) == renderPass) entityRenderer->render(renderPass, visibleDecomposedEntities.ppses);
			}
		}

		// unuse particle shader
		if (particlesShader != nullptr) particlesShader->unUseProgram(context);
	}

	// render objects and particles together
	if (applyPostProcessing == true) {
		if (postProcessingPrograms.size() > 0) {
			doPostProcessing(PostProcessingProgram::RENDERPASS_FINAL, {{postProcessingFrameBuffer1, postProcessingFrameBuffer2 }}, frameBuffer);
		}
	}

	// render objects that are have post post processing render pass
	if (visibleDecomposedEntities.objectsPostPostProcessing.size() > 0) {
		// use lighting shader
		if (lightingShader != nullptr) {
			lightingShader->useProgram(this);
		}

		// render post processing objects
		for (auto i = 0; i < Entity::RENDERPASS_MAX; i++) {
			auto renderPass = static_cast<Entity::RenderPass>(Math::pow(2, i));
			if ((renderPassMask & renderPass) == renderPass) {
				entityRenderer->render(
					renderPass,
					visibleDecomposedEntities.objectsPostPostProcessing,
					true,
					((renderTypes & EntityRenderer::RENDERTYPE_NORMALS) == EntityRenderer::RENDERTYPE_NORMALS?EntityRenderer::RENDERTYPE_NORMALS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTUREARRAYS) == EntityRenderer::RENDERTYPE_TEXTUREARRAYS?EntityRenderer::RENDERTYPE_TEXTUREARRAYS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_EFFECTCOLORS) == EntityRenderer::RENDERTYPE_EFFECTCOLORS?EntityRenderer::RENDERTYPE_EFFECTCOLORS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_MATERIALS) == EntityRenderer::RENDERTYPE_MATERIALS?EntityRenderer::RENDERTYPE_MATERIALS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTURES) == EntityRenderer::RENDERTYPE_TEXTURES?EntityRenderer::RENDERTYPE_TEXTURES:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY:0)|
					((renderTypes & EntityRenderer::RENDERTYPE_LIGHTS) == EntityRenderer::RENDERTYPE_LIGHTS?EntityRenderer::RENDERTYPE_LIGHTS:0)
				);
			}
		}

		// unuse lighting shader
		if (lightingShader != nullptr) lightingShader->unUseProgram();

		// render shadows if required
		if (applyShadowMapping == true && shadowMapping != nullptr) shadowMapping->renderShadowMaps(visibleDecomposedEntities.objectsPostPostProcessing);
	}

	// render objects that are have post post processing render pass
	if (visibleDecomposedEntities.objectsNoDepthTest.size() > 0) {
		// use lighting shader
		if (lightingShader != nullptr) {
			lightingShader->useProgram(this);
		}

		//
		renderer->disableDepthBufferTest();

		// render post processing objects
		for (auto i = 0; i < Entity::RENDERPASS_MAX; i++) {
			auto renderPass = static_cast<Entity::RenderPass>(Math::pow(2, i));
			if ((renderPassMask & renderPass) == renderPass) {
				entityRenderer->render(
					renderPass,
					visibleDecomposedEntities.objectsNoDepthTest,
					true,
					((renderTypes & EntityRenderer::RENDERTYPE_NORMALS) == EntityRenderer::RENDERTYPE_NORMALS?EntityRenderer::RENDERTYPE_NORMALS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTUREARRAYS) == EntityRenderer::RENDERTYPE_TEXTUREARRAYS?EntityRenderer::RENDERTYPE_TEXTUREARRAYS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_EFFECTCOLORS) == EntityRenderer::RENDERTYPE_EFFECTCOLORS?EntityRenderer::RENDERTYPE_EFFECTCOLORS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_MATERIALS) == EntityRenderer::RENDERTYPE_MATERIALS?EntityRenderer::RENDERTYPE_MATERIALS:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTURES) == EntityRenderer::RENDERTYPE_TEXTURES?EntityRenderer::RENDERTYPE_TEXTURES:0) |
					((renderTypes & EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY) == EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY?EntityRenderer::RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY:0)|
					((renderTypes & EntityRenderer::RENDERTYPE_LIGHTS) == EntityRenderer::RENDERTYPE_LIGHTS?EntityRenderer::RENDERTYPE_LIGHTS:0)
				);
			}
		}

		//
		renderer->enableDepthBufferTest();

		// unuse lighting shader
		if (lightingShader != nullptr) lightingShader->unUseProgram();

		// render shadows if required
		if (applyShadowMapping == true && shadowMapping != nullptr) shadowMapping->renderShadowMaps(visibleDecomposedEntities.objectsNoDepthTest);
	}

	if (doRenderLightSource == true) {
		auto _width = scaledWidth != -1?scaledWidth:width;
		auto _height = scaledHeight != -1?scaledHeight:height;
		//
		renderLightSources(_width, _height);
	}

	//
	Engine::renderer->setShaderPrefix(string());
	Engine::renderer->setEffectPass(0);
}

bool Engine::renderLightSources(int width, int height) {
	auto lightSourceVisible = false;
	for (auto& light: lights) {
		if (light.isEnabled() == false || light.isRenderLightSource() == false) continue;
		auto lightSourceSize = light.getLightSourceSize();
		auto lightSourcePixelSize = width < height?static_cast<float>(lightSourceSize) * static_cast<float>(width):static_cast<float>(lightSourceSize) * static_cast<float>(height);;
		Vector2 lightSourceDimension2D = Vector2(lightSourcePixelSize, lightSourcePixelSize);
		Vector2 lightSourcePosition2D;
		Vector3 lightSourcePosition = Vector3(light.getPosition().getX(), light.getPosition().getY(), light.getPosition().getZ());
		lightSourcePosition.scale(1.0f / light.getPosition().getW());
		auto visible = computeScreenCoordinateByWorldCoordinate(lightSourcePosition, lightSourcePosition2D, width, height);
		lightSourcePosition2D.sub(lightSourceDimension2D.clone().scale(0.5f));
		if (visible == true) {
			texture2DRenderShader->renderTexture(this, lightSourcePosition2D, lightSourceDimension2D, light.getLightSourceTextureId(), width, height);
			lightSourceVisible = true;
		}
	}
	return lightSourceVisible;
}

void Engine::dumpShaders() {
	for (auto shaderType = 0; shaderType < SHADERTYPE_MAX; shaderType++)
	for (auto& shaderId: getRegisteredShader(static_cast<ShaderType>(shaderType))) {
		string shaderTypeString = "unknowm";
		switch (shaderType) {
			case SHADERTYPE_OBJECT3D: shaderTypeString = "object3d"; break;
			case SHADERTYPE_POSTPROCESSING: shaderTypeString = "postprocessing"; break;
			default: break;
		}
		Console::println(string("TDME2::registered " + shaderTypeString + " shader: ") + shaderId);
		auto& defaultShaderParameters = getShaderParameterDefaults(shaderId);
		if (defaultShaderParameters.size() > 0) {
			Console::print("\t");
			for (auto it: defaultShaderParameters) {
				auto& parameterName = it.first;
				Console::print(parameterName);
				switch(it.second.getType()) {
					case ShaderParameter::TYPE_NONE:
						Console::print("=none; ");
						break;
					case ShaderParameter::TYPE_FLOAT:
						Console::print("=float(");
						Console::print(to_string(getShaderParameter(shaderId, parameterName).getFloatValue()));
						Console::print("); ");
						break;
					case ShaderParameter::TYPE_VECTOR3:
						{
							Console::print("=float(");
							auto shaderParameterArray = getShaderParameter(shaderId, parameterName).getVector3Value().getArray();
							for (auto i = 0; i < shaderParameterArray.size(); i++) {
								if (i != 0) Console::print(",");
								Console::print(to_string(shaderParameterArray[i]));
							}
							Console::print("); ");
						}
						break;
					default:
						Console::print("=unknown; ");
						break;
				}
			}
			Console::println();
		}
	}
}
