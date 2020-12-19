#include <tdme/tests/RayTracingTest.h>

#include <string>

#include <tdme/utilities/Time.h>

#include <tdme/application/Application.h>
#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/physics/Body.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/Capsule.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/PrimitiveModel.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeBoundingVolume.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/LinesObject3D.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINodeConditions.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/gui/GUI.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/math/Math.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/ObjectDeleter.h>
#include <tdme/utilities/Time.h>

using std::string;
using std::to_string;

using tdme::tests::RayTracingTest;

using tdme::application::Application;
using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::physics::Body;
using tdme::engine::physics::World;
using tdme::engine::primitives::Capsule;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::PrimitiveModel;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::LinesObject3D;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::SceneConnector;
using tdme::engine::SceneConnector;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINodeConditions;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::gui::GUI;
using tdme::gui::GUIParser;
using tdme::math::Math;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utilities::Character;
using tdme::utilities::Console;
using tdme::utilities::MutableString;
using tdme::utilities::ObjectDeleter;
using tdme::utilities::Time;

constexpr int32_t RayTracingTest::RIGID_TYPEID_STANDARD;

constexpr int32_t RayTracingTest::BOX_COUNT;

RayTracingTest::RayTracingTest()
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
	world = new World();
}

RayTracingTest::~RayTracingTest()
{
	delete world;
}

void RayTracingTest::main(int argc, char** argv)
{
	auto rayTracingTest = new RayTracingTest();
	rayTracingTest->run(argc, argv, "RayTracingTest", rayTracingTest);
}

void RayTracingTest::display()
{
	auto transformations = world->getBody("player")->getTransformations();
	auto& yRotation = transformations.getRotation(0);
	yRotation.setAxis(Rotation::Y_AXIS);
	yRotation.setAngle(rotationY);
	transformations.update();
	world->getBody("player")->fromTransformations(transformations);

	// movement player
	{
		Vector3 movementVector;
		Vector3 tmpMovementVector;
		if (keyLeft == true) {
			movementVector.add(
				yRotation.getQuaternion().multiply(Vector3(1.0f, 0.0f, 0.0f), tmpMovementVector)
			);
		} else
		if (keyRight == true) {
			movementVector.sub(
				yRotation.getQuaternion().multiply(Vector3(1.0f, 0.0f, 0.0f), tmpMovementVector)
			);
		}
		if (keyUp == true) {
			movementVector.add(
				yRotation.getQuaternion().multiply(Vector3(0.0f, 0.0f, 1.0f), tmpMovementVector)
			);
		} else
		if (keyDown == true) {
			movementVector.sub(
				yRotation.getQuaternion().multiply(Vector3(0.0f, 0.0f, 1.0f), tmpMovementVector)
			);
		}
		world->getBody("player")->setLinearVelocity(movementVector.scale(4.0f));
	}

	Vector3 camLookFrom;
	Vector3 camLookAt;
	{
		auto headYPosition = 1.65f;
		float trdMovemventPlayerXAxis = 0.25f;
		float trdDistanceCamPlayer = 1.0f;

		Quaternion rotationQuaternion = transformations.getRotation(0).getQuaternion();
		rotationQuaternion.multiply((Quaternion().rotate(Vector3(1.0f, 0.0f, 0.0f), rotationX)));
		Vector3 vectorRotated;

		camLookAt.set(transformations.getTranslation().clone().add(Vector3(0.0f, headYPosition, 0.0f)));
		camLookAt.add(rotationQuaternion.multiply(Vector3(0.0f, 0.0f, 1.0f), vectorRotated).scale(80.0f));

		camLookFrom.set(transformations.getTranslation().clone().add(Vector3(0.0f, headYPosition, 0.0f)));
		camLookFrom.sub(rotationQuaternion.multiply(Vector3(0.0f, 0.0f, 1.0f), vectorRotated).scale(trdDistanceCamPlayer));
		camLookFrom.sub(transformations.getRotation(0).getQuaternion().multiply(Vector3(trdMovemventPlayerXAxis, 0.0f, 0.0f), vectorRotated));

		engine->getCamera()->setLookFrom(camLookFrom);
		engine->getCamera()->setLookAt(camLookAt);
		engine->getCamera()->setUpVector(Camera::computeUpVector(camLookFrom, camLookAt));
	}


	{
		dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("crosshair")->getNodeById("crosshair_hud"))->getActiveConditions().removeAll();
		float trdTraceLength = 3.5f;
		Vector3 traceVector;
		Vector3 traceEnd;
		Vector3 hitPoint;
		traceVector.set(camLookAt).sub(camLookFrom);
		traceEnd.set(traceVector);
		traceEnd.scale(1.0f / traceEnd.computeLength());
		traceEnd.scale(trdTraceLength);
		traceEnd.add(camLookFrom);
		//rayStart = camLookFrom;
		//rayEnd = traceEnd;
		auto rayTracedRigidBody = world->doRayCasting(
			SceneConnector::RIGIDBODY_TYPEID_STATIC | SceneConnector::RIGIDBODY_TYPEID_DYNAMIC,
			camLookFrom,
			traceEnd,
			hitPoint,
			"player"
		);
		if (rayTracedRigidBody != nullptr) {
			dynamic_cast<GUITextNode*>(engine->getGUI()->getScreen("crosshair")->getNodeById("crosshair_hud_id"))->setText(MutableString("Interact with " + rayTracedRigidBody->getId()));
			dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("crosshair")->getNodeById("crosshair_hud"))->getActiveConditions().add("pickup");
		}
		if (keyInfo == true) {
			// draw ray
			auto linesObject3D = new LinesObject3D("ray", 5.0f, { camLookFrom, traceEnd }, { 1.0f, 0.0f, 0.0f, 1.0f});
			linesObject3D->setEffectColorMul(Color4(1.0f, 0.0f, 0.0f, 1.0f));
			engine->addEntity(linesObject3D);

			// draw aabb
			if (rayTracedRigidBody != nullptr && rayTracedRigidBody->getId() != "ground") {
				auto bvEntity = new Object3D(
					"bv",
					entityBoundingVolumeModel
				);
				bvEntity->setTranslation(engine->getEntity(rayTracedRigidBody->getId())->getTransformations().getTranslation());
				bvEntity->update();
				engine->addEntity(bvEntity);
			} else {
				engine->removeEntity("bv");
			}
		}
	}

	if (keyInfo == true) keyInfo = false;

	// update world, display engine
	world->update(1.0f / 60.0f);
	engine->getEntity("player")->fromTransformations(transformations);
	engine->display();
	engine->getGUI()->render();
}

