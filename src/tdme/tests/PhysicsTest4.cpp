#include <tdme/tests/PhysicsTest4.h>

#include <string>

#include <tdme/application/Application.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Object3DModel.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/physics/Body.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/PrimitiveModel.h>
#include <tdme/engine/primitives/TerrainMesh.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utils/Time.h>
#include <tdme/utils/Exception.h>
#include <tdme/utils/Console.h>

using std::string;
using std::to_string;

using tdme::tests::PhysicsTest4;

using tdme::application::Application;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::Object3DModel;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::physics::Body;
using tdme::engine::physics::World;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::PrimitiveModel;
using tdme::engine::primitives::TerrainMesh;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utils::Time;
using tdme::utils::Console;
using tdme::utils::Exception;

constexpr int32_t PhysicsTest4::RIGID_TYPEID_STANDARD;

PhysicsTest4::PhysicsTest4()
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
	world = new World();
}

PhysicsTest4::~PhysicsTest4()
{
	delete world;
}

void PhysicsTest4::main(int argc, char** argv)
{
	auto physicsTest4 = new PhysicsTest4();
	physicsTest4->run(argc, argv, "PhysicsTest4", physicsTest4);
}

void PhysicsTest4::display()
{
	auto start = Time::getCurrentMillis();
	world->update(1.0f / 60.0f);
	world->synch(engine);
	engine->display();
	auto end = Time::getCurrentMillis();
	Console::println(string("PhysicsTest4::display::" + to_string(end - start) + "ms"));
}

void PhysicsTest4::dispose()
{
	engine->dispose();
}

void PhysicsTest4::initialize()
{
	engine->initialize();
	Object3D* entity;
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(15.0f);
	cam->setLookFrom(Vector3(0.0f, 4.0f, -6.0f));
	cam->setLookAt(Vector3(0.0f, 0.0f, 0.0f));
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
	auto box = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 0.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(0.1f, 0.1f, 0.1f)));
	auto boxModel = modelDeleter.add(PrimitiveModel::createModel(box, "box_model"));
	boxModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.5f, 0.5f, 1.0f));
	boxModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 0.0f, 0.0f, 1.0f));
	entity = new Object3D("box", boxModel);
	entity->setContributesShadows(true);
	entity->setReceivesShadows(true);
	entity->setTranslation(Vector3(0.0f, 3.0f, 0.0f));
	entity->update();
	engine->addEntity(entity);
	world->addRigidBody("box", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 100.0f, Vector3(1.0f, 1.0f, 1.0f), {box});
	world->getBody("box")->setLinearVelocity(world->getBody("box")->getLinearVelocity().clone().setX(1.0f));
	try {
		auto _terrainModel = modelDeleter.add(ModelReader::read("resources/tests/models/physicstest4", "TestGround.fbx.tm"));
		entity = new Object3D("terrain", _terrainModel);
		entity->setTranslation(Vector3(1.75f, 0.0f, -1.65f));
		entity->setReceivesShadows(true);
		entity->update();
		engine->addEntity(entity);
		Object3DModel terrainModel(_terrainModel);
		auto terrainMesh = new TerrainMesh(&terrainModel, entity->getTransformations());
		world->addStaticRigidBody("ground", true, RIGID_TYPEID_STANDARD, Transformations(), 0.5f, {terrainMesh});
	} catch (Exception& exception) {
		Console::print(string("PhysicsTest4::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
		Application::exit(1);
	}
}

void PhysicsTest4::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}

void PhysicsTest4::onChar(unsigned int key, int x, int y) {
}

void PhysicsTest4::onKeyDown (unsigned char key, int x, int y) {
}

void PhysicsTest4::onKeyUp(unsigned char key, int x, int y) {
}

void PhysicsTest4::onSpecialKeyDown (int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = true;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = true;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = true;
	if (key == KEYBOARD_KEYCODE_DOWN) keyDown = true;
}

void PhysicsTest4::onSpecialKeyUp(int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = false;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = false;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = false;
	if (key == KEYBOARD_KEYCODE_DOWN) keyDown = false;
}

void PhysicsTest4::onMouseDragged(int x, int y) {
}

void PhysicsTest4::onMouseMoved(int x, int y) {
}

void PhysicsTest4::onMouseButton(int button, int state, int x, int y) {
}

void PhysicsTest4::onMouseWheel(int button, int direction, int x, int y) {
}
