#include <tdme/gui/renderer/GUIFont.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/subsystems/manager/TextureManager.h>
#include <tdme/engine/Engine.h>
#include <tdme/gui/renderer/GUIFont_CharacterDefinition.h>
#include <tdme/gui/renderer/GUIRenderer.h>
#include <tdme/math/Math.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::Engine;
using tdme::gui::renderer::GUIFont;
using tdme::gui::renderer::GUIFont_CharacterDefinition;
using tdme::gui::renderer::GUIRenderer;
using tdme::math::Math;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Integer;
using tdme::utilities::MutableString;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;

MutableString GUIFont::LINEHEIGHT_STRING = MutableString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUV0123456789");

GUIFont::GUIFont()
{
}

GUIFont::~GUIFont()
{
	for (auto& charIt: chars) delete charIt.second;
	if (texture != nullptr) texture->releaseReference();
}

GUIFont* GUIFont::parse(const string& pathName, const string& fileName)
{
	int lineIdx = 0;
	auto font = new GUIFont();
	vector<string> lines;
	FileSystem::getInstance()->getContentAsStringArray(pathName, fileName, lines);
	auto info = lines[lineIdx++];
	auto common = lines[lineIdx++];
	auto page = lines[lineIdx++];
	font->texture = TextureReader::read(
		pathName,
		StringTools::substring(page, page.find("file=") + string("file=\"").length(), page.find_last_of("\"")),
		false
	);
	auto done = false;
	while (lineIdx < lines.size()) {
		auto line = lines[lineIdx++];
		if (StringTools::startsWith(line, "chars c")) {
		} else
		if (StringTools::startsWith(line, "char")) {
			auto def = font->parseCharacter(line);
			font->chars[def->id] = def;
		}
		if (StringTools::startsWith(line, "kernings c")) {
		} else
		if (StringTools::startsWith(line, "kerning")) {
			/*
			// TODO: not yet supported in the moment
			StringTokenizer t;
			t.tokenize(line, " =");
			t.nextToken();
			t.nextToken();
			auto first = Integer::parseInt(t.nextToken());
			t.nextToken();
			auto second = Integer::parseInt(t.nextToken());
			t.nextToken();
			auto offset = Integer::parseInt(t.nextToken());
			*/
		}
	}
	font->lineHeight = font->getTextHeight(LINEHEIGHT_STRING);
	return font;
}

GUIFont_CharacterDefinition* GUIFont::parseCharacter(const string& line)
{
	auto characterDefinition = new GUIFont_CharacterDefinition(this);
	StringTokenizer t;
	t.tokenize(line, " =");
	t.nextToken();
	t.nextToken();
	characterDefinition->id = Integer::parseInt(t.nextToken());
	t.nextToken();
	characterDefinition->x = Integer::parseInt(t.nextToken());
	t.nextToken();
	characterDefinition->y = Integer::parseInt(t.nextToken());
	t.nextToken();
	characterDefinition->width = Integer::parseInt(t.nextToken());
	t.nextToken();
	characterDefinition->height = Integer::parseInt(t.nextToken());
	t.nextToken();
	characterDefinition->xOffset = Integer::parseInt(t.nextToken());
	t.nextToken();
	characterDefinition->yOffset = Integer::parseInt(t.nextToken());
	t.nextToken();
	characterDefinition->xAdvance = Integer::parseInt(t.nextToken());
	if (characterDefinition->id != u' ') {
		lineHeight = Math::max(characterDefinition->height + characterDefinition->yOffset, lineHeight);
	}
	return characterDefinition;
}

void GUIFont::initialize()
{
	textureId = Engine::getInstance()->getTextureManager()->addTexture(texture, nullptr);
}

void GUIFont::dispose()
{
	Engine::getInstance()->getTextureManager()->removeTexture(texture->getId());
}

void GUIFont::drawString(GUIRenderer* guiRenderer, int x, int y, const MutableString& text, int offset, int length, const GUIColor& color)
{
	guiRenderer->bindTexture(textureId);
	guiRenderer->setFontColor(color);
	y -= getYOffset(text) / 2;
	if (length == 0)
		length = text.length();

	for (auto i = offset; i < text.length() && i < length; i++) {
		uint32_t id = text.charAt(i);
		GUIFont_CharacterDefinition* charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		charDef->draw(guiRenderer, x, y);
		auto xAdvance = charDef->xAdvance;
		x += xAdvance;
	}
	guiRenderer->render();
	guiRenderer->bindTexture(0);
}

int GUIFont::getTextIndexX(const MutableString& text, int offset, int length, int index)
{
	if (length == 0)
		length = text.length();

	auto x = 0;
	for (auto i = offset; i < index && i < text.length() && i < length; i++) {
		uint32_t id = text.charAt(i);
		GUIFont_CharacterDefinition* charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		auto xAdvance = charDef->xAdvance;
		x += xAdvance;
	}
	return x;
}

int GUIFont::getTextIndexByX(const MutableString& text, int offset, int length, int textX)
{
	auto x = 0;
	auto index = offset;
	if (length == 0)
		length = text.length();

	for (; index < text.length() && index < length; index++) {
		uint32_t id = text.charAt(index);
		GUIFont_CharacterDefinition* charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		auto xAdvance = charDef->xAdvance;
		x += xAdvance;
		if (x - xAdvance / 2 > textX) {
			return index;
		}
	}
	return index;
}

int GUIFont::getYOffset(const MutableString& text)
{
	auto minYOffset = 10000;
	for (auto i = 0; i < text.length(); i++) {
		uint32_t id = text.charAt(i);
		GUIFont_CharacterDefinition* charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		minYOffset = Math::min(charDef->yOffset, minYOffset);
	}
	return minYOffset;
}

int GUIFont::getTextHeight(const MutableString& text)
{
	auto maxHeight = 0;
	for (auto i = 0; i < text.length(); i++) {
		uint32_t id = text.charAt(i);
		if (id == ' ') continue;
		auto charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		maxHeight = Math::max(charDef->height + charDef->yOffset, maxHeight);
	}
	return maxHeight;
}

int GUIFont::getTextWidth(const MutableString& text)
{
	auto width = 0;
	for (auto i = 0; i < text.length(); i++) {
		uint32_t id = text.charAt(i);
		auto charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		auto xAdvance = charDef->xAdvance;
		width += xAdvance;
	}
	return width;
}

int GUIFont::getTextIndexXAtWidth(const MutableString& text, int width) {
	auto x = 0;
	for (auto i = 0; i < text.length(); i++) {
		uint32_t id = text.charAt(i);
		auto charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		auto xAdvance = charDef->xAdvance;
		x += xAdvance;
		if (x > width) return i;
	}
	return text.length() - 1;
}

int GUIFont::getLineHeight()
{
	return lineHeight;
}
