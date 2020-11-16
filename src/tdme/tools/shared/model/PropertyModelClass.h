#pragma once

#include <string>

#include <tdme/tools/shared/model/fwd-tdme.h>

using std::string;

/**
 * Property model class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::PropertyModelClass final
{
private:
	string name;
	string value;

public:
	/**
	 * Constructor
	 * @param name name
	 * @param value value
	 */
	inline PropertyModelClass(const string& name, const string& value): name(name), value(value) {}

	/**
	 * Destructor
	 */
	~PropertyModelClass();

	/**
	 * @return name
	 */
	inline const string& getName() {
		return name;
	}

	/**
	 * Set up name
	 * @param name name
	 */
	inline void setName(const string& name) {
		this->name = name;
	}

	/**
	 * @return value
	 */
	inline const string& getValue() {
		return value;
	}

	/**
	 * Set up value
	 * @param value value
	 */
	inline void setValue(const string& value) {
		this->value = value;
	}

	/**
	 * Clones this property model entity
	 */
	inline PropertyModelClass* clone() {
		return new PropertyModelClass(name, value);
	}

};
