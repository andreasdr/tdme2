#include <tdme/engine/fileio/scenes/SceneReader.h>

#include <string>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/models/TMWriter.h>
#include <tdme/engine/ModelUtilities.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/Model.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/subsystems/rendering/ModelStatistics.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/fileio/scenes/SceneWriter.h>
#include <tdme/engine/fileio/ProgressCallback.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/scene/SceneLight.h>
#include <tdme/engine/scene/SceneEntity.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/StringTools.h>

#include <rapidjson/document.h>

using std::string;
using std::to_string;

using tdme::engine::fileio::scenes::SceneReader;
using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::models::TMWriter;
using tdme::engine::ModelUtilities;
using tdme::engine::Transformations;
using tdme::engine::model::Animation;
using tdme::engine::model::Color4;
using tdme::engine::model::Node;
using tdme::engine::model::Model;
using tdme::utilities::ModelTools;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::UpVector;
using tdme::engine::subsystems::rendering::ModelStatistics;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::engine::fileio::scenes::SceneWriter;
using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::fileio::ProgressCallback;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::scene::SceneLight;
using tdme::engine::scene::SceneEntity;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Float;
using tdme::utilities::Console;
using tdme::utilities::StringTools;

using rapidjson::Document;
using rapidjson::Value;

void SceneReader::read(const string& pathName, const string& fileName, Scene& scene, ProgressCallback* progressCallback)
{
	read(pathName, fileName, scene, "", progressCallback);
}

