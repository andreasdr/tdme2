#include <tdme/engine/subsystems/rendering/EntityRenderer.h>

#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityHierarchy.h>
#include <tdme/engine/EnvironmentMapping.h>
#include <tdme/engine/FogParticleSystem.h>
#include <tdme/engine/LinesObject3D.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/PointsParticleSystem.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/PBRMaterialProperties.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/physics/CollisionDetection.h>
#include <tdme/engine/subsystems/lighting/LightingShader.h>
#include <tdme/engine/subsystems/lighting/LightingShaderConstants.h>
#include <tdme/engine/subsystems/lines/LinesShader.h>
#include <tdme/engine/subsystems/manager/VBOManager.h>
#include <tdme/engine/subsystems/manager/VBOManager_VBOManaged.h>
#include <tdme/engine/subsystems/rendering/BatchRendererPoints.h>
#include <tdme/engine/subsystems/rendering/BatchRendererTriangles.h>
#include <tdme/engine/subsystems/rendering/Object3DBase.h>
#include <tdme/engine/subsystems/rendering/Object3DNode.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeMesh.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeRenderer.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer_InstancedRenderFunctionParameters.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer_TransparentRenderFacesGroupPool.h>
#include <tdme/engine/subsystems/rendering/ObjectBuffer.h>
#include <tdme/engine/subsystems/rendering/RenderTransparentRenderPointsPool.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFace.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesGroup.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesPool.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderPoint.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderPointsPool.h>
#include <tdme/engine/subsystems/particlesystem/FogParticleSystemInternal.h>
#include <tdme/engine/subsystems/particlesystem/ParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/ParticlesShader.h>
#include <tdme/engine/subsystems/particlesystem/PointsParticleSystemInternal.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/shadowmapping/ShadowMapping.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Matrix4x4Negative.h>
#include <tdme/math/Vector2.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/threading/Semaphore.h>
#include <tdme/os/threading/Thread.h>
#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/FloatBuffer.h>
#include <tdme/utilities/Pool.h>
#include <tdme/utilities/Console.h>

using std::find;
using std::map;
using std::set;
using std::sort;
using std::vector;
using std::string;
using std::to_string;
using std::unordered_map;
using std::unordered_set;

using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityHierarchy;
using EnvironmentMappingEntity = tdme::engine::EnvironmentMapping;
using tdme::engine::FogParticleSystem;
using tdme::engine::LinesObject3D;
using tdme::engine::Object3D;
using tdme::engine::PointsParticleSystem;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Node;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::PBRMaterialProperties;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::physics::CollisionDetection;
using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lighting::LightingShaderConstants;
using tdme::engine::subsystems::lines::LinesShader;
using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::manager::VBOManager_VBOManaged;
using tdme::engine::subsystems::rendering::BatchRendererPoints;
using tdme::engine::subsystems::rendering::BatchRendererTriangles;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::engine::subsystems::rendering::Object3DNodeMesh;
using tdme::engine::subsystems::rendering::Object3DNodeRenderer;
using tdme::engine::subsystems::rendering::EntityRenderer_InstancedRenderFunctionParameters;
using tdme::engine::subsystems::rendering::EntityRenderer_TransparentRenderFacesGroupPool;
using tdme::engine::subsystems::rendering::ObjectBuffer;
using tdme::engine::subsystems::rendering::RenderTransparentRenderPointsPool;
using tdme::engine::subsystems::rendering::TransparentRenderFace;
using tdme::engine::subsystems::rendering::TransparentRenderFacesGroup;
using tdme::engine::subsystems::rendering::TransparentRenderFacesPool;
using tdme::engine::subsystems::rendering::TransparentRenderPoint;
using tdme::engine::subsystems::rendering::TransparentRenderPointsPool;
using tdme::engine::subsystems::particlesystem::FogParticleSystemInternal;
using tdme::engine::subsystems::particlesystem::ParticleEmitter;
using tdme::engine::subsystems::particlesystem::ParticlesShader;
using tdme::engine::subsystems::particlesystem::PointsParticleSystemInternal;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Matrix4x4Negative;
using tdme::math::Vector3;
using tdme::os::threading::Semaphore;
using tdme::os::threading::Thread;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::Pool;
using tdme::utilities::Console;

constexpr int32_t EntityRenderer::BATCHRENDERER_MAX;
constexpr int32_t EntityRenderer::INSTANCEDRENDERING_OBJECTS_MAX;

EntityRenderer::EntityRenderer(Engine* engine, Renderer* renderer) {
	this->engine = engine;
	this->renderer = renderer;
	transparentRenderFacesGroupPool = new EntityRenderer_TransparentRenderFacesGroupPool();
	transparentRenderFacesPool = new TransparentRenderFacesPool();
	renderTransparentRenderPointsPool = new RenderTransparentRenderPointsPool(65535);
	psePointBatchRenderer = new BatchRendererPoints(renderer, 0);
	threadCount = renderer->isSupportingMultithreadedRendering() == true?Engine::getThreadCount():1;
	contexts.resize(threadCount);
	if (this->renderer->isInstancedRenderingAvailable() == true) {
		for (auto& context: contexts) {
			context.bbEffectColorMuls = ByteBuffer::allocate(4 * sizeof(float) * INSTANCEDRENDERING_OBJECTS_MAX);
			context.bbEffectColorAdds = ByteBuffer::allocate(4 * sizeof(float) * INSTANCEDRENDERING_OBJECTS_MAX);
			context.bbMvMatrices = ByteBuffer::allocate(16 * sizeof(float) * INSTANCEDRENDERING_OBJECTS_MAX);
		}
	}
}

EntityRenderer::~EntityRenderer() {
	if (this->renderer->isInstancedRenderingAvailable() == true) {
		for (auto& context: contexts) {
			delete context.bbEffectColorMuls;
			delete context.bbEffectColorAdds;
			delete context.bbMvMatrices;
		}
	}
	for (auto batchRenderer: trianglesBatchRenderers) {
		delete batchRenderer;
	}
	delete transparentRenderFacesGroupPool;
	delete transparentRenderFacesPool;
	delete renderTransparentRenderPointsPool;
	delete psePointBatchRenderer;
}

void EntityRenderer::initialize()
{
	psePointBatchRenderer->initialize();
	for (auto i = 0; i < threadCount; i++) {
		auto created = false;
		auto vboManaged = Engine::getInstance()->getVBOManager()->addVBO("tdme.object3drenderer.instancedrendering." + to_string(i), 3, false, false, created);
		contexts[i].vboInstancedRenderingIds = vboManaged->getVBOIds();
	}
}

void EntityRenderer::dispose()
{
	// dispose VBOs
	for (auto i = 0; i < threadCount; i++) {
		Engine::getInstance()->getVBOManager()->removeVBO("tdme.object3drenderer.instancedrendering." + to_string(i));
	}
	// dispose batch vbo renderer
	for (auto batchRenderer: trianglesBatchRenderers) {
		batchRenderer->dispose();
		batchRenderer->release();
	}
	psePointBatchRenderer->dispose();
}

BatchRendererTriangles* EntityRenderer::acquireTrianglesBatchRenderer()
{
	// check for free batch vbo renderer
	auto i = 0;
	for (auto batchRenderer: trianglesBatchRenderers) {
		if (batchRenderer->acquire()) return batchRenderer;
		i++;
	}
	// try to add one
	if (i < BATCHRENDERER_MAX) {
		auto batchRenderer = new BatchRendererTriangles(renderer, i);
		batchRenderer->initialize();
		trianglesBatchRenderers.push_back(batchRenderer);
		if (batchRenderer->acquire()) return batchRenderer;

	}
	Console::println(string("EntityRenderer::acquireTrianglesBatchRenderer()::failed"));
	// nope
	return nullptr;
}

