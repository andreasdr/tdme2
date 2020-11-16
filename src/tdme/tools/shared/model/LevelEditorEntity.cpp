#include <tdme/tools/shared/model/LevelEditorEntity.h>

#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/ShaderModel.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorEntityAudio.h>
#include <tdme/tools/shared/model/LevelEditorEntityBoundingVolume.h>
#include <tdme/tools/shared/model/LevelEditorEntityLODLevel.h>
#include <tdme/tools/shared/model/LevelEditorEntityModel.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem.h>
#include <tdme/tools/shared/model/LevelEditorEntityPhysics.h>
#include <tdme/utilities/StringTools.h>

using tdme::tools::shared::model::LevelEditorEntity;
using tdme::engine::model::Model;
using tdme::engine::model::ShaderModel;
using tdme::math::Vector3;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntityAudio;
using tdme::tools::shared::model::LevelEditorEntityBoundingVolume;
using tdme::tools::shared::model::LevelEditorEntityModel;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem;
using tdme::utilities::StringTools;

constexpr int32_t LevelEditorEntity::ID_NONE;
constexpr int32_t LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT;
constexpr int32_t LevelEditorEntity::MODEL_SOUNDS_COUNT;

char LevelEditorEntity::MODEL_BOUNDINGVOLUME_EDITING_ID[] = "model_bv.editing";

char LevelEditorEntity::MODEL_BOUNDINGVOLUMES_ID[] = "model_bv";

char LevelEditorEntity::MODEL_BOUNDINGVOLUME_IDS[][LevelEditorEntity::MODEL_BOUNDINGVOLUME_COUNT] = {
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

LevelEditorEntity::LevelEditorEntity(int32_t id, LevelEditorEntity_EntityType* entityType, const string& name, const string& description, const string& entityFileName, const string& fileName, const string& thumbnail, Model* model, const Vector3& pivot)
{
	this->id = id;
	this->type = entityType;
	this->name = name;
	this->description = description;
	this->entityFileName = entityFileName;
	this->fileName = fileName;
	this->thumbnail = thumbnail;
	this->model = model;
	this->pivot.set(pivot);
	this->lodLevel2 = nullptr;
	this->lodLevel3 = nullptr;
	this->physics = nullptr;
	this->modelSettings = nullptr;
	if (this->type == LevelEditorEntity_EntityType::PARTICLESYSTEM) {
		this->physics = new LevelEditorEntityPhysics();
	} else
	if (this->type == LevelEditorEntity_EntityType::MODEL) {
		if (model->getShaderModel() == ShaderModel::PBR) {
			shaderId = StringTools::startsWith(shaderId, "pbr-") == true || shaderId.empty() == true?shaderId:"pbr-" + shaderId;
			distanceShaderId = StringTools::startsWith(distanceShaderId, "pbr-") == true || distanceShaderId.empty() == true?distanceShaderId:"pbr-" + distanceShaderId;
		}
		this->modelSettings = new LevelEditorEntityModel(this);
		this->physics = new LevelEditorEntityPhysics();
	}
	renderGroups = false;
	distanceShaderDistance = 10000.0f;
	contributesShadows = true;
	receivesShadows = true;
}

LevelEditorEntity::~LevelEditorEntity() {
	if (model != nullptr) delete model;
	if (lodLevel2 != nullptr) delete lodLevel2;
	if (lodLevel3 != nullptr) delete lodLevel3;
	if (physics != nullptr) delete physics;
	for (auto particleSystem: particleSystems) delete particleSystem;
	if (modelSettings != nullptr) delete modelSettings;
	for (auto i = 0; i < boundingVolumes.size(); i++) delete boundingVolumes[i];
	for (auto sound: sounds) delete sound;
}

void LevelEditorEntity::setModel(Model* model) {
	if (this->model == model) return;
	if (this->model != nullptr) delete this->model;
	this->model = model;
}

bool LevelEditorEntity::addBoundingVolume(int32_t idx, LevelEditorEntityBoundingVolume* levelEditorEntityBoundingVolume)
{
	if (idx < 0)
		return false;

	if (idx > boundingVolumes.size())
		return false;

	if (idx == boundingVolumes.size()) {
		boundingVolumes.push_back(levelEditorEntityBoundingVolume);
	}
	return false;
}

void LevelEditorEntity::removeBoundingVolume(int32_t idx)
{
	delete boundingVolumes[idx];
	boundingVolumes.erase(boundingVolumes.begin() + idx);
}

void LevelEditorEntity::setDefaultBoundingVolumes()
{
	for (auto i = boundingVolumes.size(); i < MODEL_BOUNDINGVOLUME_COUNT; i++) {
		auto bv = new LevelEditorEntityBoundingVolume(i, this);
		addBoundingVolume(i, bv);
	}
}

void LevelEditorEntity::setLODLevel2(LevelEditorEntityLODLevel* lodLevel) {
	if (this->type != LevelEditorEntity_EntityType::MODEL) return;
	if (lodLevel2 != nullptr) delete lodLevel2;
	lodLevel2 = lodLevel;
}

void LevelEditorEntity::setLODLevel3(LevelEditorEntityLODLevel* lodLevel) {
	if (this->type != LevelEditorEntity_EntityType::MODEL) return;
	if (lodLevel3 != nullptr) delete lodLevel3;
	lodLevel3 = lodLevel;
}

LevelEditorEntityAudio* LevelEditorEntity::addSound(const string& id) {
	if (sounds.size() == MODEL_SOUNDS_COUNT) return nullptr;
	auto sound = getSound(id);
	if (sound != nullptr) return nullptr;
	auto audio = new LevelEditorEntityAudio(this, id);
	soundsById[id] = audio;
	sounds.push_back(audio);
	return audio;
}

