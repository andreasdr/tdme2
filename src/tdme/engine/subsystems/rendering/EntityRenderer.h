#pragma once

#if defined(_WIN32) && defined(_MSC_VER)
	#include <windows.h>
#endif

#include <string>
#include <unordered_map>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/Object3DGroup.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesPool.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix2D3x3.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Matrix4x4Negative.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/utils/ByteBuffer.h>
#include <tdme/utils/Pool.h>

using std::unordered_map;
using std::string;
using std::to_string;
using std::vector;

using tdme::engine::Engine;
using tdme::engine::FogParticleSystem;
using tdme::engine::LinesObject3D;
using tdme::engine::Object3D;
using tdme::engine::PointsParticleSystem;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::subsystems::rendering::BatchRendererPoints;
using tdme::engine::subsystems::rendering::BatchRendererTriangles;
using tdme::engine::subsystems::rendering::Object3DGroup;
using tdme::engine::subsystems::rendering::TransparentRenderFacesPool;
using tdme::engine::subsystems::rendering::TransparentRenderPointsPool;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix2D3x3;
using tdme::math::Matrix4x4;
using tdme::math::Matrix4x4Negative;
using tdme::math::Vector3;
using tdme::utils::ByteBuffer;
using tdme::utils::Pool;

/** 
 * Rendering class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::EntityRenderer final {
	friend class Object3DGroupRenderer;
	friend class TransparentRenderFacesGroup;
	friend class tdme::engine::Engine;

private:
	static constexpr int32_t BATCHRENDERER_MAX { 256 };
	static constexpr int32_t INSTANCEDRENDERING_OBJECTS_MAX { 16384 };

	struct Object3DRenderContext {
		vector<int32_t>* vboInstancedRenderingIds { nullptr };
		ByteBuffer* bbEffectColorMuls { nullptr };
		ByteBuffer* bbEffectColorAdds { nullptr };
		ByteBuffer* bbMvMatrices { nullptr };
		Matrix4x4Negative matrix4x4Negative;
		vector<Object3D*> objectsToRender;
		vector<Object3D*> objectsNotRendered;
		vector<Object3D*> objectsByModelToRender;
		vector<Object3D*> objectsByModelNotRendered;
	};

	Engine* engine { nullptr };
	Renderer* renderer { nullptr };

	vector<BatchRendererTriangles*> trianglesBatchRenderers;
	unordered_map<string, unordered_map<string, vector<Object3D*>>> objectsByShadersAndModels;
	vector<TransparentRenderFace*> groupTransparentRenderFaces;
	EntityRenderer_TransparentRenderFacesGroupPool* transparentRenderFacesGroupPool { nullptr };
	TransparentRenderFacesPool* transparentRenderFacesPool { nullptr };
	unordered_map<string, TransparentRenderFacesGroup*> transparentRenderFacesGroups;
	RenderTransparentRenderPointsPool* renderTransparentRenderPointsPool { nullptr };
	BatchRendererPoints* psePointBatchRenderer { nullptr };
	int threadCount;
	vector<Object3DRenderContext> contexts;

	/** 
	 * Renders transparent faces
	 * TODO:	guess this should be optimized regarding GL commands
	 * skinned mesh is not supported when using GPU
	 * @param transparentRenderFaces transparent render faces
	 */
	void prepareTransparentFaces(const vector<TransparentRenderFace*>& transparentRenderFaces);

	/** 
	 * Render transparent faces groups
	 * @param context context
	 */
	void renderTransparentFacesGroups(void* context);

	/** 
	 * Release transparent faces groups
	 */
	void releaseTransparentFacesGroups();

	/** 
	 * Renders multiple objects of same type(with same model)
	 * @param threadIdx thread idx
	 * @param objects objects of same type/ with same models
	 * @param collectTransparentFaces collect render faces
	 * @param renderTypes render types
	 * @param transparentRenderFacesPool transparent render faces pool
	 */
	inline void renderObjectsOfSameType(int threadIdx, const vector<Object3D*>& objects, bool collectTransparentFaces, int32_t renderTypes, TransparentRenderFacesPool* transparentRenderFacesPool) {
		if (renderer->isInstancedRenderingAvailable() == true) {
			renderObjectsOfSameTypeInstanced(threadIdx, objects, collectTransparentFaces, renderTypes, transparentRenderFacesPool);
		} else {
			renderObjectsOfSameTypeNonInstanced(objects, collectTransparentFaces, renderTypes);
		}
	}

	/** 
	 * Renders multiple objects of same type(with same model) not using instancing
	 * @param objects objects of same type/ with same models
	 * @param collectTransparentFaces collect render faces
	 * @param renderTypes render types
	 */
	void renderObjectsOfSameTypeNonInstanced(const vector<Object3D*>& objects, bool collectTransparentFaces, int32_t renderTypes);

	/**
	 * Renders multiple objects of same type(with same model) using instancing
	 * @param threadIdx thread idx
	 * @param objects objects of same type/ with same models
	 * @param collectTransparentFaces collect render faces
	 * @param renderTypes render types
	 */
	void renderObjectsOfSameTypeInstanced(int threadIdx, const vector<Object3D*>& objects, bool collectTransparentFaces, int32_t renderTypes, TransparentRenderFacesPool* transparentRenderFacesPool);

	/**
	 * Checks if a material could change when having multiple objects but same model
	 * @param object3DGroup object 3d group
	 * @param facesEntityIdx faces entity idx
	 * @param renderTypes render types
	 */
	inline bool checkMaterialChangable(Object3DGroup* object3DGroup, int32_t facesEntityIdx, int32_t renderTypes) {
		return object3DGroup->specularMaterialDynamicDiffuseTextureIdsByEntities[facesEntityIdx] != Object3DGroup::TEXTUREID_NONE;
	}

	/**
	 * Set ups a material for rendering
	 * @param context context
	 * @param object3DGroup object 3d group
	 * @param facesEntityIdx faces entity idx
	 * @param renderTypes render types
	 * @param updateOnly update only, means material has been set up already, only do changes
	 * @param materialKey material key
	 * @param currentMaterialKey current material key or empty
	 */
	void setupMaterial(void* context, Object3DGroup* object3DGroup, int32_t facesEntityIdx, int32_t renderTypes, bool updateOnly, string& materialKey, const string& currentMaterialKey = string());

	/** 
	 * Clear material for rendering
	 * @param context context
	 */
	void clearMaterial(void* context);

	/**
	 * Render function
	 * @param threadCount thread count
	 * @param threadIdx thread index
	 * @param objects objects
	 * @param visibleObjectsByModels objects by models storage
	 * @param renderTransparentFaces render transparent faces
	 * @param renderTypes render types
	 */
	inline void renderFunction(
		int threadCount,
		int threadIdx,
		const vector<Object3D*>& objects,
		unordered_map<string, unordered_map<string, vector<Object3D*>>>& objectsByShadersAndModels,
		bool renderTransparentFaces,
		int renderTypes,
		TransparentRenderFacesPool* transparentRenderFacesPool) {
		// reset shader
		renderer->setShader(renderer->getContext(threadIdx), string());
		auto effectPass = renderer->getEffectPass();

		// sort objects by model
		Vector3 objectCamFromAxis;
		auto camera = engine->getCamera();
		for (auto objectIdx = 0; objectIdx < objects.size(); objectIdx++) {
			if (threadCount > 1 && objectIdx % threadCount != threadIdx) continue;
			auto object = objects[objectIdx];
			if (object->enabledInstances == 0) continue;
			if (effectPass != 0 && object->excludeFromEffectPass == effectPass) continue;
			auto objectShader = object->getDistanceShader().length() == 0?
				object->getShader():
				objectCamFromAxis.set(object->getBoundingBoxTransformed()->getCenter()).sub(camera->getLookFrom()).computeLengthSquared() < Math::square(object->getDistanceShaderDistance())?
					object->getShader():
					object->getDistanceShader();
			auto& objectsByShaders = objectsByShadersAndModels[objectShader];
			auto& objectsByModel = objectsByShaders[object->getModel()->getId()];
			objectsByModel.push_back(object);
		}

		// render objects
		auto& context = contexts[threadIdx];
		for (auto& objectsByShaderAndModelIt: objectsByShadersAndModels) {
			auto& objectsByModels = objectsByShaderAndModelIt.second;
			for (auto& objectsByModelIt: objectsByModels) {
				auto& objectsByModel = objectsByModelIt.second;
				if (objectsByModel.size() == 0) {
					continue;
				} else
				if (objectsByModel.size() > 0) {
					do {
						for (auto object: objectsByModel) {
							if (context.objectsByModelToRender.size() == 0 ||
								(object->instances == context.objectsByModelToRender[0]->instances &&
								object->enabledInstances == context.objectsByModelToRender[0]->enabledInstances)) {
								context.objectsByModelToRender.push_back(object);
							} else {
								context.objectsByModelNotRendered.push_back(object);
							}
						}
						renderObjectsOfSameType(threadIdx, context.objectsByModelToRender, renderTransparentFaces, renderTypes, transparentRenderFacesPool);
						objectsByModel = context.objectsByModelNotRendered;
						context.objectsByModelToRender.clear();
						context.objectsByModelNotRendered.clear();
					} while (objectsByModel.size() > 0);
				}
				objectsByModel.clear();
			}
		}
	}