void RayTracingTest::dispose()
{
	engine->dispose();
}

void RayTracingTest::initialize()
{
	engine->initialize();
	engine->setSceneColor(Color4(0.8f, 0.8f, 0.8f, 1.0f));
	Object3D* entity;
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(25.0f);
	cam->setLookFrom(Vector3(0.0f, 5.0f, 10.0f));
	cam->setLookAt(Vector3(0.0f, 2.5f, 0.0f));
	cam->setUpVector(cam->computeUpVector(cam->getLookFrom(), cam->getLookAt()));
	auto light0 = engine->getLightAt(0);
	light0->setAmbient(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	light0->setDiffuse(Color4(0.5f, 0.5f, 0.5f, 1.0f));
	light0->setSpecular(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	light0->setPosition(Vector4(0.0f, 20000.0f, 0.0f, 1.0f));
	light0->setSpotDirection(Vector3(0.0f, 0.0f, 0.0f).sub(Vector3(light0->getPosition().getX(), light0->getPosition().getY(), light0->getPosition().getZ())));
	light0->setConstantAttenuation(0.5f);
	light0->setLinearAttenuation(0.0f);
	light0->setQuadraticAttenuation(0.0f);
	light0->setSpotExponent(0.0f);
	light0->setSpotCutOff(180.0f);
	light0->setEnabled(true);
	auto ground = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 0.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(240.0f, 1.0f, 240.0f)));
	auto groundModel = modelDeleter.add(PrimitiveModel::createModel(ground, "ground_model"));
	groundModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.25f, 0.25f, 0.25f, 1.0f));
	groundModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(0.5f, 0.5f, 0.5f, 1.0f));
	entity = new Object3D("ground", groundModel);
	entity->setTranslation(Vector3(0.0f, -1.0f, 0.0f));
	entity->setReceivesShadows(true);
	entity->update();
	engine->addEntity(entity);
	world->addStaticRigidBody("ground", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.5f, {ground});
	auto interactionTable = prototypeDeleter.add(PrototypeReader::read("resources/tests/asw", "Mesh_Interaction_Table.fbx.tmm"));
	entityBoundingVolumeModel = modelDeleter.add(PrimitiveModel::createModel(interactionTable->getBoundingVolume(0)->getBoundingVolume(), "interactiontable.bv"));
	int interactionTableIdx = 0;
	for (float z = -20.0f; z < 20.0f; z+= 5.0f)
	for (float x = -20.0f; x < 20.0f; x+= 5.0f) {
		// engine
		auto id = "interactionTable." + to_string(interactionTableIdx);
		auto entity = new Object3D(
			id,
			interactionTable->getModel()
		);
		entity->setTranslation(Vector3(x, 0.0f, z));
		entity->update();
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		engine->addEntity(entity);

		// physics
		SceneConnector::createBody(
			world,
			interactionTable,
			id,
			entity->getTransformations(),
			SceneConnector::RIGIDBODY_TYPEID_STATIC
		);

		//
		interactionTableIdx++;

	}
	//auto capsuleBig = new Capsule(Vector3(0.0f, 0.1f, 0.0f), Vector3(0.0f, 0.11f, 0.0f), 0.1f);
	auto capsuleBig = bvDeleter.add(new Capsule(Vector3(0.0f, 0.25f, 0.0f), Vector3(0.0f, 1.5f, 0.0f), 0.25f));
	auto capsuleBigModel = modelDeleter.add(PrimitiveModel::createModel(capsuleBig, "capsulebig_model"));
	capsuleBigModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(1.0f, 0.8f, 0.8f, 1.0f));
	capsuleBigModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 0.0f, 0.0f, 1.0f));
	entity = new Object3D("player", capsuleBigModel);
	entity->setContributesShadows(true);
	entity->setReceivesShadows(true);
	entity->setTranslation(Vector3(-2.0f, 0.0f, 0.0f));
	entity->addRotation(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
	entity->update();
	engine->addEntity(entity);
	world->addRigidBody("player", true, SceneConnector::RIGIDBODY_TYPEID_DYNAMIC, entity->getTransformations(), 0.0f, 1.0f, 80.0f, Body::getNoRotationInertiaTensor(), {capsuleBig});

	//
	engine->getGUI()->addScreen("crosshair", GUIParser::parse("resources/tests/screens", "crosshair.xml"));
	engine->getGUI()->addRenderScreen("crosshair");
}

