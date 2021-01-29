#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/prototype/PrototypeParticleSystem.h>
#include <tdme/engine/prototype/PrototypeProperties.h>
#include <tdme/engine/Entity.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::remove;
using std::string;
using std::vector;

using tdme::engine::model::Model;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::prototype::PrototypeAudio;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::prototype::PrototypeLODLevel;
using tdme::engine::prototype::PrototypeParticleSystem;
using tdme::engine::prototype::PrototypePhysics;
using tdme::engine::prototype::PrototypeProperties;
using tdme::engine::prototype::PrototypeTerrain;
using tdme::engine::Entity;
using tdme::math::Vector3;

/**
 * Prototype definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::prototype::Prototype final
	: public PrototypeProperties
{
	friend class Prototype_Type;

public:
	static constexpr int ID_NONE { -1 };
	static constexpr int MODEL_BOUNDINGVOLUME_COUNT { 64 };
	static constexpr int MODEL_SOUNDS_COUNT { 32 };
	static char MODEL_BOUNDINGVOLUME_EDITING_ID[];
	static char MODEL_BOUNDINGVOLUMES_ID[];
	static char MODEL_BOUNDINGVOLUME_IDS[][MODEL_BOUNDINGVOLUME_COUNT];

private:
	int id;
	Prototype_Type* type { nullptr };
	string name;
	string description;
	string fileName;
	string modelFileName;
	string thumbnailFileName;
	Model* model { nullptr };
	Vector3 pivot;
	PrototypeLODLevel* lodLevel2 { nullptr };
	PrototypeLODLevel* lodLevel3 { nullptr };
	vector<PrototypeBoundingVolume*> boundingVolumes;
	PrototypePhysics* physics { nullptr };
	vector<PrototypeParticleSystem*> particleSystems;
	bool terrainMesh { false };
	bool renderGroups { false };
	string shaderId { "default"};
	string distanceShaderId { "default"};
	float distanceShaderDistance { 10000.0f };
	bool contributesShadows { true };
	bool receivesShadows { true };
	map<string, PrototypeAudio*> soundsById;
	vector<PrototypeAudio*> sounds;
	map<string, string> shaderParameters;
	map<string, string> distanceShaderParameters;
	int32_t environmentMapRenderPassMask { Entity::RENDERPASS_ALL };
	int64_t environmentMapTimeRenderUpdateFrequency { -1LL };
	PrototypeTerrain* terrain { nullptr };

public:

	/**
	 * Creates a prototype
	 * @param id id
	 * @param prototypType prototype type
	 * @param name name
	 * @param description description
	 * @param fileName file name
	 * @param modelFileName model file name
	 * @param thumbnail thumbnail
	 * @param model model
	 * @param pivot pivot
	 */
	Prototype(int id, Prototype_Type* prototypeType, const string& name, const string& description, const string& fileName, const string& modelFileName, const string& thumbnail, Model* model, const Vector3& pivot);

	/**
	 * Destructor
	 */
	virtual ~Prototype();

	/**
	 * @return id
	 */
	inline int getId() {
		return id;
	}

	/**
	 * @return type
	 */
	inline Prototype_Type* getType() {
		return type;
	}

	/**
	 * @return name
	 */
	inline const string& getName() {
		return name;
	}

	/**
	 * Set up name
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
	 * Set up description
	 * @param description description
	 */
	inline void setDescription(const string& description) {
		this->description = description;
	}

	/**
	 * @return prototype file name
	 */
	inline const string& getFileName() {
		return fileName;
	}

	/**
	 * Set prototype file name
	 * @param fileName prototype file name
	 */
	inline void setFileName(const string& fileName) {
		this->fileName = fileName;
	}

	/**
	 * @return model file name
	 */
	inline const string& getModelFileName() {
		return modelFileName;
	}

	/**
	 * Set model file name
	 * @param fileName file name
	 */
	inline void setModelFileName(const string& fileName) {
		this->modelFileName = fileName;
	}

	/**
	 * @return thumbnail file name
	 */
	inline const string& getThumbnailFileName() {
		return thumbnailFileName;
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
	 * Unset model without deleting current one
	 * @return model
	 */
	inline Model* unsetModel() {
		auto currentModel = model;
		model = nullptr;
		return currentModel;
	}

	/**
	 * @return pivot
	 */
	inline Vector3& getPivot() {
		return pivot;
	}

	/**
	 * @return bounding volume count
	 */
	inline int getBoundingVolumeCount() {
		return boundingVolumes.size();
	}

	/**
	 * Get bounding volume at given index
	 * @param idx index
	 * @return prototype bounding volume
	 */
	inline PrototypeBoundingVolume* getBoundingVolume(int idx) {
		return idx >= 0 && idx < boundingVolumes.size()?boundingVolumes[idx]:nullptr;
	}

	/**
	 * Add bounding volume at given index
	 * @param idx index
	 * @param prototypeBoundingVolume prototype bounding volume
	 * @return scene editor bounding volume
	 */
	bool addBoundingVolume(int idx, PrototypeBoundingVolume* prototypeBoundingVolume);

	/**
	 * Remove bounding volume at given index
	 * @param idx index
	 */
	void removeBoundingVolume(int idx);

	/**
	 * Set default bounding volumes(to be used with SceneEditor)
	 * @param maxBoundingVolumeCount maximum number of editable bounding volumes or -1 for default
	 */
	void setDefaultBoundingVolumes(int maxBoundingVolumeCount = -1);

	/**
	 * @return physics
	 */
	inline PrototypePhysics* getPhysics() {
		return physics;
	}

	/**
	 * @return lod level 2
	 */
	inline PrototypeLODLevel* getLODLevel2() {
		return lodLevel2;
	}

	/**
	 * Set LOD level 2
	 * @param lodLevel lod level settings
	 */
	void setLODLevel2(PrototypeLODLevel* lodLevel);

	/**
	 * @return lod level 3
	 */
	inline PrototypeLODLevel* getLODLevel3() {
		return lodLevel3;
	}

	/**
	 * Set LOD level 3
	 * @param lodLevel lod level settings
	 */
	void setLODLevel3(PrototypeLODLevel* lodLevel);

	/**
	 * @return particle systems count
	 */
	inline int getParticleSystemsCount() {
		return particleSystems.size();
	}

	/**
	 * Add particle system
	 */
	inline void addParticleSystem() {
		particleSystems.push_back(new PrototypeParticleSystem());
	}

	/**
	 * Remove particle system at given index
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
	 * @return prototype particle system
	 */
	inline PrototypeParticleSystem* getParticleSystemAt(int idx) {
		return particleSystems[idx];
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
	 * Is terrain mesh
	 * @return terrain mesh
	 */
	inline bool isTerrainMesh() {
		return terrainMesh;
	}

	/**
	 * Set terrain mesh
	 * @param terrainMesh terrain mesh
	 */
	inline void setTerrainMesh(bool terrainMesh) {
		this->terrainMesh = terrainMesh;
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
	 * @return distance shader distance
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
	inline const vector<PrototypeAudio*>& getSounds() {
		return sounds;
	}

	/**
	 * Returns sound of given sound id
	 * @param id id
	 * @return sound
	 */
	inline PrototypeAudio* getSound(const string& id) {
		auto soundIt = soundsById.find(id);
		if (soundIt == soundsById.end()) return nullptr;
		return soundIt->second;
	}

	/**
	 * Remove sound of given sound id
	 * @param id id
	 */
	inline void removeSound(const string& id) {
		auto soundIt = soundsById.find(id);
		if (soundIt == soundsById.end()) return;
		sounds.erase(remove(sounds.begin(), sounds.end(), soundIt->second), sounds.end());
		soundsById.erase(soundIt);
	}

	/**
	 * Renames sound of given sound id with new id
	 * @param id existing id
	 * @param id new id
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
	 * Add sound with given id
	 * @param id id
	 * @return prototype audio
	 */
	PrototypeAudio* addSound(const string& id);

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

	/**
	 * @return environment map render pass mask
	 */
	inline int getEnvironmentMapRenderPassMask() {
		return environmentMapRenderPassMask;
	}

	/**
	 * Set up environment render pass mask
	 * @param renderPassMask render pass mask
	 */
	inline void setEnvironmentMapRenderPassMask(int renderPassMask) {
		this->environmentMapRenderPassMask = renderPassMask;
	}

	/**
	 * @return environment map render update time frequency in milliseconds
	 */
	inline int64_t getEnvironmentMapTimeRenderUpdateFrequency() {
		return environmentMapTimeRenderUpdateFrequency;
	}

	/**
	 * Set up environment map render update time frequency
	 * @param frequency frequency in milliseconds
	 */
	inline void setEnvironmentMapTimeRenderUpdateFrequency(int64_t frequency) {
		environmentMapTimeRenderUpdateFrequency = frequency;
	}

	/**
	 * @return terrain
	 */
	inline PrototypeTerrain* getTerrain() {
		return terrain;
	}

};
