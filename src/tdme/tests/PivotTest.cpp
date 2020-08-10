#include <tdme/tests/PivotTest.h>

#include <string>

#include <tdme/application/Application.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/PrimitiveModel.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utils/Console.h>
#include <tdme/utils/ObjectDeleter.h>
#include <tdme/utils/Time.h>

using std::string;
using std::to_string;

using tdme::tests::PivotTest;

using tdme::application::Application;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::PrimitiveModel;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utils::Console;
using tdme::utils::ObjectDeleter;
using tdme::utils::Time;

PivotTest::PivotTest()
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
}


void PivotTest::main(int argc, char** argv)
{
	auto pivotTest = new PivotTest();
	pivotTest->run(argc, argv, "PivotTest");
}

void PivotTest::display()
{
	auto fps = 60.0f;
	auto start = Time::getCurrentMillis();
	auto entity = engine->getEntity("box");
	entity->getRotation(0).setAngle(entity->getRotation(1).getAngle() + 0.1f);
	entity->getRotation(1).setAngle(entity->getRotation(1).getAngle() + 0.1f);
	entity->getRotation(2).setAngle(entity->getRotation(1).getAngle() + 0.1f);
	entity->update();
	engine->display();
	auto end = Time::getCurrentMillis();
	Console::println(string("PivotTest::display::" + to_string(end - start) + "ms"));
}

void PivotTest::dispose()
{
	engine->dispose();
}

void PivotTest::initialize()
{
	engine->initialize();
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
	entity->setReceivesShadows(true);
	entity->update();
	engine->addEntity(entity);
	auto box = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 10.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(1.0f, 1.0f, 1.0f)));
	auto boxModel = modelDeleter.add(PrimitiveModel::createModel(box, "box_model"));
	boxModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.5f, 0.5f, 1.0f));
	boxModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 0.0f, 0.0f, 1.0f));
	entity = new Object3D("box", boxModel);
	entity->setContributesShadows(true);
	entity->setReceivesShadows(true);
	entity->setPivot(Vector3(0.0f, 10.0f, 0.0f));
	entity->addRotation(Vector3(1.0f, 0.0f, 0.0f), 0.0f);
	entity->addRotation(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
	entity->addRotation(Vector3(0.0f, 0.0f, 1.0f), 0.0f);
	entity->update();
	engine->addEntity(entity);
}

void PivotTest::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}