void SceneReader::read(const string& pathName, const string& fileName, Scene& scene, const string& objectIdPrefix, ProgressCallback* progressCallback)
{
	if (progressCallback != nullptr) progressCallback->progress(0.0f);

	auto jsonContent = FileSystem::getInstance()->getContentAsString(pathName, fileName);
	if (progressCallback != nullptr) progressCallback->progress(0.165f);

	Document jRoot;
	jRoot.Parse(jsonContent.c_str());
	if (progressCallback != nullptr) progressCallback->progress(0.33f);

	scene.setApplicationRoot(Tools::getApplicationRootPath(pathName));
	// auto version = Float::parseFloat((jRoot["version"].GetString()));
	scene.setRotationOrder(jRoot.FindMember("ro") != jRoot.MemberEnd()?RotationOrder::valueOf(jRoot["ro"].GetString()) : RotationOrder::XYZ);
	scene.clearProperties();
	for (auto i = 0; i < jRoot["properties"].GetArray().Size(); i++) {
		auto& jMapProperty = jRoot["properties"].GetArray()[i];
		scene.addProperty(
			jMapProperty["name"].GetString(),
			jMapProperty["value"].GetString()
		);
	}
	if (jRoot.FindMember("lights") != jRoot.MemberEnd()) {
		for (auto i = 0; i < jRoot["lights"].GetArray().Size(); i++) {
			auto& jLight = jRoot["lights"].GetArray()[i];
			auto light = scene.getLightAt(jLight.FindMember("id") != jLight.MemberEnd()? jLight["id"].GetInt() : i);
			light->getAmbient().set(
				jLight["ar"].GetFloat(),
				jLight["ag"].GetFloat(),
				jLight["ab"].GetFloat(),
				jLight["aa"].GetFloat()
			);
			light->getDiffuse().set(
				jLight["dr"].GetFloat(),
				jLight["dg"].GetFloat(),
				jLight["db"].GetFloat(),
				jLight["da"].GetFloat()
			);
			light->getSpecular().set(
				jLight["sr"].GetFloat(),
				jLight["sg"].GetFloat(),
				jLight["sb"].GetFloat(),
				jLight["sa"].GetFloat()
			);
			light->getPosition().set(
				jLight["px"].GetFloat(),
				jLight["py"].GetFloat(),
				jLight["pz"].GetFloat(),
				jLight["pw"].GetFloat()
			);
			light->setConstantAttenuation(jLight["ca"].GetFloat());
			light->setLinearAttenuation(jLight["la"].GetFloat());
			light->setQuadraticAttenuation(jLight["qa"].GetFloat());
			light->getSpotTo().set(
				jLight["stx"].GetFloat(),
				jLight["sty"].GetFloat(),
				jLight["stz"].GetFloat()
			);
			light->getSpotDirection().set(
				jLight["sdx"].GetFloat(),
				jLight["sdy"].GetFloat(),
				jLight["sdz"].GetFloat()
			);
			light->setSpotExponent(jLight["se"].GetFloat());
			light->setSpotCutOff(jLight["sco"].GetFloat());
			light->setEnabled(jLight["e"].GetBool());
		}
	}
	scene.getLibrary()->clear();

	auto progressStepCurrent = 0;
	for (auto i = 0; i < jRoot["models"].GetArray().Size(); i++) {
		auto& jModel = jRoot["models"].GetArray()[i];
		Prototype* prototype = PrototypeReader::read(
			jModel["id"].GetInt(),
			pathName,
			jModel["entity"]
		);
		if (prototype == nullptr) {
			Console::println("SceneReader::doImport(): Invalid entity = " + to_string(jModel["id"].GetInt()));
			continue;
		}
		scene.getLibrary()->addPrototype(prototype);
		if (jModel.FindMember("properties") != jModel.MemberEnd()) {
			for (auto j = 0; j < jModel["properties"].GetArray().Size(); j++) {
				auto& jModelProperty = jModel["properties"].GetArray()[j];
				prototype->addProperty(
					(jModelProperty["name"].GetString()),
					(jModelProperty["value"].GetString())
				);
			}
		}

		if (progressCallback != nullptr) progressCallback->progress(0.33f + static_cast<float>(progressStepCurrent) / static_cast<float>(jRoot["models"].GetArray().Size()) * 0.33f);
		progressStepCurrent++;
	}
	scene.clearEntities();

	for (auto i = 0; i < jRoot["objects"].GetArray().Size(); i++) {
		auto& jObject = jRoot["objects"].GetArray()[i];
		auto model = scene.getLibrary()->getPrototype(jObject["mid"].GetInt());
		if (model == nullptr) {
			Console::println("SceneReader::doImport(): No entity found with id = " + to_string(jObject["mid"].GetInt()));

			if (progressCallback != nullptr && progressStepCurrent % 1000 == 0) progressCallback->progress(0.66f + static_cast<float>(progressStepCurrent) / static_cast<float>(jRoot["objects"].GetArray().Size()) * 0.33f);
			progressStepCurrent++;

			continue;
		}

		Transformations transformations;
		transformations.setPivot(model->getPivot());
		transformations.setTranslation(
			Vector3(
				jObject["tx"].GetFloat(),
				jObject["ty"].GetFloat(),
				jObject["tz"].GetFloat()
			)
		);
		transformations.setScale(
			Vector3(
				jObject["sx"].GetFloat(),
				jObject["sy"].GetFloat(),
				jObject["sz"].GetFloat()
			)
		);
		Vector3 rotation(
			jObject["rx"].GetFloat(),
			jObject["ry"].GetFloat(),
			jObject["rz"].GetFloat()
		);
		transformations.addRotation(scene.getRotationOrder()->getAxis0(), rotation.getArray()[scene.getRotationOrder()->getAxis0VectorIndex()]);
		transformations.addRotation(scene.getRotationOrder()->getAxis1(), rotation.getArray()[scene.getRotationOrder()->getAxis1VectorIndex()]);
		transformations.addRotation(scene.getRotationOrder()->getAxis2(), rotation.getArray()[scene.getRotationOrder()->getAxis2VectorIndex()]);
		transformations.update();
		auto sceneEntity = new SceneEntity(
			objectIdPrefix != "" ?
				objectIdPrefix + jObject["id"].GetString() :
				(jObject["id"].GetString()),
			 jObject.FindMember("descr") != jObject.MemberEnd()?jObject["descr"].GetString() : "",
			 transformations,
			 model
		);
		if (jObject.FindMember("properties") != jObject.MemberEnd()) {
			for (auto j = 0; j < jObject["properties"].GetArray().Size(); j++) {
				auto& jObjectProperty = jObject["properties"].GetArray()[j];
				sceneEntity->addProperty(
					jObjectProperty["name"].GetString(),
					jObjectProperty["value"].GetString()
				);
			}
		}
		sceneEntity->setReflectionEnvironmentMappingId(jObject.FindMember("r") != jObject.MemberEnd()?jObject["r"].GetString():"");
		scene.addEntity(sceneEntity);

		if (progressCallback != nullptr && progressStepCurrent % 1000 == 0) progressCallback->progress(0.66f + static_cast<float>(progressStepCurrent) / static_cast<float>(jRoot["objects"].GetArray().Size()) * 0.33f);
		progressStepCurrent++;
	}
	scene.setEntityIdx(jRoot["objects_eidx"].GetInt());
	scene.setPathName(pathName);
	scene.setFileName(fileName);
	scene.update();

	//
	if (jRoot.FindMember("sky") != jRoot.MemberEnd()) {
		auto& jSky = jRoot["sky"];
		scene.setSkyModelFileName(jSky["file"].GetString());
		scene.setSkyModelScale(
			Vector3(
				jSky["sx"].GetFloat(),
				jSky["sy"].GetFloat(),
				jSky["sz"].GetFloat()
			)
		);
		if (scene.getSkyModelFileName().empty() == false) {
			auto skyModelPathName = PrototypeReader::getResourcePathName(pathName, scene.getSkyModelFileName());
			scene.setSkyModel(
				ModelReader::read(
					skyModelPathName,
					FileSystem::getInstance()->getFileName(scene.getSkyModelFileName())
				)
			);
		}
	}

	//
	if (progressCallback != nullptr) {
		progressCallback->progress(1.0f);
		delete progressCallback;
	}
}

