#pragma once

#include <vector>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>
#include <tdme/engine/subsystems/particlesystem/Particle.h>
#include <tdme/engine/subsystems/particlesystem/ParticleEmitter.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/subsystems/particlesystem/ParticleSystemEntityInternal.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/fwd-tdme.h>

using std::vector;
using std::string;

using tdme::engine::Transformations;
using tdme::engine::subsystems::particlesystem::ParticleSystemEntityInternal;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Object3D;
using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DInternal;
using tdme::engine::subsystems::particlesystem::Particle;
using tdme::engine::subsystems::particlesystem::ParticleEmitter;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Particle system which displays objects as particles
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::particlesystem::ObjectParticleSystemInternal
	: public Transformations
	, public virtual ParticleSystemEntityInternal
{
protected:
	Engine* engine { nullptr };
	Renderer* renderer { nullptr };
	string id;
	bool enabled;
	Model* model { nullptr };
	Vector3 objectScale;
	bool autoEmit;
	bool contributesShadows;
	bool receivesShadows;
	vector<Particle> particles;
	vector<Object3D*> objects;
	vector<Object3D*> enabledObjects;
	BoundingBox boundingBox;
	BoundingBox boundingBoxTransformed;
	Transformations inverseTransformation;
	ParticleEmitter* emitter { nullptr };
	bool pickable;
	Color4 effectColorMul;
	Color4 effectColorAdd;
	float particlesToSpawnRemainder;
	Transformations localTransformations;

	/**
	 * Update internal
	 */
	inline void updateInternal() {
		Vector3 scale;
		getTransformationsMatrix().getScale(scale);
		scale.scale(objectScale);
		scale.scale(localTransformations.getScale());
		for (auto object: objects) {
			object->setScale(scale);
			object->update();
		}
		inverseTransformation.fromTransformations(*this);
		inverseTransformation.invert();
	}

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param model model
	 * @param scale scale
	 * @param autoEmit auto emit
	 * @param contributesShadows enable contributes shadows
	 * @param receivesShadows enable receives shadows
	 * @param maxCount maxCount
	 * @param emitter emitter
	 */
	ObjectParticleSystemInternal(const string& id, Model* model, const Vector3& scale, bool autoEmit, bool contributesShadows, bool receivesShadows, int32_t maxCount, ParticleEmitter* emitter);

	/**
	 * Destructor
	 */
	virtual ~ObjectParticleSystemInternal();

	// overridden methods
	inline ParticleEmitter* getEmitter() override {
		return emitter;
	}

	const string& getId() override;
	void setEngine(Engine* engine);
	void setRenderer(Renderer* renderer);
	inline bool isEnabled() override {
		return enabled;
	}
	inline bool isActive() override {
		return enabledObjects.size() > 0;
	}
	inline void setEnabled(bool enabled) override {
		this->enabled = enabled;
	}
	inline const Color4& getEffectColorMul() const override {
		return effectColorMul;
	}
	inline void setEffectColorMul(const Color4& effectColorMul) override {
		this->effectColorMul = effectColorMul;
	}
	inline const Color4& getEffectColorAdd() const override {
		return effectColorAdd;
	}
	inline void setEffectColorAdd(const Color4& effectColorAdd) override {
		this->effectColorAdd = effectColorAdd;
	}
	inline bool isPickable() override {
		return pickable;
	}
	inline void setPickable(bool pickable) override {
		this->pickable = pickable;
	}
	inline bool isAutoEmit() override {
		return autoEmit;
	}
	inline void setAutoEmit(bool autoEmit) override {
		this->autoEmit = autoEmit;
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
		for (auto i = 0; i < objects.size(); i++) objects[i]->setContributesShadows(contributesShadows);
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
		for (auto i = 0; i < objects.size(); i++) objects[i]->setReceivesShadows(receivesShadows);
	}

	/**
	 * Update transformations
	 */
	void update() override;
	void fromTransformations(const Transformations& transformations) override;
	int32_t emitParticles() override;
	void updateParticles() override;
	void dispose();

	inline const Transformations& getLocalTransformations() override {
		return localTransformations;
	}
	inline void setLocalTransformations(const Transformations& transformations) override {
		this->localTransformations = transformations;
		updateInternal();
	}

};
