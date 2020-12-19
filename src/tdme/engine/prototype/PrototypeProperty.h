#pragma once

#include <string>

#include <tdme/engine/prototype/fwd-tdme.h>

using std::string;

/**
 * Prototype property model class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::prototype::PrototypeProperty final
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
	inline PrototypeProperty(const string& name, const string& value): name(name), value(value) {}

	/**
	 * Destructor
	 */
	~PrototypeProperty();

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
	inline PrototypeProperty* clone() {
		return new PrototypeProperty(name, value);
	}

};
