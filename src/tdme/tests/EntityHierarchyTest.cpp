#include <tdme/tests/EntityHierarchyTest.h>

#include <string>

#include <tdme/utilities/Time.h>

#include <tdme/application/Application.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/EntityHierarchy.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/ParticleSystemEntity.h>
#include <tdme/engine/ParticleSystemGroup.h>
#include <tdme/engine/PointsParticleSystem.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/PrimitiveModel.h>
#include <tdme/engine/subsystems/particlesystem/SphereParticleEmitter.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/ObjectDeleter.h>

using std::string;
using std::to_string;

using tdme::tests::EntityHierarchyTest;

using tdme::utilities::Time;

using tdme::application::Application;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::EntityHierarchy;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::ParticleSystemEntity;
using tdme::engine::ParticleSystemGroup;
using tdme::engine::PointsParticleSystem;
using tdme::engine::Rotation;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::PrimitiveModel;
using tdme::engine::subsystems::particlesystem::SphereParticleEmitter;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utilities::Console;
using tdme::utilities::ObjectDeleter;

EntityHierarchyTest::EntityHierarchyTest()
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
}


void EntityHierarchyTest::main(int argc, char** argv)
{
	auto entityHierarchyTest = new EntityHierarchyTest();
	entityHierarchyTest->run(argc, argv, "EntityHierarchyTest");
}

void EntityHierarchyTest::display()
{
	auto fps = 60.0f;
	auto start = Time::getCurrentMillis();
	auto entity = engine->getEntity("test");
	entity->getRotation(1).setAngle(entity->getRotation(1).getAngle() + 0.1f);
	entity->update();
	engine->display();
	auto end = Time::getCurrentMillis();
	auto rendererStatistics = engine->getRendererStatistics();
	Console::println(
		string("EntityHierarchyTest::display::") + to_string(end - start) + "ms; " +
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

void EntityHierarchyTest::dispose()
{
	engine->dispose();
}

void EntityHierarchyTest::initialize()
{
	engine->initialize();
	Entity* entity = nullptr;
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(25.0f);
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
	entity->setReceivesShadows(true);
	entity->update();
	engine->addEntity(entity);
	auto box = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 1.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(1.0f, 1.0f, 1.0f)));
	auto boxModel = modelDeleter.add(PrimitiveModel::createModel(box, "box_model"));
	boxModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.5f, 0.5f, 1.0f));
	boxModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 0.0f, 0.0f, 1.0f));
	auto entityHierarchy = new EntityHierarchy("test");
	entityHierarchy->setContributesShadows(true);
	entityHierarchy->setReceivesShadows(true);
	entityHierarchy->addRotation(Vector3(1.0f, 0.0f, 0.0f), 0.0f);
	entityHierarchy->addRotation(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
	entityHierarchy->addRotation(Vector3(0.0f, 0.0f, 1.0f), 0.0f);
	entityHierarchy->setTranslation(Vector3(-5.0f, 0.0f, 0.0f));
	entityHierarchy->addEntity(entity = new Object3D("root", boxModel)); entity->setTranslation(Vector3(0.0f, 0.0f, 0.0f));
	entityHierarchy->addEntity(entity = new Object3D("child1", boxModel), "root"); entity->setTranslation(Vector3(-3.0f, 2.0f, -3.0f));
	entityHierarchy->addEntity(entity = new Object3D("child2", boxModel), "root"); entity->setTranslation(Vector3(+3.0f, 2.0f, -3.0f));
	entityHierarchy->addEntity(entity = new Object3D("child3", boxModel), "root"); entity->setTranslation(Vector3(-3.0f, 2.0f, +3.0f));
	entityHierarchy->addEntity(entity = new Object3D("child4", boxModel), "root"); entity->setTranslation(Vector3(+3.0f, 2.0f, +3.0f));
	entityHierarchy->addEntity(entity = new Object3D("child1.1", boxModel), "child1"); entity->setTranslation(Vector3(-1.5f, 2.0f, -1.5f));
	entityHierarchy->addEntity(entity = new Object3D("child1.2", boxModel), "child1"); entity->setTranslation(Vector3(+1.5f, 2.0f, -1.5f));
	entityHierarchy->addEntity(entity = new Object3D("child1.3", boxModel), "child1"); entity->setTranslation(Vector3(-1.5f, 2.0f, +1.5f));
	entityHierarchy->addEntity(entity = new Object3D("child1.4", boxModel), "child1"); entity->setTranslation(Vector3(+1.5f, 2.0f, +1.5f));
	entityHierarchy->addEntity(
		entity = new ParticleSystemGroup(
			"fire",
			true,
			false,
			false,
			{
				new PointsParticleSystem("firebase", new SphereParticleEmitter(2048, 1024, 2048, 0, 0, new Sphere(Vector3(0.0f, 0.2f, 0.0f), 0.2f), Vector3(0.0f, 0.1f, 0.0f), Vector3(0.0f, 0.1f, 0.0f), Color4(0.0f, 0.0f, 0.0f, 0.5f), Color4(0.4f, 0.0f, 0.0f, 0.5f)), 2048, 0.1f, true),
				new PointsParticleSystem("firetop", new SphereParticleEmitter(2048, 1024, 2048, 0, 0, new Sphere(Vector3(0.0f, 0.7f, 0.0f), 0.1f), Vector3(0.0f, 0.06f, 0.0f), Vector3(0.0f, 0.12f, 0.0f), Color4(0.75f, 0.0f, 0.0f, 0.5f), Color4(1.0f, 1.0f, 0.0f, 0.5f)), 2048, 0.1f, true),
				new PointsParticleSystem("firesmoke", new SphereParticleEmitter(2048, 1024, 2048, 0, 0, new Sphere(Vector3(0.0f, 0.7f, 0.0f), 0.1f), Vector3(0.0f, 0.2f, 0.0f), Vector3(0.0f, 0.4f, 0.0f), Color4(0.8f, 0.8f, 0.8f, 0.1f), Color4(0.8f, 0.8f, 0.8f, 0.1f)), 2048, 0.1f, true)
			}
		),
		"child2"
	);
	entity->setTranslation(Vector3(0.0f, 2.0f, 0.0f));
	entityHierarchy->addEntity(entity = new PointsParticleSystem("firebase", new SphereParticleEmitter(2048, 1024, 2048, 0, 0, new Sphere(Vector3(0.0f, 0.2f, 0.0f), 0.2f), Vector3(0.0f, 0.1f, 0.0f), Vector3(0.0f, 0.1f, 0.0f), Color4(0.0f, 0.0f, 0.0f, 0.5f), Color4(0.4f, 0.0f, 0.0f, 0.5f)), 2048, 0.1f, true), "child3");
	entity->setTranslation(Vector3(0.0f, 2.0f, 0.0f));
	entityHierarchy->update();
	for(auto entity: entityHierarchy->query()) Console::println(entity->getId());
	for(auto entity: entityHierarchy->query("root")) Console::println("root: " + entity->getId());
	for(auto entity: entityHierarchy->query("child1")) Console::println("child1: " + entity->getId());
	for(auto entity: entityHierarchy->query("child2")) Console::println("child2: " + entity->getId());
	for(auto entity: entityHierarchy->query("child3")) Console::println("child3: " + entity->getId());
	for(auto entity: entityHierarchy->query("child4")) Console::println("child4: " + entity->getId());
	engine->addEntity(entityHierarchy);
}

void EntityHierarchyTest::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}
