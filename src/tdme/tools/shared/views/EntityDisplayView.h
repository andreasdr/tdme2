#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>

using std::vector;
using std::string;

using tdme::engine::Engine;
using tdme::tools::shared::controller::EntityDisplaySubScreenController;
using tdme::tools::shared::model::LevelEditorEntity;

/**
 * Entity display view
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::views::EntityDisplayView final
{
private:
	Engine* engine { nullptr };
	EntityDisplaySubScreenController* entityDisplaySubScreenController { nullptr };
	bool displayGroundPlate;
	bool displayShadowing;

public:
	/**
	 * Public constructor
	 * @param entityDisplaySubScreenController entity display sub screen controller
	 */
	EntityDisplayView(EntityDisplaySubScreenController* entityDisplaySubScreenController);

	/**
	 * Destructor
	 */
	 ~EntityDisplayView();

	/**
	 * @return display ground plate
	 */
	inline bool isDisplayGroundPlate() {
		return this->displayGroundPlate;
	}

	/**
	 * Set up ground plate visibility
	 * @param groundPlate ground plate visible
	 */
	inline void setDisplayGroundPlate(bool groundPlate) {
		this->displayGroundPlate = groundPlate;
	}

	/**
	 * @return display shadowing
	 */
	inline bool isDisplayShadowing() {
		return displayShadowing;
	}

	/**
	 * Set up shadow rendering
	 * @param shadowing shadow rendering
	 */
	inline void setDisplayShadowing(bool shadowing) {
		this->displayShadowing = shadowing;
	}

	/**
	 * Display
	 * @param entity entity
	 */
	 void display(LevelEditorEntity* entity);

};
