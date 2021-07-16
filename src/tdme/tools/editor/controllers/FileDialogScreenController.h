#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/events/GUIFocusListener.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/editor/controllers/ScreenController.h>
#include <tdme/utilities/fwd-tdme.h>

using std::string;
using std::vector;

using tdme::utilities::Action;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::events::GUIFocusListener;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::tools::editor::controllers::ScreenController;
using tdme::utilities::MutableString;

/**
 * File dialog screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::editor::controllers::FileDialogScreenController final
	: public ScreenController
	, public virtual GUIActionListener
	, public virtual GUIChangeListener
	, public virtual GUIFocusListener
{

private:
	GUIScreenNode* screenNode { nullptr };
	string cwd;
	vector<string> extensions;
	string captionText;
	GUIElementNode* tabsHeaderNode { nullptr };
	GUIElementNode* pathNode { nullptr };
	GUIElementNode* fileNameNode { nullptr };
	GUIElementNode* filesNode { nullptr };
	GUIElementNode* typeDropDownNode { nullptr };
	Action* applyAction { nullptr };
	Action* cancelAction { nullptr };
	vector<string> fileList;
	bool enableFilter;
	bool filtered;

public:
	GUIScreenNode* getScreenNode() override;

	/**
	 * @return path name
	 */
	const string& getPathName();

	/**
	 * @return file name
	 */
	const string getFileName();

	void initialize() override;
	void dispose() override;

private:

	/**
	 * Set up file dialog list box
	 * @return success
	 */
	bool setupFileDialogListBox();

	/**
	 * Set up file dialog list box files
	 * @param fileList file list
	 * @param selectedFile selected file
	 */
	void setupFileDialogListBoxFiles(const vector<string>& fileList, const string& selectedFile = string());

public:
	/**
	 * Public constructor
	 */
	FileDialogScreenController();

	/**
	 * Destructor
	 */
	virtual ~FileDialogScreenController();

	/**
	 * Shows the file dialog pop up
	 * @param cwd current working directory
	 * @param captionText caption text
	 * @param extensions extensions
	 * @param fileName file name
	 * @param enableFilter enable filter
	 * @param applyAction apply action
	 * @param cancelAction cancel action
	 * @throws IOException
	 */
	void show(const string& cwd, const string& captionText, const vector<string>& extensions, const string& fileName, bool enableFilter, Action* applyAction, Action* cancelAction = nullptr);

	/**
	 * Abort the file dialog pop up
	 */
	void close();

	// overridden methods
	void onValueChanged(GUIElementNode* node) override;
	void onActionPerformed(GUIActionListenerType type, GUIElementNode* node) override;
	void onFocus(GUIElementNode* node) override;
	void onUnfocus(GUIElementNode* node) override;

};