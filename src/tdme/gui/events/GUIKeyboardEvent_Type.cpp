#include <tdme/gui/events/GUIKeyboardEvent_Type.h>

#include <tdme/utilities/Enum.h>

using tdme::gui::events::GUIKeyboardEvent_Type;
using tdme::utilities::Enum;

GUIKeyboardEvent_Type::GUIKeyboardEvent_Type(const string& name, int ordinal)
	: Enum(name, ordinal)
{
}

GUIKeyboardEvent_Type* tdme::gui::events::GUIKeyboardEvent_Type::KEYBOARDEVENT_NONE = new GUIKeyboardEvent_Type("NONE", 0);
GUIKeyboardEvent_Type* tdme::gui::events::GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_PRESSED = new GUIKeyboardEvent_Type("KEY_PRESSED", 1);
GUIKeyboardEvent_Type* tdme::gui::events::GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_RELEASED = new GUIKeyboardEvent_Type("KEY_RELEASED", 2);
GUIKeyboardEvent_Type* tdme::gui::events::GUIKeyboardEvent_Type::KEYBOARDEVENT_KEY_TYPED = new GUIKeyboardEvent_Type("KEY_TYPED", 3);

GUIKeyboardEvent_Type* GUIKeyboardEvent_Type::valueOf(const string& a0)
{
	if (KEYBOARDEVENT_KEY_PRESSED->getName() == a0) return KEYBOARDEVENT_KEY_PRESSED;
	if (KEYBOARDEVENT_KEY_RELEASED->getName() == a0) return KEYBOARDEVENT_KEY_RELEASED;
	if (KEYBOARDEVENT_KEY_TYPED->getName() == a0) return KEYBOARDEVENT_KEY_TYPED;
	if (KEYBOARDEVENT_NONE->getName() == a0) return KEYBOARDEVENT_NONE;
	// TODO: throw exception here maybe
	return nullptr;
}
