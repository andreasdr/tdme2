#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::vector;
using std::string;

using tdme::tools::shared::model::PropertyModelClass;

/**
 * Properties
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::ModelProperties
{
private:
	map<string, PropertyModelClass*> propertiesByName;
	vector<PropertyModelClass*> properties;

public:
	/**
	 * Public constructor
	 */
	ModelProperties();

	/**
	 * Destructor
	 */
	virtual ~ModelProperties();

	/**
	 * Clears properties
	 */
	void clearProperties();

	/**
	 * Retrieve property by name
	 * @param name name
	 * @return property or null
	 */
	PropertyModelClass* getProperty(const string& name);

	/**
	 * @return property count
	 */
	inline int32_t getPropertyCount() {
		return properties.size();
	}

	/**
	 * Get property index
	 * @param name name
	 * @return index or -1 if not found
	 */
	int32_t getPropertyIndex(const string& name);

	/**
	 * Get property by index
	 * @param idx idx
	 * @return property or null
	 */
	inline PropertyModelClass* getPropertyByIndex(int32_t idx) {
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
