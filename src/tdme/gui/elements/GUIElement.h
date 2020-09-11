#pragma once

#include <unordered_map>
#include <string>

#include <tdme/gui/elements/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::unordered_map;
using std::string;

using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIScreenNode;

/** 
 * GUI element base class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::elements::GUIElement
{

public:

	/** 
	 * @return name
	 */
	virtual const string& getName() = 0;

	/** 
	 * Retrieve template from given path name and optional file name
	 * @param pathName path name
	 * @param fileName file name
	 * @throws tdme::os::filesystem::FileSystemException
	 * @return template
	 */
	virtual const string getTemplate(const string& pathName, const string& fileName = string()) = 0;

	/** 
	 * Get default attributes
	 * @param screenNode screen node
	 * @return default attributes
	 */
	virtual unordered_map<string, string> getAttributes(GUIScreenNode* screenNode) = 0;

	/** 
	 * Create controller which is attached to this node
	 * @param node node
	 * @return
	 */
	virtual GUINodeController* createController(GUINode* node) = 0;

	/**
	 * Public constructor
	 */
	GUIElement();

	/**
	 * Destructor
	 */
	virtual ~GUIElement();
};