void EntityRenderer::reset()
{
	objectsByShadersAndModels.clear();
}

void EntityRenderer::render(Entity::RenderPass renderPass, const vector<Object3D*>& objects, bool renderTransparentFaces, int32_t renderTypes)
{
	// clear transparent render faces data
	transparentRenderFacesPool->reset();
	releaseTransparentFacesGroups();

	if (renderer->isSupportingMultithreadedRendering() == false) {
		renderFunction(1, 0, renderPass, objects, objectsByShadersAndModels, renderTransparentFaces, renderTypes, transparentRenderFacesPool);
	} else {
		EntityRenderer_InstancedRenderFunctionParameters parameters;
		parameters.renderPass = renderPass;
		parameters.objects = objects;
		parameters.collectTransparentFaces = renderTransparentFaces;
		parameters.renderTypes = renderTypes;

		for (auto engineThread: Engine::engineThreads) engineThread->engine = engine;
		for (auto engineThread: Engine::engineThreads) engineThread->rendering.parameters = parameters;
		for (auto engineThread: Engine::engineThreads) engineThread->state = Engine::EngineThread::STATE_RENDERING;

		renderFunction(threadCount, 0, renderPass, objects, objectsByShadersAndModels, renderTransparentFaces, renderTypes, transparentRenderFacesPool);

		for (auto engineThread: Engine::engineThreads) while(engineThread->state == Engine::EngineThread::STATE_RENDERING);
		for (auto engineThread: Engine::engineThreads) transparentRenderFacesPool->merge(engineThread->rendering.transparentRenderFacesPool);
	}

	// use default context
	auto context = renderer->getDefaultContext();
	// render transparent render faces if any exist
	auto& transparentRenderFaces = transparentRenderFacesPool->getTransparentRenderFaces();
	if (transparentRenderFaces.size() > 0) {
		// sort transparent render faces from far to near
		sort(transparentRenderFaces.begin(), transparentRenderFaces.end(), TransparentRenderFace::compare);
		// second render pass, draw color buffer for transparent objects
		// 	set up blending, but no culling and no depth buffer
		//	TODO: enabling depth buffer let shadow disappear
		//renderer->disableDepthBufferWriting();  // TODO: a.drewke, verify that this works ok in all cases?
		renderer->disableCulling(context);
		renderer->enableBlending();
		// disable foliage animation
		// reset shader
		// TODO: shader parameters
		renderer->setShader(renderer->getDefaultContext(), string());
		renderer->onUpdateShader(renderer->getDefaultContext());
		// have identity texture matrix
		renderer->getTextureMatrix(context).identity();
		renderer->onUpdateTextureMatrix(context);
		// actually this should not make any difference as culling is disabled
		// but having a fixed value is not a bad idea except that it is a renderer call
		// TODO: confirm this
		renderer->setFrontFace(context, renderer->FRONTFACE_CCW);
		for (auto transparentRenderFace: transparentRenderFaces) {
			// do we have any faces yet?
			if (nodeTransparentRenderFaces.size() == 0) {
				// nope, so add this one
				nodeTransparentRenderFaces.push_back(transparentRenderFace);
			} else
			// do we have more than face already?
			if (nodeTransparentRenderFaces.size() > 0) {
				// check if we have more of first type
				if (nodeTransparentRenderFaces[0]->object3DNode == transparentRenderFace->object3DNode) {
					// yep, we can add this one
					nodeTransparentRenderFaces.push_back(transparentRenderFace);
				} else {
					// no, render nodeed faces
					prepareTransparentFaces(nodeTransparentRenderFaces);
					// reset
					nodeTransparentRenderFaces.clear();
					// add current face
					nodeTransparentRenderFaces.push_back(transparentRenderFace);
				}
			}
		}
		// 	check if there are any left overs
		if (nodeTransparentRenderFaces.size() > 0) {
			prepareTransparentFaces(nodeTransparentRenderFaces);
			nodeTransparentRenderFaces.clear();
		}
		// render transparent faces nodes
		renderTransparentFacesGroups(context);
		//	no blending, but culling and depth buffer
		renderer->disableBlending();
		renderer->enableCulling(context);
		//renderer->enableDepthBufferWriting(); // TODO: a.drewke, verify that this works ok in all cases?
		// done!
	}
}

void EntityRenderer::prepareTransparentFaces(const vector<TransparentRenderFace*>& transparentRenderFaces)
{
	// all those faces should share the object and object 3d node, ...
	auto object3DNode = transparentRenderFaces[0]->object3DNode;
	auto object3D = static_cast<Object3D*>(object3DNode->object);
	// model view matrix to be used with given transparent render faces
	Matrix4x4 modelViewMatrix;
	if (object3DNode->mesh->skinning == true) {
		modelViewMatrix.identity();
	} else {
		modelViewMatrix.set(*object3DNode->nodeTransformationsMatrix).multiply(object3D->getTransformationsMatrix());
	}
	//
	auto model = object3DNode->object->getModel();
	auto& facesEntities = object3DNode->node->getFacesEntities();
	const FacesEntity* facesEntity = nullptr;
	// attributes we collect for a transparent render face node
	auto& effectColorAdd = object3D->getEffectColorAdd();
	auto& effectColorMul = object3D->getEffectColorMul();
	const Material* material = nullptr;
	auto textureCoordinates = false;
	Vector3 transformedVector;
	Vector3 transformedNormal;
	Vector2 transformedTextureCoordinate;
	// render transparent faces
	for (auto i = 0; i < transparentRenderFaces.size(); i++) {
		auto transparentRenderFace = transparentRenderFaces[i];
		auto facesEntityIdx = transparentRenderFace->facesEntityIdx;
		// determine if faces entity and so material did switch between last face and current face
		if (facesEntity != &facesEntities[facesEntityIdx]) {
			facesEntity = &facesEntities[facesEntityIdx];
			material = facesEntity->getMaterial();
		}
		textureCoordinates = facesEntity->isTextureCoordinatesAvailable();
		// create node key
		auto transparentRenderFacesNodeKey = TransparentRenderFacesGroup::createKey(model, object3DNode, facesEntityIdx, effectColorAdd, effectColorMul, material, textureCoordinates, object3D->getShader());
		// get node
		TransparentRenderFacesGroup* trfNode = nullptr;
		auto trfNodeIt = transparentRenderFacesGroups.find(transparentRenderFacesNodeKey);
		if (trfNodeIt != transparentRenderFacesGroups.end()) {
			trfNode = trfNodeIt->second;
		}
		if (trfNode == nullptr) {
			// we do not have the node, create node
			trfNode = transparentRenderFacesGroupPool->allocate();
			trfNode->set(this, model, object3DNode, facesEntityIdx, effectColorAdd, effectColorMul, material, textureCoordinates, object3D->getShader());
			transparentRenderFacesGroups[transparentRenderFacesNodeKey] = trfNode;
		}
		auto& textureCoordinates = transparentRenderFace->object3DNode->mesh->node->getTextureCoordinates();
		for (auto vertexIdx = 0; vertexIdx < 3; vertexIdx++) {
			auto arrayIdx = transparentRenderFace->object3DNode->mesh->indices[transparentRenderFace->faceIdx * 3 + vertexIdx];
			trfNode->addVertex(
				modelViewMatrix.multiply((*transparentRenderFace->object3DNode->mesh->vertices)[arrayIdx], transformedVector),
				modelViewMatrix.multiplyNoTranslation((*transparentRenderFace->object3DNode->mesh->normals)[arrayIdx], transformedNormal),
				transparentRenderFace->object3DNode->textureMatricesByEntities[facesEntityIdx].multiply(
					textureCoordinates.size() > 0?
						Vector2(textureCoordinates[arrayIdx].getArray()):
						Vector2(0.0f, 0.0f),
					transformedTextureCoordinate
				)
			);
		}
	}
}