public:
	static constexpr int32_t RENDERTYPE_NORMALS { 1 };
	static constexpr int32_t RENDERTYPE_TEXTUREARRAYS { 2 };
	static constexpr int32_t RENDERTYPE_TEXTUREARRAYS_DIFFUSEMASKEDTRANSPARENCY { 4 };
	static constexpr int32_t RENDERTYPE_EFFECTCOLORS { 8 };
	static constexpr int32_t RENDERTYPE_MATERIALS { 16 };
	static constexpr int32_t RENDERTYPE_MATERIALS_DIFFUSEMASKEDTRANSPARENCY { 32 };
	static constexpr int32_t RENDERTYPE_TEXTURES { 64 };
	static constexpr int32_t RENDERTYPE_TEXTURES_DIFFUSEMASKEDTRANSPARENCY { 128 };
	static constexpr int32_t RENDERTYPE_LIGHTS { 256 };
	static constexpr int32_t RENDERTYPE_SHADOWMAPPING { 512 };
	static constexpr int32_t RENDERTYPE_RENDERGROUP_OBJECTORIGIN { 1024 };
	static constexpr int32_t RENDERTYPE_ALL { 2047 };

	/**
	 * Init
	 */
	void initialize();

	/**
	 * Dispose
	 */
	void dispose();

	/**
	 * @return batch renderer for triangles
	 */
	BatchRendererTriangles* acquireTrianglesBatchRenderer();

	/**
	 * Resets the object 3d renderer
	 */
	void reset();

	/**
	 * Renders all given objects
	 * @param objects objects
	 * @param renderTransparentFaces render transparent faces
	 * @param renderTypes render types
	 */
	void render(const vector<Object3D*>& objects, bool renderTransparentFaces, int32_t renderTypes);

	/** 
	 * Render points particle system entities
	 * @param pses points particle system entities
	 */
	void render(const vector<Entity*>& pses);

	/**
	 * Renders all given lines objects
	 * @param objects lines objects
	 */
	void render(const vector<LinesObject3D*>& objects);

	/**
	 * Public constructor
	 * @param engine engine
	 * @param renderer renderer
	 */
	EntityRenderer(Engine* engine, Renderer* renderer);

	/**
	 * Destructor
	 */
	~EntityRenderer();

	/**
	 * Compare entities by distance from camera
	 * @param entity1 entity 1
	 * @param entity2 entity 2
	 */
	static bool compareParticleSystemEntities(Entity* entity1, Entity* entity2);

};
