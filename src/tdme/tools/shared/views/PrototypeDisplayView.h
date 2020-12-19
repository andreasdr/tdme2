#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>

using std::string;
using std::vector;

using tdme::engine::prototype::Prototype;
using tdme::engine::Engine;
using tdme::tools::shared::controller::PrototypeDisplaySubScreenController;

/**
 * Prototype display view
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::views::PrototypeDisplayView final
{
private:
	Engine* engine { nullptr };
	PrototypeDisplaySubScreenController* prototypeDisplaySubScreenController { nullptr };
	bool displayGroundPlate;
	bool displayShadowing;

public:
	/**
	 * Public constructor
	 * @param prototypeDisplaySubScreenController entity display sub screen controller
	 */
	PrototypeDisplayView(PrototypeDisplaySubScreenController* prototypeDisplaySubScreenController);

	/**
	 * Destructor
	 */
	 ~PrototypeDisplayView();

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
	 * @param prototype prototype
	 */
	 void display(Prototype* prototype);

};
