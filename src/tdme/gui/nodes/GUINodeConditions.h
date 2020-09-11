#pragma once

#include <string>
#include <vector>

#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::vector;
using std::string;

using tdme::gui::nodes::GUIElementNode;

/** 
 * GUI element node conditions
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUINodeConditions final
{
	friend class GUINode;

private:
	GUIElementNode* elementNode { nullptr };
	vector<string> conditions;

	/**
	 * Update node
	 * @param node node
	 * @param conditions conditions that have been set
	 */
	void updateNode(GUINode* node, const vector<string>& conditions) const;

	/**
	 * Update element node
	 * @param conditions conditions that have been set
	 */
	void updateElementNode(const vector<string>& conditions) const;

public:

	/** 
	 * @return conditions
	 */
	const vector<string>& getConditions() const;

	/**
	 * Returns if condition is set
	 * @param condition condition name
	 * @return if condition is set
	 */
	bool has(const string& condition) const;

	/**
	 * Set condition
	 * @param condition condition
	 */
	void set(const string& condition);

	/**
	 * Set multiple conditions
	 * @param conditions conditions
	 */
	void set(const vector<string>& conditions);

	/** 
	 * Add a condition
	 * @param condition condition
	 * @return condition changed
	 */
	bool add(const string& condition);

	/** 
	 * Remove a condition
	 * @param condition condition
	 * @return condition changed
	 */
	bool remove(const string& condition);

	/** 
	 * Remove all
	 * @return condition changed
	 */
	bool removeAll();

	/**
	 * Public constructor
	 * @param node node
	 */
	GUINodeConditions(GUIElementNode* node = nullptr);
};
