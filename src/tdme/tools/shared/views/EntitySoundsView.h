
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
using tdme::tools::shared::controller::EntitySoundsSubScreenController;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::views::PopUps;

/**
 * Entity sounds view
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::views::EntitySoundsView final
{
private:
	Engine* engine {  };
	EntitySoundsSubScreenController* entitySoundsSubScreenController {  };
	PopUps* popUps {  };

public:
	/**
	 * Public constructor
	 * @param entitySoundsSubScreenController entity sounds sub screen controller
	 * @param popUps pop ups
	 */
	EntitySoundsView(EntitySoundsSubScreenController* entitySoundsSubScreenController, PopUps* popUps);

	/**
	 * @return pop ups view
	 */
	PopUps* getPopUpsViews();

	/**
	 * Unset sounds
	 */
	void unsetSounds();

	/**
	 * Set sounds
	 * @param entity
	 */
	void setSounds(LevelEditorEntity* entity);

};
