#pragma once

#include <memory>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using std::string;
using std::unique_ptr;

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
 */
class tdme::engine::prototype::PrototypeParticleSystem_ObjectParticleSystem final
{
private:
	Vector3 scale;
	int maxCount;
	bool autoEmit;
	unique_ptr<Model> model;
	string modelFileName;

public:
	// forbid class copy
	FORBID_CLASS_COPY(PrototypeParticleSystem_ObjectParticleSystem)

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
	 * @returns scale
	 */
	inline const Vector3& getScale() {
		return scale;
	}

	/**
	 * Set scale
	 * @param scale scale
	 */
	inline void setScale(const Vector3& scale) {
		this->scale = scale;
	}

	/**
	 * @returns max count
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
	 * @returns is auto emit
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
	 * @returns model
	 */
	inline Model* getModel() {
		return model.get();
	}

	/**
	 * @returns model file
	 */
	inline const string& getModelFileName() {
		return modelFileName;
	}

	/**
	 * Set model file
	 * @param modelFileName model file name
	 * @param useBC7TextureCompression use BC7 texture compression
	 */
	void setModelFile(const string& modelFileName, bool useBC7TextureCompression = true);

};
