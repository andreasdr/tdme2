#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/model/fwd-tdme.h>

using tdme::engine::model::Color4;
using tdme::math::Vector3;

/**
 * Bounding box particle emitter
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::shared::model::LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter final
{
private:
	int32_t count;
	int64_t lifeTime;
	int64_t lifeTimeRnd;
	float mass;
	float massRnd;
	Vector3 velocity;
	Vector3 velocityRnd;
	Color4 colorStart;
	Color4 colorEnd;
	Vector3 obbCenter;
	Vector3 obbHalfextension;
	Vector3 obbAxis0;
	Vector3 obbAxis1;
	Vector3 obbAxis2;

public:
	/**
	 * Public constructor
	 */
	LevelEditorEntityParticleSystem_BoundingBoxParticleEmitter();

	/**
	 * @return count
	 */
	inline int32_t getCount() {
		return count;
	}

	/**
	 * Set count
	 * @param count count
	 */
	inline void setCount(int32_t count) {
		this->count = count;
	}

	/**
	 * @return life time
	 */
	inline int64_t getLifeTime() {
		return lifeTime;
	}

	/**
	 * Set life time
	 * @param lifeTime life time
	 */
	inline void setLifeTime(int64_t lifeTime) {
		this->lifeTime = lifeTime;
	}

	/**
	 * @return life time rnd
	 */
	inline int64_t getLifeTimeRnd() {
		return lifeTimeRnd;
	}

	/**
	 * Set life time rnd
	 * @param lifeTimeRnd life time rnd
	 */
	inline void setLifeTimeRnd(int64_t lifeTimeRnd) {
		this->lifeTimeRnd = lifeTimeRnd;
	}

	/**
	 * @return mass
	 */
	inline float getMass() {
		return mass;
	}

	/**
	 * Set mass
	 * @param mass mass
	 */
	inline void setMass(float mass) {
		this->mass = mass;
	}

	/**
	 * @return mass rnd
	 */
	inline float getMassRnd() {
		return massRnd;
	}

	/**
	 * Set mass rnd
	 * @param massRnd mass rnd
	 */
	inline void setMassRnd(float massRnd) {
		this->massRnd = massRnd;
	}

	/**
	 * @return velocity
	 */
	inline const Vector3& getVelocity() {
		return velocity;
	}

	/**
	 * Set velocity
	 * @param velocity velocity
	 */
	inline void setVelocity(const Vector3& velocity) {
		this->velocity = velocity;
	}

	/**
	 * @return velocity rnd
	 */
	inline const Vector3& getVelocityRnd() {
		return velocityRnd;
	}

	/**
	 * Set random velocity
	 * @param velocityRnd random velocity
	 */
	inline void setVelocityRnd(const Vector3& velocityRnd) {
		this->velocityRnd = velocityRnd;
	}

	/**
	 * @return color start
	 */
	inline const Color4& getColorStart() {
		return colorStart;
	}

	/**
	 * Set start color
	 * @param colorStart start color
	 */
	inline void setColorStart(const Color4& colorStart) {
		this->colorStart = colorStart;
	}

	/**
	 * @return color end
	 */
	inline const Color4& getColorEnd() {
		return colorEnd;
	}

	/**
	 * Set end color
	 * @param colorEnd color end
	 */
	inline void setColorEnd(const Color4& colorEnd) {
		this->colorEnd = colorEnd;
	}

	/**
	 * @return obb center
	 */
	inline const Vector3& getObbCenter() {
		return obbCenter;
	}

	/**
	 * Set obb center
	 * @param obbCenter obb center
	 */
	inline void setObbCenter(const Vector3& obbCenter) {
		this->obbCenter = obbCenter;
	}

	/**
	 * @return obb half extension
	 */
	inline const Vector3& getObbHalfextension() {
		return obbHalfextension;
	}

	/**
	 * Set obb half extension
	 * @param obbHalfExtension obb half extension
	 */
	inline void setObbHalfextension(const Vector3& obbHalfExtension) {
		this->obbHalfextension = obbHalfExtension;
	}

	/**
	 * @return obb axis 0
	 */
	inline const Vector3& getObbAxis0() {
		return obbAxis0;
	}

	/**
	 * Set obb axis 0
	 * @param obbAxis0 obb axis 0
	 */
	inline void setObbAxis0(const Vector3& obbAxis0) {
		this->obbAxis0 = obbAxis0;
	}

	/**
	 * @return obb axis 1
	 */
	inline const Vector3& getObbAxis1() {
		return obbAxis1;
	}

	/**
	 * Set obb axis 1
	 * @param obbAxis1 obb axis 1
	 */
	inline void setObbAxis1(const Vector3& obbAxis1) {
		this->obbAxis1 = obbAxis1;
	}

	/**
	 * @return obb axis 2
	 */
	inline const Vector3& getObbAxis2() {
		return obbAxis2;
	}

	/**
	 * Set obb axis 2
	 * @param obbAxis2 obb axis 2
	 */
	inline void setObbAxis2(const Vector3& obbAxis2) {
		this->obbAxis2 = obbAxis2;
	}

};
