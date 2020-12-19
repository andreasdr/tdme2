#include <tdme/engine/subsystems/rendering/Object3DBase_TransformedFacesIterator.h>

#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/subsystems/rendering/Object3DBase.h>
#include <tdme/engine/subsystems/rendering/Object3DNode.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeMesh.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Node;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DBase_TransformedFacesIterator;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::engine::subsystems::rendering::Object3DNodeMesh;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

Object3DBase_TransformedFacesIterator::Object3DBase_TransformedFacesIterator(Object3DBase* object3DBase)
{
	this->object3DBase = object3DBase;
	reset();
}

void Object3DBase_TransformedFacesIterator::reset()
{
	faceCount = 0;
	for (auto object3DNode : object3DBase->object3dNodes) {
		for (auto& facesEntity : object3DNode->node->getFacesEntities()) {
			faceCount += facesEntity.getFaces().size();
		}
	}
	faceIdx = 0;
	faceIdxTotal = 0;
	object3DNodeIdx = 0;
	facesEntityIdx = 0;
	auto object3DNode = object3DBase->object3dNodes[object3DNodeIdx];
	if (object3DNode->mesh->skinning == true) {
		matrix.identity();
	} else {
		matrix.set(*object3DNode->nodeTransformationsMatrix);
	}
	matrix.multiply(object3DBase->getTransformationsMatrix());
	node = object3DNode->node;
}

Object3DBase_TransformedFacesIterator* Object3DBase_TransformedFacesIterator::iterator() {
	reset();
	return this;
}

const array<Vector3, 3>& Object3DBase_TransformedFacesIterator::next()
{
	auto object3DNode = object3DBase->object3dNodes[object3DNodeIdx];
	auto& facesEntities = object3DNode->node->getFacesEntities();
	auto& facesEntity = facesEntities[facesEntityIdx];
	auto& faces = facesEntity.getFaces();
	auto& face = faces[faceIdx];
	// compute vertices
	auto& faceVertexIndices = face.getVertexIndices();
	auto nodeVerticesTransformed = object3DNode->mesh->vertices;
	matrix.multiply((*nodeVerticesTransformed)[faceVertexIndices[0]], vertices[0]);
	matrix.multiply((*nodeVerticesTransformed)[faceVertexIndices[1]], vertices[1]);
	matrix.multiply((*nodeVerticesTransformed)[faceVertexIndices[2]], vertices[2]);
	// set up current node
	node = object3DNode->node;
	// increment to next face
	faceIdxTotal++;
	faceIdx++;
	// check if idxes over flow, if not yet finished
	if (faceIdxTotal < faceCount) {
		if (faceIdx == faces.size()) {
			faceIdx = 0;
			facesEntityIdx++;
			if (facesEntityIdx == facesEntities.size()) {
				facesEntityIdx = 0;
				object3DNodeIdx++;
				object3DNode = object3DBase->object3dNodes[object3DNodeIdx];
				if (object3DNode->mesh->skinning == true) {
					matrix.identity();
				} else {
					matrix.set(*object3DNode->nodeTransformationsMatrix);
				}
				matrix.multiply(object3DBase->getTransformationsMatrix());
			}
		}
	}
	//
	return vertices;
}

