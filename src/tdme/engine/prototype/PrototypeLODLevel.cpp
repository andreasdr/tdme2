#include <tdme/engine/prototype/PrototypeLODLevel.h>

#include <string>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/LODObject3D.h>

using tdme::engine::prototype::PrototypeLODLevel;

using std::string;

using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::LODObject3D;

PrototypeLODLevel::PrototypeLODLevel(
	LODObject3D::LODLevelType type,
	const string& fileName,
	Model* model,
	float minDistance
) :
	type(type),
	fileName(fileName),
	model(model),
	minDistance(minDistance) {
	colorMul.set(1.0f, 1.0f, 1.0f, 1.0f);
	colorAdd.set(0.0f, 0.0f, 0.0f, 0.0f);
}

PrototypeLODLevel::~PrototypeLODLevel() {
	if (model != nullptr) delete model;
}