void EntityRenderer::renderTransparentFacesGroups(void* context) {
	for (auto it: transparentRenderFacesGroups) {
		it.second->render(engine, renderer, context);
	}
}

void EntityRenderer::releaseTransparentFacesGroups()
{
	for (auto it: transparentRenderFacesGroups) {
		transparentRenderFacesGroupPool->release(it.second);
	}
	transparentRenderFacesGroups.clear();
}

void EntityRenderer::renderObjectsOfSameTypeNonInstanced(const vector<Object3D*>& objects, bool collectTransparentFaces, int32_t renderTypes) {
	Vector3 objectCamFromAxis;
	auto camera = engine->getCamera();

	// use default context
	auto& object3DRenderContext = contexts[0];
	auto context = renderer->getDefaultContext();

	//
	auto shadowMapping = engine->getShadowMapping();
	Matrix4x4 modelViewMatrix;
	Matrix4x4 cameraMatrix;
	cameraMatrix.set(renderer->getModelViewMatrix());
	// render faces entities
	auto currentFrontFace = -1;
	auto firstObject = objects[0];
	// all objects share the same object 3d node structure, so we just take the first one
	vector<int32_t>* boundVBOBaseIds = nullptr;
	vector<int32_t>* boundVBOTangentBitangentIds = nullptr;
	vector<int32_t>* boundVBOOrigins = nullptr;
	for (auto object3DNodeIdx = 0; object3DNodeIdx < firstObject->object3dNodes.size(); object3DNodeIdx++) {
		auto object3DNode = firstObject->object3dNodes[object3DNodeIdx];
		// render each faces entity
		auto& facesEntities = object3DNode->node->getFacesEntities();
		auto faceIdx = 0;
		auto facesEntityIdxCount = facesEntities.size();
		for (auto faceEntityIdx = 0; faceEntityIdx < facesEntityIdxCount; faceEntityIdx++) {
			auto facesEntity = &facesEntities[faceEntityIdx];
			auto isTextureCoordinatesAvailable = facesEntity->isTextureCoordinatesAvailable();
			auto faces = facesEntity->getFaces().size() * firstObject->instances;
			auto facesToRender = facesEntity->getFaces().size() * firstObject->enabledInstances;
			// material
			auto material = facesEntity->getMaterial();
			auto specularMaterialProperties = material != nullptr?material->getSpecularMaterialProperties():nullptr;
			// determine if transparent
			auto transparentFacesEntity = false;
			//	via material
			if (specularMaterialProperties != nullptr) {
				if (specularMaterialProperties->hasColorTransparency() == true || specularMaterialProperties->hasTextureTransparency() == true) transparentFacesEntity = true;
				if (specularMaterialProperties->hasDiffuseTextureTransparency() == true && specularMaterialProperties->hasDiffuseTextureMaskedTransparency() == true) {
					renderer->disableCulling(context);
				}
			}
			// skip, if requested
			if (transparentFacesEntity == true) {
				// add to transparent render faces, if requested
				auto objectCount = objects.size();
				for (auto objectIdx = 0; objectIdx < objectCount; objectIdx++) {
					auto object = objects[objectIdx];
					auto _object3DNode = object->object3dNodes[object3DNodeIdx];
					// set up textures
					Object3DNode::setupTextures(renderer, context, object3DNode, faceEntityIdx);
					// set up transparent render faces
					if (collectTransparentFaces == true) {
						transparentRenderFacesPool->createTransparentRenderFaces(
							(_object3DNode->mesh->skinning == true?
								modelViewMatrix.identity():
								modelViewMatrix.set(*_object3DNode->nodeTransformationsMatrix).multiply(object->getTransformationsMatrix())
							).multiply(cameraMatrix),
							object->object3dNodes[object3DNodeIdx],
							faceEntityIdx,
							faceIdx
						);
					}
				}
				// keep track of rendered faces
				faceIdx += faces;
				// skip to next entity
				continue;
			}
			// draw this faces entity for each object
			bool materialUpdateOnly = false;
			auto objectCount = objects.size();
			for (auto objectIdx = 0; objectIdx < objectCount; objectIdx++) {
				auto object = objects[objectIdx];
				auto _object3DNode = object->object3dNodes[object3DNodeIdx];
				//	check transparency via effect
				if (object->effectColorMul.getAlpha() < 1.0f - Math::EPSILON ||
					object->effectColorAdd.getAlpha() < -Math::EPSILON) {
					// add to transparent render faces, if requested
					if (collectTransparentFaces == true) {
						transparentRenderFacesPool->createTransparentRenderFaces(
							(_object3DNode->mesh->skinning == true ?
								modelViewMatrix.identity() :
								modelViewMatrix.set(*_object3DNode->nodeTransformationsMatrix).multiply(object->getTransformationsMatrix())
							).multiply(cameraMatrix),
							_object3DNode,
							faceEntityIdx,
							faceIdx
						);
					}
					// skip to next object
					continue;
				}
				// shader
				auto objectShader = object->getDistanceShader().length() == 0?
					object->getShader():
					objectCamFromAxis.set(object->getBoundingBoxTransformed()->getCenter()).sub(camera->getLookFrom()).computeLengthSquared() < Math::square(object->getDistanceShaderDistance())?
						object->getShader():
						object->getDistanceShader();
				// TODO: shader parameters
				if (renderer->getShader(context) != objectShader) {
					renderer->setShader(context, objectShader);
					renderer->onUpdateShader(context);
					// update lights
					for (auto j = 0; j < engine->lights.size(); j++) engine->lights[j].update(context);
					materialUpdateOnly = false;
				}
				// set up material on first object
				string materialKey;
				if (materialUpdateOnly == false || checkMaterialChangable(_object3DNode, faceEntityIdx, renderTypes) == true) {
					setupMaterial(context, _object3DNode, faceEntityIdx, renderTypes, materialUpdateOnly, materialKey);
					// only update materials for next calls
					materialUpdateOnly = true;
				}
				// bind buffer base objects if not bound yet
				auto currentVBOIds = _object3DNode->renderer->vboBaseIds;
				if (boundVBOBaseIds != currentVBOIds) {
					boundVBOBaseIds = currentVBOIds;
					//	texture coordinates
					if (isTextureCoordinatesAvailable == true &&
						(((renderTypes & RENDERTYPE_TEXTUREARRAYS) == RENDERTYPE_TEXTUREARRAYS) ||
						((renderTypes & RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY) == RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY && specularMaterialProperties != nullptr && specularMaterialProperties->hasDiffuseTextureMaskedTransparency() == true))) {
						renderer->bindTextureCoordinatesBufferObject(context, (*currentVBOIds)[3]);
					}
					// 	indices
					renderer->bindIndicesBufferObject(context, (*currentVBOIds)[0]);
					// 	vertices
					renderer->bindVerticesBufferObject(context, (*currentVBOIds)[1]);
					// 	normals
					if ((renderTypes & RENDERTYPE_NORMALS) == RENDERTYPE_NORMALS) renderer->bindNormalsBufferObject(context, (*currentVBOIds)[2]);
				}
				// bind tangent, bitangend buffers if not yet bound
				auto currentVBONormalMappingIds = _object3DNode->renderer->vboNormalMappingIds;
				if ((renderTypes & RENDERTYPE_NORMALS) == RENDERTYPE_NORMALS &&
					renderer->isNormalMappingAvailable() && currentVBONormalMappingIds != nullptr && currentVBONormalMappingIds != boundVBOTangentBitangentIds) {
					// tangent
					renderer->bindTangentsBufferObject(context, (*currentVBONormalMappingIds)[0]);
					// bitangent
					renderer->bindBitangentsBufferObject(context, (*currentVBONormalMappingIds)[1]);
				}
				// bind render node object origins
				auto currentVBOOrigins = _object3DNode->renderer->vboOrigins;
				if (currentVBOOrigins != nullptr && currentVBOOrigins != boundVBOOrigins) {
					renderer->bindOriginsBufferObject(context, (*currentVBOOrigins)[0]);
				}
				// set up local -> world transformations matrix
				renderer->getModelViewMatrix().set(
					_object3DNode->mesh->skinning == true?
						modelViewMatrix.identity():
						modelViewMatrix.set(*_object3DNode->nodeTransformationsMatrix).multiply(object->getTransformationsMatrix())
				);
				renderer->onUpdateModelViewMatrix(context);
				// set up front face
				auto objectFrontFace = object3DRenderContext.matrix4x4Negative.isNegative(renderer->getModelViewMatrix()) == false ? renderer->FRONTFACE_CCW : renderer->FRONTFACE_CW;
				if (objectFrontFace != currentFrontFace) {
					renderer->setFrontFace(context, objectFrontFace);
					currentFrontFace = objectFrontFace;
				}
				// set up effect color
				if ((renderTypes & RENDERTYPE_EFFECTCOLORS) == RENDERTYPE_EFFECTCOLORS) {
					renderer->getEffectColorMul(context) = object->effectColorMul.getArray();
					renderer->getEffectColorAdd(context) = object->effectColorAdd.getArray();
					renderer->onUpdateEffect(context);
				}
				// do transformation start to shadow mapping
				if ((renderTypes & RENDERTYPE_SHADOWMAPPING) == RENDERTYPE_SHADOWMAPPING &&
					shadowMapping != nullptr) {
					shadowMapping->startObjectTransformations(
						context,
						_object3DNode->mesh->skinning == true ?
							modelViewMatrix.identity() :
							modelViewMatrix.set(*_object3DNode->nodeTransformationsMatrix).multiply(object->getTransformationsMatrix())
					);
				}
				// set up texture matrix
				//	TODO: check if texture is in use
				if ((renderTypes & RENDERTYPE_TEXTURES) == RENDERTYPE_TEXTURES ||
					(renderTypes & RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY) == RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY) {
					renderer->getTextureMatrix(context).set(_object3DNode->textureMatricesByEntities[faceEntityIdx]);
					renderer->onUpdateTextureMatrix(context);
				}
				// draw
				renderer->drawIndexedTrianglesFromBufferObjects(context, facesToRender, faceIdx);
				// do transformations end to shadow mapping
				if ((renderTypes & RENDERTYPE_SHADOWMAPPING) == RENDERTYPE_SHADOWMAPPING &&
					shadowMapping != nullptr) {
					shadowMapping->endObjectTransformations();
				}
			}
			// keep track of rendered faces
			faceIdx += faces;
			if (specularMaterialProperties != nullptr) {
				if (specularMaterialProperties->hasDiffuseTextureTransparency() == true && specularMaterialProperties->hasDiffuseTextureMaskedTransparency() == true) {
					renderer->enableCulling(context);
				}
			}
		}
	}
	// unbind buffers
	renderer->unbindBufferObjects(context);
	// restore model view matrix / view matrix
	renderer->getModelViewMatrix().set(cameraMatrix);
}

