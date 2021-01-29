#include <tdme/engine/prototype/Prototype.h>

#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/ShaderModel.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/prototype/PrototypeAudio.h>
#include <tdme/engine/prototype/PrototypeBoundingVolume.h>
#include <tdme/engine/prototype/PrototypeLODLevel.h>
#include <tdme/engine/prototype/PrototypeParticleSystem.h>
#include <tdme/engine/prototype/PrototypePhysics.h>
#include <tdme/engine/prototype/PrototypeTerrain.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/StringTools.h>

using tdme::engine::model::Model;
using tdme::engine::model::ShaderModel;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::prototype::PrototypeAudio;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::prototype::PrototypeParticleSystem;
using tdme::engine::prototype::PrototypeTerrain;
using tdme::math::Vector3;
using tdme::utilities::StringTools;

constexpr int Prototype::ID_NONE;
constexpr int Prototype::MODEL_BOUNDINGVOLUME_COUNT;
constexpr int Prototype::MODEL_SOUNDS_COUNT;

char Prototype::MODEL_BOUNDINGVOLUME_EDITING_ID[] = "model_bv.editing";

char Prototype::MODEL_BOUNDINGVOLUMES_ID[] = "model_bv";

char Prototype::MODEL_BOUNDINGVOLUME_IDS[][Prototype::MODEL_BOUNDINGVOLUME_COUNT] = {
	"model_bv.0",
	"model_bv.1",
	"model_bv.2",
	"model_bv.3",
	"model_bv.4",
	"model_bv.5",
	"model_bv.6",
	"model_bv.7",
	"model_bv.8",
	"model_bv.9",
	"model_bv.10",
	"model_bv.11",
	"model_bv.12",
	"model_bv.13",
	"model_bv.14",
	"model_bv.15",
	"model_bv.16",
	"model_bv.17",
	"model_bv.18",
	"model_bv.19",
	"model_bv.20",
	"model_bv.21",
	"model_bv.22",
	"model_bv.23",
	"model_bv.24",
	"model_bv.25",
	"model_bv.26",
	"model_bv.27",
	"model_bv.28",
	"model_bv.29",
	"model_bv.30",
	"model_bv.31",
	"model_bv.32",
	"model_bv.33",
	"model_bv.34",
	"model_bv.35",
	"model_bv.36",
	"model_bv.37",
	"model_bv.38",
	"model_bv.39",
	"model_bv.40",
	"model_bv.41",
	"model_bv.42",
	"model_bv.43",
	"model_bv.44",
	"model_bv.45",
	"model_bv.46",
	"model_bv.47",
	"model_bv.48",
	"model_bv.49",
	"model_bv.50",
	"model_bv.51",
	"model_bv.52",
	"model_bv.53",
	"model_bv.54",
	"model_bv.55",
	"model_bv.56",
	"model_bv.57",
	"model_bv.58",
	"model_bv.59",
	"model_bv.60",
	"model_bv.61",
	"model_bv.62",
	"model_bv.63"
};

Prototype::Prototype(int id, Prototype_Type* entityType, const string& name, const string& description, const string& fileName, const string& modelFileName, const string& thumbnail, Model* model, const Vector3& pivot)
{
	this->id = id;
	this->type = entityType;
	this->name = name;
	this->description = description;
	this->fileName = fileName;
	this->modelFileName = modelFileName;
	this->thumbnailFileName = thumbnail;
	this->model = model;
	this->pivot.set(pivot);
	if (this->type == Prototype_Type::PARTICLESYSTEM) {
		this->physics = new PrototypePhysics();
	} else
	if (this->type == Prototype_Type::MODEL) {
		if (model->getShaderModel() == ShaderModel::PBR) {
			shaderId = StringTools::startsWith(shaderId, "pbr-") == true || shaderId.empty() == true?shaderId:"pbr-" + shaderId;
			distanceShaderId = StringTools::startsWith(distanceShaderId, "pbr-") == true || distanceShaderId.empty() == true?distanceShaderId:"pbr-" + distanceShaderId;
		}
		this->physics = new PrototypePhysics();
	} else
	if (this->type == Prototype_Type::TERRAIN) {
		this->terrain = new PrototypeTerrain();
	}
}

Prototype::~Prototype() {
	if (model != nullptr) delete model;
	if (lodLevel2 != nullptr) delete lodLevel2;
	if (lodLevel3 != nullptr) delete lodLevel3;
	if (physics != nullptr) delete physics;
	for (auto particleSystem: particleSystems) delete particleSystem;
	for (auto i = 0; i < boundingVolumes.size(); i++) delete boundingVolumes[i];
	for (auto sound: sounds) delete sound;
	if (terrain != nullptr) delete terrain;
}

void Prototype::setModel(Model* model) {
	if (this->model == model) return;
	if (this->model != nullptr) delete this->model;
	this->model = model;
}

bool Prototype::addBoundingVolume(int idx, PrototypeBoundingVolume* prototypeBoundingVolume)
{
	if (idx < 0)
		return false;

	if (idx > boundingVolumes.size())
		return false;

	if (idx == boundingVolumes.size()) {
		boundingVolumes.push_back(prototypeBoundingVolume);
	}
	return false;
}

void Prototype::removeBoundingVolume(int idx)
{
	delete boundingVolumes[idx];
	boundingVolumes.erase(boundingVolumes.begin() + idx);
}

void Prototype::setDefaultBoundingVolumes(int maxBoundingVolumeCount)
{
	auto boundingVolumeCount = maxBoundingVolumeCount == -1?MODEL_BOUNDINGVOLUME_COUNT:maxBoundingVolumeCount;
	for (auto i = boundingVolumes.size(); i < boundingVolumeCount; i++) {
		auto bv = new PrototypeBoundingVolume(i, this);
		addBoundingVolume(i, bv);
	}
}

void Prototype::setLODLevel2(PrototypeLODLevel* lodLevel) {
	if (this->type != Prototype_Type::MODEL) return;
	if (lodLevel2 != nullptr) delete lodLevel2;
	lodLevel2 = lodLevel;
}

void Prototype::setLODLevel3(PrototypeLODLevel* lodLevel) {
	if (this->type != Prototype_Type::MODEL) return;
	if (lodLevel3 != nullptr) delete lodLevel3;
	lodLevel3 = lodLevel;
}

PrototypeAudio* Prototype::addSound(const string& id) {
	if (sounds.size() == MODEL_SOUNDS_COUNT) return nullptr;
	auto sound = getSound(id);
	if (sound != nullptr) return nullptr;
	auto audio = new PrototypeAudio(id);
	soundsById[id] = audio;
	sounds.push_back(audio);
	return audio;
}

