#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/scene/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>

using tdme::engine::model::Color4;
using tdme::math::Vector3;
using tdme::math::Vector4;

/**
 * Scene light definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::scene::SceneLight final
{
private:
	int id;
	bool enabled;
	Color4 ambient;
	Color4 diffuse;
	Color4 specular;
	Vector4 position;
	Vector3 spotTo;
	Vector3 spotDirection;
	float spotExponent;
	float spotCutOff;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

public:
	/**
	 * Public default constructor
	 * @param id id
	 */
	SceneLight(int id);

	/**
	 * @return light id
	 */
	inline int getId() {
		return id;
	}

	/**
	 * @return enabled
	 */
	inline bool isEnabled() {
		return enabled;
	}

	/**
	 * Set enabled
	 * @param enabled enabled
	 */
	inline void setEnabled(bool enabled) {
		this->enabled = enabled;
	}

	/**
	 * @return ambient light
	 */
	inline Color4& getAmbient() {
		return ambient;
	}

	/**
	 * Diffuse light
	 * @return diffuse light
	 */
	inline Color4& getDiffuse() {
		return diffuse;
	}

	/**
	 * @return specular light
	 */
	inline Color4& getSpecular() {
		return specular;
	}

	/**
	 * @return position of light
	 */
	inline Vector4& getPosition() {
		return position;
	}

	/**
	 * @return spot to
	 */
	inline Vector3& getSpotTo() {
		return spotTo;
	}

	/**
	 * @return spot direction
	 */
	inline Vector3& getSpotDirection() {
		return spotDirection;
	}

	/**
	 * @return spot exponent
	 */
	inline float getSpotExponent() {
		return spotExponent;
	}

	/**
	 * set up spot exponent
	 * @param spotExponent spot exponent
	 */
	inline void setSpotExponent(float spotExponent) {
		this->spotExponent = spotExponent;
	}

	/**
	 * @return spot cutoff
	 */
	inline float getSpotCutOff() {
		return spotCutOff;
	}

	/**
	 * set spot cut off
	 * @param spotCutOff spot cut off
	 */
	inline void setSpotCutOff(float spotCutOff) {
		this->spotCutOff = spotCutOff;
	}

	/**
	 * @return constant attenuation
	 */
	inline float getConstantAttenuation() {
		return constantAttenuation;
	}

	/**
	 * set up constant attenuation
	 * @param constantAttenuation constant attenuation
	 */
	inline void setConstantAttenuation(float constantAttenuation) {
		this->constantAttenuation = constantAttenuation;
	}

	/**
	 * @return linear attenuation
	 */
	inline float getLinearAttenuation() {
		return linearAttenuation;
	}

	/**
	 * set up linear attenuation
	 * @param linarAttenuation linarAttenuation
	 */
	inline void setLinearAttenuation(float linearAttenuation) {
		this->linearAttenuation = linearAttenuation;
	}

	/**
	 * @return quadratic attenuation
	 */
	inline float getQuadraticAttenuation() {
		return quadraticAttenuation;
	}

	/**
	 * Set up quadratic attenuation
	 * @param quadraticAttenuation quadraticAttenuation
	 */
	inline void setQuadraticAttenuation(float quadraticAttenuation) {
		this->quadraticAttenuation = quadraticAttenuation;
	}

};