void EntityRenderer::renderObjectsOfSameTypeInstanced(int threadIdx, const vector<Object3D*>& objects, bool collectTransparentFaces, int32_t renderTypes, TransparentRenderFacesPool* transparentRenderFacesPool)
{
	// contexts
	auto& object3DRenderContext = contexts[threadIdx];
	auto context = renderer->getContext(threadIdx);

	//
	Vector3 objectCamFromAxis;
	Matrix4x4 cameraMatrix(renderer->getCameraMatrix());
	Matrix4x4 modelViewMatrixTemp;
	Matrix4x4 modelViewMatrix;

	//
	auto camera = engine->camera;
	auto frontFace = -1;
	auto cullingMode = -1;

	// render faces entities
	auto firstObject = objects[0];

	// all objects share the same object 3d node structure, so we just take the first one
	for (auto object3DNodeIdx = 0; object3DNodeIdx < firstObject->object3dNodes.size(); object3DNodeIdx++) {
		auto object3DNode = firstObject->object3dNodes[object3DNodeIdx];
		// render each faces entity
		auto& facesEntities = object3DNode->node->getFacesEntities();
		auto faceIdx = 0;
		auto facesEntityIdxCount = facesEntities.size();
		for (auto faceEntityIdx = 0; faceEntityIdx < facesEntityIdxCount; faceEntityIdx++) {
			auto facesEntity = &facesEntities[faceEntityIdx];
			auto isTextureCoordinatesAvailable = facesEntity->isTextureCoordinatesAvailable();
			auto faces = facesEntity->getFaces().size() * firstObject->instances;
			auto facesToRender = facesEntity->getFaces().size() * firstObject->enabledInstances;
			// material
			auto material = facesEntity->getMaterial();
			auto specularMaterialProperties = material != nullptr?material->getSpecularMaterialProperties():nullptr;
			// determine if transparent
			auto transparentFacesEntity = false;
			//	via material
			if (specularMaterialProperties != nullptr) {
				if (specularMaterialProperties->hasColorTransparency() == true || specularMaterialProperties->hasTextureTransparency() == true) transparentFacesEntity = true;

				// skip, if requested
				if (transparentFacesEntity == true) {
					// add to transparent render faces, if requested
					auto objectCount = objects.size();
					for (auto objectIdx = 0; objectIdx < objectCount; objectIdx++) {
						auto object = objects[objectIdx];
						auto _object3DNode = object->object3dNodes[object3DNodeIdx];
						// set up textures
						Object3DNode::setupTextures(renderer, context, object3DNode, faceEntityIdx);
						// set up transparent render faces
						if (collectTransparentFaces == true) {
							transparentRenderFacesPool->createTransparentRenderFaces(
								(_object3DNode->mesh->skinning == true?
									modelViewMatrixTemp.identity() :
									modelViewMatrixTemp.set(*_object3DNode->nodeTransformationsMatrix).multiply(object->getTransformationsMatrix())
								).multiply(cameraMatrix),
								object->object3dNodes[object3DNodeIdx],
								faceEntityIdx,
								faceIdx
							);
						}
					}
					// keep track of rendered faces
					faceIdx += faces;
					// skip to next entity
					continue;
				}

				if (specularMaterialProperties->hasDiffuseTextureTransparency() == true && specularMaterialProperties->hasDiffuseTextureMaskedTransparency() == true) {
					if (cullingMode != 0) {
						renderer->disableCulling(context);
						cullingMode = 0;
					}
				} else {
					if (cullingMode != 1) {
						renderer->enableCulling(context);
						cullingMode = 1;
					}
				}
			} else {
				if (cullingMode != 1) {
					renderer->enableCulling(context);
					cullingMode = 1;
				}
			}

			// draw this faces entity for each object
			object3DRenderContext.objectsToRender = objects;
			do {
				auto hadFrontFaceSetup = false;
				auto hadShaderSetup = false;
				Matrix4x4Negative matrix4x4Negative;

				Vector3 objectCamFromAxis;
				Matrix4x4 modelViewMatrixTemp;
				Matrix4x4 modelViewMatrix;

				FloatBuffer fbEffectColorMuls = object3DRenderContext.bbEffectColorMuls->asFloatBuffer();
				FloatBuffer fbEffectColorAdds = object3DRenderContext.bbEffectColorAdds->asFloatBuffer();
				FloatBuffer fbMvMatrices = object3DRenderContext.bbMvMatrices->asFloatBuffer();

				string materialKey;
				bool materialUpdateOnly = false;
				vector<int32_t>* boundVBOBaseIds = nullptr;
				vector<int32_t>* boundVBOTangentBitangentIds = nullptr;
				vector<int32_t>* boundVBOOrigins = nullptr;
				int32_t boundEnvironmentMappingCubeMapTextureId = -1;
				auto objectCount = object3DRenderContext.objectsToRender.size();

				//
				auto textureMatrix = object3DRenderContext.objectsToRender[0]->object3dNodes[object3DNodeIdx]->textureMatricesByEntities[faceEntityIdx];

				// draw objects
				for (auto objectIdx = 0; objectIdx < objectCount; objectIdx++) {
					auto object = object3DRenderContext.objectsToRender[objectIdx];
					auto _object3DNode = object->object3dNodes[object3DNodeIdx];

					//	check transparency via effect
					if (object->effectColorMul.getAlpha() < 1.0f - Math::EPSILON ||
						object->effectColorAdd.getAlpha() < -Math::EPSILON) {
						// add to transparent render faces, if requested
						if (collectTransparentFaces == true) {
							transparentRenderFacesPool->createTransparentRenderFaces(
								(_object3DNode->mesh->skinning == true ?
									modelViewMatrixTemp.identity() :
									modelViewMatrixTemp.set(*_object3DNode->nodeTransformationsMatrix).multiply(object->getTransformationsMatrix())
								).multiply(cameraMatrix),
								_object3DNode,
								faceEntityIdx,
								faceIdx
							);
						}
						// skip to next object
						continue;
					}

					// limit objects to render to INSTANCEDRENDERING_OBJECTS_MAX
					if (fbMvMatrices.getPosition() / 16 == INSTANCEDRENDERING_OBJECTS_MAX) {
						object3DRenderContext.objectsNotRendered.push_back(object);
						continue;
					}

					// check if texture matrix did change
					if (_object3DNode->textureMatricesByEntities[faceEntityIdx].equals(textureMatrix) == false) {
						object3DRenderContext.objectsNotRendered.push_back(object);
						continue;
					}

					// check if shader did change
					// shader
					auto objectShader = object->getDistanceShader().length() == 0?
						object->getShader():
						objectCamFromAxis.set(object->getBoundingBoxTransformed()->getCenter()).sub(camera->getLookFrom()).computeLengthSquared() < Math::square(object->getDistanceShaderDistance())?
							object->getShader():
							object->getDistanceShader();
					if (hadShaderSetup == false) {
						if (objectShader != renderer->getShader(context)) {
							renderer->setShader(context, objectShader);
							renderer->onUpdateShader(context);
							for (auto j = 0; j < engine->lights.size(); j++) engine->lights[j].update(context);
							// issue upload matrices
							renderer->onUpdateCameraMatrix(context);
							renderer->onUpdateProjectionMatrix(context);
							renderer->onUpdateTextureMatrix(context);
						}
						hadShaderSetup = true;
					} else
					if (objectShader != renderer->getShader(context)) {
						object3DRenderContext.objectsNotRendered.push_back(object);
						continue;
					}

					// set up material on first object and update on succeeding
					auto materialKeyCurrent = materialKey;
					if (materialUpdateOnly == false || checkMaterialChangable(_object3DNode, faceEntityIdx, renderTypes) == true) {
						setupMaterial(context, _object3DNode, faceEntityIdx, renderTypes, materialUpdateOnly, materialKeyCurrent, materialKey);
						// only update material for next material calls
						if (materialUpdateOnly == false) {
							materialKey = materialKeyCurrent;
							materialUpdateOnly = true;
						}
					}

					// check if material key has not been set yet
					if (materialKey != materialKeyCurrent) {
						object3DRenderContext.objectsNotRendered.push_back(object);
						continue;
					}

					// bind buffer base objects if not bound yet
					auto currentVBOBaseIds = _object3DNode->renderer->vboBaseIds;
					if (boundVBOBaseIds == nullptr) {
						boundVBOBaseIds = currentVBOBaseIds;
						//	texture coordinates
						if (isTextureCoordinatesAvailable == true &&
							(((renderTypes & RENDERTYPE_TEXTUREARRAYS) == RENDERTYPE_TEXTUREARRAYS) ||
							((renderTypes & RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY) == RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY && specularMaterialProperties != nullptr && specularMaterialProperties->hasDiffuseTextureMaskedTransparency() == true))) {
							renderer->bindTextureCoordinatesBufferObject(context, (*currentVBOBaseIds)[3]);
						}
						// 	indices
						renderer->bindIndicesBufferObject(context, (*currentVBOBaseIds)[0]);
						// 	vertices
						renderer->bindVerticesBufferObject(context, (*currentVBOBaseIds)[1]);
						// 	normals
						if ((renderTypes & RENDERTYPE_NORMALS) == RENDERTYPE_NORMALS) {
							renderer->bindNormalsBufferObject(context, (*currentVBOBaseIds)[2]);
						}
					} else
					// check if buffers did change, then skip and render in next step
					if (boundVBOBaseIds != currentVBOBaseIds) {
						object3DRenderContext.objectsNotRendered.push_back(object);
						continue;
					}
					// bind tangent, bitangend buffers
					auto currentVBONormalMappingIds = _object3DNode->renderer->vboNormalMappingIds;
					if ((renderTypes & RENDERTYPE_NORMALS) == RENDERTYPE_NORMALS &&
						renderer->isNormalMappingAvailable() == true && currentVBONormalMappingIds != nullptr) {
						// bind tangent, bitangend buffers if not yet done
						if (boundVBOTangentBitangentIds == nullptr) {
							// tangent
							renderer->bindTangentsBufferObject(context, (*currentVBONormalMappingIds)[0]);
							// bitangent
							renderer->bindBitangentsBufferObject(context, (*currentVBONormalMappingIds)[1]);
							//
							boundVBOTangentBitangentIds = currentVBONormalMappingIds;
						} else
						// check if buffers did change, then skip and render in next step
						if (currentVBONormalMappingIds != boundVBOTangentBitangentIds) {
							object3DRenderContext.objectsNotRendered.push_back(object);
							continue;
						}
					}

					// bind render node object origins
					auto currentVBOOrigins = _object3DNode->renderer->vboOrigins;
					if (currentVBOOrigins != nullptr) {
						// bind render node object origins if not yet done
						if (boundVBOOrigins == nullptr) {
							renderer->bindOriginsBufferObject(context, (*currentVBOOrigins)[0]);
							//
							boundVBOOrigins = currentVBOOrigins;
						} else
						// check if buffers did change, then skip and render in next step
						if (currentVBOOrigins != boundVBOOrigins) {
							object3DRenderContext.objectsNotRendered.push_back(object);
							continue;
						}
					}

					// set up local -> world transformations matrix
					modelViewMatrix.set(
						_object3DNode->mesh->skinning == true?
							modelViewMatrixTemp.identity() :
							modelViewMatrixTemp.set(*_object3DNode->nodeTransformationsMatrix).
							multiply(object->getTransformationsMatrix())
					);

					// set up front face
					auto objectFrontFace = matrix4x4Negative.isNegative(modelViewMatrix) == false ? renderer->FRONTFACE_CCW : renderer->FRONTFACE_CW;
					// if front face changed just render in next step, this all makes only sense if culling is enabled
					if (cullingMode == 1) {
						if (hadFrontFaceSetup == false) {
							hadFrontFaceSetup = true;
							if (objectFrontFace != frontFace) {
								frontFace = objectFrontFace;
								renderer->setFrontFace(context, frontFace);
							}
						} else
						if (objectFrontFace != frontFace) {
							object3DRenderContext.objectsNotRendered.push_back(object);
							continue;
						}
					}

					if (object->getReflectionEnvironmentMappingId().empty() == false) {
						EnvironmentMappingEntity* environmentMappingEntity = dynamic_cast<EnvironmentMappingEntity*>(engine->getEntity(object->getReflectionEnvironmentMappingId()));
						if (environmentMappingEntity == nullptr) {
							auto environmentMappingEntityHierarchy = dynamic_cast<EntityHierarchy*>(engine->getEntity(object->getReflectionEnvironmentMappingId()));
							if (environmentMappingEntityHierarchy != nullptr) environmentMappingEntity = dynamic_cast<EnvironmentMappingEntity*>(environmentMappingEntityHierarchy->getEntity("environmentmapping"));
						}
						if (environmentMappingEntity != nullptr) {
							auto environmentMappingCubeMapTextureId = environmentMappingEntity->getCubeMapTextureId();
							if (boundEnvironmentMappingCubeMapTextureId == -1) {
								Vector3 translation;
								environmentMappingEntity->getTransformationsMatrix().getTranslation(translation);
								renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_ENVIRONMENT);
								renderer->bindCubeMapTexture(context, environmentMappingCubeMapTextureId);
								renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE);
								renderer->setEnvironmentMappingCubeMapPosition(context, translation.getArray());
								boundEnvironmentMappingCubeMapTextureId = environmentMappingCubeMapTextureId;
							} else
							if (boundEnvironmentMappingCubeMapTextureId != environmentMappingCubeMapTextureId) {
								object3DRenderContext.objectsNotRendered.push_back(object);
								continue;
							}
						}
					}

					// set up effect color
					if ((renderTypes & RENDERTYPE_EFFECTCOLORS) == RENDERTYPE_EFFECTCOLORS) {
						fbEffectColorMuls.put(object->effectColorMul.getArray());
						fbEffectColorAdds.put(object->effectColorAdd.getArray());
					}

					// push mv, mvp to layouts
					fbMvMatrices.put(modelViewMatrix.getArray());
				}

				// it can happen that all faces to be rendered were transparent ones, check this and skip if feasible
				auto objectsToRenderIssue = fbMvMatrices.getPosition() / 16;
				if (objectsToRenderIssue > 0) {
					// upload model view matrices
					{
						renderer->uploadBufferObject(context, (*object3DRenderContext.vboInstancedRenderingIds)[0], fbMvMatrices.getPosition() * sizeof(float), &fbMvMatrices);
						renderer->bindModelMatricesBufferObject(context, (*object3DRenderContext.vboInstancedRenderingIds)[0]);
					}

					// upload effects
					if ((renderTypes & RENDERTYPE_EFFECTCOLORS) == RENDERTYPE_EFFECTCOLORS) {
						// upload effect color mul
						renderer->uploadBufferObject(context, (*object3DRenderContext.vboInstancedRenderingIds)[1], fbEffectColorMuls.getPosition() * sizeof(float), &fbEffectColorMuls);
						renderer->bindEffectColorMulsBufferObject(context, (*object3DRenderContext.vboInstancedRenderingIds)[1], 1);
						renderer->uploadBufferObject(context, (*object3DRenderContext.vboInstancedRenderingIds)[2], fbEffectColorAdds.getPosition() * sizeof(float), &fbEffectColorAdds);
						renderer->bindEffectColorAddsBufferObject(context, (*object3DRenderContext.vboInstancedRenderingIds)[2], 1);
					}

					// set up texture matrix
					//	TODO: check if texture is in use
					if ((renderTypes & RENDERTYPE_TEXTURES) == RENDERTYPE_TEXTURES ||
						(renderTypes & RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY) == RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY) {
						renderer->getTextureMatrix(context).set(textureMatrix);
						renderer->onUpdateTextureMatrix(context);
					}

					// draw
					renderer->drawInstancedIndexedTrianglesFromBufferObjects(context, facesToRender, faceIdx, objectsToRenderIssue);
				}

				// clear list of objects we did not render
				object3DRenderContext.objectsToRender = object3DRenderContext.objectsNotRendered;
				object3DRenderContext.objectsNotRendered.clear();
			} while (object3DRenderContext.objectsToRender.size() > 0);

			// keep track of rendered faces
			faceIdx += faces;
		}
	}

	//
	if (cullingMode != 1) {
		renderer->enableCulling(context);
		cullingMode = 1;
	}

	// unbind buffers
	renderer->unbindBufferObjects(context);

	// reset objects to render
	object3DRenderContext.objectsToRender.clear();
	object3DRenderContext.objectsNotRendered.clear();
}

