#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::prototype::PrototypeProperty;

/**
 * Prototype properties
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::prototype::PrototypeProperties
{
private:
	map<string, PrototypeProperty*> propertiesByName;
	vector<PrototypeProperty*> properties;

public:
	/**
	 * Public constructor
	 */
	PrototypeProperties();

	/**
	 * Destructor
	 */
	virtual ~PrototypeProperties();

	/**
	 * Clears properties
	 */
	void clearProperties();

	/**
	 * Retrieve property by name
	 * @param name name
	 * @return property or null
	 */
	PrototypeProperty* getProperty(const string& name);

	/**
	 * @return property count
	 */
	inline int getPropertyCount() {
		return properties.size();
	}

	/**
	 * Get property index
	 * @param name name
	 * @return index or -1 if not found
	 */
	int getPropertyIndex(const string& name);

	/**
	 * Get property by index
	 * @param idx idx
	 * @return property or null
	 */
	inline PrototypeProperty* getPropertyByIndex(int idx) {
		return idx >= 0 && idx < properties.size()?properties[idx]:nullptr;
	}

	/**
	 * Add a property
	 * @param name name
	 * @param value value
	 */
	bool addProperty(const string& name, const string& value);

	/**
	 * Update a property
	 * @param oldName old name
	 * @param name name
	 * @param value value
	 * @return success
	 */
	bool updateProperty(const string& oldName, const string& name, const string& value);

	/**
	 * Removes a property
	 * @param name property name
	 */
	bool removeProperty(const string& name);

};
