#pragma once

#include <string>

#include <tdme/utilities/fwd-tdme.h>

using std::string;

/**
 * Enum base class
 * @author Andreas Drewke
 */
class tdme::utilities::Enum {
private:
	string name;
	int32_t ordinal;

public:
	/**
	 * Public constructor
	 * @param name name
	 * @param ordinal ordinal or index
	 */
	Enum(const string& name, int32_t ordinal);

	/**
	 * @return name
	 */
	const string& getName() const;

	/**
	 * @return ordinal or index
	 */
	int32_t getOrdinal() const;

	/**
	 * Compare enum with another enum
	 * @return equality
	 */
	bool equals(Enum* enumObject) const;

};
