#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using std::string;
using std::to_string;

/**
 * Shader parameter model class
 */
class tdme::engine::ShaderParameter {
public:
	enum Type { TYPE_NONE, TYPE_FLOAT, TYPE_VECTOR3 };

private:
	Type type { TYPE_NONE };
	float floatValue { 0.0f };
	Vector3 vector3Value;

	/**
	 * @return value as string
	 */
	inline const string toString(float value) const {
		string floatString = to_string(value).substr(0, floatString.length() - 3);
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
	ShaderParameter(float floatValue): type(TYPE_FLOAT), floatValue(floatValue) {
	}

	/**
	 * Public constructor for Vector3 value
	 * @param vector3Value Vector3 Value
	 */
	ShaderParameter(const Vector3& vector3Value): type(TYPE_VECTOR3), vector3Value(vector3Value) {
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
		return floatValue;
	}

	/**
	 * @return vector3 value
	 */
	inline const Vector3& getVector3Value() const {
		return vector3Value;
	}

	/**
	 * @return string representation
	 */
	inline const string toString() const {
		switch(type) {
			case ShaderParameter::TYPE_NONE:
				return string();
			case ShaderParameter::TYPE_FLOAT:
				return toString(floatValue);
			case ShaderParameter::TYPE_VECTOR3:
				{
					string result;
					auto& shaderParameterArray = vector3Value.getArray();
					for (auto i = 0; i < shaderParameterArray.size(); i++) {
						if (i != 0) result+= ",";
						result+= toString(shaderParameterArray[i]);
					}
					return result;
				}
				break;
			default:
				return string();
		}
	}

};
