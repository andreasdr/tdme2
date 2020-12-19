#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/skinning/fwd-tdme.h>
#include <tdme/engine/Engine.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix2D3x3.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::model::Node;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::engine::subsystems::rendering::Object3DNodeMesh;
using tdme::engine::subsystems::rendering::Object3DNodeRenderer;
using tdme::engine::Engine;
using tdme::engine::Object3D;
using tdme::math::Matrix2D3x3;
using tdme::math::Matrix4x4;

/**
 * Object 3d node specifically for rendering
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::Object3DNode final
{
	friend class tdme::engine::Object3D;
	friend class ModelUtilitiesInternal;
	friend class Object3DBase;
	friend class Object3DBase_TransformedFacesIterator;
	friend class Object3DNodeMesh;
	friend class Object3DNodeRenderer;
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
	Node* node { nullptr };
	bool animated { false };
	vector<Matrix2D3x3> textureMatricesByEntities;
	vector<int32_t> specularMaterialDiffuseTextureIdsByEntities;
	vector<int32_t> specularMaterialDynamicDiffuseTextureIdsByEntities;
	vector<int32_t> specularMaterialSpecularTextureIdsByEntities;
	vector<int32_t> specularMaterialNormalTextureIdsByEntities;
	vector<int32_t> pbrMaterialBaseColorTextureIdsByEntities;
	vector<int32_t> pbrMaterialMetallicRoughnessTextureIdsByEntities;
	vector<int32_t> pbrMaterialNormalTextureIdsByEntities;
	Object3DNodeRenderer* renderer { nullptr };
	Object3DNodeMesh* mesh { nullptr };
	Matrix4x4* nodeTransformationsMatrix { nullptr };

	/**
	 * Creates object 3d nodes from given object3d base object
	 * @param object object 3d base
	 * @param useManagers use mesh and object 3d renderer node managers
	 * @param animationProcessingTarget animation processing target
	 * @param object3DNodes object 3d nodes array
	 * @return object 3d node
	 */
	static void createNodes(Object3DBase* object, bool useManagers, Engine::AnimationProcessingTarget animationProcessingTarget, vector<Object3DNode*>& object3DNodes);

	/**
	 * Applies transformations to meshes for given object 3d nodes
	 * @param context context
	 * @param object3DNodes node render data list
	 */
	static void computeTransformations(void* context, vector<Object3DNode*>& object3DNodes);

	/**
	 * Set up textures for given object3d node and faces entity
	 * @param renderer renderer
	 * @param context context
	 * @param object3DNode object 3D node
	 * @param facesEntityIdx faces entity idx
	 */
	static void setupTextures(Renderer* renderer, void* context, Object3DNode* object3DNode, int32_t facesEntityIdx);

	/**
	 * Creates a object 3d nodes recursively for given node and it sub nodes
	 * @param object3D object 3D base
	 * @param nodes nodes
	 * @param animated animated
	 * @param useManagers use mesh and object 3d render node managers
	 * @param animationProcessingTarget animation processing target
	 * @param object3DNodes object 3D nodes
	 */
	static void createNodes(Object3DBase* object3D, const map<string, Node*>& nodes, bool animated, bool useManagers, Engine::AnimationProcessingTarget animationProcessingTarget, vector<Object3DNode*>& object3DNodes);

	/**
	 * Dispose
	 */
	void dispose();

public:
	/**
	 * Public constructor
	 */
	Object3DNode();

	/**
	 * Destructor
	 */
	~Object3DNode();
};