void EntityRenderer::setupMaterial(void* context, Object3DNode* object3DNode, int32_t facesEntityIdx, int32_t renderTypes, bool updateOnly, string& materialKey, const string& currentMaterialKey)
{
	auto& facesEntities = object3DNode->node->getFacesEntities();
	auto material = facesEntities[facesEntityIdx].getMaterial();
	// get material or use default
	if (material == nullptr) material = Material::getDefaultMaterial();
	auto specularMaterialProperties = material->getSpecularMaterialProperties();
	auto pbrMaterialProperties = material->getPBRMaterialProperties();

	// material key
	materialKey = material->getId();

	// setup textures
	Object3DNode::setupTextures(renderer, context, object3DNode, facesEntityIdx);

	//
	if (updateOnly == false) {
		if (renderer->getLighting(context) == renderer->LIGHTING_SPECULAR) {
			// apply materials
			if ((renderTypes & RENDERTYPE_MATERIALS) == RENDERTYPE_MATERIALS) {
				auto& rendererMaterial = renderer->getSpecularMaterial(context);
				rendererMaterial.ambient = specularMaterialProperties->getAmbientColor().getArray();
				rendererMaterial.diffuse = specularMaterialProperties->getDiffuseColor().getArray();
				rendererMaterial.specular = specularMaterialProperties->getSpecularColor().getArray();
				rendererMaterial.emission = specularMaterialProperties->getEmissionColor().getArray();
				rendererMaterial.shininess = specularMaterialProperties->getShininess();
				rendererMaterial.diffuseTextureMaskedTransparency = specularMaterialProperties->hasDiffuseTextureMaskedTransparency() == true?1:0;
				rendererMaterial.diffuseTextureMaskedTransparencyThreshold = specularMaterialProperties->getDiffuseTextureMaskedTransparencyThreshold();
				renderer->onUpdateMaterial(context);
			}
			if ((renderTypes & RENDERTYPE_TEXTURES) == RENDERTYPE_TEXTURES) {
				// bind specular texture
				if (renderer->isSpecularMappingAvailable() == true) {
					renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_SPECULAR);
					renderer->bindTexture(context, object3DNode->specularMaterialSpecularTextureIdsByEntities[facesEntityIdx]);
				}
				// bind normal texture
				if (renderer->isNormalMappingAvailable() == true) {
					renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_NORMAL);
					renderer->bindTexture(context, object3DNode->specularMaterialNormalTextureIdsByEntities[facesEntityIdx]);
				}
				// switch back texture unit to diffuse unit
				renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE);
			}
		} else
		if (renderer->getLighting(context) == renderer->LIGHTING_PBR) {
			// apply materials
			if ((renderTypes & RENDERTYPE_MATERIALS) == RENDERTYPE_MATERIALS) {
				auto& rendererMaterial = renderer->getPBRMaterial(context);
				if (pbrMaterialProperties == nullptr) {
					rendererMaterial.baseColorFactor = {{ 1.0f, 1.0f, 1.0f, 1.0f }};
					rendererMaterial.metallicFactor = 1.0f;
					rendererMaterial.roughnessFactor = 1.0f;
					rendererMaterial.normalScale = 1.0f;
					rendererMaterial.exposure = 1.0f;
					rendererMaterial.baseColorTextureMaskedTransparency = 0;
					rendererMaterial.baseColorTextureMaskedTransparencyThreshold = 0.0f;
				} else {
					rendererMaterial.baseColorFactor = pbrMaterialProperties->getBaseColorFactor().getArray();
					rendererMaterial.metallicFactor = pbrMaterialProperties->getMetallicFactor();
					rendererMaterial.roughnessFactor = pbrMaterialProperties->getRoughnessFactor();
					rendererMaterial.normalScale = pbrMaterialProperties->getNormalScale();
					rendererMaterial.exposure = pbrMaterialProperties->getExposure();
					rendererMaterial.baseColorTextureMaskedTransparency = pbrMaterialProperties->hasBaseColorTextureMaskedTransparency() == true?1:0;
					rendererMaterial.baseColorTextureMaskedTransparencyThreshold = pbrMaterialProperties->getBaseColorTextureMaskedTransparencyThreshold();
				}
				renderer->onUpdateMaterial(context);
			}
			if ((renderTypes & RENDERTYPE_TEXTURES) == RENDERTYPE_TEXTURES) {
				// bind metallic roughness texture
				renderer->setTextureUnit(context, LightingShaderConstants::PBR_TEXTUREUNIT_METALLICROUGHNESS);
				renderer->bindTexture(context, object3DNode->pbrMaterialMetallicRoughnessTextureIdsByEntities[facesEntityIdx]);
				// bind normal texture
				renderer->setTextureUnit(context, LightingShaderConstants::PBR_TEXTUREUNIT_NORMAL);
				renderer->bindTexture(context, object3DNode->pbrMaterialNormalTextureIdsByEntities[facesEntityIdx]);
				// switch back texture unit to base color unit
				renderer->setTextureUnit(context, LightingShaderConstants::PBR_TEXTUREUNIT_BASECOLOR);
			}
		}
	}

	// bind diffuse/base color texture
	if ((renderTypes & RENDERTYPE_TEXTURES) == RENDERTYPE_TEXTURES ||
		((renderTypes & RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY) == RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY)) {
		if (renderer->getLighting(context) == renderer->LIGHTING_SPECULAR) {
			auto& rendererMaterial = renderer->getSpecularMaterial(context);
			auto diffuseTexture = specularMaterialProperties->getDiffuseTexture();
			rendererMaterial.diffuseTextureMaskedTransparency = specularMaterialProperties->hasDiffuseTextureMaskedTransparency() == true?1:0;
			rendererMaterial.diffuseTextureMaskedTransparencyThreshold = specularMaterialProperties->getDiffuseTextureMaskedTransparencyThreshold();
			rendererMaterial.textureAtlasSize = specularMaterialProperties->getTextureAtlasSize();
			rendererMaterial.textureAtlasPixelDimension = { 0.0f, 0.0f };
			if (rendererMaterial.textureAtlasSize > 1 && diffuseTexture != nullptr) {
				rendererMaterial.textureAtlasPixelDimension[0] = 1.0f / diffuseTexture->getTextureWidth();
				rendererMaterial.textureAtlasPixelDimension[1] = 1.0f / diffuseTexture->getTextureHeight();
			}
			renderer->onUpdateMaterial(context);
			if ((renderTypes & RENDERTYPE_TEXTURES) == RENDERTYPE_TEXTURES ||
				specularMaterialProperties->hasDiffuseTextureMaskedTransparency() == true) {
				auto diffuseTextureId =
					object3DNode->specularMaterialDynamicDiffuseTextureIdsByEntities[facesEntityIdx] != Object3DNode::TEXTUREID_NONE ?
					object3DNode->specularMaterialDynamicDiffuseTextureIdsByEntities[facesEntityIdx] :
					object3DNode->specularMaterialDiffuseTextureIdsByEntities[facesEntityIdx];
				materialKey+= ",";
				materialKey.append((const char*)&diffuseTextureId, sizeof(diffuseTextureId));
				if (updateOnly == false || currentMaterialKey.empty() == true) {
					renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE);
					renderer->bindTexture(context, diffuseTextureId);
				}
			}
		} else
		if (renderer->getLighting(context) == renderer->LIGHTING_PBR) {
			auto& rendererMaterial = renderer->getPBRMaterial(context);
			if (pbrMaterialProperties == nullptr) {
				rendererMaterial.baseColorTextureMaskedTransparency = 0;
				rendererMaterial.baseColorTextureMaskedTransparencyThreshold = 0.0f;
			} else {
				rendererMaterial.baseColorTextureMaskedTransparency = pbrMaterialProperties->hasBaseColorTextureMaskedTransparency() == true?1:0;
				rendererMaterial.baseColorTextureMaskedTransparencyThreshold = pbrMaterialProperties->getBaseColorTextureMaskedTransparencyThreshold();
			}
			renderer->onUpdateMaterial(context);
			renderer->setTextureUnit(context, LightingShaderConstants::PBR_TEXTUREUNIT_BASECOLOR);
			renderer->bindTexture(context, object3DNode->pbrMaterialBaseColorTextureIdsByEntities[facesEntityIdx]);
		}
	}
}

