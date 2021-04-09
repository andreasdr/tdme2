#include <cstdlib>
#include <string>

#include <tdme/application/Application.h>
#include <tdme/engine/fileio/scenes/SceneReader.h>
#include <tdme/engine/fileio/scenes/SceneWriter.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneEntity.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/Version.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using tdme::application::Application;
using tdme::engine::fileio::scenes::SceneReader;
using tdme::engine::fileio::scenes::SceneWriter;
using tdme::engine::model::RotationOrder;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneEntity;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::Transformations;
using tdme::engine::Version;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::Exception;

int main(int argc, char** argv)
{
	Console::println(string("scenefixmodelszup2yup ") + Version::getVersion());
	Console::println(Version::getCopyright());
	Console::println();

	if (argc != 2) {
		Console::println("Usage: scenefixmodelszup2yup scene.tscene");
		Application::exit(1);
	}
	string sceneFileName = string(argv[1]);
	try {
		Console::println("Loading scene: " + sceneFileName);
		Scene scene;
		SceneReader::read(
			FileSystem::getInstance()->getPathName(sceneFileName),
			FileSystem::getInstance()->getFileName(sceneFileName),
			scene
		);
		Console::println("Fixing scene models up axis from Z-Up to Y-Up");
		Matrix4x4 z2yUpMatrix;
		z2yUpMatrix.identity().rotate(Vector3(1.0f, 0.0f, 0.0f), -90.0f);
		// scene prototype library
		auto sceneLibray = scene.getLibrary();
		for (auto i = 0; i < sceneLibray->getPrototypeCount(); i++) {
			auto prototype = sceneLibray->getPrototypeAt(i);
			if (prototype->getType() != Prototype_Type::MODEL) continue;
			prototype->getModel()->setImportTransformationsMatrix(prototype->getModel()->getImportTransformationsMatrix().clone().multiply(z2yUpMatrix));
			prototype->getModel()->getBoundingBox()->getMin() = z2yUpMatrix.multiply(prototype->getModel()->getBoundingBox()->getMin());
			prototype->getModel()->getBoundingBox()->getMax() = z2yUpMatrix.multiply(prototype->getModel()->getBoundingBox()->getMax());
			prototype->getModel()->getBoundingBox()->update();
		}
		// scene entities
		for (auto i = 0; i < scene.getEntityCount(); i++) {
			auto sceneEntity = scene.getEntityAt(i);
			if (sceneEntity->getPrototype()->getType() != Prototype_Type::MODEL) continue;
			auto scale = sceneEntity->getTransformations().getScale();
			sceneEntity->getTransformations().setScale(Vector3(scale.getX(), scale.getZ(), scale.getY()));
			auto rotationX = sceneEntity->getTransformations().getRotationAngle(scene.getRotationOrder()->getAxisXIndex());
			sceneEntity->getTransformations().setRotationAngle(scene.getRotationOrder()->getAxisXIndex(), rotationX + 90);
			sceneEntity->getTransformations().update();
		}
		// TODO: bvs
		Console::println("Saving scene: " + sceneFileName);
		SceneWriter::write(
			FileSystem::getInstance()->getPathName(sceneFileName),
			FileSystem::getInstance()->getFileName(sceneFileName),
			scene
		);
	} catch (Exception& exception) {
		Console::println("An error occurred: " + string(exception.what()));
	}
}
