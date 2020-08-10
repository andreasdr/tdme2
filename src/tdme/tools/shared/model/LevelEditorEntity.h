#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <map>

#include <tdme/tdme.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/model/Model.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/tools/shared/model/ModelProperties.h>

using std::map;
using std::string;
using std::vector;
using std::remove;

using tdme::engine::LODObject3D;
using tdme::engine::model::Model;
using tdme::math::Vector3;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorEntityAudio;
using tdme::tools::shared::model::LevelEditorEntityBoundingVolume;
using tdme::tools::shared::model::LevelEditorEntityModel;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityPhysics;
using tdme::tools::shared::model::ModelProperties;
using tdme::tools::shared::model::LevelEditorEntityLODLevel;

/**
 * Level Editor Model
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::LevelEditorEntity final
	: public ModelProperties
{
	friend class LevelEditorEntity_EntityType;

public:
	static constexpr int32_t ID_NONE { -1 };
	static constexpr int32_t MODEL_BOUNDINGVOLUME_COUNT { 32 };
	static constexpr int32_t MODEL_SOUNDS_COUNT { 32 };
	static char MODEL_BOUNDINGVOLUME_EDITING_ID[];
	static char MODEL_BOUNDINGVOLUMES_ID[];
	static char MODEL_BOUNDINGVOLUME_IDS[][MODEL_BOUNDINGVOLUME_COUNT];

private:
	int32_t id;
	LevelEditorEntity_EntityType* type { nullptr };
	string name;
	string description;
	string entityFileName;
	string fileName;
	string thumbnail;
	Model* model { nullptr };
	Vector3 pivot;
	LevelEditorEntityLODLevel* lodLevel2 { nullptr };
	LevelEditorEntityLODLevel* lodLevel3 { nullptr };
	vector<LevelEditorEntityBoundingVolume*> boundingVolumes;
	LevelEditorEntityPhysics* physics { nullptr };
	vector<LevelEditorEntityParticleSystem*> particleSystems;
	LevelEditorEntityModel* modelSettings { nullptr };
	bool renderGroups;
	string shaderId { "default"};
	string distanceShaderId { "default"};
	float distanceShaderDistance;
	bool contributesShadows;
	bool receivesShadows;
	map<string, LevelEditorEntityAudio*> soundsById;
	vector<LevelEditorEntityAudio*> sounds;
	map<string, string> shaderParameters;
	map<string, string> distanceShaderParameters;

public:

	/**
	 * Creates a level editor model
	 * @param id id
	 * @param entityType entity type
	 * @param name name
	 * @param description description
	 * @param entityFileName entity file name
	 * @param fileName file name
	 * @param thumbnail thumbnail
	 * @param model model
	 * @param pivot pivot
	 */
	LevelEditorEntity(int32_t id, LevelEditorEntity_EntityType* entityType, const string& name, const string& description, const string& entityFileName, const string& fileName, const string& thumbnail, Model* model, const Vector3& pivot);

	/**
	 * Destructor
	 */
	virtual ~LevelEditorEntity();

	/**
	 * @return id
	 */
	inline int32_t getId() {
		return id;
	}

	/**
	 * @return entity type
	 */
	inline LevelEditorEntity_EntityType* getType() {
		return type;
	}

	/**
	 * @return name
	 */
	inline const string& getName() {
		return name;
	}

	/**
	 * Set up model name
	 * @param name name
	 */
	inline void setName(const string& name) {
		this->name = name;
	}

	/**
	 * @return description
	 */
	inline const string& getDescription() {
		return description;
	}

	/**
	 * Set up model description
	 * @param description description
	 */
	inline void setDescription(const string& description) {
		this->description = description;
	}

	/**
	 * @return entity file name
	 */
	inline const string& getEntityFileName() {
		return entityFileName;
	}

	/**
	 * Set entity file name
	 * @param entityFileName entity file name
	 */
	inline void setEntityFileName(const string& entityFileName) {
		this->entityFileName = entityFileName;
	}

	/**
	 * @return file name
	 */
	inline const string& getFileName() {
		return fileName;
	}

	/**
	 * Set file name
	 * @param fileName file name
	 */
	inline void setFileName(const string& fileName) {
		this->fileName = fileName;
	}

	/**
	 * @return thumbnail
	 */
	inline const string& getThumbnail() {
		return thumbnail;
	}

	/**
	 * @return model
	 */
	inline Model* getModel() {
		return model;
	}

	/**
	 * Set model
	 * @param model model
	 */
	void setModel(Model* model);

	/**
	 * @return pivot
	 */
	inline Vector3& getPivot() {
		return pivot;
	}

	/**
	 * @return bounding volume count
	 */
	inline int32_t getBoundingVolumeCount() {
		return boundingVolumes.size();
	}

	/**
	 * Get bounding volume at
	 * @param idx idx
	 * @return level editor object bounding volume
	 */
	inline LevelEditorEntityBoundingVolume* getBoundingVolumeAt(int32_t idx) {
		return idx >= 0 && idx < boundingVolumes.size()?boundingVolumes[idx]:nullptr;
	}

	/**
	 * Add bounding volume
	 * @param idx idx
	 * @param levelEditorEntityBoundingVolume level editor entity bounding volume
	 * @return level editor bounding volume
	 */
	bool addBoundingVolume(int32_t idx, LevelEditorEntityBoundingVolume* levelEditorEntityBoundingVolume);

	/**
	 * Set default (up to 24) bounding volumes, to be used with LevelEditor
	 */
	void setDefaultBoundingVolumes();

	/**
	 * @return physics
	 */
	inline LevelEditorEntityPhysics* getPhysics() {
		return physics;
	}

	/**
	 * @return lod level 2
	 */
	inline LevelEditorEntityLODLevel* getLODLevel2() {
		return lodLevel2;
	}

	/**
	 * Set LOD level 2
	 * @param lodLevel lod level settings
	 */
	void setLODLevel2(LevelEditorEntityLODLevel* lodLevel);

	/**
	 * @return lod level 3
	 */
	inline LevelEditorEntityLODLevel* getLODLevel3() {
		return lodLevel3;
	}

	/**
	 * Set LOD level 3
	 * @param lodLevel lod level settings
	 */
	void setLODLevel3(LevelEditorEntityLODLevel* lodLevel);

	/**
	 * @return particle systems count
	 */
	inline int32_t getParticleSystemsCount() {
		return particleSystems.size();
	}

	/**
	 * Add particle system
	 */
	inline void addParticleSystem() {
		particleSystems.push_back(new LevelEditorEntityParticleSystem());
	}

	/**
	 * Remove particle system from given index
	 * @param idx particle system index
	 * @return success
	 */
	inline bool removeParticleSystemAt(int idx) {
		if (idx < 0 || idx >= particleSystems.size()) return false;
		auto particleSystem = particleSystems[idx];
		particleSystems.erase(remove(particleSystems.begin(), particleSystems.end(), particleSystem), particleSystems.end());
		delete particleSystem;
		return true;
	}

	/**
	 * Get particle system at given index
	 * @param idx particle system index
	 * @return level editor entity particle system
	 */
	inline LevelEditorEntityParticleSystem* getParticleSystemAt(int idx) {
		return particleSystems[idx];
	}

	/**
	 * @return model settings
	 */
	inline LevelEditorEntityModel* getModelSettings() {
		return modelSettings;
	}

	/**
	 * @return if entity contributes to shadows
	 */
	inline bool isContributesShadows() {
		return contributesShadows;
	}

	/**
	 * Enable/disable contributes shadows
	 * @param contributesShadows contributes shadows
	 */
	inline void setContributesShadows(bool contributesShadows) {
		this->contributesShadows = contributesShadows;
	}

	/**
	 * @return if entity receives shadows
	 */
	inline bool isReceivesShadows() {
		return receivesShadows;
	}

	/**
	 * Enable/disable receives shadows
	 * @param receivesShadows receives shadows
	 */
	inline void setReceivesShadows(bool receivesShadows) {
		this->receivesShadows = receivesShadows;
	}

	/**
	 * Is using render groups
	 * @return render groups enabled
	 */
	inline bool isRenderGroups() {
		return renderGroups;
	}

	/**
	 * Set using render groups
	 * @param renderGroups use render groups
	 */
	inline void setRenderGroups(bool renderGroups) {
		this->renderGroups = renderGroups;
	}

	/**
	 * Get shader
	 * @return shader id
	 */
	inline const string& getShader() {
		return shaderId;
	}

	/**
	 * Set shader
	 * @param id shader id
	 */
	inline void setShader(const string& id) {
		this->shaderId = id;
	}

	/**
	 * Get distance shader
	 * @return shader id
	 */
	inline const string& getDistanceShader() {
		return distanceShaderId;
	}

	/**
	 * Set distance shader
	 * @param id shader id
	 */
	inline void setDistanceShader(const string& id) {
		this->distanceShaderId = id;
	}

	/**
	 * Get distance shader distance
	 * @return shader id
	 */
	inline float getDistanceShaderDistance() {
		return distanceShaderDistance;
	}

	/**
	 * Set distance shader distance
	 * @param distance distance
	 */
	inline void setDistanceShaderDistance(float distance) {
		this->distanceShaderDistance = distance;
	}

	/**
	 * @return sounds list
	 */
	inline const vector<LevelEditorEntityAudio*>& getSounds() {
		return sounds;
	}

	/**
	 * Returns sound of given sound id
	 * @param id id
	 * @return sound with given id
	 */
	inline LevelEditorEntityAudio* getSound(const string& id) {
		auto soundIt = soundsById.find(id);
		if (soundIt == soundsById.end()) return nullptr;
		return soundIt->second;
	}

	/**
	 * Remove sound of given sound id
	 * @param id id
	 * @return sound with given id
	 */
	inline void removeSound(const string& id) {
		auto soundIt = soundsById.find(id);
		if (soundIt == soundsById.end()) return;
		sounds.erase(remove(sounds.begin(), sounds.end(), soundIt->second), sounds.end());
		soundsById.erase(soundIt);
	}

	/**
	 * Returns sound of given sound id
	 * @param id id
	 * @return success
	 */
	inline bool renameSound(const string& id, const string& newId) {
		auto soundIt = soundsById.find(id);
		if (soundIt == soundsById.end()) return false;
		auto sound = soundIt->second;
		soundsById.erase(soundIt);
		soundsById[newId] = sound;
		return true;
	}

	/**
	 * Set up sound
	 * @param id id
	 * @param audio audio
	 * @return success
	 */
	LevelEditorEntityAudio* addSound(const string& id);

	/**
	 * Get shader parameters
	 * @return shader parameters
	 */
	inline const map<string, string>& getShaderParameters(const map<string, string>& parameters) {
		return shaderParameters;
	}

	/**
	 * Set shader parameters
	 * @param parameters shader parameters
	 */
	inline void setShaderParameters(const map<string, string>& parameters) {
		shaderParameters = parameters;
	}

	/**
	 * Get distance shader parameters
	 * @return shader parameters
	 */
	inline const map<string, string>& getDistanceShaderParameters(const map<string, string>& parameters) {
		return distanceShaderParameters;
	}

	/**
	 * Set distance shader parameters
	 * @param parameters distance shader parameters
	 */
	inline void setDistanceShaderParameters(const map<string, string>& parameters) {
		distanceShaderParameters = parameters;
	}

};