void EntityRenderer::clearMaterial(void* context)
{
	// TODO: optimize me! We do not need always to clear material
	if (renderer->getLighting(context) == renderer->LIGHTING_SPECULAR) {
		// unbind diffuse texture
		renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE);
		renderer->bindTexture(context, renderer->ID_NONE);
		// unbind specular texture
		if (renderer->isSpecularMappingAvailable() == true) {
			renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_SPECULAR);
			renderer->bindTexture(context, renderer->ID_NONE);
		}
		// unbind normal texture
		if (renderer->isNormalMappingAvailable()) {
			renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_NORMAL);
			renderer->bindTexture(context, renderer->ID_NONE);
		}
		// set diffuse texture unit
		renderer->setTextureUnit(context, LightingShaderConstants::SPECULAR_TEXTUREUNIT_DIFFUSE);
	} else
	if (renderer->getLighting(context) == renderer->LIGHTING_PBR) {
		// unbind base color texture
		renderer->setTextureUnit(context, LightingShaderConstants::PBR_TEXTUREUNIT_BASECOLOR);
		renderer->bindTexture(context, renderer->ID_NONE);
		// unbind metallic roughness texture
		renderer->setTextureUnit(context, LightingShaderConstants::PBR_TEXTUREUNIT_METALLICROUGHNESS);
		renderer->bindTexture(context, renderer->ID_NONE);
		// unbind normal texture
		renderer->setTextureUnit(context, LightingShaderConstants::PBR_TEXTUREUNIT_NORMAL);
		renderer->bindTexture(context, renderer->ID_NONE);
		// set diffuse texture unit
		renderer->setTextureUnit(context, LightingShaderConstants::PBR_TEXTUREUNIT_BASECOLOR);
	}
}

