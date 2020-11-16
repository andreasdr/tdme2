#include <tdme/tests/PhysicsTest3.h>

#include <string>

#include <tdme/application/Application.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Object3DModel.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/physics/Body.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/Capsule.h>
#include <tdme/engine/primitives/ConvexMesh.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/PrimitiveModel.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/engine/primitives/TerrainMesh.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utilities/Time.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/ObjectDeleter.h>

using std::string;
using std::to_string;

using tdme::tests::PhysicsTest3;

using tdme::application::Application;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::Object3DModel;
using tdme::engine::Rotation;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::physics::Body;
using tdme::engine::physics::World;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::Capsule;
using tdme::engine::primitives::ConvexMesh;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::PrimitiveModel;
using tdme::engine::primitives::Sphere;
using tdme::engine::primitives::TerrainMesh;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utilities::Time;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ObjectDeleter;

constexpr int32_t PhysicsTest3::RIGID_TYPEID_STANDARD;

constexpr int32_t PhysicsTest3::BOX_COUNT;

constexpr int32_t PhysicsTest3::BOXSTACK_COUNT;

constexpr int32_t PhysicsTest3::CAPSULE_COUNT;

constexpr int32_t PhysicsTest3::SPHERE_COUNT;

PhysicsTest3::PhysicsTest3()
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
	world = new World();
}

PhysicsTest3::~PhysicsTest3()
{
	delete world;
}

void PhysicsTest3::main(int argc, char** argv)
{
	auto physicsTest3 = new PhysicsTest3();
	physicsTest3->run(argc, argv, "PhysicsTest3", physicsTest3);
}

void PhysicsTest3::display()
{
	{
		auto velocity = world->getBody("capsulebig1")->getLinearVelocity();
		if (keyLeft) velocity.setX(8.0f); else
		if (keyRight) velocity.setX(-8.0f); else
			velocity.setX(0.0f);
		if (keyUp) velocity.setZ(8.0f); else
		if (keyDown) velocity.setZ(-8.0f); else
			velocity.setZ(0.0f);
		world->getBody("capsulebig1")->setLinearVelocity(velocity);
	}

	{
		auto velocity = world->getBody("capsulebig2")->getLinearVelocity();
		if (keyA) velocity.setX(8.0f); else
		if (keyD) velocity.setX(-8.0f); else
			velocity.setX(0.0f);
		if (keyW) velocity.setZ(8.0f); else
		if (keyS) velocity.setZ(-8.0f); else
			velocity.setZ(0.0f);
		world->getBody("capsulebig2")->setLinearVelocity(velocity);
	}

	auto start = Time::getCurrentMillis();
	auto fps = 60.0f;
	world->update(1.0f / fps);
	world->synch(engine);
	engine->display();
	auto end = Time::getCurrentMillis();
	auto rendererStatistics = engine->getRendererStatistics();
	Console::println(
		string("PhysicsTest3::display::") + to_string(end - start) + "ms; " +
		"clear calls: " + to_string(rendererStatistics.clearCalls) + ", " +
		"render calls: " + to_string(rendererStatistics.renderCalls) + ", " +
		"compute calls: " + to_string(rendererStatistics.computeCalls) + ", " +
		"tris: " + to_string(rendererStatistics.triangles) + ", " +
		"points: " + to_string(rendererStatistics.points) + ", " +
		"line points: " + to_string(rendererStatistics.linePoints) + ", " +
		"buffer up: " + to_string(rendererStatistics.bufferUploads) + ", " +
		"texture up: " + to_string(rendererStatistics.textureUploads) + ", " +
		"render passes: " + to_string(rendererStatistics.renderPasses) + ", " +
		"draw cmds: " + to_string(rendererStatistics.drawCommands) + ", " +
		"submits: " + to_string(rendererStatistics.submits)
	);
}

void PhysicsTest3::dispose()
{
	engine->dispose();
}

