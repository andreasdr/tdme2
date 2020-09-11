#pragma once

#include <tdme/gui/events/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using tdme::utilities::Enum;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListener_Type;

/**
 * GUI action listener type enum
 */
class tdme::gui::events::GUIActionListener_Type final: public Enum
{
	friend class GUIActionListener;

public:
	static GUIActionListener_Type *PERFORMED;
	static GUIActionListener_Type *PERFORMING;
	GUIActionListener_Type(const string& name, int ordinal);
	static GUIActionListener_Type* valueOf(const string& a0);
};