void EntityRenderer::render(Entity::RenderPass renderPass, const vector<Entity*>& pses)
{
	// TODO: Move me into own class
	if (pses.size() == 0) return;

	//
	struct PseParameters {
		const Color4* effectColorAdd;
		const Color4* effectColorMul;
		int32_t textureIndex;
		int32_t textureId;
		int32_t textureHorizontalSprites;
		int32_t textureVerticalSprites;
		float pointSize;
	};
	unordered_map<void*, PseParameters> rendererPseParameters;

	// use default context
	auto context = renderer->getDefaultContext();

	// store model view matrix
	Matrix4x4 modelViewMatrix;
	modelViewMatrix.set(renderer->getModelViewMatrix());

	// set up renderer state
	renderer->enableBlending();
	// 	model view matrix
	renderer->getModelViewMatrix().identity();
	renderer->onUpdateModelViewMatrix(context);

	// switch back to texture unit 0, TODO: check where its set to another value but not set back
	renderer->setTextureUnit(context, 0);

	// textures
	unordered_map<int, int> textureIndices;

	// find particle systems that are combined, merge thos pses, transform them into camera space and sort them
	auto& cameraMatrix = renderer->getCameraMatrix();
	for (auto entity: pses) {
		if (entity->getRenderPass() != renderPass) continue;
		auto ppse = dynamic_cast<PointsParticleSystem*>(entity);
		if (ppse != nullptr) {
			auto textureIndexIt = textureIndices.find(ppse->getTextureId());
			int textureIndex = textureIndexIt == textureIndices.end()?-1:textureIndexIt->second;
			if (textureIndex == -1) textureIndices[ppse->getTextureId()] = textureIndex = textureIndices.size();
			rendererPseParameters[ppse] = {
				.effectColorAdd = &ppse->getEffectColorAdd(),
				.effectColorMul = &ppse->getEffectColorMul(),
				.textureIndex = textureIndex,
				.textureId = ppse->getTextureId(),
				.textureHorizontalSprites = ppse->getTextureHorizontalSprites(),
				.textureVerticalSprites = ppse->getTextureVerticalSprites(),
				.pointSize = ppse->getPointSize()
			};
			renderTransparentRenderPointsPool->merge(ppse->getRenderPointsPool(), cameraMatrix);
		} else {
			auto fpse = dynamic_cast<FogParticleSystem*>(entity);
			if (fpse != nullptr) {
				auto textureIndexIt = textureIndices.find(fpse->getTextureId());
				int textureIndex = textureIndexIt == textureIndices.end()?-1:textureIndexIt->second;
				if (textureIndex == -1) textureIndices[fpse->getTextureId()] = textureIndex = textureIndices.size();
				rendererPseParameters[fpse] = {
					.effectColorAdd = &fpse->getEffectColorAdd(),
					.effectColorMul = &fpse->getEffectColorMul(),
					.textureIndex = textureIndex,
					.textureId = fpse->getTextureId(),
					.textureHorizontalSprites = fpse->getTextureHorizontalSprites(),
					.textureVerticalSprites = fpse->getTextureVerticalSprites(),
					.pointSize = fpse->getPointSize()
				};
				renderTransparentRenderPointsPool->merge(fpse->getRenderPointsPool(), cameraMatrix);
			}
		}
	}
	if (renderTransparentRenderPointsPool->getTransparentRenderPointsCount() > 0) {
		renderTransparentRenderPointsPool->sort();
		// render
		auto points =  renderTransparentRenderPointsPool->getTransparentRenderPoints();
		auto pseParameters = &rendererPseParameters.find(points[0]->particleSystem)->second;
		auto currentPpse = static_cast<void*>(points[0]->particleSystem);
		if (renderer->isSupportingIntegerProgramAttributes() == true) {
			for (auto i = 0; i < renderTransparentRenderPointsPool->getTransparentRenderPointsCount(); i++) {
				auto point = points[i];
				if (point->particleSystem != (void*)currentPpse) {
					pseParameters = &rendererPseParameters.find(point->particleSystem)->second;
					currentPpse = point->particleSystem;
				}
				psePointBatchRenderer->addPoint(
					point,
					pseParameters->textureIndex,
					pseParameters->pointSize,
					*pseParameters->effectColorMul,
					*pseParameters->effectColorAdd,
					pseParameters->textureHorizontalSprites,
					pseParameters->textureVerticalSprites
				);
			}
		} else {
			for (auto i = 0; i < renderTransparentRenderPointsPool->getTransparentRenderPointsCount(); i++) {
				auto point = points[i];
				if (point->particleSystem != (void*)currentPpse) {
					pseParameters = &rendererPseParameters.find(point->particleSystem)->second;
					currentPpse = point->particleSystem;
				}
				psePointBatchRenderer->addPointNoInteger(
					point,
					pseParameters->textureIndex,
					pseParameters->pointSize,
					*pseParameters->effectColorMul,
					*pseParameters->effectColorAdd,
					pseParameters->textureHorizontalSprites,
					pseParameters->textureVerticalSprites
				);
			}
		}

		//
		array<int32_t, 16> textureIds;
		textureIds.fill(0);
		for (auto& textureIt: textureIndices) {
			if (textureIt.second >= 16) continue;
			textureIds[textureIt.second] = textureIt.first;
		}
		engine->getParticlesShader()->setParameters(context, textureIds);

		// render, clear
		psePointBatchRenderer->render(context);
		psePointBatchRenderer->clear();

		// done
		renderTransparentRenderPointsPool->reset();
	}

	// unset renderer state
	renderer->disableBlending();
	// restore renderer state
	renderer->unbindBufferObjects(context);
	renderer->getModelViewMatrix().set(modelViewMatrix);
}

