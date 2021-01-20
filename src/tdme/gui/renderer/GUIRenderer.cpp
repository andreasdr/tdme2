#include <tdme/gui/renderer/GUIRenderer.h>

#include <tdme/math/Math.h>
#include <tdme/utilities/Time.h>
#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/FloatBuffer.h>
#include <tdme/utilities/IntBuffer.h>
#include <tdme/utilities/ShortBuffer.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/subsystems/manager/VBOManager_VBOManaged.h>
#include <tdme/engine/subsystems/manager/VBOManager.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/nodes/GUIColor.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/renderer/GUIShader.h>
#include <tdme/math/Matrix2D3x3.h>
#include <tdme/utilities/Console.h>

using tdme::gui::renderer::GUIRenderer;
using tdme::math::Math;
using tdme::utilities::Time;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;
using tdme::utilities::IntBuffer;
using tdme::utilities::ShortBuffer;
using tdme::engine::Engine;
using tdme::engine::subsystems::manager::VBOManager_VBOManaged;
using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::gui::GUI;
using tdme::gui::nodes::GUIColor;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::renderer::GUIShader;
using tdme::math::Matrix2D3x3;
using tdme::utilities::Console;

GUIRenderer::GUIRenderer(Renderer* renderer)
{
	this->renderer = renderer;
	sbIndicesByteBuffer = ByteBuffer::allocate(QUAD_COUNT * 6 * (renderer->isUsingShortIndices() == true?sizeof(uint16_t):sizeof(uint32_t)));
	fbVertices = (fbVerticesByteBuffer = ByteBuffer::allocate(QUAD_COUNT * 6 * 3 * sizeof(float)))->asFloatBuffer();
	fbColors = (fbColorsByteBuffer = ByteBuffer::allocate(QUAD_COUNT * 6 * 4 * sizeof(float)))->asFloatBuffer();
	fbTextureCoordinates = (fbTextureCoordinatesByteBuffer = ByteBuffer::allocate(QUAD_COUNT * 6 * 2 * sizeof(float)))->asFloatBuffer();
	renderAreaLeft = 0.0f;
	renderAreaTop = 0.0f;
	renderAreaRight = 0.0f;
	renderAreaBottom = 0.0f;
	renderOffsetX = 0.0f;
	renderOffsetY = 0.0f;
	guiEffectOffsetX = 0.0f;
	guiEffectOffsetY = 0.0f;
}

GUIRenderer::~GUIRenderer() {
	delete sbIndicesByteBuffer;
	delete fbVerticesByteBuffer;
	delete fbColorsByteBuffer;
	delete fbTextureCoordinatesByteBuffer;
}


constexpr int32_t GUIRenderer::QUAD_COUNT;

constexpr float GUIRenderer::SCREEN_LEFT;

constexpr float GUIRenderer::SCREEN_TOP;

constexpr float GUIRenderer::SCREEN_RIGHT;

constexpr float GUIRenderer::SCREEN_BOTTOM;

void GUIRenderer::setGUI(GUI* gui)
{
	this->gui = gui;
}

GUI* GUIRenderer::getGUI()
{
	return gui;
}

void GUIRenderer::initialize()
{
	if (vboIds == nullptr) {
		auto created = false;
		auto vboManaged = Engine::getInstance()->getVBOManager()->addVBO("tdme.guirenderer", 4, false, false, created);
		vboIds = vboManaged->getVBOIds();
		if (renderer->isUsingShortIndices() == true) {
			auto sbIndices = sbIndicesByteBuffer->asShortBuffer();
			for (auto i = 0; i < QUAD_COUNT; i++) {
				sbIndices.put(static_cast< uint16_t >((i * 4 + 0)));
				sbIndices.put(static_cast< uint16_t >((i * 4 + 1)));
				sbIndices.put(static_cast< uint16_t >((i * 4 + 2)));
				sbIndices.put(static_cast< uint16_t >((i * 4 + 2)));
				sbIndices.put(static_cast< uint16_t >((i * 4 + 3)));
				sbIndices.put(static_cast< uint16_t >((i * 4 + 0)));
			}
			renderer->uploadIndicesBufferObject(renderer->getDefaultContext(), (*vboIds)[0], sbIndices.getPosition() * sizeof(uint16_t), &sbIndices);
		} else {
			auto ibIndices = sbIndicesByteBuffer->asIntBuffer();
			for (auto i = 0; i < QUAD_COUNT; i++) {
				ibIndices.put(i * 4 + 0);
				ibIndices.put(i * 4 + 1);
				ibIndices.put(i * 4 + 2);
				ibIndices.put(i * 4 + 2);
				ibIndices.put(i * 4 + 3);
				ibIndices.put(i * 4 + 0);
			}
			renderer->uploadIndicesBufferObject(renderer->getDefaultContext(), (*vboIds)[0], ibIndices.getPosition() * sizeof(uint32_t), &ibIndices);
		}
	}
}

