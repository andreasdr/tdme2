#pragma once

#include <string>

#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/utilities/Enum.h>

using std::string;

using tdme::utilities::Enum;
using tdme::engine::model::Model;
using tdme::engine::model::ShaderModel;

/**
 * Shader model
 */
class tdme::engine::model::ShaderModel final: public Enum
{
	friend class Model;

public:
	static ShaderModel* SPECULAR;
	static ShaderModel* PBR;

public:
	ShaderModel(const string& name, int ordinal);

public:
	static ShaderModel* valueOf(const string& a0);

};
