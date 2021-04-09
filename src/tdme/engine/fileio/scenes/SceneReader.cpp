#include <tdme/engine/fileio/scenes/SceneReader.h>

#include <string>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/models/TMWriter.h>
#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/fileio/scenes/SceneWriter.h>
#include <tdme/engine/fileio/ProgressCallback.h>
#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneEntity.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/engine/scene/SceneLight.h>
#include <tdme/engine/subsystems/rendering/ModelStatistics.h>
#include <tdme/engine/ModelUtilities.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Vector4.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/utilities/StringTools.h>

#include <rapidjson/document.h>

using std::string;
using std::to_string;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::models::TMWriter;
using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::fileio::scenes::SceneReader;
using tdme::engine::fileio::scenes::SceneWriter;
using tdme::engine::fileio::ProgressCallback;
using tdme::engine::model::Animation;
using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::UpVector;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneEntity;
using tdme::engine::scene::SceneLibrary;
using tdme::engine::scene::SceneLight;
using tdme::engine::subsystems::rendering::ModelStatistics;
using tdme::engine::ModelUtilities;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::math::Vector4;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Console;
using tdme::utilities::Float;
using tdme::utilities::ModelTools;
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

	scene.setApplicationRootPathName(Tools::getApplicationRootPathName(pathName));
	// auto version = Float::parseFloat((jRoot["version"].GetString()));
	scene.setRotationOrder(jRoot.FindMember("ro") != jRoot.MemberEnd()?RotationOrder::valueOf(jRoot["ro"].GetString()) : RotationOrder::XYZ);
	scene.clearProperties();
	for (auto i = 0; i < jRoot["properties"].GetArray().Size(); i++) {
		auto& jSceneProperty = jRoot["properties"].GetArray()[i];
		scene.addProperty(
			jSceneProperty["name"].GetString(),
			jSceneProperty["value"].GetString()
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
		auto& jPrototype = jRoot["models"].GetArray()[i];
		Prototype* prototype = PrototypeReader::read(
			jPrototype["id"].GetInt(),
			pathName,
			jPrototype["entity"]
		);
		if (prototype == nullptr) {
			Console::println("SceneReader::doImport(): Invalid prototype = " + to_string(jPrototype["id"].GetInt()));
			continue;
		}
		scene.getLibrary()->addPrototype(prototype);
		if (jPrototype.FindMember("properties") != jPrototype.MemberEnd()) {
			for (auto j = 0; j < jPrototype["properties"].GetArray().Size(); j++) {
				auto& jPrototypeProperty = jPrototype["properties"].GetArray()[j];
				prototype->addProperty(
					(jPrototypeProperty["name"].GetString()),
					(jPrototypeProperty["value"].GetString())
				);
			}
		}

		if (progressCallback != nullptr) progressCallback->progress(0.33f + static_cast<float>(progressStepCurrent) / static_cast<float>(jRoot["models"].GetArray().Size()) * 0.33f);
		progressStepCurrent++;
	}
	scene.clearEntities();

	for (auto i = 0; i < jRoot["objects"].GetArray().Size(); i++) {
		auto& jSceneEntity = jRoot["objects"].GetArray()[i];
		auto prototype = scene.getLibrary()->getPrototype(jSceneEntity["mid"].GetInt());
		if (prototype == nullptr) {
			Console::println("SceneReader::doImport(): No prototype found with id = " + to_string(jSceneEntity["mid"].GetInt()));

			if (progressCallback != nullptr && progressStepCurrent % 1000 == 0) progressCallback->progress(0.66f + static_cast<float>(progressStepCurrent) / static_cast<float>(jRoot["objects"].GetArray().Size()) * 0.33f);
			progressStepCurrent++;

			continue;
		}

		Transformations transformations;
		transformations.setPivot(prototype->getPivot());
		transformations.setTranslation(
			Vector3(
				jSceneEntity["tx"].GetFloat(),
				jSceneEntity["ty"].GetFloat(),
				jSceneEntity["tz"].GetFloat()
			)
		);
		transformations.setScale(
			Vector3(
				jSceneEntity["sx"].GetFloat(),
				jSceneEntity["sy"].GetFloat(),
				jSceneEntity["sz"].GetFloat()
			)
		);
		Vector3 rotation(
			jSceneEntity["rx"].GetFloat(),
			jSceneEntity["ry"].GetFloat(),
			jSceneEntity["rz"].GetFloat()
		);
		transformations.addRotation(scene.getRotationOrder()->getAxis0(), rotation.getArray()[scene.getRotationOrder()->getAxis0VectorIndex()]);
		transformations.addRotation(scene.getRotationOrder()->getAxis1(), rotation.getArray()[scene.getRotationOrder()->getAxis1VectorIndex()]);
		transformations.addRotation(scene.getRotationOrder()->getAxis2(), rotation.getArray()[scene.getRotationOrder()->getAxis2VectorIndex()]);
		transformations.update();
		auto sceneEntity = new SceneEntity(
			objectIdPrefix != "" ?
				objectIdPrefix + jSceneEntity["id"].GetString() :
				(jSceneEntity["id"].GetString()),
			 jSceneEntity.FindMember("descr") != jSceneEntity.MemberEnd()?jSceneEntity["descr"].GetString() : "",
			 transformations,
			 prototype
		);
		if (jSceneEntity.FindMember("properties") != jSceneEntity.MemberEnd()) {
			for (auto j = 0; j < jSceneEntity["properties"].GetArray().Size(); j++) {
				auto& jSceneEntityProperty = jSceneEntity["properties"].GetArray()[j];
				sceneEntity->addProperty(
					jSceneEntityProperty["name"].GetString(),
					jSceneEntityProperty["value"].GetString()
				);
			}
		}
		sceneEntity->setReflectionEnvironmentMappingId(jSceneEntity.FindMember("r") != jSceneEntity.MemberEnd()?jSceneEntity["r"].GetString():"");
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
	auto sceneLibrary = scene.getLibrary();
	auto nodeId = node->getId();
	if (parentName.length() > 0) nodeId = parentName + "." + nodeId;
	auto modelName = nodeId;
	modelName = StringTools::regexReplace(modelName, "[-_]{1}[0-9]+$", "");
	modelName = StringTools::regexReplace(modelName, "[0-9]+$", "");
	auto haveName = sceneLibrary->getPrototypeCount() == 0;
	if (haveName == false) {
		for (auto i = 0; i < 10000; i++) {
			haveName = true;
			auto modelNameTry = modelName;
			if (i > 0) modelNameTry+= to_string(i);
			for (auto entityIdx = 0; entityIdx < sceneLibrary->getPrototypeCount(); entityIdx++) {
				auto entity = sceneLibrary->getPrototypeAt(entityIdx);
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

	auto sceneModel = ModelReader::read(pathName, fileName);

	if (progressCallback != nullptr) progressCallback->progress(0.1f);

	auto upVector = sceneModel->getUpVector();
	RotationOrder* rotationOrder = sceneModel->getRotationOrder();

	scene.setRotationOrder(rotationOrder);

	auto sceneLibrary = scene.getLibrary();
	auto nodeIdx = 0;
	Prototype* emptyPrototype = nullptr;
	Matrix4x4 sceneModelImportRotationMatrix;
	Vector3 sceneModelScale;
	sceneModelImportRotationMatrix.set(sceneModel->getImportTransformationsMatrix());
	sceneModelImportRotationMatrix.getScale(sceneModelScale);
	sceneModelImportRotationMatrix.scale(Vector3(1.0f / sceneModelScale.getX(), 1.0f / sceneModelScale.getY(), 1.0f / sceneModelScale.getZ()));
	auto progressTotal = sceneModel->getSubNodes().size();
	auto progressIdx = 0;
	for (auto nodeIt: sceneModel->getSubNodes()) {
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
			model->setImportTransformationsMatrix(sceneModel->getImportTransformationsMatrix());
			float importFixScale = 1.0f;
			Vector3 translation, scale;
			Vector3 xAxis, yAxis, zAxis;
			Matrix4x4 nodeTransformationsMatrix;
			nodeTransformationsMatrix.set(meshNode.transformationsMatrix);
			nodeTransformationsMatrix.getAxes(xAxis, yAxis, zAxis);
			nodeTransformationsMatrix.getTranslation(translation);
			nodeTransformationsMatrix.getScale(scale);
			xAxis.normalize();
			yAxis.normalize();
			zAxis.normalize();
			nodeTransformationsMatrix.setAxes(xAxis, yAxis, zAxis);
			if ((upVector == UpVector::Y_UP && Vector3::computeDotProduct(Vector3::computeCrossProduct(xAxis, yAxis), zAxis) < 0.0f) ||
				(upVector == UpVector::Z_UP && Vector3::computeDotProduct(Vector3::computeCrossProduct(xAxis, zAxis), yAxis) < 0.0f)) {
				xAxis.scale(-1.0f);
				yAxis.scale(-1.0f);
				zAxis.scale(-1.0f);
				nodeTransformationsMatrix.setAxes(xAxis, yAxis, zAxis);
				scale.scale(-1.0f);
			}
			auto rotation = nodeTransformationsMatrix.computeEulerAngles();
			scale = sceneModelImportRotationMatrix.multiply(scale);
			rotation = sceneModelImportRotationMatrix.multiply(rotation);
			translation = model->getImportTransformationsMatrix().multiply(translation);

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
			auto prototypeType = Prototype_Type::MODEL;
			if (meshNode.node->getVertices().size() == 0) {
				prototypeType = Prototype_Type::EMPTY;
				delete model;
				model = nullptr;
			}
			Prototype* prototype = nullptr;
			if (prototypeType == Prototype_Type::MODEL && model != nullptr) {
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
					prototype = sceneLibrary->addModel(
						nodeIdx++,
						meshNode.name,
						meshNode.name,
						modelPathName,
						modelFileName,
						Vector3()
					);
				}
			} else
			if (prototypeType == Prototype_Type::EMPTY) {
				if (emptyPrototype == nullptr) {
					emptyPrototype = sceneLibrary->addEmpty(nodeIdx++, "Default Empty", "");
				}
				prototype = emptyPrototype;
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
			auto sceneEntity = new SceneEntity(
				meshNode.id,
				meshNode.id,
				sceneEntityTransformations,
				prototype
			);
			scene.addEntity(sceneEntity);
		}
		//
		progressIdx++;
	}

	if (progressCallback != nullptr) progressCallback->progress(0.9f);

	// export to tl
	SceneWriter::write(
		pathName,
		Tools::removeFileEnding(fileName) + ".tscene",
		scene
	);

	//
	delete sceneModel;

	//
	if (progressCallback != nullptr) progressCallback->progress(1.0f);
}