void GUIRenderer::dispose()
{
	if (vboIds != nullptr) {
		Engine::getInstance()->getVBOManager()->removeVBO("tdme.guirenderer");
		vboIds = nullptr;
	}
}

void GUIRenderer::initRendering()
{
	setRenderAreaLeft(SCREEN_LEFT);
	setRenderAreaTop(SCREEN_TOP);
	setRenderAreaRight(SCREEN_RIGHT);
	setRenderAreaBottom(SCREEN_BOTTOM);
	Engine::getGUIShader()->useProgram();
	renderer->getTextureMatrix(renderer->getDefaultContext()).identity();
}

void GUIRenderer::doneRendering()
{
	renderer->unbindBufferObjects(renderer->getDefaultContext());
	Engine::getGUIShader()->unUseProgram();
}

void GUIRenderer::initScreen(GUIScreenNode* screenNode)
{
	this->screenNode = screenNode;
	guiEffectOffsetX = 0.0f;
	guiEffectOffsetY = 0.0f;
	screenNode->setGUIEffectOffsetX(0);
	screenNode->setGUIEffectOffsetY(0);
}

void GUIRenderer::doneScreen()
{
	this->screenNode = nullptr;
	guiEffectColorMul = GUIColor::GUICOLOR_WHITE.getArray();
	guiEffectColorAdd = GUIColor::GUICOLOR_BLACK.getArray();
}

void GUIRenderer::setFontColor(const GUIColor& color)
{
	fontColor = color.getArray();
}

void GUIRenderer::setEffectColorMul(const GUIColor& color)
{
	effectColorMul = color.getArray();
}

void GUIRenderer::setEffectColorAdd(const GUIColor& color)
{
	effectColorAdd = color.getArray();
}

void GUIRenderer::setGUIEffectColorMul(const GUIColor& color)
{
	guiEffectColorMul = color.getArray();
}

void GUIRenderer::setGUIEffectColorAdd(const GUIColor& color)
{
	guiEffectColorAdd = color.getArray();
}

float GUIRenderer::getGuiEffectOffsetX()
{
	return guiEffectOffsetX;
}

void GUIRenderer::setGUIEffectOffsetX(float guiEffectOffsetX)
{
	this->guiEffectOffsetX = guiEffectOffsetX;
	screenNode->setGUIEffectOffsetX(static_cast< int32_t >((guiEffectOffsetX * screenNode->getScreenWidth() / 2.0f)));
}

float GUIRenderer::getGuiEffectOffsetY()
{
	return guiEffectOffsetY;
}

void GUIRenderer::setGUIEffectOffsetY(float guiEffectOffsetY)
{
	this->guiEffectOffsetY = guiEffectOffsetY;
	screenNode->setGUIEffectOffsetY(static_cast< int32_t >((guiEffectOffsetY * screenNode->getScreenHeight() / 2.0f)));
}

