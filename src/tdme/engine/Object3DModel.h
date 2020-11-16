#pragma once

#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/Object3DModelInternal.h>

using tdme::engine::subsystems::rendering::Object3DModelInternal;
using tdme::engine::model::Model;

/**
 * Object 3D model
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::Object3DModel final
	: public Object3DModelInternal
{
public:
	/**
	 * Public constructor
	 * @param model model
	 */
	Object3DModel(Model* model);
};
