#include <tdme/engine/model/UpVector.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::engine::model::UpVector;
using tdme::utilities::Enum;

UpVector::UpVector(const string& name, int ordinal): Enum(name, ordinal)
{
}

UpVector* tdme::engine::model::UpVector::Y_UP = new UpVector("Y_UP", 0);
UpVector* tdme::engine::model::UpVector::Z_UP = new UpVector("Z_UP", 1);

UpVector* UpVector::valueOf(const string& a0)
{
	if (Y_UP->getName() == a0) return Y_UP;
	if (Z_UP->getName() == a0) return Z_UP;
	// TODO: throw exception here maybe
	return nullptr;
}
