#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/math/Matrix2D3x3.h>

using std::string;

using tdme::engine::model::PBRMaterialProperties;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::math::Matrix2D3x3;

/**
 * Represents a material
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::model::Material final
{
private:
	static string defaultMaterialId;
	static Material* defaultMaterial;

public:

	/**
	 * @return default material
	 */
	inline static Material* getDefaultMaterial() {
		return defaultMaterial;
	}

private:
	string id;
	SpecularMaterialProperties* specularMaterialProperties { nullptr };
	PBRMaterialProperties* pbrMaterialProperties { nullptr };
	bool doubleSided { false };
	Matrix2D3x3 textureMatrix;

public:
	/**
	 * Public constructor
	 * @param id id
	 */
	Material(const string& id);

	/**
	 * Destructor
	 */
	~Material();

	/**
	 * @return material id
	 */
	inline const string& getId() const {
		return id;
	}

	/**
	 * @return specular material properties
	 */
	inline const SpecularMaterialProperties* getSpecularMaterialProperties() const {
		return specularMaterialProperties;
	}

	/**
	 * @return specular material properties
	 */
	inline SpecularMaterialProperties* getSpecularMaterialProperties() {
		return specularMaterialProperties;
	}

	/**
	 * @return PBR material properties
	 */
	inline const PBRMaterialProperties* getPBRMaterialProperties() const {
		return pbrMaterialProperties;
	}

	/**
	 * @return PBR material properties
	 */
	inline PBRMaterialProperties* getPBRMaterialProperties() {
		return pbrMaterialProperties;
	}

	/**
	 * Set specular material properties
	 * @param specularMaterialProperties specular material properties
	 */
	void setSpecularMaterialProperties(SpecularMaterialProperties* specularMaterialProperties);

	/**
	 * Set PBR material properties
	 * @param pbrMaterialProperties PBR material properties
	 */
	void setPBRMaterialProperties(PBRMaterialProperties* pbrMaterialProperties);

	/**
	 * @return is double sided
	 */
	inline bool isDoubleSided() const {
		return doubleSided;
	}

	/**
	 * Set double sided
	 * @param texture matrix
	 */
	inline void setDoubleSided(bool doubleSided) {
		this->doubleSided = doubleSided;
	}

	/**
	 * @return texture matrix
	 */
	inline const Matrix2D3x3& getTextureMatrix() const {
		return textureMatrix;
	}

	/**
	 * Set texture matrix
	 * @param textureMatrix texture matrix
	 */
	inline void setTextureMatrix(const Matrix2D3x3& textureMatrix) {
		this->textureMatrix = textureMatrix;
	}

};
