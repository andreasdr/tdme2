#include <string>
#include <cstdlib>

#include <tdme/application/Application.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/files/LevelFileImport.h>
#include <tdme/tools/shared/files/LevelFileExport.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityLibrary.h>
#include <tdme/tools/shared/model/LevelEditorEntity_EntityType.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/model/LevelEditorObject.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using tdme::application::Application;
using tdme::engine::Transformations;
using tdme::engine::model::RotationOrder;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::files::LevelFileImport;
using tdme::tools::shared::files::LevelFileExport;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityLibrary;
using tdme::tools::shared::model::LevelEditorEntity_EntityType;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::model::LevelEditorObject;
using tdme::utilities::Console;
using tdme::utilities::Exception;

int main(int argc, char** argv)
{
	Console::println(string("levelfixmodelszup2yup 1.9.9"));
	Console::println(string("Programmed 2018 by Andreas Drewke, drewke.net."));
	Console::println();
	if (argc != 2) {
		Console::println("Usage: levelfixmodelszup2yup level.tl");
		Application::exit(1);
	}
	string levelFileName = string(argv[1]);
	try {
		Console::println("Loading level: " + levelFileName);
		LevelEditorLevel level;
		LevelFileImport::doImport(
			FileSystem::getInstance()->getPathName(levelFileName),
			FileSystem::getInstance()->getFileName(levelFileName),
			&level
		);
		Console::println("Fixing level models up axis from Z-Up to Y-Up");
		Matrix4x4 z2yUpMatrix;
		z2yUpMatrix.identity().rotate(-90.0f, Vector3(1.0f, 0.0f, 0.0f));
		// entity library
		auto entityLibrary = level.getEntityLibrary();
		for (auto i = 0; i < entityLibrary->getEntityCount(); i++) {
			auto entity = entityLibrary->getEntityAt(i);
			if (entity->getType() != LevelEditorEntity_EntityType::MODEL) continue;
			entity->getModel()->setImportTransformationsMatrix(entity->getModel()->getImportTransformationsMatrix().clone().multiply(z2yUpMatrix));
			z2yUpMatrix.multiply(
				entity->getModel()->getBoundingBox()->getMin(),
				entity->getModel()->getBoundingBox()->getMin()
			);
			z2yUpMatrix.multiply(
				entity->getModel()->getBoundingBox()->getMax(),
				entity->getModel()->getBoundingBox()->getMax()
			);
			entity->getModel()->getBoundingBox()->update();
		}
		// objects
		for (auto i = 0; i < level.getObjectCount(); i++) {
			auto object = level.getObjectAt(i);
			if (object->getEntity()->getType() != LevelEditorEntity_EntityType::MODEL) continue;
			auto scale = object->getTransformations().getScale();
			object->getTransformations().setScale(Vector3(scale.getX(), scale.getZ(), scale.getY()));
			auto rotationX = object->getTransformations().getRotationAngle(level.getRotationOrder()->getAxisXIndex());
			object->getTransformations().setRotationAngle(level.getRotationOrder()->getAxisXIndex(), rotationX + 90);
			object->getTransformations().update();
		}
		// TODO: bvs
		Console::println("Saving level: " + levelFileName);
		LevelFileExport::doExport(
			FileSystem::getInstance()->getPathName(levelFileName),
			FileSystem::getInstance()->getFileName(levelFileName),
			&level
		);
	} catch (Exception& exception) {
		Console::println("An error occurred: " + string(exception.what()));
	}
}
