#include <tdme/engine/prototype/PrototypeParticleSystem_ObjectParticleSystem.h>

#include <memory>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Model.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/editor/misc/Tools.h>
#include <tdme/utilities/ExceptionBase.h>
#include <tdme/utilities/StringTools.h>

using std::string;
using std::unique_ptr;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Model;
using tdme::engine::prototype::PrototypeParticleSystem_ObjectParticleSystem;
using tdme::math::Vector3;
using tdme::tools::editor::misc::Tools;
using tdme::utilities::ExceptionBase;
using tdme::utilities::StringTools;

void PrototypeParticleSystem_ObjectParticleSystem::setModelFile(const string& modelFileName, bool useBC7TextureCompression)
{
	if (modelFileName.empty() == false) {
		model = unique_ptr<Model>(
			ModelReader::read(
				Tools::getPathName(modelFileName),
				Tools::getFileName(modelFileName),
				useBC7TextureCompression
			)
		);
	}
}

PrototypeParticleSystem_ObjectParticleSystem::~PrototypeParticleSystem_ObjectParticleSystem() {
}
