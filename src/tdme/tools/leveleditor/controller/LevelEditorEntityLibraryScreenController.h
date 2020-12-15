#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/leveleditor/controller/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>

using std::string;

using tdme::tools::shared::controller::ScreenController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::shared::views::PopUps;

/**
 * Level editor model library screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::leveleditor::controller::LevelEditorEntityLibraryScreenController final
	: public ScreenController
	, public GUIActionListener
	, public GUIChangeListener
{
private:
	GUIScreenNode* screenNode { nullptr };
	GUIElementNode* entityLibraryListBox { nullptr };
	GUIElementNode* buttonEntityPlace { nullptr };
	GUIElementNode* buttonLevelEdit { nullptr };
	PopUps* popUps { nullptr };
	string modelPath;

public:
	/**
	 * Public constructor
	 */
	LevelEditorEntityLibraryScreenController(PopUps* popUps);

	// overridden methods
	GUIScreenNode* getScreenNode() override;

	/**
	 * @return model path
	 */
	const string& getModelPath();

	/**
	 * Set model path
	 * @param modelPath model path
	 */
	void setModelPath(const string& modelPath);
	void initialize() override;
	void dispose() override;

	/**
	 * Select entity by entity id
	 * @param entityId entity id
	 */
	void selectEntity(int entityId);

	/**
	 * Set up complete entity library
	 */
	void setEntityLibrary();

	/**
	 * On entity selection changed
	 */
	void onEntitySelectionChanged();

	/**
	 * On edit entity
	 */
	void onEditEntity();

	/**
	 * On edit level
	 */
	void onEditLevel();

	/**
	 * place object button clicked
	 */
	void onPlaceEntity();

	/**
	 * place model entity clicked
	 */
	void onDeleteEntity();

	/**
	 * partition entity
	 */
	void onPartitionEntity();

	// overridden methods
	void onValueChanged(GUIElementNode* node) override;
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node) override;

};
