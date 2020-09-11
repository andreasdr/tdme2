#pragma once

#include <string>

#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::utilities::Enum;
using tdme::engine::model::Model;
using tdme::engine::model::UpVector;

/**
 * Model up vector
 */
class tdme::engine::model::UpVector final
	: public Enum
{
	friend class Model;

public:
	static UpVector* Y_UP;
	static UpVector* Z_UP;

public:
	UpVector(const string& name, int ordinal);

public:
	static UpVector* valueOf(const string& a0);

};
