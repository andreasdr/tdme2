#include <tdme/tests/FlowMapTest.h>

#include <string>

#include <tdme/utils/Time.h>

#include <tdme/application/Application.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/fileio/models/ModelReader.h>
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
#include <tdme/utils/Console.h>
#include <tdme/utils/FlowMap.h>
#include <tdme/utils/FlowMapCell.h>
#include <tdme/utils/PathFinding.h>

using std::string;
using std::to_string;

using tdme::tests::FlowMapTest;

using tdme::application::Application;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::Timing;
using tdme::engine::fileio::models::ModelReader;
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
using tdme::utils::Console;
using tdme::utils::FlowMap;
using tdme::utils::FlowMapCell;
using tdme::utils::PathFinding;
using tdme::utils::Time;

FlowMapTest::FlowMapTest()
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
	world = new World();
	timeLastUpdate = -1;
}

FlowMapTest::~FlowMapTest() {
	if (flowMap != nullptr) flowMap->releaseReference();
	delete world;
	delete emptyModel;
	delete playerModelEntity;
	delete pathFinding;
}

void FlowMapTest::main(int argc, char** argv)
{
	auto flowMapTest = new FlowMapTest();
	flowMapTest->run(argc, argv, "FlowMapTest");
}

void FlowMapTest::display()
{
	if (startPlayerCellPosition.clone().sub(startPlayerObject->getTranslation()).computeLength() < 0.1f) {
		auto cell = flowMap->getCell(startPlayerObject->getTranslation().getX(), startPlayerObject->getTranslation().getZ());
		if (cell != nullptr) {
			startPlayerCellDirection = cell->getDirection();
			startPlayerCellPosition = startPlayerObject->getTranslation().clone().add(startPlayerCellDirection.clone().scale(flowMap->getStepSize()));
			if (startPlayerObject->getAnimation() != "walk") startPlayerObject->setAnimation("walk");
			auto yRotationAngle = Vector3::computeAngle(Vector3(0.0f, 0.0f, 1.0f), cell->getDirection(), Vector3(0.0f, 1.0f, 0.0f));
			startPlayerObject->setRotationAngle(0, yRotationAngle);
		} else {
			startPlayerObject->setAnimation("still");
			startPlayerCellDirection = Vector3();
		}
	} else {
		if (endPlayerObject1->getTranslation().clone().sub(startPlayerObject->getTranslation()).computeLength() < 1.0f ||
			endPlayerObject2->getTranslation().clone().sub(startPlayerObject->getTranslation()).computeLength() < 1.0f) {
			doPathFinding();
		} else {
			startPlayerObject->setTranslation(startPlayerObject->getTranslation() + startPlayerCellDirection * 2.0f / 60.0f);
			startPlayerObject->update();
		}
	}
	engine->display();
}

void FlowMapTest::dispose()
{
	engine->dispose();
}

void FlowMapTest::initialize()
{
	engine->initialize();
	LevelFileImport::doImport("resources/tests/levels/pathfinding", "test.tl", &level);
	Level::setLight(engine, &level);
	Level::addLevel(engine, &level, false, false, false);
	Level::addLevel(world, &level);
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(15.0f);
	cam->setLookFrom(level.getCenter() + Vector3(0.0f, 10.0f, 0.0f));
	cam->setLookAt(level.getCenter());
	cam->setUpVector(cam->computeUpVector(cam->getLookFrom(), cam->getLookAt()));
	emptyModel = ModelReader::read("resources/engine/tools/leveleditor/models", "empty.dae");
	playerModelEntity = ModelMetaDataFileImport::doImport("resources/tests/models/mementoman", "mementoman.dae.tmm");
	playerModelEntity->getModel()->addAnimationSetup("walk", 0, 23, true);
	playerModelEntity->getModel()->addAnimationSetup("still", 24, 99, true);
	playerModelEntity->getModel()->addAnimationSetup("death", 109, 169, false);
	startPlayerObject = new Object3D("startPlayerObject", playerModelEntity->getModel());
	startPlayerObject->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
	startPlayerObject->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
	startPlayerObject->update();
	startPlayerObject->setAnimation("still");
	startPlayerObject->setContributesShadows(playerModelEntity->isContributesShadows());
	startPlayerObject->setReceivesShadows(playerModelEntity->isReceivesShadows());
	engine->addEntity(startPlayerObject);
	endPlayerObject1 = new Object3D("endPlayerObject1", playerModelEntity->getModel());
	endPlayerObject1->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
	endPlayerObject1->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
	endPlayerObject1->update();
	endPlayerObject1->setAnimation("still");
	endPlayerObject1->setContributesShadows(playerModelEntity->isContributesShadows());
	endPlayerObject1->setReceivesShadows(playerModelEntity->isReceivesShadows());
	endPlayerObject1->setEffectColorAdd(Color4(1.0f, 0.0f, 0.0f, 0.0f));
	endPlayerObject1->setEffectColorMul(Color4(1.0f, 0.0f, 0.0f, 1.0f));
	engine->addEntity(endPlayerObject1);
	endPlayerObject2 = new Object3D("endPlayerObject2", playerModelEntity->getModel());
	endPlayerObject2->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
	endPlayerObject2->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
	endPlayerObject2->update();
	endPlayerObject2->setAnimation("still");
	endPlayerObject2->setContributesShadows(playerModelEntity->isContributesShadows());
	endPlayerObject2->setReceivesShadows(playerModelEntity->isReceivesShadows());
	endPlayerObject2->setEffectColorAdd(Color4(1.0f, 0.0f, 0.0f, 0.0f));
	endPlayerObject2->setEffectColorMul(Color4(1.0f, 0.0f, 0.0f, 1.0f));
	engine->addEntity(endPlayerObject2);
	pathPositions.push_back(Vector3(-2.5f, 0.25f, -4.5f));
	pathPositions.push_back(Vector3(-2.5f, 0.25f, 0.5f));
	pathPositions.push_back(Vector3(2.5f, 0.25f, 0.5f));
	pathPositions.push_back(Vector3(2.5f, 0.25f, -4.5f));
	pathFinding = new PathFinding(world, false, 1000, 2.0f, 0.5f, 0.5f);
	doPathFinding();
}