void SceneReader::determineMeshNodes(Scene& scene, Node* node, const string& parentName, const Matrix4x4& parentTransformationsMatrix, vector<PrototypeMeshNode>& meshNodes) {
	auto entityLibrary = scene.getLibrary();
	auto nodeId = node->getId();
	if (parentName.length() > 0) nodeId = parentName + "." + nodeId;
	auto modelName = nodeId;
	modelName = StringTools::regexReplace(modelName, "[-_]{1}[0-9]+$", "");
	modelName = StringTools::regexReplace(modelName, "[0-9]+$", "");
	auto haveName = entityLibrary->getPrototypeCount() == 0;
	if (haveName == false) {
		for (auto i = 0; i < 10000; i++) {
			haveName = true;
			auto modelNameTry = modelName;
			if (i > 0) modelNameTry+= to_string(i);
			for (auto entityIdx = 0; entityIdx < entityLibrary->getPrototypeCount(); entityIdx++) {
				auto entity = entityLibrary->getPrototypeAt(entityIdx);
				if (entity->getName() == modelNameTry) {
					haveName = false;
					break;
				}
			}
			if (haveName == true) {
				modelName = modelNameTry;
				break;
			}
		}
	}
	if (haveName == false) {
		Console::println(
			string(
				"SceneReader::doImportFromModel(): Skipping model '" +
				modelName +
				"' as no name could be created for it."
			)
		 );
		return;
	}
	Matrix4x4 transformationsMatrix;
	// compute animation matrix if animation setups exist
	auto animation = node->getAnimation();
	if (animation != nullptr) {
		auto& animationMatrices = animation->getTransformationsMatrices();
		transformationsMatrix.set(animationMatrices[0 % animationMatrices.size()]);
	} else {
		// no animation matrix, set up local transformation matrix up as node matrix
		transformationsMatrix.set(node->getTransformationsMatrix());
	}

	// apply parent transformation matrix
	transformationsMatrix.multiply(parentTransformationsMatrix);

	// check if no mesh?
	if (node->getVertices().size() == 0 && node->getSubNodes().size() > 0) {
		// ok, check sub meshes
		for (auto subNodeIt: node->getSubNodes()) {
			determineMeshNodes(scene, subNodeIt.second, nodeId, transformationsMatrix.clone(), meshNodes);
		}
	} else {
		// add to node meshes, even if empty as its a empty :D
		PrototypeMeshNode prototypeMeshNode;
		prototypeMeshNode.id = nodeId;
		prototypeMeshNode.name = modelName;
		prototypeMeshNode.node = node;
		prototypeMeshNode.transformationsMatrix.set(transformationsMatrix);
		meshNodes.push_back(prototypeMeshNode);
	}
}

