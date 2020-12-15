#include <tdme/tests/PathFindingTest.h>

#include <string>

#include <tdme/utilities/Time.h>

#include <tdme/application/Application.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/physics/Body.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/PrimitiveModel.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/tools/leveleditor/logic/Level.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityBoundingVolume.h>
#include <tdme/tools/shared/model/LevelEditorLevel.h>
#include <tdme/tools/shared/files/LevelFileImport.h>
#include <tdme/tools/shared/files/ModelMetaDataFileImport.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/PathFinding.h>

using std::string;
using std::to_string;

using tdme::tests::PathFindingTest;

using tdme::application::Application;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::Timing;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::physics::Body;
using tdme::engine::physics::World;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::PrimitiveModel;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::tools::leveleditor::logic::Level;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityBoundingVolume;
using tdme::tools::shared::model::LevelEditorLevel;
using tdme::tools::shared::files::LevelFileImport;
using tdme::tools::shared::files::ModelMetaDataFileImport;
using tdme::utilities::Console;
using tdme::utilities::PathFinding;
using tdme::utilities::Time;

PathFindingTest::PathFindingTest()
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
	world = new World();
	pathFinding = nullptr;
	timeLastUpdate = -1;
	pathIdx = 0;
	playerXDirection = 0.0f;
	playerZDirection = 0.0f;
	playerObject = nullptr;
	playerModelEntity = nullptr;
}

PathFindingTest::~PathFindingTest()
{
	delete world;
	delete pathFinding;
	delete playerModelEntity;
}

void PathFindingTest::main(int argc, char** argv)
{
	auto pathFindingTest = new PathFindingTest();
	pathFindingTest->run(argc, argv, "PathFindingTest");
}

void PathFindingTest::determinePlayerMovementDirection() {
	if (pathIdx >= path.size()) {
		playerXDirection = 0.0f;
		playerZDirection = 0.0f;
		return;
	}
	if (Math::abs(playerObject->getTranslation().getX() - path[pathIdx].getX()) < 0.01f)  {
		playerXDirection = 0.0f;
	} else
	if (playerObject->getTranslation().getX() < path[pathIdx].getX())  {
		playerXDirection = +1.0f;
	} else
	if (playerObject->getTranslation().getX() > path[pathIdx].getX())  {
		playerXDirection = -1.0f;
	}
	if (Math::abs(playerObject->getTranslation().getZ() - path[pathIdx].getZ()) < 0.01f)  {
		playerZDirection = 0.0f;
	} else
	if (playerObject->getTranslation().getZ() < path[pathIdx].getZ())  {
		playerZDirection = +1.0f;
	} else
	if (playerObject->getTranslation().getZ() > path[pathIdx].getZ())  {
		playerZDirection = -1.0f;
	}
}

bool PathFindingTest::determinePlayerCompletedStepX() {
	return
		(playerXDirection > 0.1f && playerObject->getTranslation().getX() >= path[pathIdx].getX()) ||
		(playerXDirection < -0.1f && playerObject->getTranslation().getX() <= path[pathIdx].getX()) ||
		(Math::abs(playerObject->getTranslation().getX() - path[pathIdx].getX()) < 0.01f);
}

bool PathFindingTest::determinePlayerCompletedStepZ() {
	return
		(playerZDirection > 0.1f && playerObject->getTranslation().getZ() >= path[pathIdx].getZ()) ||
		(playerZDirection < -0.1f && playerObject->getTranslation().getZ() <= path[pathIdx].getZ()) ||
		(Math::abs(playerObject->getTranslation().getZ() - path[pathIdx].getZ()) < 0.01f);
}

void PathFindingTest::display()
{
	bool hadMovement = false;
	auto playerTranslation = playerObject->getTranslation();
	if (determinePlayerCompletedStepX() == false) {
		if (hadMovement == false) playerTranslation.add(Vector3(playerXDirection * engine->getTiming()->getDeltaTime() / 1000.0f * 4.0f, 0.0f, 0.0f));
		playerObject->getRotation(0).setAngle(playerXDirection > 0.1f?90.0f:270.0f);
		hadMovement = true;
	} else {
		playerTranslation.setX(path[pathIdx].getX());
	}
	if (determinePlayerCompletedStepZ() == false) {
		if (hadMovement == false) playerTranslation.add(Vector3(0.0f, 0.0f, playerZDirection * engine->getTiming()->getDeltaTime() / 1000.0f * 4.0f));
		playerObject->getRotation(0).setAngle(playerZDirection > 0.1f?0.0f:180.0f);
		hadMovement = true;
	} else {
		playerTranslation.setZ(path[pathIdx].getZ());
	}
	playerObject->setTranslation(playerTranslation);
	playerObject->update();
	if (hadMovement == false) {
		pathIdx = pathIdx + 1;
		if (pathIdx >= path.size()) {
			pathIdx = 0;
			doPathFinding();
		}
		determinePlayerMovementDirection();
	}
	engine->display();
}

void PathFindingTest::dispose()
{
	engine->dispose();
}

void PathFindingTest::initialize()
{
	engine->initialize();
	LevelFileImport::doImport("resources/tests/levels/pathfinding", "test.tl", level);
	Level::setLight(engine, level);
	Level::addLevel(engine, level, false, false, false, false);
	Level::addLevel(world, level);
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(15.0f);
	cam->setLookFrom(Vector3(0.0f, 10.0f, -6.0f));
	cam->setLookAt(level.getCenter());
	cam->setUpVector(cam->computeUpVector(cam->getLookFrom(), cam->getLookAt()));
	playerModelEntity = ModelMetaDataFileImport::doImport("resources/tests/models/mementoman", "mementoman.dae.tmm");
	playerModelEntity->getModel()->addAnimationSetup("walk", 0, 23, true);
	playerModelEntity->getModel()->addAnimationSetup("still", 24, 99, true);
	playerModelEntity->getModel()->addAnimationSetup("death", 109, 169, false);
	playerObject = new Object3D("player", playerModelEntity->getModel());
	playerObject->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
	playerObject->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
	playerObject->update();
	playerObject->setAnimation("walk");
	playerObject->setContributesShadows(playerModelEntity->isContributesShadows());
	playerObject->setReceivesShadows(playerModelEntity->isReceivesShadows());
	engine->addEntity(playerObject);
	pathIdx = 0;
	pathPositions.push_back(Vector3(-2.5f, 0.25f, -4.5f));
	pathPositions.push_back(Vector3(-2.5f, 0.25f, 0.5f));
	pathPositions.push_back(Vector3(2.5f, 0.25f, 0.5f));
	pathPositions.push_back(Vector3(2.5f, 0.25f, -4.5f));
	pathFinding = new PathFinding(world);
	doPathFinding();
	determinePlayerMovementDirection();
}

void PathFindingTest::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}

void PathFindingTest::doPathFinding() {
	if (pathFinding->findPath(
			playerObject->getTransformations().getTranslation(),
			pathPositions[(int)(Math::random() * pathPositions.size())],
			Level::RIGIDBODY_TYPEID_STATIC,
			path
		) == true) {
		Console::println("Found a path: steps: " + to_string(path.size()));
	} else {
		Console::println("Found no path");
	}
}
