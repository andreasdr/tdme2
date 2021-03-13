#include <tdme/tests/WaterTest.h>

#include <string>

#include <tdme/application/Application.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/fileio/scenes/SceneReader.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utilities/Character.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/utilities/ObjectDeleter.h>
#include <tdme/utilities/Time.h>

using std::string;
using std::to_string;

using tdme::tests::WaterTest;

using tdme::utilities::Time;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::fileio::scenes::SceneReader;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::prototype::Prototype;
using tdme::engine::scene::Scene;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::SceneConnector;
using tdme::engine::SceneConnector;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utilities::Character;
using tdme::utilities::Console;
using tdme::utilities::ModelTools;
using tdme::utilities::ObjectDeleter;
using tdme::utilities::Time;

WaterTest::WaterTest()
{
	engine = Engine::getInstance();
}


void WaterTest::main(int argc, char** argv)
{
	auto waterTest = new WaterTest();
	waterTest->run(argc, argv, "WaterTest", waterTest);
}

void WaterTest::display()
{
	// animate sky dome
	{
		auto skyDome = static_cast<Object3D*>(engine->getEntity("sky_dome"));
		skyDome->setTextureMatrix((Matrix2D3x3()).identity().translate(Vector2(0.0f, skyDomeTranslation * 0.01f)));

		auto skyPanorama = engine->getEntity("sky_panorama");
		skyPanorama->setRotationAngle(0, skyDomeTranslation * 1.0f * 0.1f);
		skyPanorama->update();

		skyDomeTranslation+= 1.0f / 60.0;
	}

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

	SceneReader::read("resources/tests/levels/water", "Level_WaterShader.tl", scene);
	SceneConnector::setLights(engine, scene);
	SceneConnector::addScene(engine, scene, false, false, false, false);

	// load sky
	skySpherePrototype = PrototypeReader::read("resources/engine/models", "sky_sphere.tmm");
	skyDomePrototype = PrototypeReader::read("resources/engine/models", "sky_dome.tmm");
	skyPanoramaPrototype = PrototypeReader::read("resources/engine/models", "sky_panorama.tmm");
	spherePrototype = PrototypeReader::read("resources/tests/levels/water/", "CM_Sphere.tmm");

	// add sky
	{
		// sky sphere
		auto skySphere = new Object3D("sky_sphere", skySpherePrototype->getModel());
		skySphere->setRenderPass(Entity::RENDERPASS_NOFRUSTUMCULLING);
		skySphere->setShader("sky");
		skySphere->setFrustumCulling(false);
		skySphere->setTranslation(Vector3(0.0f, 0.0f, 0.0f));
		skySphere->setScale(Vector3(300.0f/200.0f, 300.0f/200.0f, 300.0f/200.0f));
		skySphere->update();
		skySphere->setContributesShadows(false);
		skySphere->setReceivesShadows(false);
		skySphere->setExcludeEffectPass(Engine::EFFECTPASS_LIGHTSCATTERING);
		engine->addEntity(skySphere);

		// sky dome
		auto skyDome = new Object3D("sky_dome", skyDomePrototype->getModel());
		skyDome->setRenderPass(Entity::RENDERPASS_NOFRUSTUMCULLING);
		skyDome->setShader("sky");
		skyDome->setFrustumCulling(false);
		skyDome->setTranslation(Vector3(0.0f, 0.0f, 0.0f));
		skyDome->setScale(Vector3(295.0f/190.0f, 295.0f/190.0f, 295.0f/190.0f));
		skyDome->getModel()->getMaterials().begin()->second->getSpecularMaterialProperties()->setDiffuseTextureMaskedTransparency(true);
		skyDome->update();
		skyDome->setContributesShadows(false);
		skyDome->setReceivesShadows(false);
		skyDome->setEffectColorMul(Color4(1.0f, 1.0f, 1.0f, 0.7f));
		skyDome->setExcludeEffectPass(Engine::EFFECTPASS_LIGHTSCATTERING);
		engine->addEntity(skyDome);

		// sky panorama
		auto skyPanorama = new Object3D("sky_panorama", skyPanoramaPrototype->getModel());
		skyPanorama->setRenderPass(Entity::RENDERPASS_NOFRUSTUMCULLING);
		skyPanorama->setShader("sky");
		skyPanorama->setFrustumCulling(false);
		skyPanorama->setTranslation(Vector3(0.0f, 0.0f, 0.0f));
		skyPanorama->setScale(Vector3(280.0f/190.0f, 280.0f/180.0f, 280.0f/180.0f));
		skyPanorama->addRotation(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
		skyPanorama->update();
		skyPanorama->setContributesShadows(false);
		skyPanorama->setReceivesShadows(false);
		skyPanorama->setExcludeEffectPass(Engine::EFFECTPASS_LIGHTSCATTERING);
		engine->addEntity(skyPanorama);
	}

	{
		// sphere
		spherePrototype->getModel()->getMaterials()["CM_Shader_Sphere_M"]->getSpecularMaterialProperties()->setReflection(1.0f);
		auto sphere = new Object3D("sphere", spherePrototype->getModel());
		sphere->setScale(Vector3(5.0f, 5.0f, 5.0f));
		sphere->setTranslation(Vector3(0.0f, 20.0f, 0.0f));
		sphere->setReflectionEnvironmentMappingId("New environment mapping_2");
		sphere->setReflectionEnvironmentMappingPosition(sphere->getTranslation());
		sphere->update();
		engine->addEntity(sphere);
	}

	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(150.0f);
	cam->setLookFrom(Vector3(0.0f, 30.0f, 50.0f));
	cam->setLookAt(Vector3(0.0f, 0.0f, 0.0f));
	cam->setUpVector(Camera::computeUpVector(cam->getLookFrom(), cam->getLookAt()));
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
}

void WaterTest::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}

void WaterTest::onChar(unsigned int key, int x, int y) {
}

void WaterTest::onKeyDown (unsigned char key, int x, int y) {
	auto keyChar = Character::toLowerCase(key);
	if (keyChar == 'w') keyW = true;
	if (keyChar == 'a') keyA = true;
	if (keyChar == 's') keyS = true;
	if (keyChar == 'd') keyD = true;
	if (keyChar == '-') keyMinus = true;
	if (keyChar == '+') keyPlus = true;
}

void WaterTest::onKeyUp(unsigned char key, int x, int y) {
	auto keyChar = Character::toLowerCase(key);
	if (keyChar == 'w') keyW = false;
	if (keyChar == 'a') keyA = false;
	if (keyChar == 's') keyS = false;
	if (keyChar == 'd') keyD = false;
	if (keyChar == '-') keyMinus = false;
	if (keyChar == '+') keyPlus = false;
}

void WaterTest::onSpecialKeyDown (int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = true;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = true;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = true;
	if (key == KEYBOARD_KEYCODE_DOWN) keyDown = true;
}

void WaterTest::onSpecialKeyUp(int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = false;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = false;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = false;
	if (key == KEYBOARD_KEYCODE_DOWN) keyDown = false;
}

void WaterTest::onMouseDragged(int x, int y) {
}

void WaterTest::onMouseMoved(int x, int y) {
}

void WaterTest::onMouseButton(int button, int state, int x, int y) {
}

void WaterTest::onMouseWheel(int button, int direction, int x, int y) {
}
