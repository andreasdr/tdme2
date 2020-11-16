#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>

using tdme::engine::subsystems::rendering::Object3DNode;

/**
 * Transparent face to be rendered
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::subsystems::rendering::TransparentRenderFace final
{
	Object3DNode* object3DNode;
	int32_t facesEntityIdx;
	int32_t faceIdx;
	float distanceFromCamera;

	/**
	 * Compare
	 * @param face1 face 1
	 * @param face2 face 2
	 */
	inline static bool compare(TransparentRenderFace* face1, TransparentRenderFace* face2) {
		return face1->distanceFromCamera > face2->distanceFromCamera;
	}

};
