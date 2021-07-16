#include <tdme/tests/FlowMapTest2.h>

#include <string>

#include <tdme/utilities/Time.h>

#include <tdme/application/Application.h>
#include <tdme/application/InputEventHandler.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/fileio/scenes/SceneReader.h>
#include <tdme/engine/fileio/scenes/SceneReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/physics/Body.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeBoundingVolume.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/engine/Timing.h>
#include <tdme/math/Math.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/FlowMap.h>
#include <tdme/utilities/FlowMapCell.h>
#include <tdme/utilities/PathFinding.h>
#include <tdme/utilities/Primitives.h>

using std::string;
using std::to_string;

using tdme::tests::FlowMapTest2;

using tdme::application::Application;
using tdme::application::InputEventHandler;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::fileio::scenes::SceneReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::physics::Body;
using tdme::engine::physics::World;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::scene::Scene;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::SceneConnector;
using tdme::engine::SceneConnector;
using tdme::engine::Timing;
using tdme::math::Math;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utilities::Console;
using tdme::utilities::FlowMap;
using tdme::utilities::FlowMapCell;
using tdme::utilities::PathFinding;
using tdme::utilities::Primitives;
using tdme::utilities::Time;

FlowMapTest2::FlowMapTest2()
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
	world = new World();
	timeLastUpdate = -1;
}

FlowMapTest2::~FlowMapTest2() {
	if (flowMap != nullptr) flowMap->releaseReference();
	delete world;
	delete emptyModel;
	delete playerModelPrototype;
	delete pathFinding;
}

void FlowMapTest2::main(int argc, char** argv)
{
	auto flowMapTest2 = new FlowMapTest2();
	flowMapTest2->run(argc, argv, "FlowMapTest2", flowMapTest2);
}

