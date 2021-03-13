#pragma once

#include <array>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/math/Vector2.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>

using std::array;
using std::string;
using std::to_string;

using tdme::math::Vector2;
using tdme::math::Vector3;
using tdme::math::Vector4;

/**
 * Shader parameter model class
 */
class tdme::engine::ShaderParameter {
public:
	enum Type { TYPE_NONE, TYPE_FLOAT, TYPE_VECTOR2, TYPE_VECTOR3, TYPE_VECTOR4, TYPE_INT };

private:
	Type type { TYPE_NONE };
	int intValue { 0 };
	array<float, 4> floatValues { 0.0f, 0.0f, 0.0f, 0.0f };

	/**
	 * @return value as string
	 */
	inline const string toString(float value) const {
		string floatString = to_string(value);
		return floatString.substr(0, floatString.length() - 3);
	}

public:
	/**
	 * Public default constructor
	 */
	ShaderParameter(): type(TYPE_NONE) {
	}

	/**
	 * Public constructor for float value
	 * @param floatValue float value
	 */
	ShaderParameter(float floatValue): type(TYPE_FLOAT), floatValues( { floatValue, 0.0f, 0.0f, 0.0f} ) {
	}

	/**
	 * Public constructor for Vector2 value
	 * @param vector2Value Vector2 value
	 */
	ShaderParameter(const Vector2& vector2Value): type(TYPE_VECTOR2), floatValues( { vector2Value[0], vector2Value[1], 0.0f, 0.0f} ) {
	}

	/**
	 * Public constructor for Vector3 value
	 * @param vector3Value Vector3 value
	 */
	ShaderParameter(const Vector3& vector3Value): type(TYPE_VECTOR3), floatValues( { vector3Value[0], vector3Value[1], vector3Value[2], 0.0f} ) {
	}

	/**
	 * Public constructor for Vector4 value
	 * @param vector4Value Vector4 value
	 */
	ShaderParameter(const Vector4& vector4Value): type(TYPE_VECTOR4), floatValues( { vector4Value[0], vector4Value[1], vector4Value[2], vector4Value[3]} ) {
	}

	/**
	 * Public constructor for int value
	 * @param intValue int value
	 */
	ShaderParameter(int intValue): type(TYPE_INT), intValue(intValue) {
	}

	/**
	 * @return type
	 */
	inline Type getType() const {
		return type;
	}

	/**
	 * @return float value
	 */
	inline float getFloatValue() const {
		return floatValues[0];
	}

	/**
	 * @return Vector2 value
	 */
	inline const Vector2 getVector2Value() const {
		return Vector2(floatValues[0], floatValues[1]);
	}

	/**
	 * @return Vector3 value
	 */
	inline const Vector3 getVector3Value() const {
		return Vector3(floatValues[0], floatValues[1], floatValues[2]);
	}

	/**
	 * @return Vector4 value
	 */
	inline const Vector4 getVector4Value() const {
		return Vector4(floatValues[0], floatValues[1], floatValues[2], floatValues[3]);
	}

	/**
	 * @return int value
	 */
	inline float getIntValue() const {
		return intValue;
	}

	/**
	 * @return string representation
	 */
	inline const string toString() const {
		switch(type) {
			case ShaderParameter::TYPE_NONE:
				return string();
			case ShaderParameter::TYPE_FLOAT:
				return toString(floatValues[0]);
			case ShaderParameter::TYPE_VECTOR2:
			case ShaderParameter::TYPE_VECTOR3:
			case ShaderParameter::TYPE_VECTOR4:
				{
					string result;
					for (auto i = 0; i < floatValues.size(); i++) {
						if (i != 0) result+= ",";
						result+= toString(floatValues[i]);
					}
					return result;
				}
				break;
			case ShaderParameter::TYPE_INT:
				return toString(intValue);
			default:
				return string();
		}
	}

};
