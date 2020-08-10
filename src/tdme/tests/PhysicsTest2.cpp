#include <tdme/tests/PhysicsTest2.h>

#include <string>

#include <tdme/utils/Time.h>

#include <tdme/application/Application.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/physics/Body.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/PrimitiveModel.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utils/ObjectDeleter.h>

using std::string;
using std::to_string;

using tdme::tests::PhysicsTest2;

using tdme::utils::Time;

using tdme::application::Application;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::physics::Body;
using tdme::engine::physics::World;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::PrimitiveModel;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utils::ObjectDeleter;

constexpr int32_t PhysicsTest2::RIGID_TYPEID_STANDARD;

constexpr int32_t PhysicsTest2::BOX_COUNT;

PhysicsTest2::PhysicsTest2() 
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
	world = new World();
}

PhysicsTest2::~PhysicsTest2()
{
	delete world;
}

void PhysicsTest2::main(int argc, char** argv)
{
	auto physicsTest2 = new PhysicsTest2();
	physicsTest2->run(argc, argv, "PhysicsTest2");
}

void PhysicsTest2::display()
{
	auto start = Time::getCurrentMillis();
	auto fps = 60.0f;
	world->update(1.0f / fps);
	world->synch(engine);
	engine->display();
	auto end = Time::getCurrentMillis();
	Console::println(string("PhysicsTest2::display::" + to_string(end - start) + "ms"));
}

void PhysicsTest2::dispose()
{
	engine->dispose();
}

void PhysicsTest2::initialize()
{
	engine->initialize();
	engine->addPostProcessingProgram("ssao");
	Object3D* entity;
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(15.0f);
	cam->setLookFrom(Vector3(0.0f, 30.0f, 30.0f));
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
	auto ground = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 0.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(30.0f, 1.0f, 30.0f)));
	auto groundModel = modelDeleter.add(PrimitiveModel::createModel(ground, "ground_model"));
	groundModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.8f, 0.8f, 1.0f));
	groundModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	entity = new Object3D("ground", groundModel);
	entity->setTranslation(Vector3(0.0f, -1.0f, 0.0f));
	entity->update();
	entity->setReceivesShadows(true);
	engine->addEntity(entity);
	world->addStaticRigidBody("ground", true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.5f, {ground});
	auto box = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 0.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(1.0f, 1.0f, 1.0f)));
	auto boxModel = modelDeleter.add(PrimitiveModel::createModel(box, "box_model"));
	boxModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.5f, 0.5f, 1.0f));
	boxModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 0.0f, 0.0f, 1.0f));
	for (auto i = 0; i < BOX_COUNT; i++) {
		entity = new Object3D("box" + to_string(i), boxModel);
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->setTranslation(Vector3(0.0f, i * 2.0f + 1.0f, 0.0f));
		entity->update();
		engine->addEntity(entity);
		world->addRigidBody("box" + to_string(i), true, RIGID_TYPEID_STANDARD, entity->getTransformations(), 0.0f, 0.8f, 100.0f, Vector3(1.0f, 1.0f, 1.0f), {box});
	}
}

void PhysicsTest2::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}