void GUIRenderer::addQuad(float x1, float y1, float colorR1, float colorG1, float colorB1, float colorA1, float tu1, float tv1, float x2, float y2, float colorR2, float colorG2, float colorB2, float colorA2, float tu2, float tv2, float x3, float y3, float colorR3, float colorG3, float colorB3, float colorA3, float tu3, float tv3, float x4, float y4, float colorR4, float colorG4, float colorB4, float colorA4, float tu4, float tv4)
{
	if (quadCount > QUAD_COUNT) {
		Console::println("GUIRenderer::addQuad()::too many quads");
		return;
	}
	x1 -= renderOffsetX;
	x2 -= renderOffsetX;
	x3 -= renderOffsetX;
	x4 -= renderOffsetX;
	y1 += renderOffsetY;
	y2 += renderOffsetY;
	y3 += renderOffsetY;
	y4 += renderOffsetY;
	x1 -= guiEffectOffsetX;
	x2 -= guiEffectOffsetX;
	x3 -= guiEffectOffsetX;
	x4 -= guiEffectOffsetX;
	y1 += guiEffectOffsetY;
	y2 += guiEffectOffsetY;
	y3 += guiEffectOffsetY;
	y4 += guiEffectOffsetY;
	auto renderAreaTop = this->renderAreaTop;
	auto renderAreaBottom = this->renderAreaBottom;
	auto renderAreaRight = this->renderAreaRight;
	auto renderAreaLeft = this->renderAreaLeft;
	renderAreaTop = Math::min(renderAreaTop + guiEffectOffsetY, SCREEN_TOP);
	renderAreaBottom = Math::max(renderAreaBottom + guiEffectOffsetY, SCREEN_BOTTOM);
	renderAreaRight = Math::min(renderAreaRight - guiEffectOffsetX, SCREEN_RIGHT);
	renderAreaLeft = Math::max(renderAreaLeft - guiEffectOffsetX, SCREEN_LEFT);

	auto quadBottom = y3;
	if (quadBottom > renderAreaTop) return;
	auto quadTop = y1;
	if (quadTop < renderAreaBottom) return;
	auto quadLeft = x1;
	if (quadLeft > renderAreaRight) return;
	auto quadRight = x2;
	if (quadRight < renderAreaLeft) return;

	if (quadBottom < renderAreaBottom) {
		tv3 = tv1 + ((tv3 - tv1) * ((y1 - renderAreaBottom) / (y1 - y3)));
		tv4 = tv2 + ((tv4 - tv2) * ((y2 - renderAreaBottom) / (y1 - y4)));
		y3 = renderAreaBottom;
		y4 = renderAreaBottom;
	}
	if (quadTop > renderAreaTop) {
		tv1 = tv1 + ((tv3 - tv1) * ((y1 - renderAreaTop) / (y1 - y3)));
		tv2 = tv2 + ((tv4 - tv2) * ((y2 - renderAreaTop) / (y1 - y4)));
		y1 = renderAreaTop;
		y2 = renderAreaTop;
	}
	if (quadLeft < renderAreaLeft) {
		tu1 = tu1 + ((tu2 - tu1) * ((renderAreaLeft - x1) / (x2 - x1)));
		tu4 = tu4 + ((tu3 - tu4) * ((renderAreaLeft - x4) / (x3 - x4)));
		x1 = renderAreaLeft;
		x4 = renderAreaLeft;
	}
	if (quadRight > renderAreaRight) {
		tu2 = tu2 - ((tu2 - tu1) * ((x2 - renderAreaRight) / (x2 - x1)));
		tu3 = tu3 - ((tu3 - tu4) * ((x3 - renderAreaRight) / (x3 - x4)));
		x2 = renderAreaRight;
		x3 = renderAreaRight;
	}

	fbVertices.put(x1);
	fbVertices.put(y1);
	fbVertices.put(0.0f);
	fbColors.put(colorR1);
	fbColors.put(colorG1);
	fbColors.put(colorB1);
	fbColors.put(colorA1);
	fbTextureCoordinates.put(tu1);
	fbTextureCoordinates.put(tv1);
	fbVertices.put(x2);
	fbVertices.put(y2);
	fbVertices.put(0.0f);
	fbColors.put(colorR2);
	fbColors.put(colorG2);
	fbColors.put(colorB2);
	fbColors.put(colorA2);
	fbTextureCoordinates.put(tu2);
	fbTextureCoordinates.put(tv2);
	fbVertices.put(x3);
	fbVertices.put(y3);
	fbVertices.put(0.0f);
	fbColors.put(colorR3);
	fbColors.put(colorG3);
	fbColors.put(colorB3);
	fbColors.put(colorA3);
	fbTextureCoordinates.put(tu3);
	fbTextureCoordinates.put(tv3);
	fbVertices.put(x4);
	fbVertices.put(y4);
	fbVertices.put(0.0f);
	fbColors.put(colorR4);
	fbColors.put(colorG4);
	fbColors.put(colorB4);
	fbColors.put(colorA4);
	fbTextureCoordinates.put(tu4);
	fbTextureCoordinates.put(tv4);
	quadCount++;
}