void FlowMapTest2::display()
{
	if (pause != true) {
		Quaternion formationRotationQuaternion;
		formationRotationQuaternion.identity();
		if (combatUnits[0].pathFindingNodeIdx != -1) {
			formationMovement = combatUnits[0].movementDirection;
			auto currentFormationYRotationAngle = Vector3::computeAngle(Vector3(0.0f, 0.0f, -1.0f), formationMovement.clone().normalize(), Vector3(0.0f, 1.0f, 0.0f));
			if (Float::isNaN(currentFormationYRotationAngle) == false) formationYRotationAngle = currentFormationYRotationAngle;
		}
		formationRotationQuaternion.rotate(Vector3(0.0f, 1.0f, 0.0f), formationYRotationAngle);
		for (auto& combatUnit: combatUnits) {
			auto cell = flowMap->getCell(combatUnit.object->getTranslation().getX(), combatUnit.object->getTranslation().getZ());
			auto flowMapDirection = flowMap->computeDirection(combatUnit.object->getTranslation().getX(), combatUnit.object->getTranslation().getZ());
			if (cell != nullptr) {
				auto pathFindingNodeIdx = Math::min(cell->getPathNodeIdx() + 1, flowMap->getPath().size() - 1);
				if (pathFindingNodeIdx > combatUnit.pathFindingNodeIdx) {
					combatUnit.pathFindingNodeIdx = pathFindingNodeIdx;
					combatUnit.pathFindingNodeLast = combatUnit.pathFindingNode;
				}
				if (combatUnit.pathFindingNodeIdx != -1) {
					if (combatUnit.idx == 0) {
						combatUnit.pathFindingNode = flowMap->getPath()[combatUnit.pathFindingNodeIdx];
					} else {
						auto relativeFormationPosition = (combatUnitFormationTransformations[combatUnit.formationIdx].getTranslation() - combatUnitFormationTransformations[combatUnits[0].formationIdx].getTranslation());
						auto formationPosition = formationRotationQuaternion * relativeFormationPosition;
						combatUnit.pathFindingNode = combatUnits[0].object->getTranslation() + formationPosition;
					}
				}
				//combatUnit.cellDirection = flowMapDirection.computeLengthSquared() > Math::square(Math::EPSILON)?flowMapDirection:cell->getDirection();
				combatUnit.cellDirection = cell->getDirection();
				auto pcdDotPND = 0.0f;
				Vector3 pathFindingNodeDirection;
				if (cell->hasMissingNeighborCell() == false) {
					pathFindingNodeDirection = combatUnit.pathFindingNode - combatUnit.object->getTranslation();
					if (pathFindingNodeDirection.computeLengthSquared() > Math::square(Math::EPSILON)) {
						pathFindingNodeDirection.normalize();
						pcdDotPND = (Math::clamp(Vector3::computeDotProduct(combatUnit.cellDirection, pathFindingNodeDirection), -0.25f, 1.0f) + 0.25f) / 1.25f;
						if (pcdDotPND > 0.0f) {
							pcdDotPND = 1.0f;
							combatUnit.movementDirection = (combatUnit.cellDirection * ((1.0f - pcdDotPND)) + pathFindingNodeDirection * pcdDotPND).normalize();
							auto nextCellTranslation = combatUnit.object->getTranslation() + combatUnit.movementDirection * flowMap->getStepSize();
							auto nextCell = flowMap->getCell(nextCellTranslation.getX(), nextCellTranslation.getZ());
							if (nextCell == nullptr || nextCell->isWalkable() == false) {
								pcdDotPND = 0.0f;
								combatUnit.movementDirection = (combatUnit.cellDirection * ((1.0f - pcdDotPND)) + pathFindingNodeDirection * pcdDotPND).normalize();
							}
						}
					}
				}
				combatUnit.movementDirection = (combatUnit.cellDirection * ((1.0f - pcdDotPND)) + pathFindingNodeDirection * pcdDotPND).normalize();
				if (combatUnit.object->getAnimation() != "walk") combatUnit.object->setAnimation("walk");
				combatUnit.movementDirectionRing[combatUnit.movementDirectionRingIdx] = combatUnit.movementDirection;
				combatUnit.movementDirectionRingLength = Math::min(combatUnit.movementDirectionRingLength + 1, combatUnit.movementDirectionRing.size());
				Vector3 movementDirection;
				for (auto i = 0; i < combatUnit.movementDirectionRingLength; i++) {
					movementDirection.add(combatUnit.movementDirectionRing[(combatUnit.movementDirectionRingIdx - i) % combatUnit.movementDirectionRing.size()]);
				}
				movementDirection.normalize();
				combatUnit.object->setRotationAngle(0, Vector3::computeAngle(Vector3(0.0f, 0.0f, 1.0f), movementDirection, Vector3(0.0f, 1.0f, 0.0f)));
				combatUnit.movementDirectionRingIdx = (combatUnit.movementDirectionRingIdx + 1) % combatUnit.movementDirectionRing.size();
				{
					{
						auto pathFindingNodeId = "pathfindingnodecurrent." + to_string(combatUnit.idx);
						auto pathFindingNodeObject = new Object3D(pathFindingNodeId, emptyModel);
						pathFindingNodeObject->setScale(Vector3(2.0f, 2.0f, 2.0f));
						pathFindingNodeObject->setTranslation(combatUnit.pathFindingNode + Vector3(0.0f, 0.0f, 0.0f));
						pathFindingNodeObject->addRotation(Vector3(0.0f, 0.0f, 1.0f), 90.0f);
						pathFindingNodeObject->setDisableDepthTest(true);
						pathFindingNodeObject->setEffectColorMul(Color4(0.0f, 0.0f, 0.0f, 1.0f));
						pathFindingNodeObject->update();
						engine->addEntity(pathFindingNodeObject);
					}
					if (combatUnit.idx == 0)
					{
						auto flowDirectionEntityId = "flowdirectioncurrent" + to_string(combatUnit.idx);
						auto yRotationAngle = Vector3::computeAngle(Vector3(0.0f, 0.0f, 1.0f), pathFindingNodeDirection, Vector3(0.0f, 1.0f, 0.0f));
						auto cellObject = new Object3D(flowDirectionEntityId, emptyModel);
						cellObject->setScale(Vector3(5.0f, 5.0f, 5.0f));
						cellObject->setTranslation(Vector3(-0.0f, 0.5f, 3.0f - (combatUnit.idx * 5.0f)));
						cellObject->addRotation(Vector3(0.0f, 1.0f, 0.0f), yRotationAngle - 90.0f);
						cellObject->setDisableDepthTest(true);
						cellObject->update();
						engine->addEntity(cellObject);
					}
				}
			} else {
				combatUnit.object->setAnimation("still");
				combatUnit.cellDirection = Vector3();
				combatUnit.movementDirection = Vector3();
			}
		}
		for (auto& combatUnit: combatUnits) {
			if (path[path.size() - 1].clone().sub(combatUnit.object->getTranslation()).computeLength() < 1.0f) {
				doPathFinding();
			} else {
				combatUnit.object->setTranslation((combatUnit.object->getTranslation() + (combatUnit.movementDirection * combatUnit.speed / 60.0f)));
				combatUnit.object->update();
			}
		}
		for (auto& combatUnit: combatUnits) {
			if (combatUnit.idx == 0) continue;
			auto villagerPathNodeIdx = Math::min(combatUnit.pathFindingNodeIdx + 1, flowMap->getPath().size() - 1);
			auto formationPositionMovement = combatUnit.pathFindingNode - combatUnit.object->getTranslation();
			auto formationPositionDistance = formationPositionMovement.computeLength();
			formationPositionMovement.normalize();
			auto formationPositionMovementDotMovementDirection = Vector3::computeDotProduct(combatUnit.movementDirection, formationPositionMovement);
			if (formationPositionDistance > 0.1f) {
				combatUnit.speed = formationPositionMovementDotMovementDirection > 0.0f?2.0f:0.5f;
			} else {
				combatUnit.speed = 1.0f;
			}
		}
	}
	engine->display();
}

