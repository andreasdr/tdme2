#include <tdme/tests/EngineTest.h>

#include <cctype>
#include <string>
#include <vector>

#include <tdme/application/Application.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/Object3DModel.h>
#include <tdme/engine/ParticleSystemEntity.h>
#include <tdme/engine/ParticleSystemGroup.h>
#include <tdme/engine/PointsParticleSystem.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/subsystems/particlesystem/BoundingBoxParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/CircleParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/ParticleEmitter.h>
#include <tdme/engine/subsystems/particlesystem/SphereParticleEmitter.h>
#include <tdme/math/Matrix2D3x3.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector2.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/ObjectDeleter.h>

using std::tolower;
using std::to_string;
using std::vector;

using tdme::tests::EngineTest;
using tdme::application::Application;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::Object3DModel;
using tdme::engine::ParticleSystemEntity;
using tdme::engine::ParticleSystemGroup;
using tdme::engine::PointsParticleSystem;
using tdme::engine::Rotation;
using tdme::engine::Timing;
using tdme::engine::Transformations;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Node;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::utilities::ModelTools;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::model::UpVector;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::subsystems::particlesystem::BoundingBoxParticleEmitter;
using tdme::engine::subsystems::particlesystem::CircleParticleEmitter;
using tdme::engine::subsystems::particlesystem::ParticleEmitter;
using tdme::engine::subsystems::particlesystem::SphereParticleEmitter;
using tdme::math::Matrix2D3x3;
using tdme::math::Quaternion;
using tdme::math::Vector2;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ObjectDeleter;

EngineTest::EngineTest()
{
	Application::setLimitFPS(true);
	keyLeft = false;
	keyRight = false;
	keyUp = false;
	keyW = false;
	keyA = false;
	keyS = false;
	keyD = false;
	mouseClicked = false;
	entityClicked = nullptr;
	engine = Engine::getInstance();
}

EngineTest::~EngineTest()
{
	delete osEngine;
}
void EngineTest::main(int argc, char** argv)
{
	auto engineTest = new EngineTest();
	engineTest->run(argc, argv, "EngineTest", engineTest);
}

Model* EngineTest::createWallModel()
{
	auto wall = new Model("wall", "wall", UpVector::Y_UP, RotationOrder::XYZ, nullptr);
	auto wallMaterial = new Material("wall");
	wallMaterial->setSpecularMaterialProperties(new SpecularMaterialProperties());
	wallMaterial->getSpecularMaterialProperties()->setAmbientColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	wallMaterial->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	wall->getMaterials()["wall"] = wallMaterial;
	auto wallNode = new Node(wall, nullptr, "wall", "wall");
	vector<FacesEntity> nodeFacesEntities;
	vector<Vector3> vertices;
	vertices.push_back(Vector3(-4.0f, 0.0f, +4.0f));
	vertices.push_back(Vector3(-4.0f, +4.0f, +4.0f));
	vertices.push_back(Vector3(+4.0f, +4.0f, +4.0f));
	vertices.push_back(Vector3(+4.0f, 0.0f, +4.0f));
	vector<Vector3> normals;
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	vector<TextureCoordinate> textureCoordinates;
	textureCoordinates.push_back(TextureCoordinate(0.0f, 0.0f));
	textureCoordinates.push_back(TextureCoordinate(0.0f, 1.0f));
	textureCoordinates.push_back(TextureCoordinate(1.0f, 1.0f));
	textureCoordinates.push_back(TextureCoordinate(1.0f, 0.0f));
	vector<Face> facesFarPlane;
	facesFarPlane.push_back(Face(wallNode, 0, 1, 2, 0, 0, 0, 0, 1, 2));
	facesFarPlane.push_back(Face(wallNode, 2, 3, 0, 0, 0, 0, 2, 3, 0));
	FacesEntity nodeFacesEntityFarPlane(wallNode, "wall");
	nodeFacesEntityFarPlane.setMaterial(wallMaterial);
	nodeFacesEntityFarPlane.setFaces(facesFarPlane);
	nodeFacesEntities.push_back(nodeFacesEntityFarPlane);
	wallNode->setVertices(vertices);
	wallNode->setNormals(normals);
	wallNode->setTextureCoordinates(textureCoordinates);
	wallNode->setFacesEntities(nodeFacesEntities);
	wall->getNodes()["wall"] = wallNode;
	wall->getSubNodes()["wall"] = wallNode;
	ModelTools::prepareForIndexedRendering(wall);
	return wall;
}