void RayTracingTest::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}

void RayTracingTest::onChar(unsigned int key, int x, int y) {
}

void RayTracingTest::onKeyDown (unsigned char key, int x, int y) {
	auto keyChar = Character::toLowerCase(key);
	if (keyChar == 'w') keyUp = true;
	if (keyChar == 'a') keyLeft = true;
	if (keyChar == 's') keyDown = true;
	if (keyChar == 'd') keyRight = true;
	if (keyChar == 'i') keyInfo = true;
}

void RayTracingTest::onKeyUp(unsigned char key, int x, int y) {
	auto keyChar = Character::toLowerCase(key);
	if (keyChar == 'w') keyUp = false;
	if (keyChar == 'a') keyLeft = false;
	if (keyChar == 's') keyDown = false;
	if (keyChar == 'd') keyRight = false;
}

void RayTracingTest::onSpecialKeyDown (int key, int x, int y) {
}

void RayTracingTest::onSpecialKeyUp(int key, int x, int y) {
}

void RayTracingTest::onMouseDragged(int x, int y) {
	if (mouseLastXDragged != -1 && mouseLastYDragged != -1) {
		rotationY-= x - mouseLastXDragged;
		rotationX+= y - mouseLastYDragged;
	}
	mouseLastXDragged = x;
	mouseLastYDragged = y;
}

void RayTracingTest::onMouseMoved(int x, int y) {
}

void RayTracingTest::onMouseButton(int button, int state, int x, int y) {
	mouseLastXDragged = -1;
	mouseLastYDragged = -1;
}

void RayTracingTest::onMouseWheel(int button, int direction, int x, int y) {
}
