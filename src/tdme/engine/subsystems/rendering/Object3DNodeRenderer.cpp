#include <tdme/engine/subsystems/rendering/Object3DNodeRenderer.h>

#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/subsystems/manager/VBOManager.h>
#include <tdme/engine/subsystems/manager/VBOManager_VBOManaged.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer.h>
#include <tdme/engine/subsystems/rendering/Object3DNode.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeMesh.h>
#include <tdme/engine/Engine.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/FloatBuffer.h>
#include <tdme/utilities/ShortBuffer.h>

using tdme::engine::model::Node;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::manager::VBOManager_VBOManaged;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::engine::subsystems::rendering::Object3DNodeMesh;
using tdme::engine::subsystems::rendering::Object3DNodeRenderer;
using tdme::engine::Engine;
using tdme::math::Vector3;
using tdme::utilities::FloatBuffer;
using tdme::utilities::ShortBuffer;

Object3DNodeRenderer::Object3DNodeRenderer(Object3DNode* object3DNode)
{
	this->object3DNode = object3DNode;
	this->vboBaseIds = nullptr;
	this->vboNormalMappingIds = nullptr;
}

void Object3DNodeRenderer::preRender(void* context)
{
	// TODO: maybe store these in Object3DNodeRenderer too
	auto meshUploaded = true;
	auto created = false;

	// initialize if not yet done
	if (vboBaseIds == nullptr) {
		vboManagedBase = Engine::getInstance()->getVBOManager()->addVBO(
			object3DNode->id,
			3 + (object3DNode->mesh->node->getTextureCoordinates().size() > 0?1:0),
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
		object3DNode->mesh->node->getTangents().size() > 0 &&
		object3DNode->mesh->node->getBitangents().size() > 0 &&
		vboNormalMappingIds == nullptr) {
		vboManagedNormalMapping = Engine::getInstance()->getVBOManager()->addVBO(object3DNode->id + ".normalmapping", 2, true, true, created);
		if (created == false) while (vboManagedNormalMapping->isUploaded() == false);
		vboNormalMappingIds = vboManagedNormalMapping->getVBOIds();
	}

	// initialize tangents, bitangents
	if (object3DNode->mesh->node->getOrigins().size() > 0 &&
		vboOrigins == nullptr) {
		vboManagedOrigins = Engine::getInstance()->getVBOManager()->addVBO(object3DNode->id + ".origins", 1, true, true, created);
		if (created == false) while (vboManagedOrigins->isUploaded() == false);
		vboOrigins = vboManagedOrigins->getVBOIds();
	}

	//
	haveVBOs = true;

	// check if to upload new mesh
	if (object3DNode->mesh->getRecreatedBuffers() == true || meshUploaded == false) {
		if (meshUploaded == false) {
			// upload indices
			object3DNode->mesh->setupVertexIndicesBuffer(Engine::renderer, context, (*vboBaseIds)[0]);
			// upload texture coordinates
			if (object3DNode->mesh->node->getTextureCoordinates().size() > 0) {
				object3DNode->mesh->setupTextureCoordinatesBuffer(Engine::renderer, context, (*vboBaseIds)[3]);
			}
			// upload render node object origins
			if (object3DNode->mesh->node->getOrigins().size() > 0) {
				object3DNode->mesh->setupOriginsBuffer(Engine::renderer, context, (*vboOrigins)[0]);
				vboManagedOrigins->setUploaded(true);
			}
		}
		// upload vertices
		object3DNode->mesh->setupVerticesBuffer(Engine::renderer, context, (*vboBaseIds)[1]);
		// upload normals
		object3DNode->mesh->setupNormalsBuffer(Engine::renderer, context, (*vboBaseIds)[2]);
		// tangents, bitangents
		if (vboNormalMappingIds != nullptr) {
			object3DNode->mesh->setupTangentsBuffer(Engine::renderer, context, (*vboNormalMappingIds)[0]);
			object3DNode->mesh->setupBitangentsBuffer(Engine::renderer, context, (*vboNormalMappingIds)[1]);
			vboManagedNormalMapping->setUploaded(true);
		}
		vboManagedBase->setUploaded(true);
	}
}

void Object3DNodeRenderer::dispose()
{
	if (vboBaseIds != nullptr) {
		Engine::getInstance()->getVBOManager()->removeVBO(object3DNode->id);
		vboBaseIds = nullptr;
	}
	if (vboNormalMappingIds != nullptr) {
		Engine::getInstance()->getVBOManager()->removeVBO(object3DNode->id + ".normalmapping");
		vboNormalMappingIds = nullptr;
	}
	if (vboOrigins != nullptr) {
		Engine::getInstance()->getVBOManager()->removeVBO(object3DNode->id + ".origins");
		vboOrigins = nullptr;
	}
}
