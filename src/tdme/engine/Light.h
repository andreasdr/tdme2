#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>

using tdme::engine::model::Color4;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Vector3;
using tdme::math::Vector4;

/**
 * Light representation
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::Light final
{
private:
	int32_t id;
	bool enabled;
	Color4 ambient;
	Color4 diffuse;
	Color4 specular;
	Vector4 position;
	Vector3 spotDirection;
	float spotExponent;
	float spotCutOff;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	Renderer* renderer { nullptr };
public:
	/**
	 * Public default constructor
	 */
	Light();

	/**
	 * Public default constructor
	 * @param renderer renderer
	 * @param id id
	 */
	Light(Renderer* renderer, int32_t id);

	/**
	 * @return light id
	 */
	inline int32_t getId() const {
		return id;
	}

	/**
	 * @return enabled
	 */
	inline bool isEnabled() const {
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
	 * @return ambient light component
	 */
	inline const Color4& getAmbient() const {
		return ambient;
	}

	/**
	 * Set ambient light component
	 * @param ambient ambient light component
	 */
	inline void setAmbient(const Color4& ambient) {
		this->ambient = ambient;
	}

	/**
	 * @return diffuse light component
	 */
	inline const Color4& getDiffuse() const {
		return diffuse;
	}

	/**
	 * Set diffuse light component
	 * @param diffuse diffuse light
	 */
	inline void setDiffuse(const Color4& diffuse) {
		this->diffuse = diffuse;
	}

	/**
	 * @return specular light component
	 */
	inline const Color4& getSpecular() const {
		return specular;
	}

	/**
	 * Set specular light component
	 * @param specular specular light
	 */
	inline void setSpecular(const Color4& specular) {
		this->specular = specular;
	}

	/**
	 * @return position of light
	 */
	inline const Vector4& getPosition() const {
		return position;
	}

	/**
	 * Set light position
	 * @param position position of light
	 */
	inline void setPosition(const Vector4& position) {
		this->position = position;
	}

	/**
	 * @return spot direction
	 */
	inline const Vector3& getSpotDirection() const {
		return spotDirection;
	}

	/**
	 * Set spot direction
	 * @param spotDirection spot direction
	 */
	inline void setSpotDirection(const Vector3& spotDirection) {
		this->spotDirection = spotDirection;
	}

	/**
	 * @return spot exponent
	 */
	inline float getSpotExponent() const {
		return spotExponent;
	}

	/**
	 * Set up spot exponent
	 * @param spotExponent spot exponent
	 */
	inline void setSpotExponent(float spotExponent) {
		this->spotExponent = spotExponent;
	}

	/**
	 * @return spot cutoff
	 */
	inline float getSpotCutOff() const {
		return spotCutOff;
	}

	/**
	 * Set spot cut off
	 * @param spotCutOff spot cut off
	 */
	inline void setSpotCutOff(float spotCutOff) {
		this->spotCutOff = spotCutOff;
	}

	/**
	 * @return constant attenuation
	 */
	inline float getConstantAttenuation() const {
		return constantAttenuation;
	}

	/**
	 * Set up constant attenuation
	 * @param constantAttenuation constant attenuation
	 */
	inline void setConstantAttenuation(float constantAttenuation) {
		this->constantAttenuation = constantAttenuation;
	}

	/**
	 * @return linear attenuation
	 */
	inline float getLinearAttenuation() const {
		return linearAttenuation;
	}

	/**
	 * Set up linear attenuation
	 * @param linearAttenuation linear attenuation
	 */
	inline void setLinearAttenuation(float linearAttenuation) {
		this->linearAttenuation = linearAttenuation;
	}

	/**
	 * @return quadratic attenuation
	 */
	inline float getQuadraticAttenuation() const {
		return quadraticAttenuation;
	}

	/**
	 * Set up quadratic attenuation
	 * @param quadraticAttenuation quadraticAttenuation
	 */
	inline void setQuadraticAttenuation(float quadraticAttenuation) {
		this->quadraticAttenuation = quadraticAttenuation;
	}

	/**
	 * Update light
	 * @param context context
	 */
	void update(void* context);

};
