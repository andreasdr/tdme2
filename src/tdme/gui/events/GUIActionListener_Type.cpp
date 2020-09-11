#include <tdme/gui/events/GUIActionListener_Type.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::gui::events::GUIActionListener_Type;
using tdme::utilities::Enum;

GUIActionListener_Type::GUIActionListener_Type(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

GUIActionListener_Type* tdme::gui::events::GUIActionListener_Type::PERFORMED = new GUIActionListener_Type("PERFORMED", 0);
GUIActionListener_Type* tdme::gui::events::GUIActionListener_Type::PERFORMING = new GUIActionListener_Type("PERFORMING", 1);

GUIActionListener_Type* GUIActionListener_Type::valueOf(const string& a0)
{
	if (PERFORMED->getName() == a0) return PERFORMED;
	if (PERFORMING->getName() == a0) return PERFORMING;
	// TODO: throw exception here maybe
	return nullptr;
}