void PhysicsTest3::initialize()
{
	engine->initialize();
	engine->addPostProcessingProgram("ssao");
	Object3D* entity;
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(15.0f);
	cam->setLookFrom(Vector3(0.0f, 4.0f * 4.0f, -6.0f * 6.0f));
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
	auto side = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 0.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(1.0f, 16.0f, 8.0f)));
	auto sideModel = modelDeleter.add(PrimitiveModel::createModel(side, "side_model"));
	sideModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.8f, 0.8f, 1.0f));
	sideModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	auto nearFar = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 0.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(8.0f, 16.0f, 1.0f)));
	auto nearFarModel = modelDeleter.add(PrimitiveModel::createModel(nearFar, "far_model"));
	nearFarModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.8f, 0.8f, 1.0f));
	nearFarModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	entity = new Object3D("far", nearFarModel);
	entity->setTranslation(Vector3(0.0f, 0.0f, +9.0f));
	entity->update();
	engine->addEntity(entity);
	world->addStaticRigidBody("far", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.5f, {nearFar});
	entity = new Object3D("near", nearFarModel);
	entity->setTranslation(Vector3(0.0f, 0.0f, -9.0f));
	entity->setEffectColorMul(Color4(1.0f, 1.0f, 1.0f, 0.0f));
	entity->update();
	entity->setEnabled(false);
	engine->addEntity(entity);
	world->addStaticRigidBody("near", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.5f, {nearFar});
	entity = new Object3D("sideright", sideModel);
	entity->setTranslation(Vector3(-9.0f, 0.0f, 0.0f));
	entity->update();
	engine->addEntity(entity);
	world->addStaticRigidBody("sideright", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.5f, {side});
	entity = new Object3D("sideleft", sideModel);
	entity->setTranslation(Vector3(9.0f, 0.0f, 0.0f));
	entity->update();
	engine->addEntity(entity);
	world->addStaticRigidBody("sideleft", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.5f, {side});
	auto box = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 0.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(0.6f, 0.6f, 0.6f)));
	auto boxModel = modelDeleter.add(PrimitiveModel::createModel(box, "box_model"));
	boxModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.5f, 0.5f, 1.0f));
	boxModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 0.0f, 0.0f, 1.0f));
	for (auto i = 0; i < BOX_COUNT; i++) {
		entity = new Object3D("box" + to_string(i), boxModel);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->setTranslation(Vector3(-2.0f + i * 0.1f, 10.0f + i * 3.0f, 0.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("box" + to_string(i), true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 100.0f, Vector3(1.0f, 1.0f, 1.0f), {box});
	}
	for (auto i = 0; i < BOXSTACK_COUNT; i++) {
		entity = new Object3D("box" + to_string(BOX_COUNT + i), boxModel);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->setTranslation(Vector3(+3.0f, 1.6f + (i * 1.2f), -5.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("box" + to_string(BOX_COUNT + i), true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 100.0f, Vector3(1.0f, 1.0f, 1.0f), {box});
	}
	auto sphere = bvDeleter.add(new Sphere(Vector3(0.0f, 0.0f, 0.0f), 0.4f));
	auto sphereModel = modelDeleter.add(PrimitiveModel::createModel(sphere, "sphere_model"));
	sphereModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.5f, 0.8f, 0.8f, 1.0f));
	sphereModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(0.0f, 1.0f, 1.0f, 1.0f));
	for (auto i = 0; i < SPHERE_COUNT; i++) {
		entity = new Object3D("sphere" + to_string(i), sphereModel);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->setTranslation(Vector3(0.45f * i - 3.0f, 12.0f + (i * 1.0f), 0.1f * i - 3.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("sphere" + to_string(i), true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.75f, 0.4f, 10.0f, Vector3(1.0f, 1.0f, 1.0f), {sphere});
	}
	auto capsule = bvDeleter.add(new Capsule(Vector3(0.0f, 0.5f, 0.0f), Vector3(0.0f, -0.5f, 0.0f), 0.25f));
	auto capsuleModel = modelDeleter.add(PrimitiveModel::createModel(capsule, "capsule_model"));
	capsuleModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.0f, 0.8f, 1.0f));
	capsuleModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 0.0f, 1.0f, 1.0f));
	for (auto i = 0; i < CAPSULE_COUNT; i++) {
		entity = new Object3D("capsule" + to_string(i), capsuleModel);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->setTranslation(Vector3((i * 0.5f), 14.0f + (i * 2.0f), 0.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("capsule" + to_string(i), true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 0.4f, 3.0f, Vector3(1.0f, 1.0f, 1.0f), {capsule});
	}
	auto capsuleBig = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 0.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(0.5f, 1.0f, 0.5f)));
	auto capsuleBigModel = modelDeleter.add(PrimitiveModel::createModel(capsuleBig, "capsulebig_model"));
	capsuleBigModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(1.0f, 0.8f, 0.8f, 1.0f));
	capsuleBigModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 0.0f, 0.0f, 1.0f));
	entity = new Object3D("capsulebig1", capsuleBigModel);
	entity->setContributesShadows(true);
	entity->setReceivesShadows(true);
	entity->setTranslation(Vector3(-2.0f, 5.0f, 0.0f));
	entity->update();
	engine->addEntity(entity);
	world->addRigidBody("capsulebig1", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 80.0f, Body::getNoRotationInertiaTensor(), {capsuleBig});
	entity = new Object3D("capsulebig2", capsuleBigModel);
	entity->setContributesShadows(true);
	entity->setReceivesShadows(true);
	entity->setTranslation(Vector3(+2.0f, 5.0f, 0.0f));
	entity->update();
	engine->addEntity(entity);
	world->addRigidBody("capsulebig2", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 100.0f, Body::getNoRotationInertiaTensor(), {capsuleBig});
	try {
		auto _terrainModel = modelDeleter.add(ModelReader::read("resources/tests/environment/terrain_test", "terrain_test4.dae"));
		entity = new Object3D("terrain", _terrainModel);
		entity->setTranslation(Vector3(0.0f, -4.0f, 0.0f));
		entity->setScale(Vector3(2.0f, 2.0f, 1.0f));
		entity->setReceivesShadows(true);
		entity->update();
		entity->setShader("terrain");
		engine->addEntity(entity);
		Object3DModel terrainModel(_terrainModel);
		auto terrainMesh = new TerrainMesh(&terrainModel, entity->getTransformations());
		world->addStaticRigidBody("ground", true, RIGID_TYPEID_STANDARD, Transformations(), 0.5f, {terrainMesh});
		auto _barrel = modelDeleter.add(ModelReader::read("resources/tests/models/barrel", "barrel.dae"));
		auto barrelBoundingVolume = bvDeleter.add(new ConvexMesh(object3DModelDeleter.add(new Object3DModel(_barrel))));
		entity = new Object3D("barrel1", _barrel);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->setTranslation(Vector3(+4.0f, 5.0f, 0.0f));
		entity->setScale(Vector3(2.0f, 2.0f, 2.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("barrel1", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 100.0f, Vector3(1.0f, 1.0f, 1.0f), {barrelBoundingVolume});
		entity = new Object3D("barrel2", _barrel);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->setTranslation(Vector3(+6.0f, 5.0f, 0.0f));
		entity->setScale(Vector3(2.0f, 2.0f, 2.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("barrel2", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 100.0f, Vector3(1.0f, 1.0f, 1.0f), {barrelBoundingVolume});
		auto _cone = modelDeleter.add(ModelReader::read("resources/tests/models/cone", "cone.dae"));
		auto coneBoundingVolume = bvDeleter.add(new ConvexMesh(object3DModelDeleter.add(new Object3DModel(_cone))));
		entity = new Object3D("cone1", _cone);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->setTranslation(Vector3(-4.0f, 5.0f, 0.0f));
		entity->setScale(Vector3(3.0f, 3.0f, 3.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("cone1", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 100.0f, Vector3(1.0f, 1.0f, 1.0f), {coneBoundingVolume});
		entity = new Object3D("cone2", _cone);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->setTranslation(Vector3(-5.0f, 5.0f, 0.0f));
		entity->setScale(Vector3(3.0f, 3.0f, 3.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("cone2", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 100.0f, Vector3(1.0f, 1.0f, 1.0f), {coneBoundingVolume});
		auto _tire = modelDeleter.add(ModelReader::read("resources/tests/models/tire", "tire.dae"));
		auto tireBoundingVolume = bvDeleter.add(new ConvexMesh(object3DModelDeleter.add(new Object3DModel(_tire))));
		entity = new Object3D("tire1", _tire);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->addRotation(Vector3(1.0f, 0.0f, 0.0f), 90.0f);
		entity->setTranslation(Vector3(-4.0f, 5.0f, -2.0f));
		entity->setScale(Vector3(2.0f, 2.0f, 2.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("tire1", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 100.0f, Vector3(1.0f, 1.0f, 1.0f), {tireBoundingVolume});
		entity = new Object3D("tire2", _tire);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->addRotation(Vector3(1.0f, 0.0f, 0.0f), 90.0f);
		entity->setTranslation(Vector3(-6.0f, 5.0f, -2.0f));
		entity->setScale(Vector3(2.0f, 2.0f, 2.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("tire2", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 1.0f, 100.0f, Vector3(1.0f, 1.0f, 1.0f), {tireBoundingVolume});
	} catch (Exception& exception) {
		Console::print(string("PhysicsTest3::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
		Application::exit(1);
	}
}

void PhysicsTest3::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}

void PhysicsTest3::onChar(unsigned int key, int x, int y) {
}

void PhysicsTest3::onKeyDown (unsigned char key, int x, int y) {
	auto keyChar = tolower(key);
	if (keyChar == u'w') keyW = true;
	if (keyChar == u'a') keyA = true;
	if (keyChar == u's') keyS = true;
	if (keyChar == u'd') keyD = true;
}

void PhysicsTest3::onKeyUp(unsigned char key, int x, int y) {
	auto keyChar = tolower(key);
	if (keyChar == u'w') keyW = false;
	if (keyChar == u'a') keyA = false;
	if (keyChar == u's') keyS = false;
	if (keyChar == u'd') keyD = false;
}

void PhysicsTest3::onSpecialKeyDown (int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = true;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = true;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = true;
	if (key == KEYBOARD_KEYCODE_DOWN) keyDown = true;
}

void PhysicsTest3::onSpecialKeyUp(int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = false;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = false;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = false;
	if (key == KEYBOARD_KEYCODE_DOWN) keyDown = false;
}

void PhysicsTest3::onMouseDragged(int x, int y) {
}

void PhysicsTest3::onMouseMoved(int x, int y) {
}

void PhysicsTest3::onMouseButton(int button, int state, int x, int y) {
}

void PhysicsTest3::onMouseWheel(int button, int direction, int x, int y) {
}
