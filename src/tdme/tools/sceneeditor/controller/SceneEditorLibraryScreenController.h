#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/sceneeditor/controller/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/tools/shared/views/fwd-tdme.h>

using std::string;

using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::shared::controller::ScreenController;
using tdme::tools::shared::views::PopUps;

/**
 * Scene editor prototype library screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::sceneeditor::controller::SceneEditorLibraryScreenController final
	: public ScreenController
	, public GUIActionListener
	, public GUIChangeListener
{
private:
	GUIScreenNode* screenNode { nullptr };
	GUIElementNode* sceneLibraryListBox { nullptr };
	GUIElementNode* buttonEntityPlace { nullptr };
	GUIElementNode* buttonLevelEdit { nullptr };
	PopUps* popUps { nullptr };
	string modelPath;

public:
	/**
	 * Public constructor
	 */
	SceneEditorLibraryScreenController(PopUps* popUps);

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
	 * Select prototype by prototype id
	 * @param prototypeId prototype id
	 */
	void selectPrototype(int prototypeId);

	/**
	 * Set up complete scene prototype library
	 */
	void setPrototypeLibrary();

	/**
	 * On prototype selection changed
	 */
	void onPrototypeSelectionChanged();

	/**
	 * On edit prototype
	 */
	void onEditPrototype();

	/**
	 * On edit scene
	 */
	void onEditScene();

	/**
	 * place prototype button clicked
	 */
	void onPlacePrototype();

	/**
	 * place delete prototype clicked
	 */
	void onDeletePrototype();

	/**
	 * partition prototype
	 */
	void onPartitionPrototype();

	// overridden methods
	void onValueChanged(GUIElementNode* node) override;
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node) override;

};
