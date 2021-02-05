#include <tdme/tools/shared/tools/Tools.h>

#include <array>
#include <string>

#include <tdme/application/Application.h>
#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/prototype/PrototypeBoundingVolume.h>
#include <tdme/engine/prototype/PrototypeLODLevel.h>
#include <tdme/engine/Camera.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/EntityHierarchy.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/Object3D.h>
#include <tdme/engine/PartitionNone.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/engine/SceneConnector.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/utilities/Primitives.h>
#include <tdme/utilities/Properties.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>

using std::array;
using std::string;
using std::to_string;

using tdme::tools::shared::tools::Tools;

using tdme::application::Application;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::model::Color4;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::model::UpVector;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::Camera;
using tdme::engine::Engine;
using tdme::engine::Entity;
using tdme::engine::EntityHierarchy;
using tdme::engine::Light;
using tdme::engine::Object3D;
using tdme::engine::PartitionNone;
using tdme::engine::SceneConnector;
using tdme::engine::SceneConnector;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::Float;
using tdme::utilities::Integer;
using tdme::utilities::ModelTools;
using tdme::utilities::Primitives;
using tdme::utilities::Properties;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;

Engine* Tools::osEngine = nullptr;
float Tools::oseScale = 0.75f;
Model* Tools::gizmoAll = nullptr;
Model* Tools::gizmoTranslationScale = nullptr;
Model* Tools::gizmoTranslation = nullptr;
Model* Tools::gizmoScale = nullptr;
Model* Tools::gizmoRotations = nullptr;
Model* Tools::defaultOBB = nullptr;

string Tools::formatFloat(float value)
{
	string floatString = to_string(value).substr(0, floatString.length() - 3);
	return floatString.substr(0, floatString.length() - 3);
}

string Tools::formatVector3(const Vector3& value)
{
	return
		formatFloat(value.getX()) +
		", " +
		formatFloat(value.getY()) +
		", " +
		formatFloat(value.getZ());
}

string Tools::formatColor4(const Color4& value)
{
	return
		formatFloat(value.getRed()) +
		", " +
		formatFloat(value.getGreen()) +
		", " +
		formatFloat(value.getBlue()) +
		", " +
		formatFloat(value.getAlpha());
}

void Tools::convertToArray(const string& text, array<float, 3>& array)
{
	auto i = 0;
	StringTokenizer t;
	t.tokenize(text, ",");
	while (t.hasMoreTokens() && i < array.size()) {
		array[i++] = Float::parseFloat(t.nextToken());
	}
}

void Tools::convertToArray(const string& text, array<float, 4>& array)
{
	auto i = 0;
	StringTokenizer t;
	t.tokenize(text, ",");
	while (t.hasMoreTokens() && i < array.size()) {
		array[i++] = Float::parseFloat(t.nextToken());
	}
}

Vector3 Tools::convertToVector3(const string& text)
{
	Vector3 v;
	convertToArray(text, v.getArray());
	return v;
}

Vector4 Tools::convertToVector4(const string& text)
{
	Vector4 v;
	convertToArray(text, v.getArray());
	return v;
}

Color4 Tools::convertToColor4(const string& text)
{
	Color4 color;
	convertToArray(text, color.getArray());
	return color;
}

float Tools::convertToFloat(const string& text)
{
	return Float::parseFloat(text);
}

int Tools::convertToInt(const string& text)
{
	return Integer::parseInt(text);
}

int Tools::convertToIntSilent(const string& text)
{
	try {
		return Integer::parseInt(text);
	} catch (Exception& exception) {
		return -1;
	}
}

