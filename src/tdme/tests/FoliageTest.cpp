#include <tdme/tests/FoliageTest.h>

#include <string>

#include <tdme/utilities/Time.h>

#include <tdme/application/Application.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Object3DRenderGroup.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/PrimitiveModel.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Time.h>
#include <tdme/utilities/ObjectDeleter.h>

using std::string;
using std::to_string;

using tdme::tests::FoliageTest;

using tdme::application::Application;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::Object3DRenderGroup;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::PrimitiveModel;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utilities::Console;
using tdme::utilities::Time;
using tdme::utilities::ObjectDeleter;

FoliageTest::FoliageTest()
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
}


void FoliageTest::main(int argc, char** argv)
{
	auto foliageTest = new FoliageTest();
	foliageTest->run(argc, argv, "FoliageTest", foliageTest);
}

void FoliageTest::display()
{
	auto camLookFrom = engine->getCamera()->getLookFrom();
	if (keyA == true) camLookFrom.add(Vector3(-20.0f / 60.0f, 0.0f, 0.0f));
	if (keyD == true) camLookFrom.add(Vector3(+20.0f / 60.0f, 0.0f, 0.0f));
	if (keyW == true) camLookFrom.add(Vector3(0.0f, 0.0f, -20.0f / 60.0f));
	if (keyS == true) camLookFrom.add(Vector3(0.0f, 0.0f, +20.0f / 60.0f));
	if (keyLeft == true) camRotationY+= 1.0f;
	if (keyRight == true) camRotationY-= 1.0f;
	Quaternion camRotationYQuaternion;
	camRotationYQuaternion.rotate(camRotationY, Rotation::Y_AXIS);
	auto camLookAt = engine->getCamera()->getLookAt();
	camRotationYQuaternion.multiply(Vector3(0.0f, 0.0f, -1.0f), camLookAt).normalize().scale(80.0f);
	engine->getCamera()->setLookFrom(camLookFrom);
	engine->getCamera()->setLookAt(camLookAt.add(engine->getCamera()->getLookFrom()));
	auto start = Time::getCurrentMillis();
	engine->display();
	auto end = Time::getCurrentMillis();
	auto rendererStatistics = engine->getRendererStatistics();
	Console::println(
		string("FoliageTest::display::") + to_string(end - start) + "ms; " +
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

void FoliageTest::dispose()
{
	engine->dispose();
}

void FoliageTest::initialize()
{
	engine->initialize();
	engine->setSceneColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	Object3D* entity;
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(50.0f);
	cam->setLookFrom(Vector3(0.0f, 30.0f/10.0f, 280.0f / 10.0f));
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
	groundModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.8f, 0.8f, 1.0f));
	groundModel->getMaterials()["tdme.primitive.material"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	entity = new Object3D("ground", groundModel);
	entity->setTranslation(Vector3(0.0f, -1.0f, 0.0f));
	entity->setReceivesShadows(true);
	entity->update();
	engine->addEntity(entity);
	auto reedModel = modelDeleter.add(ModelReader::read("resources/tests/models/reed", "Mesh_Environment_Reed_06.fbx.tm"));
	auto foliageObject = new Object3DRenderGroup("foliage");
	int reedIdx = 0;
	#if defined(GLES2)
		for (float z = -10.0f; z < 10.0f;) {
			for (float x = -10.0f; x < 10.0f;) {
				Transformations transformations;
				transformations.setTranslation(Vector3(x, 0.0f, z));
				transformations.update();
				foliageObject->addObject(reedModel, transformations);
				x+= Math::random() * 1.0f + 1.0;
			}
			z+= Math::random() * 1.0f + 1.0;
		}
	#else
		for (float z = -20.0f; z < 20.0f;) {
			for (float x = -20.0f; x < 20.0f;) {
				Transformations transformations;
				transformations.setTranslation(Vector3(x, 0.0f, z));
				transformations.update();
				foliageObject->addObject(reedModel, transformations);
				x+= Math::random() * 0.5f + 0.5;
			}
			z+= Math::random() * 0.5f + 0.5;
		}
	#endif
	foliageObject->setShader("foliage");
	foliageObject->setContributesShadows(true);
	foliageObject->setReceivesShadows(true);
	foliageObject->updateRenderGroup();
	engine->addEntity(foliageObject);
}

void FoliageTest::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}

void FoliageTest::onChar(unsigned int key, int x, int y) {
}

void FoliageTest::onKeyDown (unsigned char key, int x, int y) {
	auto keyChar = tolower(key);
	if (keyChar == u'w') keyW = true;
	if (keyChar == u'a') keyA = true;
	if (keyChar == u's') keyS = true;
	if (keyChar == u'd') keyD = true;
}

void FoliageTest::onKeyUp(unsigned char key, int x, int y) {
	auto keyChar = tolower(key);
	if (keyChar == u'w') keyW = false;
	if (keyChar == u'a') keyA = false;
	if (keyChar == u's') keyS = false;
	if (keyChar == u'd') keyD = false;
}

void FoliageTest::onSpecialKeyDown (int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = true;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = true;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = true;
	if (key == KEYBOARD_KEYCODE_DOWN) keyDown = true;
}

void FoliageTest::onSpecialKeyUp(int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = false;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = false;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = false;
	if (key == KEYBOARD_KEYCODE_DOWN) keyDown = false;
}

void FoliageTest::onMouseDragged(int x, int y) {
}

void FoliageTest::onMouseMoved(int x, int y) {
}

void FoliageTest::onMouseButton(int button, int state, int x, int y) {
}

void FoliageTest::onMouseWheel(int button, int direction, int x, int y) {
}