void FlowMapTest2::dispose()
{
	engine->dispose();
}

void FlowMapTest2::initialize()
{
	engine->initialize();
	SceneReader::read("resources/tests/levels/pathfinding", "test.flowmap.tscene", scene);
	SceneConnector::setLights(engine, scene);
	SceneConnector::addScene(engine, scene, false, false, false, false);
	SceneConnector::addScene(world, scene);
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(15.0f);
	cam->setLookFrom(scene.getCenter() + Vector3(0.0f, 25.0f, 0.0f));
	cam->setLookAt(scene.getCenter());
	cam->setUpVector(cam->computeUpVector(cam->getLookFrom(), cam->getLookAt()));
	emptyModel = ModelReader::read("resources/engine/models", "empty.tm");
	formationLinePrototype = ModelReader::read("resources/tests/levels/pathfinding", "Formation_Line.tm");
	playerModelPrototype = PrototypeReader::read("resources/tests/models/mementoman", "mementoman.tmodel");
	playerModelPrototype->getModel()->addAnimationSetup("walk", 0, 23, true);
	playerModelPrototype->getModel()->addAnimationSetup("still", 24, 99, true);
	playerModelPrototype->getModel()->addAnimationSetup("death", 109, 169, false);
	// first line
	{
		CombatUnit combatUnit;
		combatUnit.idx = 0;
		combatUnit.formationIdx = 2;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	{
		CombatUnit combatUnit;
		combatUnit.idx = 1;
		combatUnit.formationIdx = 0;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	{
		CombatUnit combatUnit;
		combatUnit.idx = 2;
		combatUnit.formationIdx = 1;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	{
		CombatUnit combatUnit;
		combatUnit.idx = 3;
		combatUnit.formationIdx = 3;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	{
		CombatUnit combatUnit;
		combatUnit.idx = 4;
		combatUnit.formationIdx = 4;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	{
		CombatUnit combatUnit;
		combatUnit.idx = 5;
		combatUnit.formationIdx = 5;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	// second line
	{
		CombatUnit combatUnit;
		combatUnit.idx = 6;
		combatUnit.formationIdx = 6;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.speed = 1.0f;
		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	{
		CombatUnit combatUnit;
		combatUnit.idx = 7;
		combatUnit.formationIdx = 7;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	{
		CombatUnit combatUnit;
		combatUnit.idx = 8;
		combatUnit.formationIdx = 8;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	{
		CombatUnit combatUnit;
		combatUnit.idx = 9;
		combatUnit.formationIdx = 9;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	{
		CombatUnit combatUnit;
		combatUnit.idx = 10;
		combatUnit.formationIdx = 10;
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.object = new Object3D("combatunit." + to_string(combatUnit.idx), playerModelPrototype->getModel());
		combatUnit.object->addRotation(Vector3(0.0f, 1.0f, 0.0f), 90.0f);
		combatUnit.object->setTranslation(Vector3(2.5f, 0.25f, 0.5f));
		combatUnit.object->update();
		combatUnit.object->setAnimation("still");
		combatUnit.object->setContributesShadows(playerModelPrototype->isContributesShadows());
		combatUnit.object->setReceivesShadows(playerModelPrototype->isReceivesShadows());
		engine->addEntity(combatUnit.object);
		combatUnits.push_back(combatUnit);
	}
	endPositions.push_back(Vector3(-17.0f, 0.25f, 2.5f));
	endPositions.push_back(Vector3(0.0f, 0.25f, 2.5f));
	endPositions.push_back(Vector3(0.0f, 0.25f, -10.0f));
	pathFinding = new PathFinding(world, true, 1000, 2.0f, 0.5f, 0.5f);
	doPathFinding();

	//
	for (auto i = 0; i < combatUnitFormationTransformations.size(); i++) {
		auto emptyName = "Waypoint_" + to_string(i);
		Matrix4x4 transformationsMatrix;
		if (formationLinePrototype->computeTransformationsMatrix(
			emptyName,
			formationLinePrototype->getImportTransformationsMatrix(),
			transformationsMatrix) == false) {
			Console::println("FlowMapTest2::initialize(): Missing '" + emptyName + "'");
		}
		combatUnitFormationTransformations[i].fromMatrix(transformationsMatrix, RotationOrder::ZYX);
	}
}

void FlowMapTest2::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}

void FlowMapTest2::doPathFinding() {
	if (flowMap != nullptr) {
		flowMap->releaseReference();
		flowMap = nullptr;
	}

	//
	auto startPosition = endPositions[(int)(Math::random() * endPositions.size())];
	for (auto& combatUnit: combatUnits) {
		combatUnit.pathFindingNodeIdx = -1;
		combatUnit.speed = 1.0f;
		combatUnit.movementDirectionRing.fill(Vector3());
		combatUnit.movementDirectionRingLength = 0;
		combatUnit.movementDirectionRingIdx = 0;
		combatUnit.object->update();
		combatUnit.object->setTranslation(startPosition);
		combatUnit.object->update();
		combatUnit.cellDirection.set(0.0f, 0.0f, 0.0f);
	}

	//
	pathFinding->findPath(
		combatUnits[0].object->getTransformations().getTranslation(),
		endPositions[(int)(Math::random() * endPositions.size())],
		SceneConnector::RIGIDBODY_TYPEID_STATIC,
		path
	);
	Console::println("Found a path: steps: " + to_string(path.size()));
	auto center = scene.getBoundingBox()->getCenter();
	auto depth = Math::ceil(scene.getBoundingBox()->getDimensions().getZ());
	auto width = Math::ceil(scene.getBoundingBox()->getDimensions().getX());

	if (path.size() > 12) {
		vector<Vector3> pathA;
		vector<Vector3> pathB;
		for (auto i = 0; i < path.size(); i++) {
			if (i < (path.size() / 2)) {
				pathA.push_back(path[i]);
			} else {
				if (pathB.empty() == true) {
					pathB.push_back(pathA[pathA.size() - 8]);
					pathB.push_back(pathA[pathA.size() - 7]);
					pathB.push_back(pathA[pathA.size() - 6]);
					pathB.push_back(pathA[pathA.size() - 5]);
					pathB.push_back(pathA[pathA.size() - 4]);
					pathB.push_back(pathA[pathA.size() - 3]);
					pathB.push_back(pathA[pathA.size() - 2]);
					pathB.push_back(pathA[pathA.size() - 1]);
				}
				pathB.push_back(path[i]);
			}
		}
		flowMap = pathFinding->createFlowMap(
			{
				pathA[pathA.size() - 1]
			},
			center,
			12.0f,
			12.0f,
			SceneConnector::RIGIDBODY_TYPEID_STATIC,
			pathA,
			false
		);
		auto flowMap2 = pathFinding->createFlowMap(
			{
				path[path.size() - 1]
			},
			center,
			12.0f,
			12.0f,
			SceneConnector::RIGIDBODY_TYPEID_STATIC,
			pathB,
			true
		);
		flowMap->merge(flowMap2);
		flowMap2->releaseReference();
	} else {
		flowMap = pathFinding->createFlowMap(
			{
				path[path.size() - 1]
			},
			center,
			12.0f,
			12.0f,
			SceneConnector::RIGIDBODY_TYPEID_STATIC,
			path,
			true
		);
	}
	{
		auto i = 0;
		while(true == true) {
			auto flowDirectionEntityId = "flowdirection." + to_string(i);
			if (engine->getEntity(flowDirectionEntityId) == nullptr) break;
			engine->removeEntity(flowDirectionEntityId);
			i++;
		}
	}
	{
		auto i = 0;
		while(true == true) {
			auto flowDirectionEntityId = "pathfindingnode." + to_string(i);
			if (engine->getEntity(flowDirectionEntityId) == nullptr) break;
			engine->removeEntity(flowDirectionEntityId);
			i++;
		}
	}
	if (flowMap == nullptr) return;

	Quaternion formationRotationQuaternion;
	formationRotationQuaternion.identity();
	formationMovement = path[1] - path[0];
	formationYRotationAngle = Vector3::computeAngle(Vector3(0.0f, 0.0f, -1.0f), formationMovement.clone().normalize(), Vector3(0.0f, 1.0f, 0.0f));
	formationRotationQuaternion.rotate(Vector3(0.0f, 1.0f, 0.0f), formationYRotationAngle);
	for (auto& combatUnit: combatUnits) {
		if (combatUnit.idx == 0) continue;
		auto relativeFormationPosition = (combatUnitFormationTransformations[combatUnit.formationIdx].getTranslation() - combatUnitFormationTransformations[combatUnits[0].formationIdx].getTranslation());
		auto formationPosition = formationRotationQuaternion * relativeFormationPosition;
		combatUnit.movementDirection = formationMovement.clone().normalize();
		combatUnit.object->setTranslation(combatUnit.object->getTranslation() + formationPosition);
		combatUnit.object->setRotationAngle(0, Vector3::computeAngle(Vector3(0.0f, 0.0f, 1.0f), combatUnit.movementDirection, Vector3(0.0f, 1.0f, 0.0f)));
		combatUnit.object->update();
	}

	// draw flow map
	{
		auto i = 0;
		auto j = 0;
		for (auto z = -depth / 2; z <= depth / 2; z+= flowMap->getStepSize())
		for (auto x = -width / 2; x <= width / 2; x+= flowMap->getStepSize()) {
			auto cellPosition = Vector3(
				x + center.getX(),
				0.0f,
				z + center.getZ()
			);
			auto cell = flowMap->getCell(cellPosition.getX(), cellPosition.getZ());
			if (cell == nullptr) continue;
			{
				auto flowDirectionEntityId = "flowdirection." + to_string(i);
				auto yRotationAngle = Vector3::computeAngle(Vector3(0.0f, 0.0f, 1.0f), cell->getDirection(), Vector3(0.0f, 1.0f, 0.0f));
				auto cellObject = new Object3D(flowDirectionEntityId, emptyModel);
				cellObject->setScale(Vector3(0.5f, 0.5f, 0.5f));
				cellObject->setTranslation(cellPosition + Vector3(0.0f, 0.25f, 0.0f));
				cellObject->addRotation(Vector3(0.0f, 1.0f, 0.0f), yRotationAngle - 90.0f);
				cellObject->setDisableDepthTest(true);
				cellObject->update();
				engine->addEntity(cellObject);
				i++;
			}
			/*
			{
				auto pathFindingNodeId = "pathfindingnode." + to_string(j);
				auto pathFindingNodeObject = new Object3D(pathFindingNodeId, emptyModel);
				pathFindingNodeObject->setScale(Vector3(2.0f, 2.0f, 2.0f));
				pathFindingNodeObject->setTranslation(flowMap->getPath()[cell->getPathIdx()] + Vector3(0.0f, 0.4f, 0.0f));
				pathFindingNodeObject->addRotation(Vector3(0.0f, 0.0f, 1.0f), 90.0f);
				pathFindingNodeObject->setDisableDepthTest(true);
				pathFindingNodeObject->setEffectColorMul(Color4(0.0f, 0.0f, 0.0f, 1.0f));
				pathFindingNodeObject->update();
				engine->addEntity(pathFindingNodeObject);
				j++;
			}
			*/
		}
	}
}

void FlowMapTest2::onChar(unsigned int key, int x, int y) {
	if (key == ' ') pause = pause == true?false:true;
}

void FlowMapTest2::onKeyDown (unsigned char key, int x, int y) {
}

void FlowMapTest2::onKeyUp(unsigned char key, int x, int y) {
}

void FlowMapTest2::onSpecialKeyDown (int key, int x, int y) {
}

void FlowMapTest2::onSpecialKeyUp(int key, int x, int y) {
}

void FlowMapTest2::onMouseDragged(int x, int y) {
}

void FlowMapTest2::onMouseMoved(int x, int y) {
}

void FlowMapTest2::onMouseButton(int button, int state, int x, int y) {
}

void FlowMapTest2::onMouseWheel(int button, int direction, int x, int y) {
}