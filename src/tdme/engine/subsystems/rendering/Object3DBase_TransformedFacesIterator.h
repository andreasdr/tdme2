#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/subsystems/rendering/Object3DBase.h>
#include <tdme/engine/subsystems/rendering/Object3DNode.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using std::array;

using tdme::engine::model::Node;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Transformed faces iterator
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::Object3DBase_TransformedFacesIterator
{
private:
	Object3DBase* object3DBase { nullptr };
	Node* node { nullptr };
	array<Vector3, 3> vertices;
	Matrix4x4 matrix;
	int32_t faceCount;
	int32_t faceIdxTotal;
	int32_t faceIdx;
	int32_t object3DNodeIdx;
	int32_t facesEntityIdx;

private:

	/**
	 * Reset
	 */
	void reset();
public:
	/**
	 * Public constructor
	 */
	Object3DBase_TransformedFacesIterator(Object3DBase* object3DBase);

	/**
	 * Return iterator ready to iterate
	 */
	Object3DBase_TransformedFacesIterator* iterator();

	/**
	 * Has next
	 */
	inline bool hasNext() {
		return faceIdxTotal < faceCount;
	}

	/**
	 * Retrieve next triangle
	 * @return next 3 triangle vectors
	 */
	const array<Vector3, 3>& next();

	/**
	 * @return current node
	 */
	inline Node* getNode() {
		return node;
	}

};