void EngineTest::display()
{
	auto circleTranslation = circleTransformations.getTranslation();
	circleTranslation.setX(players[0]->getTranslation().getX());
	circleTranslation.setZ(players[0]->getTranslation().getZ());
	circleTranslation.add(Vector3(0.0f, 0.1f, 0.0f));
	if (circleTranslation.getY() > 1.5f) {
		circleTranslation.setY(0.0f);
	}
	circleTransformations.setTranslation(circleTranslation);
	circleTransformations.update();
	(dynamic_cast< ParticleSystemEntity* >(engine->getEntity("circle")))->fromTransformations(circleTransformations);
	doPlayerControl(0, keyLeft, keyRight, keyUp);
	doPlayerControl(1, keyA, keyD, keyW);
	osEngine->display();
	engine->display();
	if (mouseClicked == true) {
		if (entityClicked != nullptr) {
			entityClicked->setEffectColorMul(Color4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		Vector3 clickWorldPosition;
		engine->computeWorldCoordinateByMousePosition(mouseClickedXY[0], mouseClickedXY[1], clickWorldPosition);
		Console::println(
			"EngineTest::display(): clicked@" +
			to_string(clickWorldPosition.getX()) + ", " +
			to_string(clickWorldPosition.getY()) + ", " +
			to_string(clickWorldPosition.getZ())
		);
		auto _object3DClicked = engine->getEntityByMousePosition(mouseClickedXY[0], mouseClickedXY[1]);
		if (_object3DClicked != nullptr) {
			_object3DClicked->setEffectColorMul(Color4(2.0f, 2.0f, 2.0f, 1.0f));
		}
		entityClicked = _object3DClicked;
		mouseClicked = false;
	}
}

void EngineTest::doPlayerControl(int32_t idx, bool keyLeft, bool keyRight, bool keyUp)
{
	auto fps = engine->getTiming()->getCurrentFPS();
	auto player = players[idx];
	auto& r = player->getRotation(0);
	player->update();
	Vector3 movement;
	if (keyRight)
		r.setAngle(r.getAngle() - (135.0f / fps));

	if (keyLeft)
		r.setAngle(r.getAngle() + (135.0f / fps));

	if (keyRight || keyLeft) {
		player->update();
	}
	if (keyUp) {
		r.getQuaternion().multiply(Vector3(0.0f, 0.0f, 1.0f), movement);
		movement.scale(1.5f / fps);
		player->setTranslation(player->getTranslation().clone().add(movement));
		player->update();
		if (player->getAnimation() != "walk") {
			player->setAnimation("walk");
		}
	} else {
		if (player->getAnimation() == "walk") {
			player->setAnimation("still");
		}
	}
}

void EngineTest::dispose()
{
	engine->dispose();
	osEngine->dispose();
}

void EngineTest::initialize()
{
	engine->initialize();
	engine->addPostProcessingProgram("ssao");
	if (osEngine == nullptr) {
		osEngine = Engine::createOffScreenInstance(512, 512, false);
		auto osLight0 = osEngine->getLightAt(0);
		osLight0->setAmbient(Color4(1.0f, 1.0f, 1.0f, 1.0f));
		osLight0->setDiffuse(Color4(1.0f, 1.0f, 1.0f, 1.0f));
		osLight0->setPosition(Vector4(0.0f, -4.0f, -4.0f, 1.0f));
		osLight0->setSpotDirection(Vector3(osLight0->getPosition().getX(), osLight0->getPosition().getY(), osLight0->getPosition().getZ()).sub(Vector3(0.0f, 0.0f, 0.0f)));
		osLight0->setEnabled(true);
		auto osCam = osEngine->getCamera();
		osCam->setZNear(0.1f);
		osCam->setZFar(15.0f);
		osCam->setLookFrom(Vector3(0.0f, 4.0f, -4.0f));
		osCam->setLookAt(Vector3(0.0f, 0.5f, 0.0f));
		osCam->setUpVector(osCam->computeUpVector(osCam->getLookFrom(), osCam->getLookAt()));
		osEngine->setSceneColor(Color4(0.5f, 0.0f, 0.0f, 1.0f));
	}
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(15.0f);
	cam->setLookFrom(Vector3(0.0f, 3.0f, -8.0f));
	cam->setLookAt(Vector3(0.0f, 0.5f, 0.0f));
	cam->setUpVector(cam->computeUpVector(cam->getLookFrom(), cam->getLookAt()));
	auto light0 = engine->getLightAt(0);
	light0->setAmbient(Color4(0.5f, 0.5f, 0.5f, 1.0f));
	light0->setSpecular(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	light0->setPosition(Vector4(0.0f, 20000.0f, 0.0f, 1.0f));
	light0->setSpotDirection(Vector3(0.0f, 0.0f, 0.0f).sub(Vector3(light0->getPosition().getX(), light0->getPosition().getY(), light0->getPosition().getZ())));
	light0->setConstantAttenuation(0.5f);
	light0->setLinearAttenuation(0.0f);
	light0->setQuadraticAttenuation(0.0f);
	light0->setSpotExponent(0.0f);
	light0->setSpotCutOff(180.0f);
	light0->setEnabled(true);
	auto light1 = engine->getLightAt(1);
	light1->setDiffuse(Color4(0.0f, 0.0f, 0.5f, 1.0f));
	light1->setPosition(Vector4(-4.0f, 5.0f, -5.0f, 1.0f));
	light1->setSpotDirection(Vector3(0.0f, 0.0f, 0.0f).sub(Vector3(light1->getPosition().getX(), light1->getPosition().getY(), light1->getPosition().getZ())));
	light1->setEnabled(true);
	auto light2 = engine->getLightAt(2);
	light2->setDiffuse(Color4(0.0f, 0.5f, 0.0f, 1.0f));
	light2->setPosition(Vector4(+4.0f, 5.0f, 0.0f, 1.0f));
	light2->setSpotDirection(Vector3(0.0f, 0.0f, 0.0f).sub(Vector3(light2->getPosition().getX(), light2->getPosition().getY(), light2->getPosition().getZ())));
	light2->setEnabled(true);
	try {
		auto _barrel = modelDeleter.add(ModelReader::read("resources/tests/models/barrel", "barrel.dae"));
		auto barrel = new Object3D("barrel", _barrel);
		barrel->setTranslation(Vector3(1.5f, 0.35f, -2.0f));
		barrel->setContributesShadows(true);
		barrel->setReceivesShadows(true);
		barrel->setEnabled(true);
		barrel->update();
		engine->addEntity(barrel);
		auto _farPlane = modelDeleter.add(createWallModel());
		auto farPlane = new Object3D("wall", _farPlane);
		farPlane->setTextureMatrix(
			(Matrix2D3x3()).identity().scale(Vector2(1.0f, -1.0f)),
			"wall",
			"wall"
		);

		farPlane->bindDiffuseTexture(osEngine->getFrameBuffer(), "wall", "wall");
		engine->addEntity(farPlane);
		auto _grass = modelDeleter.add(ModelReader::read("resources/tests/models/grass", "grass.dae"));
		auto grass = new Object3D("ground", _grass);
		grass->setEnabled(true);
		grass->setScale(Vector3(8.0f, 1.0f, 8.0f));
		grass->setReceivesShadows(true);
		grass->update();
		engine->addEntity(grass);
		auto _player = ModelReader::read("resources/tests/models/mementoman/", "mementoman.dae");
		_player->addAnimationSetup("walk", 0, 24, true);
		_player->addAnimationSetup("still", 25, 99, true);
		auto player1 = new Object3D("player1", _player);
		player1->setTranslation(Vector3(-1.5f, 0.0f, 0.0f));
		player1->setAnimation("still");
		player1->addRotation(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
		player1->update();
		player1->setEnabled(true);
		player1->setPickable(true);
		player1->setContributesShadows(true);
		player1->setReceivesShadows(true);
		engine->addEntity(player1);
		players.push_back(player1);
		auto player2 = new Object3D("player2", _player);
		player2->setTranslation(Vector3(1.5f, 0.0f, 0.0f));
		player2->setAnimation("still");
		player2->addRotation(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
		player2->update();
		player2->setEnabled(true);
		player2->setPickable(true);
		player2->setContributesShadows(true);
		player2->setReceivesShadows(true);
		players.push_back(player2);
		engine->addEntity(player2);
		auto _cube = modelDeleter.add(ModelReader::read("resources/tests/models/test", "cube.dae"));
		cube = new Object3D("cube", _cube);
		cube->setTranslation(Vector3(0.0f, 0.0f, 0.0f));
		cube->setScale(Vector3(2.0f, 2.0f, 2.0f));
		cube->update();
		cube->setPickable(true);
		cube->setContributesShadows(true);
		cube->setReceivesShadows(true);
		cube->setEnabled(true);
		engine->addEntity(cube);
		auto _wall = modelDeleter.add(ModelReader::read("resources/tests/models/wall", "wall.dae"));
		auto wall0 = new Object3D("wall0", _wall);
		wall0->setTranslation(Vector3(-1.0f, 0.0f, 3.0f));
		wall0->update();
		wall0->setPickable(true);
		wall0->setEnabled(true);
		engine->addEntity(wall0);
		auto wall1 = new Object3D("wall1", _wall);
		wall1->setTranslation(Vector3(0.0f, 0.0f, 3.0f));
		wall1->update();
		wall1->setPickable(true);
		wall1->setEnabled(true);
		engine->addEntity(wall1);
		auto osCube = new Object3D("cube", _cube);
		osCube->setTranslation(Vector3(0.0f, 0.0f, 0.0f));
		osCube->setScale(Vector3(2.0f, 2.0f, 2.0f));
		osCube->setScale(Vector3(2.0f, 2.0f, 2.0f));
		osCube->setContributesShadows(true);
		osCube->setReceivesShadows(true);
		osCube->update();
		osEngine->addEntity(osCube);
		engine->addEntity(new PointsParticleSystem("circle", new CircleParticleEmitter(3000, 50, 50, Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), 0.4f, 0.0f, 0.0f, Vector3(0.0f, 0.2f, 0.0f), Vector3(0.0f, 0.2f, 0.0f), Color4(1.0f, 1.0f, 1.0f, 0.3f), Color4(1.0f, 1.0f, 1.0f, 0.3f)), 1000, 0.1f, true));
		engine->getEntity("circle")->setEnabled(true);
		engine->addEntity(new PointsParticleSystem("water", new SphereParticleEmitter(4000, 1000, 0, 0.1f, 0.0f, new Sphere(Vector3(-1.0f, 1.0f, 0.0f), 0.05f), Vector3(-4.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f), Color4(0.8f, 0.8f, 1.0f, 0.25f), Color4(0.8f, 0.8f, 1.0f, 0.25f)), 4000, 0.2f, true));
		engine->getEntity("water")->setEnabled(true);
		engine->addEntity(new PointsParticleSystem("snow", new BoundingBoxParticleEmitter(15, 15000, 1000, 0, 0, new OrientedBoundingBox(Vector3(0.0f, 4.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(4.0f, 0.0f, 4.0f)), Vector3(0.0f, -0.5f, 0.0f), Vector3(0.0f, -0.1f, 0.0f), Color4(0.8f, 0.8f, 1.0f, 0.5f), Color4(0.8f, 0.8f, 1.0f, 0.5f)), 1024, 0.1f, true));
		engine->getEntity("snow")->setEnabled(true);
		engine->addEntity(
			new ParticleSystemGroup(
				"fire",
				true,
				false,
				false,
				{
					new PointsParticleSystem("firebase", new SphereParticleEmitter(2048, 1024, 2048, 0, 0, new Sphere(Vector3(2.5f, 0.2f, 0.0f), 0.2f), Vector3(0.0f, 0.1f, 0.0f), Vector3(0.0f, 0.1f, 0.0f), Color4(0.0f, 0.0f, 0.0f, 0.5f), Color4(0.4f, 0.0f, 0.0f, 0.5f)), 2048, 0.1f, true),
					new PointsParticleSystem("firetop", new SphereParticleEmitter(2048, 1024, 2048, 0, 0, new Sphere(Vector3(2.5f, 0.7f, 0.0f), 0.1f), Vector3(0.0f, 0.06f, 0.0f), Vector3(0.0f, 0.12f, 0.0f), Color4(0.75f, 0.0f, 0.0f, 0.5f), Color4(1.0f, 1.0f, 0.0f, 0.5f)), 2048, 0.1f, true),
					new PointsParticleSystem("firesmoke", new SphereParticleEmitter(2048, 1024, 2048, 0, 0, new Sphere(Vector3(2.5f, 0.7f, 0.0f), 0.1f), Vector3(0.0f, 0.2f, 0.0f), Vector3(0.0f, 0.4f, 0.0f), Color4(0.8f, 0.8f, 0.8f, 0.1f), Color4(0.8f, 0.8f, 0.8f, 0.1f)), 2048, 0.1f, true)
				}
			)
		);
		(dynamic_cast< ParticleSystemEntity* >(engine->getEntity("circle")))->setPickable(false);
		(dynamic_cast< ParticleSystemEntity* >(engine->getEntity("snow")))->setPickable(false);
		(dynamic_cast< ParticleSystemEntity* >(engine->getEntity("fire")))->setPickable(true);
	} catch (Exception& exception) {
		Console::print(string("EngineTest::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
		Application::exit(1);
	}
}

void EngineTest::reshape(int32_t width, int32_t height)
{
	engine->reshape(width, height);
}

void EngineTest::onChar(unsigned int key, int x, int y) {
}

void EngineTest::onKeyDown (unsigned char key, int x, int y) {
	auto keyChar = tolower(key);
	if (keyChar == u'w') keyW = true;
	if (keyChar == u'a') keyA = true;
	if (keyChar == u's') keyS = true;
	if (keyChar == u'd') keyD = true;
}

void EngineTest::onKeyUp(unsigned char key, int x, int y) {
	auto keyChar = tolower(key);
	if (keyChar == u'w') keyW = false;
	if (keyChar == u'a') keyA = false;
	if (keyChar == u's') keyS = false;
	if (keyChar == u'd') keyD = false;
}

void EngineTest::onSpecialKeyDown (int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = true;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = true;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = true;
}

void EngineTest::onSpecialKeyUp(int key, int x, int y) {
	if (key == KEYBOARD_KEYCODE_LEFT) keyLeft = false;
	if (key == KEYBOARD_KEYCODE_RIGHT) keyRight = false;
	if (key == KEYBOARD_KEYCODE_UP) keyUp = false;
}

void EngineTest::onMouseDragged(int x, int y) {
}

void EngineTest::onMouseMoved(int x, int y) {
}

void EngineTest::onMouseButton(int button, int state, int x, int y) {
	if (state == MOUSE_BUTTON_DOWN) {
		mouseClicked = true;
		mouseClickedXY[0] = x;
		mouseClickedXY[1] = y;
	}
}

void EngineTest::onMouseWheel(int button, int direction, int x, int y) {
}
