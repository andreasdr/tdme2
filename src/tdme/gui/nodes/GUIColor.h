#pragma once

#include <array>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/gui/GUIParserException.h>

using std::array;
using std::string;
using std::vector;

using tdme::engine::model::Color4Base;
using tdme::engine::model::Color4;
using tdme::gui::nodes::GUIColor;
using tdme::gui::GUIParserException;

/**
 * GUI color
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::nodes::GUIColor final
	: public Color4Base
{

public:
	static GUIColor GUICOLOR_WHITE;
	static GUIColor GUICOLOR_BLACK;
	static GUIColor GUICOLOR_RED;
	static GUIColor GUICOLOR_GREEN;
	static GUIColor GUICOLOR_BLUE;
	static GUIColor GUICOLOR_TRANSPARENT;
	static GUIColor GUICOLOR_EFFECT_COLOR_MUL;
	static GUIColor GUICOLOR_EFFECT_COLOR_ADD;

private:
	static vector<GUIColor*> COLOR_INSTANCES;
	static vector<string> COLOR_NAMES;

public:

	/**
	 * Public constructor
	 */
	GUIColor();

	/**
	 * Public constructor
	 * @param color color
	 */
	GUIColor(const Color4& color);

	/**
	 * Public constructor
	 * @param r red
	 * @param g green
	 * @param b blue
	 * @param a alpha
	 */
	GUIColor(float r, float g, float b, float a);

	/**
	 * Public constructor
	 * @param color color as float array
	 */
	GUIColor(const array<float, 4>& color);

	/**
	 * Public constructor
	 * @param colorString color as string
	 * @throws tdme::gui::GUIParserException
	 */
	GUIColor(const string& colorString);
};
