#include <tdme/gui/events/GUIMouseEvent_Type.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::gui::events::GUIMouseEvent_Type;
using tdme::utilities::Enum;

GUIMouseEvent_Type::GUIMouseEvent_Type(const string& name, int ordinal) : Enum(name, ordinal)
{
}

GUIMouseEvent_Type* tdme::gui::events::GUIMouseEvent_Type::MOUSEEVENT_NONE = new GUIMouseEvent_Type("NONE", 0);
GUIMouseEvent_Type* tdme::gui::events::GUIMouseEvent_Type::MOUSEEVENT_WHEEL_MOVED = new GUIMouseEvent_Type("MOUSEEVENT_WHEEL_MOVED", 1);
GUIMouseEvent_Type* tdme::gui::events::GUIMouseEvent_Type::MOUSEEVENT_PRESSED = new GUIMouseEvent_Type("MOUSEEVENT_PRESSED", 2);
GUIMouseEvent_Type* tdme::gui::events::GUIMouseEvent_Type::MOUSEEVENT_RELEASED = new GUIMouseEvent_Type("MOUSEEVENT_RELEASED", 3);
GUIMouseEvent_Type* tdme::gui::events::GUIMouseEvent_Type::MOUSEEVENT_MOVED = new GUIMouseEvent_Type("MOUSEEVENT_MOVED", 4);
GUIMouseEvent_Type* tdme::gui::events::GUIMouseEvent_Type::MOUSEEVENT_DRAGGED = new GUIMouseEvent_Type("MOUSEEVENT_DRAGGED", 5);

GUIMouseEvent_Type* GUIMouseEvent_Type::valueOf(const string& a0)
{
	if (MOUSEEVENT_DRAGGED->getName() == a0) return MOUSEEVENT_DRAGGED;
	if (MOUSEEVENT_MOVED->getName() == a0) return MOUSEEVENT_MOVED;
	if (MOUSEEVENT_PRESSED->getName() == a0) return MOUSEEVENT_PRESSED;
	if (MOUSEEVENT_RELEASED->getName() == a0) return MOUSEEVENT_RELEASED;
	if (MOUSEEVENT_WHEEL_MOVED->getName() == a0) return MOUSEEVENT_WHEEL_MOVED;
	if (MOUSEEVENT_NONE->getName() == a0) return MOUSEEVENT_NONE;
	// TODO: throw exception here maybe
	return nullptr;
}
