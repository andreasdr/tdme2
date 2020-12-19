#include <tdme/engine/subsystems/lines/LinesObject3DInternal.h>

#include <string>
#include <vector>

#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/manager/TextureManager.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/engine/subsystems/rendering/ObjectBuffer.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/ByteBuffer.h>
#include <tdme/utilities/FloatBuffer.h>

using std::string;
using std::vector;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::model::Color4;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::lines::LinesObject3DInternal;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::ObjectBuffer;
using tdme::engine::Engine;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::utilities::ByteBuffer;
using tdme::utilities::FloatBuffer;

LinesObject3DInternal::LinesObject3DInternal(const string& id, float lineWidth, const vector<Vector3>& points, const Color4& color, const vector<Color4>& colors, Texture* texture)
{
	this->id = id;
	this->enabled = true;
	this->effectColorMul.set(1.0f, 1.0f, 1.0f, 1.0f);
	this->effectColorAdd.set(0.0f, 0.0f, 0.0f, 0.0f);
	this->pickable = false;
	this->contributesShadows = false;
	this->receivesShadows = false;
	this->lineWidth = lineWidth;
	this->points = points;
	this->color = color;
	this->colors = colors;
	this->texture = texture;
	this->textureId = 0;
	if (points.size() > 1) {
		boundingBox.getMin().set(points[0]);
		boundingBox.getMax().set(points[0]);
		auto& bbMinXYZ = boundingBox.getMin().getArray();
		auto& bbMaxXYZ = boundingBox.getMax().getArray();
		for (auto& point: points) {
			auto& pointXYZ = point.getArray();
			if (pointXYZ[0] < bbMinXYZ[0]) bbMinXYZ[0] = pointXYZ[0];
			if (pointXYZ[1] < bbMinXYZ[1]) bbMinXYZ[1] = pointXYZ[1];
			if (pointXYZ[2] < bbMinXYZ[2]) bbMinXYZ[2] = pointXYZ[2];
			if (pointXYZ[0] > bbMaxXYZ[0]) bbMaxXYZ[0] = pointXYZ[0];
			if (pointXYZ[1] > bbMaxXYZ[1]) bbMaxXYZ[1] = pointXYZ[1];
			if (pointXYZ[2] > bbMaxXYZ[2]) bbMaxXYZ[2] = pointXYZ[2];
		}
		boundingBox.update();
		updateBoundingBox();
	}
	vboIds = nullptr;
}

LinesObject3DInternal::~LinesObject3DInternal() {
}

void LinesObject3DInternal::update()
{
	Transformations::update();
	updateBoundingBox();
}

void LinesObject3DInternal::fromTransformations(const Transformations& transformations)
{
	Transformations::fromTransformations(transformations);
	updateBoundingBox();
}

void LinesObject3DInternal::initialize() {
	// texture
	this->textureId = this->texture == nullptr?engine->getTextureManager()->addTexture(this->texture = TextureReader::read("resources/engine/textures", "point.png"), renderer->getDefaultContext()):engine->getTextureManager()->addTexture(this->texture, renderer->getDefaultContext());

	// initialize if not yet done
	auto created = false;
	auto vboManaged = Engine::getInstance()->getVBOManager()->addVBO(id + ".vbos", 2, true, false, created);
	vboIds = vboManaged->getVBOIds();

	//
	auto context = renderer->getDefaultContext();

	{
		// upload points
		auto fbPoints = ObjectBuffer::getByteBuffer(context, points.size() * 3 * sizeof(float))->asFloatBuffer();
		for (auto& point: points) fbPoints.put(point.getArray());
		renderer->uploadBufferObject(context, (*vboIds)[0], fbPoints.getPosition() * sizeof(float), &fbPoints);
	}

	{
		// upload colors
		auto fbColors = ObjectBuffer::getByteBuffer(context, points.size() * 4 * sizeof(float))->asFloatBuffer();
		if (colors.size() == points.size()) {
			for (auto& color: colors) fbColors.put(color.getArray());
		} else {
			for (auto& point: points) fbColors.put(color.getArray());
		}
		renderer->uploadBufferObject(context, (*vboIds)[1], fbColors.getPosition() * sizeof(float), &fbColors);
	}
}

void LinesObject3DInternal::dispose()
{
	engine->getTextureManager()->removeTexture(texture->getId());
	Engine::getInstance()->getVBOManager()->removeVBO(id + ".vbos");
}
