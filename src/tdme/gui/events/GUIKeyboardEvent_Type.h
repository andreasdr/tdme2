#pragma once

#include <string>

#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::utilities::Enum;
using tdme::gui::events::GUIKeyboardEvent;
using tdme::gui::events::GUIKeyboardEvent_Type;

/**
 * GUI keyboard event type enum
 * @author Andreas Drewke
 */
class tdme::gui::events::GUIKeyboardEvent_Type final: public Enum
{
	friend class GUIKeyboardEvent;

public:
	static GUIKeyboardEvent_Type* KEYBOARDEVENT_NONE;
	static GUIKeyboardEvent_Type* KEYBOARDEVENT_KEY_PRESSED;
	static GUIKeyboardEvent_Type* KEYBOARDEVENT_KEY_RELEASED;
	static GUIKeyboardEvent_Type* KEYBOARDEVENT_KEY_TYPED;
	GUIKeyboardEvent_Type(const string& name, int ordinal);
	static GUIKeyboardEvent_Type* valueOf(const string& a0);
};
