#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/skinning/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix2D3x3.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::vector;
using std::string;

using tdme::engine::Engine;
using tdme::engine::Object3D;
using tdme::engine::model::Group;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DGroupMesh;
using tdme::engine::subsystems::rendering::Object3DGroupRenderer;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix2D3x3;
using tdme::math::Matrix4x4;
using tdme::engine::subsystems::rendering::Object3DGroup;

/** 
 * Object 3d group specifically for rendering
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::Object3DGroup final
{
	friend class tdme::engine::Object3D;
	friend class ModelUtilitiesInternal;
	friend class Object3DBase;
	friend class Object3DBase_TransformedFacesIterator;
	friend class Object3DGroupMesh;
	friend class Object3DGroupRenderer;
	friend class Object3DInternal;
	friend class EntityRenderer;
	friend class TransparentRenderFacesPool;
	friend class TransparentRenderFacesGroup;
	friend class tdme::engine::subsystems::skinning::SkinningShader;

private:
	static int64_t counter;

	static constexpr int32_t TEXTUREID_NONE { -1 };
	static constexpr int32_t TEXTUREID_NOTUSED { 0 };

	string id;
	Object3DBase* object { nullptr };
	Group* group { nullptr };
	bool animated { false };
	vector<Matrix2D3x3> textureMatricesByEntities;
	vector<int32_t> specularMaterialDiffuseTextureIdsByEntities;
	vector<int32_t> specularMaterialDynamicDiffuseTextureIdsByEntities;
	vector<int32_t> specularMaterialSpecularTextureIdsByEntities;
	vector<int32_t> specularMaterialNormalTextureIdsByEntities;
	vector<int32_t> pbrMaterialBaseColorTextureIdsByEntities;
	vector<int32_t> pbrMaterialMetallicRoughnessTextureIdsByEntities;
	vector<int32_t> pbrMaterialNormalTextureIdsByEntities;
	Object3DGroupRenderer* renderer { nullptr };
	Object3DGroupMesh* mesh { nullptr };
	Matrix4x4* groupTransformationsMatrix { nullptr };

	/** 
	 * Creates object 3d groups from given object3d base object
	 * @param object object 3d base
	 * @param useManagers use mesh and object 3d renderer group managers
	 * @param animationProcessingTarget animation processing target
	 * @param object3DGroups object 3d groups array
	 * @return object 3d group
	 */
	static void createGroups(Object3DBase* object, bool useManagers, Engine::AnimationProcessingTarget animationProcessingTarget, vector<Object3DGroup*>& object3DGroups);

	/** 
	 * Applies transformations to meshes for given object 3d groups
	 * @param context context
	 * @param object3DGroups group render data list
	 */
	static void computeTransformations(void* context, vector<Object3DGroup*>& object3DGroups);

	/** 
	 * Set up textures for given object3d group and faces entity
	 * @param renderer renderer
	 * @param context context
	 * @param object3DGroup object 3D group
	 * @param facesEntityIdx faces entity idx
	 */
	static void setupTextures(Renderer* renderer, void* context, Object3DGroup* object3DGroup, int32_t facesEntityIdx);

	/**
	 * Creates a object 3d groups recursively for given group and it sub groups
	 * @param object3D object 3D base
	 * @param groups groups
	 * @param animated animated
	 * @param useManagers use mesh and object 3d render group managers
	 * @param animationProcessingTarget animation processing target
	 * @param object3DGroups object 3D groups
	 */
	static void createGroups(Object3DBase* object3D, const map<string, Group*>& groups, bool animated, bool useManagers, Engine::AnimationProcessingTarget animationProcessingTarget, vector<Object3DGroup*>& object3DGroups);

	/** 
	 * Dispose
	 */
	void dispose();

public:
	/**
	 * Public constructor
	 */
	Object3DGroup();

	/**
	 * Destructor
	 */
	~Object3DGroup();
};