void EntityRenderer::render(Entity::RenderPass renderPass, const vector<LinesObject3D*>& objects) {
	// TODO: Move me into own class
	// TODO: check me performance wise again
	if (objects.size() == 0) return;

	// use default context
	auto context = renderer->getDefaultContext();

	// switch back to texture unit 0, TODO: check where its set to another value but not set back
	renderer->setTextureUnit(context, 0);

	// store model view matrix
	Matrix4x4 modelViewMatrix;
	modelViewMatrix.set(renderer->getModelViewMatrix());

	// set up renderer state
	renderer->enableBlending();

	//
	for (auto object: objects) {
		if (object->getRenderPass() != renderPass) continue;

		// 	model view matrix
		renderer->getModelViewMatrix().set(object->getTransformationsMatrix()).multiply(renderer->getCameraMatrix());
		renderer->onUpdateModelViewMatrix(context);

		// render
		// issue rendering
		renderer->getEffectColorAdd(context) = object->getEffectColorAdd().getArray();
		renderer->getEffectColorMul(context) = object->getEffectColorMul().getArray();
		renderer->onUpdateEffect(context);

		// TODO: maybe use onBindTexture() or onUpdatePointSize()
		engine->getLinesShader()->setParameters(context, object->getTextureId(), object->getLineWidth());

		//
		renderer->bindVerticesBufferObject(context, (*object->vboIds)[0]);
		renderer->bindColorsBufferObject(context, (*object->vboIds)[1]);
		renderer->drawLinesFromBufferObjects(context, object->points.size(), 0);
	}

	// unbind texture
	renderer->bindTexture(context, renderer->ID_NONE);
	// unset renderer state
	renderer->disableBlending();
	// restore renderer state
	renderer->unbindBufferObjects(context);
	renderer->getModelViewMatrix().set(modelViewMatrix);
}

inline bool EntityRenderer::compareParticleSystemEntities(Entity* entity1, Entity* entity2) {
	auto& camLookFrom = Engine::currentEngine->getCamera()->getLookFrom();
	return
		entity1->getBoundingBoxTransformed()->computeClosestPointInBoundingBox(camLookFrom).sub(camLookFrom).computeLengthSquared() >
		entity2->getBoundingBoxTransformed()->computeClosestPointInBoundingBox(camLookFrom).sub(camLookFrom).computeLengthSquared();
}
