#include <tdme/tests/WaterTest.h>

#include <string>

#include <tdme/application/Application.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Model.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/ObjectDeleter.h>
#include <tdme/utilities/Time.h>

using std::string;
using std::to_string;

using tdme::tests::WaterTest;

using tdme::utilities::Time;

using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Model;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utilities::Console;
using tdme::utilities::ObjectDeleter;

WaterTest::WaterTest()
{
	using tdme::application::Application;
	engine = Engine::getInstance();
}


void WaterTest::main(int argc, char** argv)
{
	auto waterTest = new WaterTest();
	waterTest->run(argc, argv, "WaterTest");
}

void WaterTest::display()
{
	auto start = Time::getCurrentMillis();
	engine->display();
	auto end = Time::getCurrentMillis();
	auto rendererStatistics = engine->getRendererStatistics();
	Console::println(
		string("WaterTest::display::") + to_string(end - start) + "ms; " +
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

void WaterTest::dispose()
{
	engine->dispose();
}

void WaterTest::initialize()
{
	engine->initialize();
	engine->addPostProcessingProgram("ssao");
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(100.0f);
	cam->setLookFrom(Vector3(0.0f, 2.5f, 5.0f));
	cam->setLookAt(Vector3(0.0f, 0.0f, 0.0f));
	cam->setUpVector(cam->computeUpVector(cam->getLookFrom(), cam->getLookAt()));
	auto light0 = engine->getLightAt(0);
	light0->setAmbient(Color4(0.75f, 0.75f, 0.75f, 1.0f));
	light0->setDiffuse(Color4(0.40f, 0.40f, 0.40f, 1.0f));
	light0->setSpecular(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	light0->setPosition(Vector4(0.0f, 20000.0f, 0.0f, 1.0f));
	light0->setSpotDirection(Vector3(0.0f, 0.0f, 0.0f).sub(Vector3(light0->getPosition().getX(), light0->getPosition().getY(), light0->getPosition().getZ())));
	light0->setConstantAttenuation(0.5f);
	light0->setLinearAttenuation(0.0f);
	light0->setQuadraticAttenuation(0.0f);
	light0->setSpotExponent(0.0f);
	light0->setSpotCutOff(180.0f);
	light0->setEnabled(true);
	auto entity = new Object3D("ground", modelDeleter.add(ModelReader::read("resources/tests/water", "Water.tm")));
	entity->setShader("water");
	engine->addEntity(entity);
}

void WaterTest::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}
