#include <tdme/tools/shared/model/LevelEditorEntityParticleSystem_ObjectParticleSystem.h>

#include <string>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Model.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/ExceptionBase.h>
#include <tdme/utilities/StringTools.h>

using std::string;

using tdme::tools::shared::model::LevelEditorEntityParticleSystem_ObjectParticleSystem;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Model;
using tdme::math::Vector3;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::ExceptionBase;
using tdme::utilities::StringTools;

void LevelEditorEntityParticleSystem_ObjectParticleSystem::setModelFile(const string& modelFileName) /* throws(Exception) */
{
	this->modelFileName = modelFileName;
	model = ModelReader::read(
		Tools::getPath(modelFileName),
		Tools::getFileName(modelFileName)
	);
}

LevelEditorEntityParticleSystem_ObjectParticleSystem::~LevelEditorEntityParticleSystem_ObjectParticleSystem() {
	if (model != nullptr) delete model;
}