void FlowMapTest::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}

void FlowMapTest::doPathFinding() {
	if (flowMap != nullptr) {
		flowMap->releaseReference();
		flowMap = nullptr;
	}
	startPlayerObject->update();
	startPlayerCellPosition.set(pathPositions[(int)(Math::random() * pathPositions.size())]);
	startPlayerObject->setTranslation(startPlayerCellPosition);
	startPlayerObject->update();
	startPlayerCellDirection.set(0.0f, 0.0f, 0.0f);
	endPlayerObject1->setTranslation(pathPositions[(int)(Math::random() * pathPositions.size())]);
	endPlayerObject1->update();
	endPlayerObject2->setTranslation(pathPositions[(int)(Math::random() * pathPositions.size())]);
	endPlayerObject2->update();
	vector<Vector3> path;
	pathFinding->findPath(
		startPlayerObject->getTransformations().getTranslation(),
		endPlayerObject1->getTransformations().getTranslation(),
		Level::RIGIDBODY_TYPEID_STATIC,
		path
	);
	Console::println("Found a path: steps: " + to_string(path.size()));
	auto center = level.getBoundingBox()->getCenter();
	auto depth = Math::ceil(level.getBoundingBox()->getDimensions().getZ());
	auto width = Math::ceil(level.getBoundingBox()->getDimensions().getX());
	flowMap = pathFinding->createFlowMap(
		{
			endPlayerObject1->getTransformations().getTranslation(),
			endPlayerObject2->getTransformations().getTranslation(),
		},
		center,
		width * 2.0f,
		depth * 2.0f,
		Level::RIGIDBODY_TYPEID_STATIC,
		path
	);
	auto i = 0;
	while(true == true) {
		auto flowDirectionEntityId = "flowdirection." + to_string(i);
		if (engine->getEntity(flowDirectionEntityId) == nullptr) break;
		engine->removeEntity(flowDirectionEntityId);
		i++;
	}
	if (flowMap == nullptr) return;
	i = 0;
	for (auto z = -depth / 2; z <= depth / 2; z+= flowMap->getStepSize())
	for (auto x = -width / 2; x <= width / 2; x+= flowMap->getStepSize()) {
		auto cellPosition = Vector3(
			x + center.getX(),
			0.0f,
			z + center.getZ()
		);
		auto cell = flowMap->getCell(cellPosition.getX(), cellPosition.getZ());
		if (cell == nullptr) continue;
		auto flowDirectionEntityId = "flowdirection." + to_string(i);
		auto yRotationAngle = Vector3::computeAngle(Vector3(0.0f, 0.0f, 1.0f), cell->getDirection(), Vector3(0.0f, 1.0f, 0.0f));
		auto cellObject = new Object3D(flowDirectionEntityId, emptyModel);
		cellObject->setScale(Vector3(0.5f, 0.5f, 0.5f));
		cellObject->setTranslation(cellPosition + Vector3(0.0f, 0.25f, 0.0f));
		cellObject->addRotation(Vector3(0.0f, 1.0f, 0.0f), yRotationAngle - 90.0f);
		cellObject->setDisableDepthTest(true);
		if (cell->isWalkable() == false) cellObject->setEffectColorMul(Color4(1.0f, 0.0f, 0.0f, 1.0f));
		cellObject->update();
		engine->addEntity(cellObject);
		i++;
	}
}