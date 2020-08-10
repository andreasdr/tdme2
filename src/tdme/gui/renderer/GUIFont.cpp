#include <tdme/gui/renderer/GUIFont.h>

#include <map>
#include <vector>
#include <string>

#include <tdme/math/Math.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/subsystems/manager/TextureManager.h>
#include <tdme/gui/renderer/GUIFont_CharacterDefinition.h>
#include <tdme/gui/renderer/GUIRenderer.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utils/Integer.h>
#include <tdme/utils/MutableString.h>
#include <tdme/utils/StringTokenizer.h>
#include <tdme/utils/StringUtils.h>

using std::map;
using std::vector;
using std::string;

using tdme::gui::renderer::GUIFont;
using tdme::math::Math;
using tdme::engine::Engine;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::gui::renderer::GUIFont_CharacterDefinition;
using tdme::gui::renderer::GUIRenderer;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utils::Integer;
using tdme::utils::StringTokenizer;
using tdme::utils::StringUtils;
using tdme::utils::MutableString;

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
		StringUtils::substring(page, page.find("file=") + string("file=\"").length(), page.find_last_of("\"")),
		false
	);
	auto done = false;
	while (lineIdx < lines.size()) {
		auto line = lines[lineIdx++];
		if (StringUtils::startsWith(line, "chars c")) {
		} else
		if (StringUtils::startsWith(line, "char")) {
			auto def = font->parseCharacter(line);
			font->chars[def->id] = def;
		}
		if (StringUtils::startsWith(line, "kernings c")) {
		} else
		if (StringUtils::startsWith(line, "kerning")) {
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

void GUIFont::drawString(GUIRenderer* guiRenderer, int32_t x, int32_t y, const MutableString& text, int32_t offset, int32_t length, const GUIColor& color)
{
	guiRenderer->bindTexture(textureId);
	guiRenderer->setFontColor(color);
	y -= getYOffset(text) / 2;
	if (length == 0)
		length = text.length();

	for (auto i = offset; i < text.length() && i < length; i++) {
		int32_t id = text.charAt(i);
		GUIFont_CharacterDefinition* charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		charDef->draw(guiRenderer, x, y);
		auto xAdvance = charDef->xAdvance;
		x += xAdvance;
	}
	guiRenderer->render();
	guiRenderer->bindTexture(0);
}

int32_t GUIFont::getTextIndexX(const MutableString& text, int32_t offset, int32_t length, int32_t index)
{
	if (length == 0)
		length = text.length();

	auto x = 0;
	for (auto i = offset; i < index && i < text.length() && i < length; i++) {
		int32_t id = text.charAt(i);
		GUIFont_CharacterDefinition* charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		auto xAdvance = charDef->xAdvance;
		x += xAdvance;
	}
	return x;
}

int32_t GUIFont::getTextIndexByX(const MutableString& text, int32_t offset, int32_t length, int32_t textX)
{
	auto x = 0;
	auto index = offset;
	if (length == 0)
		length = text.length();

	for (; index < text.length() && index < length; index++) {
		int32_t id = text.charAt(index);
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

int32_t GUIFont::getYOffset(const MutableString& text)
{
	auto minYOffset = 10000;
	for (auto i = 0; i < text.length(); i++) {
		int32_t id = text.charAt(i);
		GUIFont_CharacterDefinition* charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		minYOffset = Math::min(charDef->yOffset, minYOffset);
	}
	return minYOffset;
}

int32_t GUIFont::getTextHeight(const MutableString& text)
{
	auto maxHeight = 0;
	for (auto i = 0; i < text.length(); i++) {
		int32_t id = text.charAt(i);
		if (id == ' ') continue;
		auto charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		maxHeight = Math::max(charDef->height + charDef->yOffset, maxHeight);
	}
	return maxHeight;
}

int32_t GUIFont::getTextWidth(const MutableString& text)
{
	auto width = 0;
	for (auto i = 0; i < text.length(); i++) {
		int32_t id = text.charAt(i);
		auto charDef = getCharacter(id);
		if (charDef == nullptr) continue;
		auto xAdvance = charDef->xAdvance;
		width += xAdvance;
	}
	return width;
}

int32_t GUIFont::getTextIndexXAtWidth(const MutableString& text, int32_t width) {
	auto x = 0;
	for (auto i = 0; i < text.length(); i++) {
		auto character = text.charAt(i);
		auto charDef = getCharacter(character);
		if (charDef == nullptr) continue;
		auto xAdvance = charDef->xAdvance;
		x += xAdvance;
		if (x > width) return i;
	}
	return text.length() - 1;
}

int32_t GUIFont::getLineHeight()
{
	return lineHeight;
}
