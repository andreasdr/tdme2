
#pragma once

#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/Object3DBase.h>

using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::model::Model;

/**
 * Object 3D model
 * To be used in non engine context
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::Object3DModelInternal
	: public Object3DBase
{
public:
	/**
	 * Public constructor
	 * @param model model
	 */
	Object3DModelInternal(Model* model);

	/**
	 * Destructor
	 */
	~Object3DModelInternal();
};
