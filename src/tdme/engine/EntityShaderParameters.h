#pragma once

#include <map>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/ShaderParameter.h>

using std::map;
using std::string;
using std::to_string;

using tdme::engine::ShaderParameter;

/**
 * TDME2 engine entity shader parameters
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::EntityShaderParameters
{
private:
	map<string, ShaderParameter> parameters;
	string shaderId;

	mutable string hash;
	mutable bool changed { false };

	/**
	 * Compute shader parameters hash
	 */
	void computeHash();

public:
	/**
	 * Public constructor
	 */
	EntityShaderParameters() {
	}

	/**
	 * Destructor
	 */
	~EntityShaderParameters() {
	}

	/**
	 * Set shader
	 * @param shaderId shader id
	 */
	void setShader(const string& shaderId) {
		if (this->shaderId == shaderId) return;
		parameters.clear();
		hash = shaderId + ";";
		changed = false;
		this->shaderId = shaderId;
	}

	/**
	 * Returns shader parameter for given parameter name, if the value does not exist, the default will be returned
	 * @param shaderId shader id
	 * @param parameterName parameter name
	 * @return shader parameter
	 */
	const ShaderParameter getShaderParameter(const string& parameterName) const;

	/**
	 * Set shader parameter for given parameter name
	 * @param shaderId shader id
	 * @param parameterName parameter name
	 * @param paraemterValue parameter value
	 */
	void setShaderParameter(const string& parameterName, const ShaderParameter& parameterValue);

	/**
	 * Set shader parameter for given parameter name
	 * @param shaderId shader id
	 * @param parameterName parameter name
	 * @param paraemterValue parameter value
	 */
	void setShaderParameter(const string& parameterName, const string& parameterValueString);

	/**
	 * @return shader parameters hash
	 */
	const string& getShaderParametersHash() const {
		if (changed == true) {
			// TODO: md5 or something
			hash = shaderId + ";";
			for (auto& it: parameters) {
				hash+= it.first;
				hash+= "=";
				auto& parameterValue = it.second;
				switch(parameterValue.getType()) {
					case ShaderParameter::TYPE_NONE:
						break;
					case ShaderParameter::TYPE_BOOLEAN:
						hash+= to_string(parameterValue.getBooleanValue());
						break;
					case ShaderParameter::TYPE_INTEGER:
						hash+= to_string(parameterValue.getIntegerValue());
						break;
					case ShaderParameter::TYPE_FLOAT:
						hash+= to_string(static_cast<int>(parameterValue.getFloatValue() * 100.0f));
						break;
					case ShaderParameter::TYPE_VECTOR2:
						{
							auto& shaderParameterArray = parameterValue.getVector2Value().getArray();
							for (auto i = 0; i < shaderParameterArray.size(); i++) {
								if (i != 0) hash+= ",";
								hash+= to_string(static_cast<int>(shaderParameterArray[i] * 100.0f));
							}
						}
						break;
					case ShaderParameter::TYPE_VECTOR3:
						{
							auto& shaderParameterArray = parameterValue.getVector3Value().getArray();
							for (auto i = 0; i < shaderParameterArray.size(); i++) {
								if (i != 0) hash+= ",";
								hash+= to_string(static_cast<int>(shaderParameterArray[i] * 100.0f));
							}
						}
						break;
					case ShaderParameter::TYPE_VECTOR4:
						{
							auto& shaderParameterArray = parameterValue.getVector4Value().getArray();
							for (auto i = 0; i < shaderParameterArray.size(); i++) {
								if (i != 0) hash+= ",";
								hash+= to_string(static_cast<int>(shaderParameterArray[i] * 100.0f));
							}
						}
						break;
					default:
						break;
				}
				hash+= ";";
			}
			changed = false;
		}
		return hash;
	}
};
