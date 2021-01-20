#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/subsystems/manager/VBOManager_VBOManaged.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/Object3DNode.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeMesh.h>
#include <tdme/engine/subsystems/skinning/fwd-tdme.h>

using std::vector;

using tdme::engine::subsystems::manager::VBOManager_VBOManaged;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::engine::subsystems::rendering::Object3DNodeMesh;
using tdme::engine::subsystems::skinning::SkinningShader;

/**
 * Object 3D node VBO renderer
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::Object3DNodeRenderer final
{
	friend class EntityRenderer;
	friend class tdme::engine::Object3D;
	friend class tdme::engine::subsystems::rendering::Object3DNodeMesh;
	friend class tdme::engine::subsystems::skinning::SkinningShader;

private:
	Object3DNode* object3DNode { nullptr };
	VBOManager_VBOManaged* vboManagedBase { nullptr };
	VBOManager_VBOManaged* vboManagedNormalMapping { nullptr };
	VBOManager_VBOManaged* vboManagedOrigins { nullptr };
	vector<int32_t>* vboBaseIds { nullptr };
	vector<int32_t>* vboNormalMappingIds { nullptr  };
	vector<int32_t>* vboOrigins { nullptr };
	bool haveVBOs { false };
public:
	/**
	 * Constructor
	 * @param object3DNode object 3D node
	 */
	Object3DNodeRenderer(Object3DNode* object3DNode);

	/**
	 * @return if preRender call is required
	 */
	inline bool needsPreRender() {
		return haveVBOs == false || object3DNode->mesh->hasRecreatedBuffers() == true || object3DNode->mesh->node->hasUpdate();
	}

	/**
	 * Pre render step like uploading VBOs and such
	 * @param context context
	 */
	void preRender(void* context);

	/**
	 * Disposes the object 3d node
	 */
	void dispose();

};
