#include <tdme/engine/model/ShaderModel.h>

#include <string>

#include <tdme/utilities/Enum.h>

using std::string;

using tdme::engine::model::ShaderModel;
using tdme::utilities::Enum;

ShaderModel::ShaderModel(const string& name, int ordinal): Enum(name, ordinal)
{
}

ShaderModel* tdme::engine::model::ShaderModel::SPECULAR = new ShaderModel("SPECULAR", 0);
ShaderModel* tdme::engine::model::ShaderModel::PBR = new ShaderModel("PBR", 1);

ShaderModel* ShaderModel::valueOf(const string& a0)
{
	if (SPECULAR->getName() == a0) return SPECULAR;
	if (PBR->getName() == a0) return PBR;
	// TODO: throw exception here maybe
	return nullptr;
}
