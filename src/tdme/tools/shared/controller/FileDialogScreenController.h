#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/tools/shared/controller/ScreenController.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/events/GUIFocusListener.h>

using std::string;
using std::vector;

using tdme::tools::shared::controller::ScreenController;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIChangeListener;
using tdme::gui::events::GUIFocusListener;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::utilities::MutableString;

/** 
 * File dialog screen controller
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::controller::FileDialogScreenController
	: public ScreenController
	, public virtual GUIActionListener
	, public virtual GUIChangeListener
	, public virtual GUIFocusListener
{

private:
	GUIScreenNode* screenNode {  };
	string cwd {  };
	vector<string> extensions {  };
	string captionText {  };
	GUITextNode* caption {  };
	GUIElementNode* fileName {  };
	GUIElementNode* files {  };
	Action* applyAction {  };
	vector<string> fileList;
	bool enableFilter { false };
	bool filtered {  };

public:
	GUIScreenNode* getScreenNode() override;

	/** 
	 * @return path name
	 */
	virtual const string& getPathName();

	/** 
	 * @return file name
	 */
	virtual const string getFileName();

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
	 * Shows the file dialog pop up
	 * @param cwd current working directory
	 * @param captionText caption text
	 * @param extensions extensions
	 * @param fileName file name
	 * @param enableFilter enable filter
	 * @param applyAction apply action
	 * @throws IOException 
	 */
	virtual void show(const string& cwd, const string& captionText, const vector<string>& extensions, const string& fileName, bool enableFilter, Action* applyAction);

	/** 
	 * Abort the file dialog pop up
	 */
	virtual void close();

	// overriden methods
	void onValueChanged(GUIElementNode* node) override;
	void onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node) override;
	void onFocus(GUIElementNode* node) override;
	void onUnfocus(GUIElementNode* node) override;

	/**
	 * Public constructor
	 */
	FileDialogScreenController();

	/**
	 * Destructor
	 */
	virtual ~FileDialogScreenController();
};
