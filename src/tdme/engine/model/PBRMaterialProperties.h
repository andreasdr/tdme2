#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>


using std::string;

using tdme::engine::fileio::textures::Texture;
using tdme::engine::model::Color4;

/**
 * Represents specular material properties
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::model::PBRMaterialProperties final
{

private:
	Color4 baseColorFactor;
	string baseColorTexturePathName;
	string baseColorTextureFileName;
	Texture* baseColorTexture;
	bool baseColorTextureMaskedTransparency;
	float baseColorTextureMaskedTransparencyThreshold;
	bool baseColorTextureTransparency;
	float metallicFactor;
	float roughnessFactor;
	string metallicRoughnessTexturePathName;
	string metallicRoughnessTextureFileName;
	Texture* metallicRoughnessTexture;
	float normalScale;
	string normalTexturePathName;
	string normalTextureFileName;
	Texture* normalTexture;
	float exposure;

	/**
	 * Checks and set ups base color texture transparency
	 */
	void checkBaseColorTextureTransparency();

public:
	/**
	 * Public constructor
	 */
	PBRMaterialProperties();

	/**
	 * Destructor
	 */
	~PBRMaterialProperties();

	/**
	 * @return base color factor
	 */
	inline const Color4& getBaseColorFactor() const {
		return baseColorFactor;
	}

	/**
	 * Set base color factor
	 * @param baseColorFactor base color factor
	 */
	inline void setBaseColorFactor(const Color4& baseColorFactor) {
		this->baseColorFactor = baseColorFactor;
	}

	/**
	 * @return base color texture path name
	 */
	inline const string& getBaseColorTexturePathName() const {
		return baseColorTexturePathName;
	}

	/**
	 * @return base color texture file name
	 */
	inline const string& getBaseColorTextureFileName() const {
		return baseColorTextureFileName;
	}

	/**
	 * Set up a base color texture
	 * @param pathName path name
	 * @param fileName file name
	 */
	void setBaseColorTexture(const string& pathName, const string& fileName);

	/**
	 * @return if material has a base color texture
	 */
	inline bool hasBaseColorTexture() const {
		return baseColorTexture != nullptr;
	}

	/**
	 * @return the material's base color texture
	 */
	inline Texture* getBaseColorTexture() const {
		return baseColorTexture;
	}

	/**
	 * @return base color texture has transparent pixels
	 */
	inline bool hasBaseColorTextureTransparency() const {
		return baseColorTextureTransparency;
	}

	/**
	 * @return base color texture has masked transparency, means pixel are eighter opaque or not visible
	 */
	inline bool hasBaseColorTextureMaskedTransparency() const {
		return baseColorTextureMaskedTransparency;
	}

	/**
	 * Set if base color texture uses masked transparency(means pixel are eighter opaque or not visible)
	 * @param maskedTransparency masked transparency
	 */
	inline void setBaseColorTextureMaskedTransparency(bool maskedTransparency) {
		baseColorTextureMaskedTransparency = maskedTransparency;
	}

	/**
	 * @return base color texture masked transparency threshold
	 */
	inline float getBaseColorTextureMaskedTransparencyThreshold() const {
		return baseColorTextureMaskedTransparencyThreshold;
	}

	/**
	 * Set base color texture masked transparency threshold
	 * @param maskedTransparencyThreshold diffuse texture masked transparency threshold
	 */
	inline void setBaseColorTextureMaskedTransparencyThreshold(float maskedTransparencyThreshold) {
		baseColorTextureMaskedTransparencyThreshold = maskedTransparencyThreshold;
	}

	/**
	 * @return metallic factor
	 */
	inline float getMetallicFactor() const {
		return metallicFactor;
	}

	/**
	 * Set metallic factor
	 * @param metallicFactor metallic factor
	 */
	inline void setMetallicFactor(float metallicFactor) {
		this->metallicFactor = metallicFactor;
	}

	/**
	 * @return roughness factor
	 */
	inline float getRoughnessFactor() const {
		return roughnessFactor;
	}

	/**
	 * Set roughness factor
	 * @param roughnessFactor roughness factor
	 */
	inline void setRoughnessFactor(float roughnessFactor) {
		this->roughnessFactor = roughnessFactor;
	}

	/**
	 * @return metallic roughness texture path name
	 */
	inline const string& getMetallicRoughnessTexturePathName() const {
		return metallicRoughnessTexturePathName;
	}

	/**
	 * @return metallic roughness texture file name
	 */
	inline const string& getMetallicRoughnessTextureFileName() const {
		return metallicRoughnessTextureFileName;
	}

	/**
	 * Set up a metallic roughness texture
	 * @param pathName path name
	 * @param fileName file name
	 */
	void setMetallicRoughnessTexture(const string& pathName, const string& fileName);

	/**
	 * @return if material has a metallic roughness texture
	 */
	inline bool hasMetallicRoughnessTexture() const {
		return metallicRoughnessTexture != nullptr;
	}

	/**
	 * @return the material's metallic roughness texture
	 */
	inline Texture* getMetallicRoughnessTexture() const {
		return metallicRoughnessTexture;
	}

	/**
	 * @return normal scale
	 */
	inline float getNormalScale() const {
		return normalScale;
	}

	/**
	 * Set normal scale
	 * @param normalScale normal scale
	 */
	inline void setNormalScale(float normalScale) {
		this->normalScale = normalScale;
	}

	/**
	 * @return normal texture path name
	 */
	inline const string& getNormalTexturePathName() const {
		return normalTexturePathName;
	}

	/**
	 * @return normal texture file name
	 */
	inline const string& getNormalTextureFileName() const {
		return normalTextureFileName;
	}

	/**
	 * Set up a normal texture
	 * @param pathName path name
	 * @param fileName file name
	 */
	void setNormalTexture(const string& pathName, const string& fileName);

	/**
	 * @return if material has a normal texture
	 */
	inline bool hasNormalTexture() const {
		return normalTexture != nullptr;
	}

	/**
	 * @return the material's normal texture
	 */
	inline Texture* getNormalTexture() const {
		return normalTexture;
	}

	/**
	 * @return exposure
	 */
	inline float getExposure() const {
		return exposure;
	}

	/**
	 * Set exposure
	 * @param exposure exposure
	 */
	inline void setExposure(float exposure) {
		this->exposure = exposure;
	}

};