void SceneReader::readFromModel(const string& pathName, const string& fileName, Scene& scene, ProgressCallback* progressCallback) {
	if (progressCallback != nullptr) progressCallback->progress(0.0f);

	scene.clearProperties();
	scene.getLibrary()->clear();
	scene.clearEntities();

	string modelPathName = pathName + "/" + fileName + "-models";
	if (FileSystem::getInstance()->fileExists(modelPathName)) {
		FileSystem::getInstance()->removePath(modelPathName, true);
	}
	FileSystem::getInstance()->createPath(modelPathName);

	auto levelModel = ModelReader::read(pathName, fileName);

	if (progressCallback != nullptr) progressCallback->progress(0.1f);

	auto upVector = levelModel->getUpVector();
	RotationOrder* rotationOrder = levelModel->getRotationOrder();

	scene.setRotationOrder(rotationOrder);

	auto entityLibrary = scene.getLibrary();
	auto nodeIdx = 0;
	Prototype* emptyEntity = nullptr;
	Matrix4x4 modelImportRotationMatrix;
	Vector3 levelModelScale;
	modelImportRotationMatrix.set(levelModel->getImportTransformationsMatrix());
	modelImportRotationMatrix.getScale(levelModelScale);
	modelImportRotationMatrix.scale(Vector3(1.0f / levelModelScale.getX(), 1.0f / levelModelScale.getY(), 1.0f / levelModelScale.getZ()));
	auto progressTotal = levelModel->getSubNodes().size();
	auto progressIdx = 0;
	for (auto nodeIt: levelModel->getSubNodes()) {
		if (progressCallback != nullptr) progressCallback->progress(0.1f + static_cast<float>(progressIdx) / static_cast<float>(progressTotal) * 0.8f);
		vector<PrototypeMeshNode> meshNodes;
		determineMeshNodes(scene, nodeIt.second, "", (Matrix4x4()).identity(), meshNodes);
		for (auto& meshNode: meshNodes) {
			auto model = new Model(
				modelPathName + "/" + meshNode.name + ".tm",
				fileName + "-" + meshNode.name,
				upVector,
				rotationOrder,
				nullptr
			);
			model->setImportTransformationsMatrix(levelModel->getImportTransformationsMatrix());
			float importFixScale = 1.0f;
			Vector3 translation, scale, rotation;
			Vector3 xAxis, yAxis, zAxis, tmpAxis;
			Matrix4x4 nodeTransformationsMatrix;
			nodeTransformationsMatrix.set(meshNode.transformationsMatrix);
			nodeTransformationsMatrix.getAxes(xAxis, yAxis, zAxis);
			nodeTransformationsMatrix.getTranslation(translation);
			nodeTransformationsMatrix.getScale(scale);
			xAxis.normalize();
			yAxis.normalize();
			zAxis.normalize();
			nodeTransformationsMatrix.setAxes(xAxis, yAxis, zAxis);
			if ((upVector == UpVector::Y_UP && Vector3::computeDotProduct(Vector3::computeCrossProduct(xAxis, yAxis, tmpAxis), zAxis) < 0.0f) ||
				(upVector == UpVector::Z_UP && Vector3::computeDotProduct(Vector3::computeCrossProduct(xAxis, zAxis, tmpAxis), yAxis) < 0.0f)) {
				xAxis.scale(-1.0f);
				yAxis.scale(-1.0f);
				zAxis.scale(-1.0f);
				nodeTransformationsMatrix.setAxes(xAxis, yAxis, zAxis);
				scale.scale(-1.0f);
			}
			nodeTransformationsMatrix.computeEulerAngles(rotation);
			modelImportRotationMatrix.multiply(scale, scale);
			modelImportRotationMatrix.multiply(rotation, rotation);
			model->getImportTransformationsMatrix().multiply(translation, translation);

			ModelTools::cloneNode(meshNode.node, model);
			if (model->getSubNodes().begin() != model->getSubNodes().end()) {
				model->getSubNodes().begin()->second->setTransformationsMatrix(Matrix4x4().identity());
			}
			model->addAnimationSetup(Model::ANIMATIONSETUP_DEFAULT, 0, 0, true);
			ModelTools::prepareForIndexedRendering(model);
			// scale up model if dimension too less, this occurres with importing FBX that was exported by UE
			// TODO: maybe make this conditional
			{
				auto width = model->getBoundingBox()->getDimensions().getX();
				auto height = model->getBoundingBox()->getDimensions().getY();
				auto depth = model->getBoundingBox()->getDimensions().getZ();
				if (width < 0.2f && height < 0.2f && depth < 0.2f) {
					if (width > Math::EPSILON && width < height && width < depth) {
						importFixScale = 1.0f / width / 5.0f;
					} else
					if (height > Math::EPSILON && height < width && height < depth) {
						importFixScale = 1.0f / height / 5.0f;
					} else
					if (depth > Math::EPSILON) {
						importFixScale = 1.0f / depth / 5.0f;
					}
				}
				model->setImportTransformationsMatrix(model->getImportTransformationsMatrix().clone().scale(importFixScale));
				model->getBoundingBox()->getMin().scale(importFixScale);
				model->getBoundingBox()->getMax().scale(importFixScale);
				model->getBoundingBox()->update();
				scale.scale(1.0f / importFixScale);
			}
			auto entityType = Prototype_Type::MODEL;
			if (meshNode.node->getVertices().size() == 0) {
				entityType = Prototype_Type::EMPTY;
				delete model;
				model = nullptr;
			}
			Prototype* prototype = nullptr;
			if (entityType == Prototype_Type::MODEL && model != nullptr) {
				for (auto i = 0; i < scene.getLibrary()->getPrototypeCount(); i++) {
					auto prototypeCompare = scene.getLibrary()->getPrototypeAt(i);
					if (prototypeCompare->getType() != Prototype_Type::MODEL)
						continue;

					if (ModelUtilities::equals(model, prototypeCompare->getModel()) == true) {
						prototype = prototypeCompare;
						delete model;
						model = nullptr;
						break;
					}
				}
				if (prototype == nullptr && model != nullptr) {
					auto modelFileName = meshNode.name + ".tm";
					TMWriter::write(
						model,
						modelPathName,
						modelFileName
					  );
					delete model;
					prototype = entityLibrary->addModel(
						nodeIdx++,
						meshNode.name,
						meshNode.name,
						modelPathName,
						modelFileName,
						Vector3()
					);
				}
			} else
			if (entityType == Prototype_Type::EMPTY) {
				if (emptyEntity == nullptr) {
					emptyEntity = entityLibrary->addEmpty(nodeIdx++, "Default Empty", "");
				}
				prototype = emptyEntity;
			} else {
				Console::println(string("DAEReader::readLevel(): unknown entity type. Skipping"));
				delete model;
				model = nullptr;
				continue;
			}
			Transformations sceneEntityTransformations;
			sceneEntityTransformations.setTranslation(translation);
			sceneEntityTransformations.addRotation(rotationOrder->getAxis0(), rotation.getArray()[rotationOrder->getAxis0VectorIndex()]);
			sceneEntityTransformations.addRotation(rotationOrder->getAxis1(), rotation.getArray()[rotationOrder->getAxis1VectorIndex()]);
			sceneEntityTransformations.addRotation(rotationOrder->getAxis2(), rotation.getArray()[rotationOrder->getAxis2VectorIndex()]);
			sceneEntityTransformations.setScale(scale);
			sceneEntityTransformations.update();
			auto object = new SceneEntity(
				meshNode.id,
				meshNode.id,
				sceneEntityTransformations,
				prototype
			);
			scene.addEntity(object);
		}
		//
		progressIdx++;
	}

	if (progressCallback != nullptr) progressCallback->progress(0.9f);

	// export to tl
	SceneWriter::write(
		pathName,
		Tools::removeFileEnding(fileName) + ".tl",
		scene
	);

	//
	delete levelModel;

	//
	if (progressCallback != nullptr) progressCallback->progress(1.0f);
}