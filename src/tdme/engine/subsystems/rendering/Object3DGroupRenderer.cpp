#include <tdme/engine/subsystems/rendering/Object3DGroupRenderer.h>

#include <tdme/utilities/FloatBuffer.h>
#include <tdme/utilities/ShortBuffer.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/subsystems/manager/VBOManager_VBOManaged.h>
#include <tdme/engine/subsystems/manager/VBOManager.h>
#include <tdme/engine/subsystems/rendering/Object3DGroup.h>
#include <tdme/engine/subsystems/rendering/Object3DGroupMesh.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/math/Vector3.h>

using tdme::engine::subsystems::rendering::Object3DGroupRenderer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::ShortBuffer;
using tdme::engine::Engine;
using tdme::engine::model::Group;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::subsystems::manager::VBOManager_VBOManaged;
using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::rendering::Object3DGroup;
using tdme::engine::subsystems::rendering::Object3DGroupMesh;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Vector3;

Object3DGroupRenderer::Object3DGroupRenderer(Object3DGroup* object3DGroup)
{
	this->object3DGroup = object3DGroup;
	this->vboBaseIds = nullptr;
	this->vboNormalMappingIds = nullptr;
}

void Object3DGroupRenderer::preRender(void* context)
{
	// TODO: maybe store these in Object3DGroupRenderer too
	auto meshUploaded = true;
	auto created = false;

	// initialize if not yet done
	if (vboBaseIds == nullptr) {
		vboManagedBase = Engine::getInstance()->getVBOManager()->addVBO(
			object3DGroup->id,
			3 + (object3DGroup->mesh->group->getTextureCoordinates().size() > 0?1:0),
			true,
			true,
			created
		);
		if (created == false) while (vboManagedBase->isUploaded() == false);
		vboBaseIds = vboManagedBase->getVBOIds();
		meshUploaded = created == false;
	}

	// initialize tangents, bitangents
	if (Engine::renderer->isNormalMappingAvailable() &&
		object3DGroup->mesh->group->getTangents().size() > 0 &&
		object3DGroup->mesh->group->getBitangents().size() > 0 &&
		vboNormalMappingIds == nullptr) {
		vboManagedNormalMapping = Engine::getInstance()->getVBOManager()->addVBO(object3DGroup->id + ".normalmapping", 2, true, true, created);
		if (created == false) while (vboManagedNormalMapping->isUploaded() == false);
		vboNormalMappingIds = vboManagedNormalMapping->getVBOIds();
	}

	// initialize tangents, bitangents
	if (object3DGroup->mesh->group->getOrigins().size() > 0 &&
		vboOrigins == nullptr) {
		vboManagedOrigins = Engine::getInstance()->getVBOManager()->addVBO(object3DGroup->id + ".origins", 1, true, true, created);
		if (created == false) while (vboManagedOrigins->isUploaded() == false);
		vboOrigins = vboManagedOrigins->getVBOIds();
	}

	//
	haveVBOs = true;

	// check if to upload new mesh
	if (object3DGroup->mesh->getRecreatedBuffers() == true || meshUploaded == false) {
		if (meshUploaded == false) {
			// upload indices
			object3DGroup->mesh->setupVertexIndicesBuffer(Engine::renderer, context, (*vboBaseIds)[0]);
			// upload texture coordinates
			if (object3DGroup->mesh->group->getTextureCoordinates().size() > 0) {
				object3DGroup->mesh->setupTextureCoordinatesBuffer(Engine::renderer, context, (*vboBaseIds)[3]);
			}
			// upload render group object origins
			if (object3DGroup->mesh->group->getOrigins().size() > 0) {
				object3DGroup->mesh->setupOriginsBuffer(Engine::renderer, context, (*vboOrigins)[0]);
				vboManagedOrigins->setUploaded(true);
			}
		}
		// upload vertices
		object3DGroup->mesh->setupVerticesBuffer(Engine::renderer, context, (*vboBaseIds)[1]);
		// upload normals
		object3DGroup->mesh->setupNormalsBuffer(Engine::renderer, context, (*vboBaseIds)[2]);
		// tangents, bitangents
		if (vboNormalMappingIds != nullptr) {
			object3DGroup->mesh->setupTangentsBuffer(Engine::renderer, context, (*vboNormalMappingIds)[0]);
			object3DGroup->mesh->setupBitangentsBuffer(Engine::renderer, context, (*vboNormalMappingIds)[1]);
			vboManagedNormalMapping->setUploaded(true);
		}
		vboManagedBase->setUploaded(true);
	}
}

void Object3DGroupRenderer::dispose()
{
	if (vboBaseIds != nullptr) {
		Engine::getInstance()->getVBOManager()->removeVBO(object3DGroup->id);
		vboBaseIds = nullptr;
	}
	if (vboNormalMappingIds != nullptr) {
		Engine::getInstance()->getVBOManager()->removeVBO(object3DGroup->id + ".normalmapping");
		vboNormalMappingIds = nullptr;
	}
	if (vboOrigins != nullptr) {
		Engine::getInstance()->getVBOManager()->removeVBO(object3DGroup->id + ".origins");
		vboOrigins = nullptr;
	}
}