void GUIRenderer::setTexureMatrix(const Matrix2D3x3& textureMatrix) {
	renderer->getTextureMatrix(renderer->getDefaultContext()).set(textureMatrix);
}

void GUIRenderer::bindTexture(int32_t textureId)
{
	renderer->bindTexture(renderer->getDefaultContext(), textureId);
}

void GUIRenderer::bindMask(int32_t textureId)
{
	renderer->setTextureUnit(renderer->getDefaultContext(), 1);
	renderer->bindTexture(renderer->getDefaultContext(), textureId);
	renderer->setTextureUnit(renderer->getDefaultContext(), 0);
}

void GUIRenderer::setMaskMaxValue(float maskMaxValue) {
	renderer->setMaskMaxValue(renderer->getDefaultContext(), maskMaxValue);
}

void GUIRenderer::render()
{
	if (quadCount == 0) {
		fontColor = GUIColor::GUICOLOR_WHITE.getArray();
		effectColorMul = GUIColor::GUICOLOR_WHITE.getArray();
		effectColorAdd = GUIColor::GUICOLOR_BLACK.getArray();
		return;
	}
	// use default context
	auto context = renderer->getDefaultContext();
	renderer->uploadBufferObject(context, (*vboIds)[1], fbVertices.getPosition() * sizeof(float), &fbVertices);
	renderer->uploadBufferObject(context, (*vboIds)[2], fbColors.getPosition() * sizeof(float), &fbColors);
	renderer->uploadBufferObject(context, (*vboIds)[3], fbTextureCoordinates.getPosition() * sizeof(float), &fbTextureCoordinates);
	renderer->bindIndicesBufferObject(context, (*vboIds)[0]);
	renderer->bindVerticesBufferObject(context, (*vboIds)[1]);
	renderer->bindColorsBufferObject(context, (*vboIds)[2]);
	renderer->bindTextureCoordinatesBufferObject(context, (*vboIds)[3]);
	effectColorMulFinal[0] = guiEffectColorMul[0] * effectColorMul[0] * fontColor[0];
	effectColorMulFinal[1] = guiEffectColorMul[1] * effectColorMul[1] * fontColor[1];
	effectColorMulFinal[2] = guiEffectColorMul[2] * effectColorMul[2] * fontColor[2];
	effectColorMulFinal[3] = guiEffectColorMul[3] * effectColorMul[3] * fontColor[3];
	effectColorAddFinal[0] = guiEffectColorAdd[0] + effectColorAdd[0];
	effectColorAddFinal[1] = guiEffectColorAdd[1] + effectColorAdd[1];
	effectColorAddFinal[2] = guiEffectColorAdd[2] + effectColorAdd[2];
	effectColorAddFinal[3] = 0.0f;
	renderer->getEffectColorMul(context) = effectColorMulFinal;
	renderer->getEffectColorAdd(context) = effectColorAddFinal;
	renderer->onUpdateEffect(context);
	renderer->onUpdateTextureMatrix(context);
	renderer->drawIndexedTrianglesFromBufferObjects(context, quadCount * 2, 0);
	quadCount = 0;
	fbVertices.clear();
	fbColors.clear();
	fbTextureCoordinates.clear();
	fontColor = GUIColor::GUICOLOR_WHITE.getArray();
	effectColorMul = GUIColor::GUICOLOR_WHITE.getArray();
	effectColorAdd = GUIColor::GUICOLOR_BLACK.getArray();
	effectColorAdd[3] = 0.0f;
	guiEffectColorAdd[3] = 0.0f;
}

