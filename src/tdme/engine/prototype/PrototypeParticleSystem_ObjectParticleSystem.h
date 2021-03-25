#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using std::string;

using tdme::engine::model::Model;
using tdme::engine::prototype::PrototypeParticleSystem;
using tdme::engine::prototype::PrototypeParticleSystem_BoundingBoxParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_CircleParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity;
using tdme::engine::prototype::PrototypeParticleSystem_Emitter;
using tdme::engine::prototype::PrototypeParticleSystem_PointParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_PointParticleSystem;
using tdme::engine::prototype::PrototypeParticleSystem_SphereParticleEmitter;
using tdme::engine::prototype::PrototypeParticleSystem_Type;
using tdme::math::Vector3;

/**
 * Prototype object particle system definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::prototype::PrototypeParticleSystem_ObjectParticleSystem final
{
private:
	Vector3 scale;
	int maxCount;
	bool autoEmit;
	Model* model { nullptr };
	string modelFileName;

public:
	/**
	 * Public constructor
	 */
	inline PrototypeParticleSystem_ObjectParticleSystem() {
		scale.set(1.0f, 1.0f, 1.0f);
		maxCount = 10;
		autoEmit = true;
		model = nullptr;
		modelFileName = "";
	}

	/**
	 * Destructor
	 */
	~PrototypeParticleSystem_ObjectParticleSystem();

	/**
	 * @return scale
	 */
	inline Vector3& getScale() {
		return scale;
	}

	/**
	 * @return max count
	 */
	inline int getMaxCount() {
		return maxCount;
	}

	/**
	 * Set max count
	 * @param maxCount max count
	 */
	inline void setMaxCount(int maxCount) {
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
	void setModelFile(const string& modelFileName);

};