void Tools::setDefaultLight(Light* light)
{
	light->setAmbient(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	light->setDiffuse(Color4(0.5f, 0.5f, 0.5f, 1.0f));
	light->setSpecular(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	light->setPosition(Vector4(0.0f, 20000.0f, 0.0f, 1.0f));
	light->setSpotDirection(Vector3(0.0f, 0.0f, 0.0f).sub(Vector3(light->getPosition().getX(), light->getPosition().getY(), light->getPosition().getZ())));
	light->setConstantAttenuation(0.5f);
	light->setLinearAttenuation(0.0f);
	light->setQuadraticAttenuation(0.0f);
	light->setSpotExponent(0.0f);
	light->setSpotCutOff(180.0f);
	light->setEnabled(true);
}

void Tools::oseInit()
{
	osEngine = Engine::createOffScreenInstance(128, 128, false);
	osEngine->setPartition(new PartitionNone());
	setDefaultLight(osEngine->getLightAt(0));
	oseScale = 0.75f;
}

void Tools::oseDispose()
{
	osEngine->dispose();
	delete osEngine;
}

void Tools::oseThumbnail(Prototype* model)
{
	Vector3 objectScale;
	Transformations oseLookFromRotations;
	oseLookFromRotations.addRotation(Vector3(0.0f, 1.0f, 0.0f), -45.0f);
	oseLookFromRotations.addRotation(Vector3(1.0f, 0.0f, 0.0f), -45.0f);
	oseLookFromRotations.addRotation(Vector3(0.0f, 0.0f, 1.0f), 0.0f);
	oseLookFromRotations.update();
	Tools::setupPrototype(model, osEngine, oseLookFromRotations, oseScale, 1, objectScale);
	osEngine->setSceneColor(Color4(0.5f, 0.5f, 0.5f, 1.0f));
	osEngine->display();
	// osEngine->makeScreenshot("tmp", model->getThumbnail());
	osEngine->setSceneColor(Color4(0.8f, 0.0f, 0.0f, 1.0f));
	osEngine->display();
	// osEngine->makeScreenshot("tmp", "selected_" + model->getThumbnail());
	osEngine->reset();
}

float Tools::computeMaxAxisDimension(BoundingBox* boundingBox)
{
	auto maxAxisDimension = 0.0f;
	Vector3 dimension = boundingBox->getMax().clone().sub(boundingBox->getMin());
	if (dimension.getX() > maxAxisDimension) maxAxisDimension = dimension.getX();
	if (dimension.getY() > maxAxisDimension) maxAxisDimension = dimension.getY();
	if (dimension.getZ() > maxAxisDimension) maxAxisDimension = dimension.getZ();
	return maxAxisDimension;
}

Model* Tools::createGroundModel(float width, float depth, float y)
{
	auto modelId = "ground" + to_string(static_cast<int>(width * 100)) + "x" + to_string(static_cast<int>(depth * 100)) + "@" + to_string(static_cast<int>(y * 100));
	auto ground = new Model(modelId, modelId, UpVector::Y_UP, RotationOrder::ZYX, nullptr);
	auto groundMaterial = new Material("ground");
	groundMaterial->setSpecularMaterialProperties(new SpecularMaterialProperties());
	groundMaterial->getSpecularMaterialProperties()->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
	groundMaterial->getSpecularMaterialProperties()->setDiffuseTexture("resources/engine/textures", "groundplate.png");
	ground->getMaterials()["ground"] = groundMaterial;
	auto groundNode = new Node(ground, nullptr, "ground", "ground");
	vector<Vector3> groundVertices;
	groundVertices.push_back(Vector3(-width, y, -depth));
	groundVertices.push_back(Vector3(-width, y, +depth));
	groundVertices.push_back(Vector3(+width, y, +depth));
	groundVertices.push_back(Vector3(+width, y, -depth));
	vector<Vector3> groundNormals;
	groundNormals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	vector<TextureCoordinate> groundTextureCoordinates;
	groundTextureCoordinates.push_back(TextureCoordinate(0.0f, depth));
	groundTextureCoordinates.push_back(TextureCoordinate(0.0f, 0.0f));
	groundTextureCoordinates.push_back(TextureCoordinate(width, 0.0f));
	groundTextureCoordinates.push_back(TextureCoordinate(width, depth));
	vector<Face> groundFacesGround;
	groundFacesGround.push_back(Face(groundNode, 0, 1, 2, 0, 0, 0, 0, 1, 2));
	groundFacesGround.push_back(Face(groundNode, 2, 3, 0, 0, 0, 0, 2, 3, 0));
	FacesEntity nodeFacesEntityGround(groundNode, "ground.facesentity");
	nodeFacesEntityGround.setMaterial(groundMaterial);
	vector<FacesEntity> nodeFacesEntities;
	nodeFacesEntityGround.setFaces(groundFacesGround);
	nodeFacesEntities.push_back(nodeFacesEntityGround);
	groundNode->setVertices(groundVertices);
	groundNode->setNormals(groundNormals);
	groundNode->setTextureCoordinates(groundTextureCoordinates);
	groundNode->setFacesEntities(nodeFacesEntities);
	ground->getNodes()["ground"] = groundNode;
	ground->getSubNodes()["ground"] = groundNode;
	ModelTools::prepareForIndexedRendering(ground);
	return ground;
}

void Tools::setupPrototype(Prototype* prototype, Engine* engine, const Transformations& lookFromRotations, float camScale, int lodLevel, Vector3& objectScale)
{
	if (prototype == nullptr) return;

	// create engine entity
	BoundingBox* entityBoundingBoxFallback = new BoundingBox(Vector3(-2.5f, 0.0f, -2.5f), Vector3(2.5f, 2.0f, 2.5f));
	BoundingBox* entityBoundingBox = nullptr;
	Entity* modelEntity = nullptr;
	objectScale.set(1.0f, 1.0f, 1.0f);
	Color4 colorMul(1.0f, 1.0f, 1.0f, 1.0f);
	Color4 colorAdd(0.0f, 0.0f, 0.0f, 0.0f);

	// bounding volumes
	auto entityBoundingVolumesHierarchy = new EntityHierarchy(Prototype::MODEL_BOUNDINGVOLUMES_ID);
	for (auto i = 0; i < prototype->getBoundingVolumeCount(); i++) {
		auto entityBoundingVolume = prototype->getBoundingVolume(i);
		if (entityBoundingVolume->getModel() != nullptr) {
			auto bvObject = new Object3D(Prototype::MODEL_BOUNDINGVOLUME_IDS[i], entityBoundingVolume->getModel());
			bvObject->setEnabled(false);
			entityBoundingVolumesHierarchy->addEntity(bvObject);
		}
	}
	entityBoundingVolumesHierarchy->update();
	engine->addEntity(entityBoundingVolumesHierarchy);

	//
	if (prototype->getType() == Prototype_Type::TRIGGER ||
		prototype->getType() == Prototype_Type::ENVIRONMENTMAPPING) {
		entityBoundingBox = entityBoundingVolumesHierarchy->getBoundingBox();
	} else
	if (prototype->getType() == Prototype_Type::PARTICLESYSTEM) {
		modelEntity = SceneConnector::createEntity(prototype, "model", Transformations());
		if (modelEntity != nullptr) engine->addEntity(modelEntity);
	} else
	if (prototype->getModel() != nullptr) {
		// model
		Model* model = nullptr;
		switch (lodLevel) {
			case 1:
				model = prototype->getModel();
				break;
			case 2:
				{
					auto lodLevelEntity = prototype->getLODLevel2();
					if (lodLevelEntity != nullptr) {
						model = lodLevelEntity->getModel();
						colorMul.set(lodLevelEntity->getColorMul());
						colorAdd.set(lodLevelEntity->getColorAdd());
					}
					break;
				}
			case 3:
				{
					auto lodLevelEntity = prototype->getLODLevel3();
					if (lodLevelEntity != nullptr) {
						model = lodLevelEntity->getModel();
						colorMul.set(lodLevelEntity->getColorMul());
						colorAdd.set(lodLevelEntity->getColorAdd());
					}
					break;
				}
		}
		entityBoundingBox = prototype->getModel()->getBoundingBox();
		if (model != nullptr) {
			modelEntity = new Object3D("model", model);
			modelEntity->setContributesShadows(true);
			modelEntity->setReceivesShadows(true);
			modelEntity->setEffectColorMul(colorMul);
			modelEntity->setEffectColorAdd(colorAdd);
			auto object = dynamic_cast<Object3D*>(modelEntity);
			object->setShader(prototype->getShader());
			object->setDistanceShader(prototype->getDistanceShader());
			object->setDistanceShaderDistance(prototype->getDistanceShaderDistance());
			auto shaderParametersDefault = Engine::getShaderParameterDefaults(prototype->getShader());
			auto distanceShaderParametersDefault = Engine::getShaderParameterDefaults(prototype->getDistanceShader());
			for (auto& parameterIt: shaderParametersDefault) {
				auto& parameterName = parameterIt.first;
				auto parameterValue = prototype->getShaderParameters().getShaderParameter(parameterName);
				object->setShaderParameter(parameterName, parameterValue);
			}
			for (auto& parameterIt: distanceShaderParametersDefault) {
				auto& parameterName = parameterIt.first;
				auto parameterValue = prototype->getDistanceShaderParameters().getShaderParameter(parameterName);
				object->setDistanceShaderParameter(parameterName, parameterValue);
			}
			engine->addEntity(modelEntity);
		}
	}

	auto entityBoundingBoxToUse = entityBoundingBox != nullptr?entityBoundingBox:entityBoundingBoxFallback;

	// do a feasible scale
	float maxAxisDimension = Tools::computeMaxAxisDimension(entityBoundingBoxToUse);
	objectScale.scale(1.0f / maxAxisDimension * 0.75f);
	if (modelEntity != nullptr) {
		modelEntity->setPickable(true);
		modelEntity->setScale(objectScale);
		modelEntity->update();
	}

	// generate ground
	auto ground = createGroundModel(
		Math::ceil((entityBoundingBoxToUse->getMax().getX() - entityBoundingBoxToUse->getMin().getX()) * 1.0f),
		Math::ceil((entityBoundingBoxToUse->getMax().getZ() - entityBoundingBoxToUse->getMin().getZ()) * 1.0f),
		entityBoundingBoxToUse->getMin().getY() - Math::EPSILON
	);
	auto groundObject = new Object3D("ground", ground);
	groundObject->setEnabled(false);
	groundObject->setScale(objectScale);
	groundObject->update();
	engine->addEntity(groundObject);

	//
	dynamic_cast<EntityHierarchy*>(engine->getEntity(Prototype::MODEL_BOUNDINGVOLUMES_ID))->setScale(objectScale);
	dynamic_cast<EntityHierarchy*>(engine->getEntity(Prototype::MODEL_BOUNDINGVOLUMES_ID))->update();

	// lights
	for (auto i = 1; i < engine->getLightCount(); i++) engine->getLightAt(i)->setEnabled(false);
	auto light0 = engine->getLightAt(0);
	light0->setAmbient(Color4(0.7f, 0.7f, 0.7f, 1.0f));
	light0->setDiffuse(Color4(0.3f, 0.3f, 0.3f, 1.0f));
	light0->setSpecular(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	light0->setPosition(
		Vector4(
			0.0f,
			10.0f,
			10.0f,
			1.0f
		)
	);
	light0->setSpotDirection(Vector3(0.0f, 0.0f, 0.0f).sub(Vector3(light0->getPosition().getX(), light0->getPosition().getY(), light0->getPosition().getZ())).normalize());
	light0->setConstantAttenuation(0.5f);
	light0->setLinearAttenuation(0.0f);
	light0->setQuadraticAttenuation(0.0f);
	light0->setSpotExponent(0.0f);
	light0->setSpotCutOff(180.0f);
	light0->setEnabled(true);

	// cam
	auto cam = engine->getCamera();
	cam->setZNear(0.1f);
	cam->setZFar(15.0f);
	auto lookAt = cam->getLookAt();
	lookAt.set(entityBoundingBoxToUse->getCenter().clone().scale(objectScale));
	Vector3 forwardVector(0.0f, 0.0f, 1.0f);
	Vector3 forwardVectorTransformed;
	Vector3 upVector;
	// TODO: a.drewke
	Transformations _lookFromRotations;
	_lookFromRotations.fromTransformations(lookFromRotations);
	_lookFromRotations.getTransformationsMatrix().multiply(forwardVector, forwardVectorTransformed).scale(camScale);
	_lookFromRotations.getRotation(2).getQuaternion().multiply(Vector3(0.0f, 1.0f, 0.0f), upVector).normalize();
	auto lookFrom = lookAt.clone().add(forwardVectorTransformed);
	cam->setLookFrom(lookFrom);
	cam->setLookAt(lookAt);
	cam->setUpVector(upVector);

	//
	delete entityBoundingBoxFallback;
}

const string Tools::getRelativeResourcesFileName(const string& applicationRoot, const string& fileName)
{
	auto newFileName = StringTools::replace(fileName, '\\', '/');
	auto cutFileNameIdx = -1;
	if (cutFileNameIdx == -1) {
		cutFileNameIdx = fileName.rfind("/resources/");
		if (cutFileNameIdx != -1) {
			newFileName = StringTools::substring(fileName, cutFileNameIdx + 1);
		}
	}
	if (cutFileNameIdx == -1) {
		cutFileNameIdx = fileName.rfind("resources/");
		if (cutFileNameIdx != -1) {
			newFileName = StringTools::substring(fileName, cutFileNameIdx);
		}
	}
	return newFileName;
}

const string Tools::getApplicationRootPath(const string& fileName)
{
	auto newFileName = StringTools::replace(fileName, '\\', '/');
	auto filesRootIdx = -1;
	if (filesRootIdx == -1) {
		filesRootIdx = fileName.rfind("/resources/");
		if (filesRootIdx != -1) return StringTools::substring(fileName, 0, filesRootIdx);
	}
	if (filesRootIdx == -1) {
		filesRootIdx = fileName.rfind("resources/");
		if (filesRootIdx != -1) return StringTools::substring(fileName, 0, filesRootIdx);
	}
	return "";
}

const string Tools::getPathName(const string& fileName)
{
	return FileSystem::getInstance()->getPathName(fileName);
}

const string Tools::getFileName(const string& fileName)
{
	return FileSystem::getInstance()->getFileName(fileName);
}

const string Tools::removeFileEnding(const string& fileName)
{
	auto idx = fileName.rfind('.');
	if (idx == string::npos) {
		return fileName;
	} else {
		return fileName.substr(0, idx);
	}
}

void Tools::loadSettings(Application* application) {
	Properties settings;

	// settings
	try {
		settings.load("settings", "settings.properties");
	} catch (FileSystemException &exception) {
		Console::println(string("Error loading properties: ") + exception.what());
	}

	// 4k
	if (settings.get("4k", "false") == "true") {
		Console::println("Settings: enable 4k");
		Engine::set4K(true);
	}

	// Window
	Console::println("Settings: window width: " + settings.get("window_width", "1024"));
	Console::println("Settings: window height: " + settings.get("window_height", "768"));
	Console::println("Settings: window X position: " + settings.get("window_x", "100"));
	Console::println("Settings: window Y position: " + settings.get("window_y", "100"));
	Console::println("Settings: fullscreen: " + settings.get("fullscreen", "false"));
	application->setWindowWidth(Integer::parseInt(settings.get("window_width", "1024")));
	application->setWindowHeight(Integer::parseInt(settings.get("window_height", "768")));
	application->setWindowXPosition(Integer::parseInt(settings.get("window_x", "100")));
	application->setWindowYPosition(Integer::parseInt(settings.get("window_y", "100")));
	application->setFullScreen(settings.get("fullscreen", "false") == "true");
}

Model* Tools::getGizmoAll() {
	if (gizmoAll == nullptr) {
		gizmoAll = ModelReader::read("resources/engine/models", "gizmo_all.tm");
	}
	return gizmoAll;
}

Model* Tools::getGizmoTranslationScale() {
	if (gizmoTranslationScale == nullptr) {
		gizmoTranslationScale = ModelReader::read("resources/engine/models", "gizmo_transscale.tm");
	}
	return gizmoTranslationScale;
}

Model* Tools::getGizmoTranslation() {
	if (gizmoTranslation == nullptr) {
		gizmoTranslation = ModelReader::read("resources/engine/models", "gizmo_translate.tm");
	}
	return gizmoTranslation;
}

Model* Tools::getGizmoScale() {
	if (gizmoScale == nullptr) {
		gizmoScale = ModelReader::read("resources/engine/models", "gizmo_scale.tm");
	}
	return gizmoScale;
}

Model* Tools::getGizmoRotations() {
	if (gizmoRotations == nullptr) {
		gizmoRotations = ModelReader::read("resources/engine/models", "gizmo_rotate.tm");
	}
	return gizmoRotations;
}

Model* Tools::getDefaultObb() {
	if (defaultOBB == nullptr) {
		OrientedBoundingBox obb(
			Vector3(),
			OrientedBoundingBox::AABB_AXIS_X,
			OrientedBoundingBox::AABB_AXIS_Y,
			OrientedBoundingBox::AABB_AXIS_Z,
			Vector3(0.5f, 0.5f, 0.5f)
		);
		defaultOBB = Primitives::createModel(&obb, "tdme.obb.default");
	}
	return defaultOBB;

}
