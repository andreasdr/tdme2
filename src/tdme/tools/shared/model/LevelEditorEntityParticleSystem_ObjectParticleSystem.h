#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/model/fwd-tdme.h>

using std::string;

using tdme::engine::model::Model;
using tdme::math::Vector3;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_CircleParticleEmitterPlaneVelocity;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Emitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_PointParticleSystem;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_SphereParticleEmitter;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem_Type;
using tdme::tools::shared::model::LevelEditorEntityParticleSystem;

/**
 * Object particle system
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::LevelEditorEntityParticleSystem_ObjectParticleSystem final
{
private:
	Vector3 scale;
	int32_t maxCount;
	bool autoEmit;
	Model* model { nullptr };
	string modelFileName;

public:
	/**
	 * Public constructor
	 */
	inline LevelEditorEntityParticleSystem_ObjectParticleSystem() {
		scale.set(1.0f, 1.0f, 1.0f);
		maxCount = 10;
		autoEmit = true;
		model = nullptr;
		modelFileName = "";
	}

	/**
	 * Destructor
	 */
	~LevelEditorEntityParticleSystem_ObjectParticleSystem();

	/**
	 * @return scale
	 */
	inline Vector3& getScale() {
		return scale;
	}

	/**
	 * @return max count
	 */
	inline int32_t getMaxCount() {
		return maxCount;
	}

	/**
	 * Set max count
	 * @param maxCount max count
	 */
	inline void setMaxCount(int32_t maxCount) {
		this->maxCount = maxCount;
	}

	/**
	 * @return is auto emit
	 */
	inline bool isAutoEmit() {
		return autoEmit;
	}

	/**
	 * Set auto emit
	 * @param autoEmit autoEmit
	 */
	inline void setAutoEmit(bool autoEmit) {
		this->autoEmit = autoEmit;
	}

	/**
	 * @return model
	 */
	inline Model* getModel() {
		return model;
	}

	/**
	 * @return model file
	 */
	inline const string& getModelFile() {
		return modelFileName;
	}

	/**
	 * Set model file
	 * @param modelFileName model file name
	 */
	void setModelFile(const string& modelFileName) /* throws(Exception) */;

};
