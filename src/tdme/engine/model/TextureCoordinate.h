#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/math/Math.h>

using std::array;

using tdme::math::Math;

/**
 * Class representing texture UV coordinates data
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::model::TextureCoordinate final
{
private:
	array<float, 2> data;
public:
	/**
	 * Public constructor
	 */
	TextureCoordinate();

	/**
	 * Public constructor
	 * @param textureCoordinate texture coordinate
	 */
	TextureCoordinate(const TextureCoordinate& textureCoordinate);

	/**
	 * Public constructor
	 * @param uv texture coordinate
	 */
	TextureCoordinate(const array<float, 2>& uv);

	/**
	 * Public constructor
	 * @param u u
	 * @param v v
	 */
	TextureCoordinate(float u, float v);

	/**
	 * @return U
	 */
	inline float getU() const {
		return data[0];
	}

	/**
	 * @return V
	 */
	inline float getV() const {
		return data[1];
	}

	/**
	 * Set texture coordinate
	 * @param textureCoordinate texture coordinate
	 */
	TextureCoordinate& set(const TextureCoordinate& textureCoordinate);

	/**
	 * Set texture coordinate
	 * @param uv texture coordinate
	 */
	TextureCoordinate& set(const array<float, 2>& uv);

	/**
	 * Set texture coordinate
	 * @param u u
	 * @param v v
	 */
	TextureCoordinate& set(float u, float v);

	/**
	 * @return texture data as array
	 */
	inline array<float, 2>& getArray() const {
		return (array<float, 2>&)data;
	}

	/**
	 * Clones the texture coordinate
	 * @return new texture coordinate
	 */
	TextureCoordinate clone();

	/**
	 * Compares this texture coordinate with given texture coordinate
	 * @return equality
	 */
	inline bool equals(const TextureCoordinate& textureCoordinate) const {
		return
			this == &textureCoordinate ||
			(
				Math::abs(data[0] - textureCoordinate.data[0]) < Math::EPSILON &&
				Math::abs(data[1] - textureCoordinate.data[1]) < Math::EPSILON
			);
	}

};
