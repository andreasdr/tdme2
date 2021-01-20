#include <tdme/tests/LODTest.h>

#include <string>

#include <tdme/utilities/Time.h>

#include <tdme/application/Application.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Rotation.h>
#include <tdme/math/Math.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/ObjectDeleter.h>
#include <tdme/utilities/Primitives.h>
#include <tdme/utilities/Time.h>

using std::string;
using std::to_string;

using tdme::tests::LODTest;

using tdme::application::Application;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::LODObject3D;
using tdme::engine::Object3D;
using tdme::engine::Rotation;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utilities::Character;
using tdme::utilities::Console;
using tdme::utilities::ObjectDeleter;
using tdme::utilities::Primitives;
using tdme::utilities::Time;

LODTest::LODTest()
{
	Application::setLimitFPS(true);
	engine = Engine::getInstance();
}


void LODTest::main(int argc, char** argv)
{
	auto lodTest = new LODTest();
	lodTest->run(argc, argv, "LODTest", lodTest);
}

void LODTest::display()
{
	// camera
	auto camLookFrom = engine->getCamera()->getLookFrom();
	if (keyMinus == true) camLookFrom.add(Vector3(0.0f, -20.0f / 60.0f, 0.0f));
	if (keyPlus == true) camLookFrom.add(Vector3(0.0f, +20.0f / 60.0f, 0.0f));
	if (keyLeft == true) camRotationY+= 1.0f;
	if (keyRight == true) camRotationY-= 1.0f;
	if (keyDown == true) camRotationX+= 1.0f;
	if (keyUp == true) camRotationX-= 1.0f;

	Quaternion camRotationYQuaternion;
	camRotationYQuaternion.rotate(Rotation::Y_AXIS, camRotationY);
	Quaternion camRotationXQuaternion;
	camRotationXQuaternion.rotate(Rotation::X_AXIS, camRotationX);
	Quaternion camRotationQuaternion;
	camRotationQuaternion.set(camRotationYQuaternion).multiply(camRotationXQuaternion);
	Vector3 camLookAt;
	camRotationQuaternion.multiply(Vector3(0.0f, 0.0f, -1.0f), camLookAt);

	auto forwardVector = camLookAt;
	auto sideVector = Vector3();
	Vector3::computeCrossProduct(forwardVector.normalize(), Vector3(0.0f, 1.0f, 0.0f), sideVector).normalize();

	if (keyA == true) camLookFrom.add(sideVector.clone().scale(-20.0f / 60.0f));
	if (keyD == true) camLookFrom.add(sideVector.clone().scale(+20.0f / 60.0f));
	if (keyW == true) camLookFrom.add(forwardVector.clone().scale(+20.0f / 60.0f));
	if (keyS == true) camLookFrom.add(forwardVector.clone().scale(-20.0f / 60.0f));

	engine->getCamera()->setLookFrom(camLookFrom);
	engine->getCamera()->setLookAt(camLookFrom.clone().add(camLookAt.scale(25.0f)));
	engine->getCamera()->setUpVector(Camera::computeUpVector(engine->getCamera()->getLookFrom(), engine->getCamera()->getLookAt()));

	// rendering
	auto start = Time::getCurrentMillis();
	engine->display();
	auto end = Time::getCurrentMillis();
	auto rendererStatistics = engine->getRendererStatistics();
	Console::println(
		string("LODTest::display::") + to_string(end - start) + "ms; " +
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

void LODTest::dispose()
{
	engine->dispose();
}

void LODTest::initialize()
{
	engine->initialize();
	engine->setSceneColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	Object3D* entity;
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(150.0f);
	cam->setLookFrom(Vector3(0.0f, 25.0f, 280.0f));
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
	auto ground = bvDeleter.add(new OrientedBoundingBox(Vector3(0.0f, 0.0f, 0.0f), OrientedBoundingBox::AABB_AXIS_X, OrientedBoundingBox::AABB_AXIS_Y, OrientedBoundingBox::AABB_AXIS_Z, Vector3(240.0f, 1.0f, 240.0f)));
	auto groundModel = modelDeleter.add(Primitives::createModel(ground, "ground_model"));
	groundModel->getMaterials()["primitive"]->getSpecularMaterialProperties()->setAmbientColor(Color4(0.8f, 0.8f, 0.8f, 1.0f));
	groundModel->getMaterials()["primitive"]->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	entity = new Object3D("ground", groundModel);
	entity->setTranslation(Vector3(0.0f, -1.0f, 0.0f));
	entity->setReceivesShadows(true);
	entity->update();
	engine->addEntity(entity);
	auto treePine = modelDeleter.add(ModelReader::read("resources/tests/models/lod-tree", "Mesh_Environment_Tree_Pine_03.tm"));
	auto treePineLOD2 = modelDeleter.add(ModelReader::read("resources/tests/models/lod-tree", "Mesh_Environment_Tree_Pine_03_LOD_Plane.tm"));
	int treeIdx = 0;
	for (float z = -240.0f; z < 240.0f; z+= 5.0f)
	for (float x = -240.0f; x < 240.0f; x+= 5.0f) {
		auto entity = new LODObject3D(
			"tree." + to_string(treeIdx++),
			treePine,
			LODObject3D::LODLEVELTYPE_MODEL,
			100.0f,
			treePineLOD2,
			LODObject3D::LODLEVELTYPE_NONE,
			0.0f,
			nullptr
		);
		// try to fix missing/different lighting of LOD2 object plane
		entity->setEffectColorMulLOD2(Color4(3.7f, 3.7f, 3.7f, 1.0f));
		entity->addRotation(Vector3(0.0f, 1.0f, 0.0f), Math::random() * 360.0f);
		float scale = 1.0f + Math::random() / 3.0f;
		entity->setScale(
			Vector3(
				Math::random() < 0.5f?scale:-scale,
				scale,
				Math::random() < 0.5f?scale:-scale
			)
		);
		entity->setTranslation(Vector3(x, 0.0f, z));
		entity->setContributesShadows(true);
		entity->setReceivesShadows(true);
		entity->update();
		engine->addEntity(entity);
	}
}

void LODTest::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}

void LODTest::onChar(unsigned int key, int x, int y) {
}

void LODTest::onKeyDown (unsigned char key, int x, int y) {
	auto keyChar = Character::toLowerCase(key);
	if (keyChar == 'w') keyW = true;
	if (keyChar == 'a') keyA = true;
	if (keyChar == 's') keyS = true;
	if (keyChar == 'd') keyD = true;
	if (keyChar == '-') keyMinus = true;
	if (keyChar == '+') keyPlus = true;
}

void LODTest::onKeyUp(unsigned char key, int x, int y) {
	auto keyChar = Character::toLowerCase(key);
	if (keyChar == 'w') keyW = false;
	if (keyChar == 'a') keyA = false;
	if (keyChar == 's') keyS = false;
	if (keyChar == 'd') keyD = false;
	if (keyChar == '-') keyMinus = false;
	if (keyChar == '+') keyPlus = false;
}

void LODTest::onSpecialKeyDown (int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = true;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = true;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = true;
	if (key == KEYBOARD_KEYCODE_DOWN) keyDown = true;
}

void LODTest::onSpecialKeyUp(int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = false;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = false;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = false;
	if (key == KEYBOARD_KEYCODE_DOWN) keyDown = false;
}

void LODTest::onMouseDragged(int x, int y) {
}

void LODTest::onMouseMoved(int x, int y) {
}

void LODTest::onMouseButton(int button, int state, int x, int y) {
}

void LODTest::onMouseWheel(int button, int direction, int x, int y) {
}
