#include <tdme/engine/fileio/models/DAEReader.h>

#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/engine/fileio/models/TMWriter.h>
#include <tdme/engine/fileio/scenes/SceneWriter.h>
#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Joint.h>
#include <tdme/engine/model/JointWeight.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/Skinning.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/subsystems/rendering/ModelStatistics.h>
#include <tdme/engine/ModelUtilities.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>

#include <ext/tinyxml/tinyxml.h>

#define AVOID_NULLPTR_STRING(arg) (arg == nullptr?"":arg)

using std::array;
using std::map;
using std::string;
using std::to_string;
using std::unordered_set;
using std::vector;

using tdme::engine::fileio::models::DAEReader;
using tdme::engine::fileio::models::ModelFileIOException;
using tdme::engine::fileio::models::TMWriter;
using tdme::engine::model::Animation;
using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Joint;
using tdme::engine::model::JointWeight;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::Skinning;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::model::UpVector;
using tdme::engine::subsystems::rendering::ModelStatistics;
using tdme::engine::ModelUtilities;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::Float;
using tdme::utilities::Integer;
using tdme::utilities::ModelTools;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;

using tinyxml::TiXmlAttribute;
using tinyxml::TiXmlDocument;
using tinyxml::TiXmlElement;

const Color4 DAEReader::BLENDER_AMBIENT_NONE(0.0f, 0.0f, 0.0f, 1.0f);

Model* DAEReader::read(const string& pathName, const string& fileName)
{
	// load dae xml document
	auto xmlContent = FileSystem::getInstance()->getContentAsString(pathName, fileName);
	TiXmlDocument xmlDocument;
	xmlDocument.Parse(xmlContent.c_str());
	if (xmlDocument.Error() == true) {
		throw ModelFileIOException(
			string("Could not parse XML. Error='") + string(xmlDocument.ErrorDesc()) + string("'")
		);
	}
	TiXmlElement* xmlRoot = xmlDocument.RootElement();

	// authoring tool
	auto authoringTool = getAuthoringTool(xmlRoot);

	// up vector and rotation order
	auto upVector = getUpVector(xmlRoot);
	RotationOrder* rotationOrder = nullptr;
	{
		auto v = upVector;
		if (v == UpVector::Y_UP) {
			rotationOrder = RotationOrder::ZYX;
		} else
		if (v == UpVector::Z_UP) {
			rotationOrder = RotationOrder::YZX;
		}
	}

	// 	create model
	auto model = new Model(
		FileSystem::getInstance()->getCanonicalPath(pathName, fileName),
		fileName,
		upVector,
		rotationOrder,
		nullptr,
		authoringTool
	);

	// import matrix
	setupModelImportRotationMatrix(xmlRoot, model);
	setupModelImportScaleMatrix(xmlRoot, model);

	// parse scene from xml
	string xmlSceneId;
	auto xmlScene = getChildrenByTagName(xmlRoot, "scene").at(0);
	for (auto xmlInstanceVisualscene: getChildrenByTagName(xmlScene, "instance_visual_scene")) {
		xmlSceneId = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlInstanceVisualscene->Attribute("url"))), 1);
	}

	// parse visual scenes
	auto xmlLibraryVisualScenes = getChildrenByTagName(xmlRoot, "library_visual_scenes").at(0);
	for (auto xmlLibraryVisualScene: getChildrenByTagName(xmlLibraryVisualScenes, "visual_scene")) {
		auto xmlVisualSceneId = string(AVOID_NULLPTR_STRING(xmlLibraryVisualScene->Attribute("id")));
		if (xmlVisualSceneId == xmlSceneId) {
			// default FPS
			auto fps = 30.0f;
			// parse frames per second
			auto xmlExtraNodes = getChildrenByTagName(xmlLibraryVisualScene, "extra");
			if (xmlExtraNodes.empty() == false) {
				auto xmlExtraNode = xmlExtraNodes.at(0);
				for (auto xmlTechnique: getChildrenByTagName(xmlExtraNode, "technique")) {
					auto xmlFrameRateNodes = getChildrenByTagName(xmlTechnique, "frame_rate");
					if (xmlFrameRateNodes.empty() == false) {
						fps = Float::parseFloat(string(AVOID_NULLPTR_STRING(xmlFrameRateNodes.at(0)->GetText())));
						break;
					}
				}
			}
			// set up frames per seconds
			model->setFPS(fps);
			// visual scene root nodes
			for (auto xmlNode: getChildrenByTagName(xmlLibraryVisualScene, "node")) {
				auto node = readVisualSceneNode(pathName, model, nullptr, xmlRoot, xmlNode, fps);
				if (node != nullptr) {
					model->getSubNodes()[node->getId()] = node;
					model->getNodes()[node->getId()] = node;
				}
			}
		}
	}
	if (ModelTools::hasDefaultAnimation(model) == false) ModelTools::createDefaultAnimation(model, 0);
	// set up joints
	ModelTools::setupJoints(model);
	// fix animation length
	ModelTools::fixAnimationLength(model);
	// prepare for indexed rendering
	ModelTools::prepareForIndexedRendering(model);
	//
	return model;
}

Model::AuthoringTool DAEReader::getAuthoringTool(TiXmlElement* xmlRoot)
{
	for (auto xmlAsset: getChildrenByTagName(xmlRoot, "asset")) {
		for (auto xmlContributer: getChildrenByTagName(xmlAsset, "contributor")) {
			for (auto xmlAuthoringTool: getChildrenByTagName(xmlContributer, "authoring_tool")) {
				if (string(AVOID_NULLPTR_STRING(xmlAuthoringTool->GetText())).find("Blender") != -1) {
					return Model::AUTHORINGTOOL_BLENDER;
				}
			}
		}
	}
	return Model::AUTHORINGTOOL_UNKNOWN;
}

UpVector* DAEReader::getUpVector(TiXmlElement* xmlRoot)
{
	// determine up axis
	for (auto xmlAsset: getChildrenByTagName(xmlRoot, "asset")) {
		for (auto xmlAssetUpAxis: getChildrenByTagName(xmlAsset, "up_axis")) {
			auto upAxis = string(AVOID_NULLPTR_STRING(xmlAssetUpAxis->GetText()));
			if (StringTools::equalsIgnoreCase(upAxis, "Y_UP") == true) {
				return UpVector::Y_UP;
			} else
			if (StringTools::equalsIgnoreCase(upAxis, "Z_UP") == true) {
				return UpVector::Z_UP;
			} else
			if (StringTools::equalsIgnoreCase(upAxis, "X_UP") == true) {
				throw ModelFileIOException("X-Up is not supported");
			} else {
				throw ModelFileIOException("Unknown Up vector");
			}
		}
	}
	throw ModelFileIOException("Unknown Up vector");
}

void DAEReader::setupModelImportRotationMatrix(TiXmlElement* xmlRoot, Model* model)
{
	// determine rotation matrix
	for (auto xmlAsset: getChildrenByTagName(xmlRoot, "asset")) {
		for (auto xmlAssetUpAxis: getChildrenByTagName(xmlAsset, "up_axis")) {
			auto upAxis = string(AVOID_NULLPTR_STRING(xmlAssetUpAxis->GetText()));
			if (StringTools::equalsIgnoreCase(upAxis, "Y_UP") == true) {
			} else
			if (StringTools::equalsIgnoreCase(upAxis, "Z_UP") == true) {
				model->setImportTransformationsMatrix(model->getImportTransformationsMatrix().clone().rotate(Vector3(1.0f, 0.0f, 0.0f), -90.0f));
			} else
			if (StringTools::equalsIgnoreCase(upAxis, "X_UP") == true) {
				model->setImportTransformationsMatrix(model->getImportTransformationsMatrix().clone().rotate(Vector3(0.0f, 1.0f, 0.0f), -90.0f));
			} else {
				Console::println(string("Warning: Unknown up axis: " + upAxis));
			}
		}
	}
}

void DAEReader::setupModelImportScaleMatrix(TiXmlElement* xmlRoot, Model* model)
{
	// determine scale
	for (auto xmlAsset: getChildrenByTagName(xmlRoot, "asset")) {
		for (auto xmlAssetUnit: getChildrenByTagName(xmlAsset, "unit")) {
			string tmp;
			if ((tmp = string(AVOID_NULLPTR_STRING(xmlAssetUnit->Attribute("meter")))).length() > 0) {
				float scaleFactor = Float::parseFloat(tmp);
				model->setImportTransformationsMatrix(model->getImportTransformationsMatrix().clone().scale(scaleFactor));
			}
		}
	}
}

Node* DAEReader::readVisualSceneNode(const string& pathName, Model* model, Node* parentNode, TiXmlElement* xmlRoot, TiXmlElement* xmlNode, float fps)
{
	auto xmlInstanceControllers = getChildrenByTagName(xmlNode, "instance_controller");
	if (xmlInstanceControllers.empty() == false) {
		return readVisualSceneInstanceController(pathName, model, parentNode, xmlRoot, xmlNode);
	} else {
		return readNode(pathName, model, parentNode, xmlRoot, xmlNode, fps);
	}
}

Node* DAEReader::readNode(const string& pathName, Model* model, Node* parentNode, TiXmlElement* xmlRoot, TiXmlElement* xmlNode, float fps)
{
	auto xmlNodeId = string(AVOID_NULLPTR_STRING(xmlNode->Attribute("id")));
	auto xmlNodeName = string(AVOID_NULLPTR_STRING(xmlNode->Attribute("name")));
	if (xmlNodeId.length() == 0) xmlNodeId = xmlNodeName;

	// create node
	auto node = new Node(model, parentNode, xmlNodeId, xmlNodeName);

	// set up local transformations matrix
	auto xmlMatrixElements = getChildrenByTagName(xmlNode, "matrix");
	if (xmlMatrixElements.empty() == false) {
		StringTokenizer t;
		Matrix4x4 transformationsMatrix;
		auto xmlMatrix = string(AVOID_NULLPTR_STRING(xmlMatrixElements.at(0)->GetText()));
		t.tokenize(xmlMatrix, " \n\r");
		array<float, 16> transformationsMatrixArray;
		for (auto i = 0; i < transformationsMatrixArray.size(); i++) {
			transformationsMatrixArray[i] = Float::parseFloat(t.nextToken());
		}
		transformationsMatrix.set(transformationsMatrixArray).transpose();
		node->setTransformationsMatrix(transformationsMatrix);
	}

	// parse animations
	auto xmlAnimationsLibrary = getChildrenByTagName(xmlRoot, "library_animations");
	if (xmlAnimationsLibrary.empty() == false) {
		for (auto xmlAnimation: getChildrenByTagName(xmlAnimationsLibrary.at(0), "animation")) {
			// older DAE has animation/animation xml nodes
			auto _xmlAnimation = getChildrenByTagName(xmlAnimation, "animation");
			if (_xmlAnimation.empty() == false) {
				xmlAnimation = _xmlAnimation.at(0);
			}
			// find sampler source
			string xmlSamplerSource;
			auto xmlChannel = getChildrenByTagName(xmlAnimation, "channel").at(0);
			if (StringTools::startsWith(string(AVOID_NULLPTR_STRING(xmlChannel->Attribute("target"))), xmlNodeId + "/") == true) {
				xmlSamplerSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlChannel->Attribute("source"))), 1);
			}
			// check for sampler source
			if (xmlSamplerSource.length() == 0) {
				continue;
			}
			// parse animation output matrices
			string xmlSamplerOutputSource;
			string xmlSamplerInputSource;
			auto xmlSampler = getChildrenByTagName(xmlAnimation, "sampler").at(0);
			for (auto xmlSamplerInput: getChildrenByTagName(xmlSampler, "input")) {
				if (string(AVOID_NULLPTR_STRING(xmlSamplerInput->Attribute("semantic"))) == "OUTPUT") {
					xmlSamplerOutputSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlSamplerInput->Attribute("source"))), 1);
				} else
				if (string(AVOID_NULLPTR_STRING(xmlSamplerInput->Attribute("semantic"))) == "INPUT") {
					xmlSamplerInputSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlSamplerInput->Attribute("source"))), 1);
				}
			}
			// check for sampler source
			if (xmlSamplerOutputSource.length() == 0) {
				throw ModelFileIOException(
					"Could not find xml sampler output source for animation for '" +
					(xmlNodeId) +
					"'"
				);
			}
			// load animation input matrices
			// TODO: check accessor "time"
			vector<float> keyFrameTimes;
			for (auto xmlAnimationSource: getChildrenByTagName(xmlAnimation, "source")) {
				if (string(AVOID_NULLPTR_STRING(xmlAnimationSource->Attribute("id"))) == xmlSamplerInputSource) {
					auto xmlFloatArray = getChildrenByTagName(xmlAnimationSource, "float_array").at(0);
					auto frames = Integer::parseInt(string(AVOID_NULLPTR_STRING(xmlFloatArray->Attribute("count"))));
					auto valueString = string(AVOID_NULLPTR_STRING(xmlFloatArray->GetText()));
					auto keyFrameIdx = 0;
					keyFrameTimes.resize(frames);
					StringTokenizer t;
					t.tokenize(valueString, " \n\r");
					while (t.hasMoreTokens()) {
						keyFrameTimes[keyFrameIdx++] = Float::parseFloat(t.nextToken());
					}
				}
			}
			// load animation output matrices
			// TODO: check accessor "transform"
			if (keyFrameTimes.size() > 0) {
				for (auto xmlAnimationSource: getChildrenByTagName(xmlAnimation, "source")) {
					if (string(AVOID_NULLPTR_STRING(xmlAnimationSource->Attribute("id"))) == xmlSamplerOutputSource) {
						auto xmlFloatArray = getChildrenByTagName(xmlAnimationSource, "float_array").at(0);
						auto keyFrames = Integer::parseInt(string(AVOID_NULLPTR_STRING(xmlFloatArray->Attribute("count")))) / 16;
						// some models have animations without frames
						if (keyFrames > 0) {
							auto valueString = string(AVOID_NULLPTR_STRING(xmlFloatArray->GetText()));
							StringTokenizer t;
							t.tokenize(valueString, " \n\r");
							// parse key frame
							int32_t keyFrameIdx = 0;
							vector<Matrix4x4> keyFrameMatrices;
							keyFrameMatrices.resize(keyFrames);
							while (t.hasMoreTokens()) {
								// set animation transformation matrix at frame
								array<float, 16> keyFrameMatricesArray;
								for (auto i = 0; i < keyFrameMatricesArray.size() ;i++) {
									keyFrameMatricesArray[i] = Float::parseFloat(t.nextToken());
								}
								keyFrameMatrices[keyFrameIdx].set(keyFrameMatricesArray);
								keyFrameMatrices[keyFrameIdx].transpose();
								keyFrameIdx++;
							}

							auto frames = static_cast< int32_t >(Math::ceil(keyFrameTimes[keyFrameTimes.size() - 1] * fps));
							if (frames > 0) {
								ModelTools::createDefaultAnimation(model, frames);
								auto animation = new Animation();
								vector<Matrix4x4> transformationsMatrices;
								transformationsMatrices.resize(frames);
								auto tansformationsMatrixLast = &keyFrameMatrices[0];
								keyFrameIdx = 0;
								auto frameIdx = 0;
								auto timeStampLast = 0.0f;
								for (auto keyFrameTime : keyFrameTimes) {
									auto transformationsMatrixCurrent = &keyFrameMatrices[(keyFrameIdx) % keyFrameMatrices.size()];
									float timeStamp;
									for (timeStamp = timeStampLast; timeStamp < keyFrameTime; timeStamp += 1.0f / fps) {
										if (frameIdx >= frames) {
											// TODO: check me again!
											// Console::println(string("Warning: skipping frame: ") + to_string(frameIdx));
											frameIdx++;
											continue;
										}
										Matrix4x4::interpolateLinear(*tansformationsMatrixLast, *transformationsMatrixCurrent, (timeStamp - timeStampLast) / (keyFrameTime - timeStampLast), transformationsMatrices[frameIdx]);
										frameIdx++;
									}
									timeStampLast = timeStamp;
									tansformationsMatrixLast = transformationsMatrixCurrent;
									keyFrameIdx++;
								}
								animation->setTransformationsMatrices(transformationsMatrices);
								node->setAnimation(animation);
							}
						}
					}
				}
			}
		}
	}

	// parse sub nodes
	for (auto _xmlNode: getChildrenByTagName(xmlNode, "node")) {
		auto _node = readVisualSceneNode(pathName, model, node, xmlRoot, _xmlNode, fps);
		if (_node != nullptr) {
			node->getSubNodes()[_node->getId()] = _node;
			model->getNodes()[_node->getId()] = _node;
		}
	}

	// check for geometry data
	string xmlInstanceGeometryId;
	auto xmlInstanceGeometryElements = getChildrenByTagName(xmlNode, "instance_geometry");
	if (xmlInstanceGeometryElements.empty() == false) {
		auto xmlInstanceGeometryElement = xmlInstanceGeometryElements.at(0);
		// fetch instance geometry url
		xmlInstanceGeometryId = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlInstanceGeometryElement->Attribute("url"))), 1);
		// determine bound materials
		map<string, string> materialSymbols;
		for (auto xmlBindMaterial: getChildrenByTagName(xmlInstanceGeometryElement, "bind_material"))
		for (auto xmlTechniqueCommon: getChildrenByTagName(xmlBindMaterial, "technique_common"))
		for (auto xmlInstanceMaterial: getChildrenByTagName(xmlTechniqueCommon, "instance_material")) {
			materialSymbols[string(AVOID_NULLPTR_STRING(xmlInstanceMaterial->Attribute("symbol")))] =
				string(AVOID_NULLPTR_STRING(xmlInstanceMaterial->Attribute("target")));
		}
		// parse geometry
		readGeometry(pathName, model, node, xmlRoot, xmlInstanceGeometryId, materialSymbols);
		return node;
	}

	// otherwise check for "instance_node"
	string xmlInstanceNodeId;
	for (auto xmlInstanceNodeElement: getChildrenByTagName(xmlNode, "instance_node")) {
		xmlInstanceNodeId = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlInstanceNodeElement->Attribute("url"))), 1);
	}
	// do we have a instance node id?
	if (xmlInstanceNodeId.length() > 0) {
		for (auto xmlLibraryNodes: getChildrenByTagName(xmlRoot, "library_nodes"))
		for (auto xmlLibraryNode: getChildrenByTagName(xmlLibraryNodes, "node"))
		if (string(AVOID_NULLPTR_STRING(xmlLibraryNode->Attribute("id"))) == xmlInstanceNodeId) {
			// parse sub nodes
			for (auto _xmlNode: getChildrenByTagName(xmlLibraryNode, "node")) {
				auto _node = readVisualSceneNode(pathName, model, parentNode, xmlRoot, _xmlNode, fps);
				if (_node != nullptr) {
					node->getSubNodes()[_node->getId()] = _node;
					model->getNodes()[_node->getId()] = _node;
				}
			}
			// parse geometry
			for (auto xmlInstanceGeometry: getChildrenByTagName(xmlLibraryNode, "instance_geometry")) {
				auto xmlGeometryId = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlInstanceGeometry->Attribute("url"))), 1);
				// parse material symbols
				map<string, string> materialSymbols;
				for (auto xmlBindMaterial: getChildrenByTagName(xmlInstanceGeometry, "bind_material"))
				for (auto xmlTechniqueCommon: getChildrenByTagName(xmlBindMaterial, "technique_common"))
				for (auto xmlInstanceMaterial: getChildrenByTagName(xmlTechniqueCommon, "instance_material")) {
					materialSymbols[string(AVOID_NULLPTR_STRING(xmlInstanceMaterial->Attribute("symbol")))] =
						string(AVOID_NULLPTR_STRING(xmlInstanceMaterial->Attribute("target")));
				}
				// parse geometry
				readGeometry(pathName, model, node, xmlRoot, xmlGeometryId, materialSymbols);
			}
		}
	}
	return node;
}

Node* DAEReader::readVisualSceneInstanceController(const string& pathName, Model* model, Node* parentNode, TiXmlElement* xmlRoot, TiXmlElement* xmlNode)
{
	auto xmlNodeId = string(AVOID_NULLPTR_STRING(xmlNode->Attribute("id")));
	auto xmlNodeName = string(AVOID_NULLPTR_STRING(xmlNode->Attribute("name")));
	map<string, string> materialSymbols;
	// geometry id
	string xmlGeometryId;
	// parse library controllers, find our controller
	auto xmlInstanceControllers = getChildrenByTagName(xmlNode, "instance_controller");
	TiXmlElement* xmlSkin = nullptr;
	auto xmlInstanceController = xmlInstanceControllers.at(0);

	// parse material symbols
	for (auto xmlBindMaterial: getChildrenByTagName(xmlInstanceController, "bind_material"))
	for (auto xmlTechniqueCommon: getChildrenByTagName(xmlBindMaterial, "technique_common"))
	for (auto xmlInstanceMaterial: getChildrenByTagName(xmlTechniqueCommon, "instance_material")) {
		materialSymbols[string(AVOID_NULLPTR_STRING(xmlInstanceMaterial->Attribute("symbol")))] =
			string(AVOID_NULLPTR_STRING(xmlInstanceMaterial->Attribute("target")));
	}

	auto xmlInstanceControllerId = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlInstanceController->Attribute("url"))), 1);
	auto xmlLibraryControllers = getChildrenByTagName(xmlRoot, "library_controllers").at(0);
	for (auto xmlLibraryController: getChildrenByTagName(xmlLibraryControllers, "controller")) {
		// our controller ?
		if (string(AVOID_NULLPTR_STRING(xmlLibraryController->Attribute("id"))) == xmlInstanceControllerId) {
			// parse skin
			auto xmlSkins = getChildrenByTagName(xmlLibraryController, "skin");
			if (xmlSkins.empty() == false) {
				xmlSkin = xmlSkins.at(0);
			}
		}
	}

	// check for xml skin
	if (xmlSkin == nullptr) {
		throw ModelFileIOException(
			"skin not found for instance controller '" +
			(xmlNodeId) +
			"'"
		);
	}

	// get geometry id
	xmlGeometryId = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlSkin->Attribute("source"))), 1);

	// parse bind shape matrix
	auto xmlMatrix = string(AVOID_NULLPTR_STRING(getChildrenByTagName(xmlSkin, "bind_shape_matrix").at(0)->GetText()));
	StringTokenizer t;;
	t.tokenize(xmlMatrix, " \n\r");
	array<float, 16> bindShapeMatrixArray;
	for (auto i = 0; i < bindShapeMatrixArray.size(); i++) {
		bindShapeMatrixArray[i] = Float::parseFloat(t.nextToken());
	}
	Matrix4x4 bindShapeMatrix;
	bindShapeMatrix.set(bindShapeMatrixArray).transpose();

	// create node
	auto node = new Node(model, parentNode, xmlNodeId, xmlNodeName);

	// create skinning
	auto skinning = new Skinning();

	// parse geometry
	readGeometry(pathName, model, node, xmlRoot, xmlGeometryId, materialSymbols);

	// parse joints
	string xmlJointsSource;
	string xmlJointsInverseBindMatricesSource;
	auto xmlJoints = getChildrenByTagName(xmlSkin, "joints").at(0);
	for (auto xmlJointsInput: getChildrenByTagName(xmlJoints, "input")) {
		if (string(AVOID_NULLPTR_STRING(xmlJointsInput->Attribute("semantic"))) == "JOINT") {
			xmlJointsSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlJointsInput->Attribute("source"))), 1);
		} else
		if (string(AVOID_NULLPTR_STRING(xmlJointsInput->Attribute("semantic"))) == "INV_BIND_MATRIX") {
			xmlJointsInverseBindMatricesSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlJointsInput->Attribute("source"))), 1);
		}
	}

	// check for joints sources
	if (xmlJointsSource.length() == 0) {
		throw ModelFileIOException(
			"joint source not found for instance controller '" +
			(xmlNodeId) +
			"'"
		);
	}

	// parse joint ids
	vector<Joint> joints;
	for (auto xmlSkinSource: getChildrenByTagName(xmlSkin, "source")) {
		if (string(AVOID_NULLPTR_STRING(xmlSkinSource->Attribute("id"))) == xmlJointsSource) {
			t.tokenize(string(AVOID_NULLPTR_STRING(getChildrenByTagName(xmlSkinSource, "Name_array").at(0)->GetText())), " \n\r");
			while (t.hasMoreTokens()) {
				joints.push_back(Joint(t.nextToken()));
			}
		}
	}

	// check for inverse bind matrices source
	if (xmlJointsInverseBindMatricesSource.length() == 0) {
		throw ModelFileIOException(
			"inverse bind matrices source not found for instance controller '" +
			(xmlNodeId) +
			"'"
		);
	}

	// Create joints bind matrices
	for (auto xmlSkinSource: getChildrenByTagName(xmlSkin, "source")) {
		if (string(AVOID_NULLPTR_STRING(xmlSkinSource->Attribute("id"))) == xmlJointsInverseBindMatricesSource) {
			t.tokenize(string(AVOID_NULLPTR_STRING(getChildrenByTagName(xmlSkinSource, "float_array").at(0)->GetText())), " \n\r");
			auto& _joints = skinning->getJoints();
			for (auto i = 0; i < joints.size(); i++) {
				// The vertices are defined in model space
				// The transformation to the local space of the joint is called the inverse bind matrix
				array<float, 16> bindMatrixArray;
				for (auto i = 0; i < bindMatrixArray.size(); i++) {
					bindMatrixArray[i] = Float::parseFloat(t.nextToken());
				}
				Matrix4x4 bindMatrix;
				bindMatrix.set(bindShapeMatrix);
				bindMatrix.multiply((Matrix4x4(bindMatrixArray)).transpose());
				joints[i].setBindMatrix(bindMatrix);
			}
		}
	}

	skinning->setJoints(joints);

	// read vertex influences
	vector<float> weights;
	auto xmlJointOffset = -1;
	auto xmlWeightOffset = -1;
	string xmlWeightsSource;
	auto xmlVertexWeights = getChildrenByTagName(xmlSkin, "vertex_weights").at(0);
	auto xmlVertexWeightInputs = getChildrenByTagName(xmlVertexWeights, "input");
	for (auto xmlVertexWeightInput: xmlVertexWeightInputs) {
		if (string(AVOID_NULLPTR_STRING(xmlVertexWeightInput->Attribute("semantic"))) == "JOINT") {
			if ((StringTools::substring(string(AVOID_NULLPTR_STRING(xmlVertexWeightInput->Attribute("source"))), 1) == xmlJointsSource) == false) {
				throw ModelFileIOException("joint inverse bind matrices source do not match");
			}
			xmlJointOffset = Integer::parseInt(string(AVOID_NULLPTR_STRING(xmlVertexWeightInput->Attribute("offset"))));
		} else
		if (string(AVOID_NULLPTR_STRING(xmlVertexWeightInput->Attribute("semantic"))) == "WEIGHT") {
			xmlWeightOffset = Integer::parseInt(string(AVOID_NULLPTR_STRING(xmlVertexWeightInput->Attribute("offset"))));
			xmlWeightsSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlVertexWeightInput->Attribute("source"))), 1);
		}
	}

	// check for vertex weight parameter
	if (xmlJointOffset == -1) {
		throw ModelFileIOException(
			"xml vertex weight joint offset missing for node '" +
			(xmlNodeId) +
			"'"
		);
	}
	if (xmlWeightOffset == -1) {
		throw ModelFileIOException(
			"xml vertex weight weight offset missing for node " +
			(xmlNodeId) +
			"'"
		);
	}
	if (xmlWeightsSource.length() == 0) {
		throw ModelFileIOException(
			"xml vertex weight weight source missing for node '" +
			(xmlNodeId) +
			"'"
		);
	}

	// parse weights
	for (auto xmlSkinSource: getChildrenByTagName(xmlSkin, "source")) {
		if (string(AVOID_NULLPTR_STRING(xmlSkinSource->Attribute("id"))) == xmlWeightsSource) {
			t.tokenize(string(AVOID_NULLPTR_STRING(getChildrenByTagName(xmlSkinSource, "float_array").at(0)->GetText())), " \n\r");
			while (t.hasMoreTokens()) {
				weights.push_back(Float::parseFloat(t.nextToken()));
			}
		}
	}
	skinning->setWeights(weights);

	// actually do parse joint influences of each vertex
	auto xmlVertexWeightInputCount = xmlVertexWeightInputs.size();
	auto vertexJointsInfluenceCountString = string(AVOID_NULLPTR_STRING(getChildrenByTagName(xmlVertexWeights, "vcount").at(0)->GetText()));
	auto vertexJointsInfluencesString = string(AVOID_NULLPTR_STRING(getChildrenByTagName(xmlVertexWeights, "v").at(0)->GetText()));
	t.tokenize(vertexJointsInfluenceCountString, " \n\r");
	StringTokenizer t2;
	t2.tokenize(vertexJointsInfluencesString, " \n\r");
	auto offset = 0;
	vector<vector<JointWeight>> verticesJointsWeights;
	while (t.hasMoreTokens()) {
		// read joint influences for current vertex
		auto vertexJointsInfluencesCount = Integer::parseInt(t.nextToken());
		vector<JointWeight>vertexJointsWeights;
		for (auto i = 0; i < vertexJointsInfluencesCount; i++) {
			auto vertexJoint = -1;
			auto vertexWeight = -1;
			while (vertexJoint == -1 || vertexWeight == -1) {
				auto value = Integer::parseInt(t2.nextToken());
				if (offset % xmlVertexWeightInputCount == xmlJointOffset) {
					vertexJoint = value;
				} else if (offset % xmlVertexWeightInputCount == xmlWeightOffset) {
					vertexWeight = value;
				}
				offset++;
			}
			vertexJointsWeights.push_back(JointWeight(vertexJoint, vertexWeight));
		}
		verticesJointsWeights.push_back(vertexJointsWeights);
	}
	skinning->setVerticesJointsWeights(verticesJointsWeights);
	node->setSkinning(skinning);

	//
	return node;
}

void DAEReader::readGeometry(const string& pathName, Model* model, Node* node, TiXmlElement* xmlRoot, const string& xmlNodeId, const map<string, string>& materialSymbols)
{
	vector<FacesEntity> facesEntities = node->getFacesEntities();
	auto verticesOffset = node->getVertices().size();
	vector<Vector3> vertices = node->getVertices();
	auto normalsOffset = node->getNormals().size();
	vector<Vector3> normals = node->getNormals();;
	auto textureCoordinatesOffset = node->getTextureCoordinates().size();
	auto textureCoordinates = node->getTextureCoordinates();
	auto xmlLibraryGeometries = getChildrenByTagName(xmlRoot, "library_geometries").at(0);
	for (auto xmlGeometry: getChildrenByTagName(xmlLibraryGeometries, "geometry")) {
		if (string(AVOID_NULLPTR_STRING(xmlGeometry->Attribute("id"))) == xmlNodeId) {
			auto xmlMesh = getChildrenByTagName(xmlGeometry, "mesh").at(0);
			vector<TiXmlElement*> xmlPolygonsList;
			// try to read from triangles
			for (auto xmlTriangesElement: getChildrenByTagName(xmlMesh, "triangles")) {
				xmlPolygonsList.push_back(xmlTriangesElement);
			}
			// try to read from polylist
			for (auto xmlPolyListElement: getChildrenByTagName(xmlMesh, "polylist")) {
				xmlPolygonsList.push_back(xmlPolyListElement);
			}
			// try to read from polygons
			for (auto xmlPolygonsElement: getChildrenByTagName(xmlMesh, "polygons")) {
				xmlPolygonsList.push_back(xmlPolygonsElement);
			}
			// parse from xml polygons elements
			for (auto xmlPolygons: xmlPolygonsList) {
				vector<Face> faces;
				FacesEntity facesEntity(node, xmlNodeId);
				if (StringTools::toLowerCase((xmlPolygons->Value())) == "polylist") {
					StringTokenizer t;
					t.tokenize(string(AVOID_NULLPTR_STRING(getChildrenByTagName(xmlPolygons, "vcount").at(0)->GetText())), " \t\n\r\f");
					while (t.hasMoreTokens()) {
						auto vertexCount = Integer::parseInt(t.nextToken());
						if (vertexCount != 3) {
							throw ModelFileIOException(
								 "we only support triangles in '" +
								 (xmlNodeId) +
								 "'"
							);
						}
					}
				}
				auto xmlInputs = -1;
				auto xmlVerticesOffset = -1;
				string xmlVerticesSource;
				auto xmlNormalsOffset = -1;
				string xmlNormalsSource;
				auto xmlTexCoordOffset = -1;
				string xmlTexCoordSource;
				auto xmlColorOffset = -1;
				string xmlColorSource;
				auto xmlMaterialId = string(AVOID_NULLPTR_STRING(xmlPolygons->Attribute("material")));
				auto materialSymbolIt = materialSymbols.find(xmlMaterialId);
				if (materialSymbolIt != materialSymbols.end()) {
					xmlMaterialId = materialSymbolIt->second;
					xmlMaterialId = StringTools::substring(xmlMaterialId, 1);
				}

				if (xmlMaterialId.length() > 0) {
					Material* material = nullptr;
					auto materialIt = model->getMaterials().find(xmlMaterialId);
					if (materialIt != model->getMaterials().end()) {
						material = materialIt->second;
					} else {
						// parse material as we do not have it yet
						material = readMaterial(pathName, model, xmlRoot, xmlMaterialId);
					}
					// set it up
					facesEntity.setMaterial(material);
				}
				unordered_set<int32_t> xmlInputSet;
				for (auto xmlTrianglesInput: getChildrenByTagName(xmlPolygons, "input")) {
					// check for vertices sources
					if (string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("semantic"))) == "VERTEX") {
						xmlVerticesOffset = Integer::parseInt(string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("offset"))));
						xmlVerticesSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("source"))), 1);
						xmlInputSet.insert(xmlVerticesOffset);
					} else
					// check for normals sources
					if (string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("semantic"))) == "NORMAL") {
						xmlNormalsOffset = Integer::parseInt(string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("offset"))));
						xmlNormalsSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("source"))), 1);
						xmlInputSet.insert(xmlNormalsOffset);
					} else
					// check for texture coordinate sources
					if (string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("semantic"))) == "TEXCOORD") {
						xmlTexCoordOffset = Integer::parseInt(string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("offset"))));
						xmlTexCoordSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("source"))), 1);
						xmlInputSet.insert(xmlTexCoordOffset);
					} else
					// check for color coordinate sources
					if (string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("semantic"))) == "COLOR") {
						xmlColorOffset = Integer::parseInt(string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("offset"))));
						xmlColorSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlTrianglesInput->Attribute("source"))), 1);
						xmlInputSet.insert(xmlColorOffset);
					}
				}
				xmlInputs = xmlInputSet.size();
				// get vertices source
				for (auto xmlVertices: getChildrenByTagName(xmlMesh, "vertices")) {
					if (string(AVOID_NULLPTR_STRING(xmlVertices->Attribute("id"))) == xmlVerticesSource) {
						for (auto xmlVerticesInput: getChildrenByTagName(xmlVertices, "input")) {
							if (StringTools::equalsIgnoreCase(string(AVOID_NULLPTR_STRING(xmlVerticesInput->Attribute("semantic"))), "position") == true) {
								xmlVerticesSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlVerticesInput->Attribute("source"))), 1);
							} else
							if (StringTools::equalsIgnoreCase(string(AVOID_NULLPTR_STRING(xmlVerticesInput->Attribute("semantic"))), "normal") == true) {
								xmlNormalsSource = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlVerticesInput->Attribute("source"))), 1);
							}
						}
					}
				}
				// check for triangles vertices sources
				if (xmlVerticesSource.length() == 0) {
					throw ModelFileIOException(
						"Could not determine triangles vertices source for '" +
						(xmlNodeId) +
						"'"
					);
				}
				// check for triangles normals sources
				if (xmlNormalsSource.length() == 0) {
					throw ModelFileIOException(
						"Could not determine triangles normal source for '" +
						(xmlNodeId) +
						"'"
					);
				}
				// load vertices, normals, texture coordinates
				for (auto xmlMeshSource: getChildrenByTagName(xmlMesh, "source")) {
					// vertices
					if (string(AVOID_NULLPTR_STRING(xmlMeshSource->Attribute("id"))) == xmlVerticesSource) {
						auto xmlFloatArray = getChildrenByTagName(xmlMeshSource, "float_array").at(0);
						auto valueString = string(AVOID_NULLPTR_STRING(xmlFloatArray->GetText()));
						StringTokenizer t;
						t.tokenize(valueString, " \n\r");
						while (t.hasMoreTokens()) {
							float x = Float::parseFloat(t.nextToken());
							float y = Float::parseFloat(t.nextToken());
							float z = Float::parseFloat(t.nextToken());
							vertices.push_back(Vector3(x, y, z));
						}
					} else
					// normals
					if (string(AVOID_NULLPTR_STRING(xmlMeshSource->Attribute("id"))) == xmlNormalsSource) {
						auto xmlFloatArray = getChildrenByTagName(xmlMeshSource, "float_array").at(0);
						auto valueString = string(AVOID_NULLPTR_STRING(xmlFloatArray->GetText()));
						StringTokenizer t;
						t.tokenize(valueString, " \n\r");
						while (t.hasMoreTokens()) {
							float x = Float::parseFloat(t.nextToken());
							float y = Float::parseFloat(t.nextToken());
							float z = Float::parseFloat(t.nextToken());
							normals.push_back(Vector3(x, y, z));
						}
					} else
					// texture coordinates
					if (xmlTexCoordSource.length() > 0) {
						if (string(AVOID_NULLPTR_STRING(xmlMeshSource->Attribute("id"))) == xmlTexCoordSource) {
							auto xmlFloatArray = getChildrenByTagName(xmlMeshSource, "float_array").at(0);
							auto valueString = string(AVOID_NULLPTR_STRING(xmlFloatArray->GetText()));
							StringTokenizer t;
							t.tokenize(valueString, " \n\r");
							while (t.hasMoreTokens()) {
								float u = Float::parseFloat(t.nextToken());
								float v = Float::parseFloat(t.nextToken());
								textureCoordinates.push_back(TextureCoordinate(u, v));
							}
						}
					}
				}
				// load faces
				for (auto xmlPolygon: getChildrenByTagName(xmlPolygons, "p")) {
					auto valueString = string(AVOID_NULLPTR_STRING(xmlPolygon->GetText()));
					StringTokenizer t;
					t.tokenize(valueString, " \n\r");
					array<int32_t, 3> vi;
					auto viIdx = 0;
					array<int32_t, 3> ni;
					auto niIdx = 0;
					array<int32_t, 3> ti;
					auto tiIdx = 0;
					auto valueIdx = 0;
					auto valid = true;
					while (t.hasMoreTokens()) {
						auto value = Integer::parseInt(t.nextToken());
						if (valueIdx % xmlInputs == xmlVerticesOffset) {
							vi[viIdx++] = value;
							// validate
							if (value < 0 || value >= vertices.size() - verticesOffset) {
								valid = false;
							}
							// fix for some strange models
							if (xmlNormalsSource.length() > 0 && xmlNormalsOffset == -1) {
								ni[niIdx++] = value;
								if (value < 0 || value >= normals.size() - normalsOffset) {
									valid = false;
								}
							}
						}
						if (xmlNormalsOffset != -1 && valueIdx % xmlInputs == xmlNormalsOffset) {
							ni[niIdx++] = value;
							// validate
							if (value < 0 || value >= normals.size() - normalsOffset) {
								valid = false;
							}
						}
						if (xmlTexCoordOffset != -1 && valueIdx % xmlInputs == xmlTexCoordOffset) {
							ti[tiIdx++] = value;
							// validate
							if (value < 0 || value >= textureCoordinates.size() - textureCoordinatesOffset) {
								valid = false;
							}
						}
						if (viIdx == 3 && niIdx == 3 && (xmlTexCoordSource.length() == 0 || tiIdx == 3)) {
							// only add valid faces
							if (valid == true) {
								// add face
								Face f(
									node,
									vi[0] + verticesOffset,
									vi[1] + verticesOffset,
									vi[2] + verticesOffset,
									ni[0] + normalsOffset,
									ni[1] + normalsOffset,
									ni[2] + normalsOffset
								);
								if (xmlTexCoordSource.length() != 0) {
									f.setTextureCoordinateIndices(
										ti[0] + textureCoordinatesOffset,
										ti[1] + textureCoordinatesOffset,
										ti[2] + textureCoordinatesOffset
									);
								}
								faces.push_back(f);
							}
							viIdx = 0;
							niIdx = 0;
							tiIdx = 0;
							valid = true;
						}
						valueIdx++;
					}
				}
				// add faces entities if we have any
				if (faces.empty() == false) {
					facesEntity.setFaces(faces);
					facesEntities.push_back(facesEntity);
				}
			}
		}
	}

	// set up node
	node->setVertices(vertices);
	node->setNormals(normals);
	node->setTextureCoordinates(textureCoordinates);
	node->setFacesEntities(facesEntities);
}

Material* DAEReader::readMaterial(const string& pathName, Model* model, TiXmlElement* xmlRoot, const string& xmlNodeId)
{
	// determine effect id
	string xmlEffectId;
	auto xmlLibraryMaterials = getChildrenByTagName(xmlRoot, "library_materials").at(0);
	for (auto xmlMaterial: getChildrenByTagName(xmlLibraryMaterials, "material")) {
		if (string(AVOID_NULLPTR_STRING(xmlMaterial->Attribute("id"))) == xmlNodeId) {
			auto xmlInstanceEffect = getChildrenByTagName(xmlMaterial, "instance_effect").at(0);
			xmlEffectId = StringTools::substring(string(AVOID_NULLPTR_STRING(xmlInstanceEffect->Attribute("url"))), 1);
		}
	}
	if (xmlEffectId.length() == 0) {
		Console::println(
			string(
				 "Could not determine effect id for '" +
				 xmlNodeId +
				 "'"
			 )
		 );
		return nullptr;
	}
	// parse effect
	auto material = new Material(xmlNodeId);
	auto specularMaterialProperties = new SpecularMaterialProperties();
	string xmlDiffuseTextureId;
	string xmlTransparencyTextureId;
	string xmlSpecularTextureId;
	string xmlBumpTextureId;
	auto xmlLibraryEffects = getChildrenByTagName(xmlRoot, "library_effects").at(0);
	for (auto xmlEffect: getChildrenByTagName(xmlLibraryEffects, "effect")) {
		if (string(AVOID_NULLPTR_STRING(xmlEffect->Attribute("id"))) == xmlEffectId) {
			auto xmlProfile = getChildrenByTagName(xmlEffect, "profile_COMMON").at(0);
			// mappings
			map<string, string> samplerSurfaceMapping;
			map<string, string> surfaceImageMapping;
			for (auto xmlNewParam: getChildrenByTagName(xmlProfile, "newparam")) {
				auto xmlNewParamSID = string(AVOID_NULLPTR_STRING(xmlNewParam->Attribute("sid")));
				for (auto xmlSurface: getChildrenByTagName(xmlNewParam, "surface"))
				for (auto xmlSurfaceInitFrom: getChildrenByTagName(xmlSurface, "init_from")) {
					surfaceImageMapping[xmlNewParamSID] =
						string(AVOID_NULLPTR_STRING(xmlSurfaceInitFrom->GetText()));
				}
				for (auto xmlSampler2D: getChildrenByTagName(xmlNewParam, "sampler2D"))
				for (auto xmlSampler2DSource: getChildrenByTagName(xmlSampler2D, "source")) {
					samplerSurfaceMapping[xmlNewParamSID] =
						string(AVOID_NULLPTR_STRING(xmlSampler2DSource->GetText()));
				}
			}
			for (auto xmlTechnique: getChildrenByTagName(xmlProfile, "technique")) {
				for (auto xmlTechniqueNode: getChildren(xmlTechnique)) {
					for (auto xmlDiffuse: getChildrenByTagName(xmlTechniqueNode, "transparent")) {
						for (auto xmlTexture: getChildrenByTagName(xmlDiffuse, "texture")) {
							xmlTransparencyTextureId = string(AVOID_NULLPTR_STRING(xmlTexture->Attribute("texture")));

							auto sample2SurfaceIt = samplerSurfaceMapping.find(xmlTransparencyTextureId);
							string sample2Surface;
							if (sample2SurfaceIt != samplerSurfaceMapping.end()) {
								sample2Surface = sample2SurfaceIt->second;
							}
							if (sample2Surface.length() == 0) continue;

							string surface2Image;
							auto surface2ImageIt = surfaceImageMapping.find(sample2Surface);
							if (surface2ImageIt != surfaceImageMapping.end()) {
								surface2Image = surface2ImageIt->second;
							}
							if (surface2Image.length() > 0) {
								xmlTransparencyTextureId = surface2Image;
							}
						}
					}
					// diffuse
					for (auto xmlDiffuse: getChildrenByTagName(xmlTechniqueNode, "diffuse")) {
						// color
						for (auto xmlColor: getChildrenByTagName(xmlDiffuse, "color")) {
							StringTokenizer t;
							t.tokenize(string(AVOID_NULLPTR_STRING(xmlColor->GetText())), " ");
							array<float, 4> colorArray;
							for (auto i = 0; i < colorArray.size(); i++) {
								colorArray[i] = Float::parseFloat(t.nextToken());
							}
							specularMaterialProperties->setDiffuseColor(Color4(colorArray));
						}
						// texture
						for (auto xmlTexture: getChildrenByTagName(xmlDiffuse, "texture")) {
							xmlDiffuseTextureId = string(AVOID_NULLPTR_STRING(xmlTexture->Attribute("texture")));

							auto sample2SurfaceIt = samplerSurfaceMapping.find(xmlDiffuseTextureId);
							string sample2Surface;
							if (sample2SurfaceIt != samplerSurfaceMapping.end()) {
								sample2Surface = sample2SurfaceIt->second;
							}
							if (sample2Surface.length() == 0) continue;

							string surface2Image;
							auto surface2ImageIt = surfaceImageMapping.find(sample2Surface);
							if (surface2ImageIt != surfaceImageMapping.end()) {
								surface2Image = surface2ImageIt->second;
							}
							if (surface2Image.length() > 0) {
								xmlDiffuseTextureId = surface2Image;
							}
						}
					}
					// ambient
					for (auto xmlAmbient: getChildrenByTagName(xmlTechniqueNode, "ambient")) {
						// color
						for (auto xmlColor: getChildrenByTagName(xmlAmbient, "color")) {
							StringTokenizer t;
							t.tokenize(string(AVOID_NULLPTR_STRING(xmlColor->GetText())), " ");
							array<float, 4> colorArray;
							for (auto i = 0; i < colorArray.size(); i++) {
								colorArray[i] = Float::parseFloat(t.nextToken());
							}
							specularMaterialProperties->setAmbientColor(Color4(colorArray));
						}
					}
					// emission
					for (auto xmlEmission: getChildrenByTagName(xmlTechniqueNode, "emission")) {
						// color
						for (auto xmlColor: getChildrenByTagName(xmlEmission, "color")) {
							StringTokenizer t;
							t.tokenize(string(AVOID_NULLPTR_STRING(xmlColor->GetText())), " ");
							array<float, 4> colorArray;
							for (auto i = 0; i < colorArray.size(); i++) {
								colorArray[i] = Float::parseFloat(t.nextToken());
							}
							specularMaterialProperties->setEmissionColor(Color4(colorArray));
						}
					}
					// specular
					auto hasSpecularMap = false;
					auto hasSpecularColor = false;
					for (auto xmlSpecular: getChildrenByTagName(xmlTechniqueNode, "specular")) {
						// texture
						for (auto xmlTexture: getChildrenByTagName(xmlSpecular, "texture")) {
							xmlSpecularTextureId = string(AVOID_NULLPTR_STRING(xmlTexture->Attribute("texture")));

							auto sample2SurfaceIt = samplerSurfaceMapping.find(xmlSpecularTextureId);
							string sample2Surface;
							if (sample2SurfaceIt != samplerSurfaceMapping.end()) {
								sample2Surface = sample2SurfaceIt->second;
							}
							if (sample2Surface.length() == 0) continue;

							string surface2Image;
							auto surface2ImageIt = surfaceImageMapping.find(sample2Surface);
							if (surface2ImageIt != surfaceImageMapping.end()) {
								surface2Image = surface2ImageIt->second;
							}

							if (surface2Image.length() > 0) {
								xmlSpecularTextureId = surface2Image;
								hasSpecularMap = true;
							}
						}
						// color
						for (auto xmlColor: getChildrenByTagName(xmlSpecular, "color")) {
							StringTokenizer t;
							t.tokenize(string(AVOID_NULLPTR_STRING(xmlColor->GetText())), " ");
							array<float, 4> colorArray;
							for (auto i = 0; i < colorArray.size(); i++) {
								colorArray[i] = Float::parseFloat(t.nextToken());
							}
							specularMaterialProperties->setSpecularColor(Color4(colorArray));
							hasSpecularColor = true;
						}
					}
					if (hasSpecularMap == true && hasSpecularColor == false) {
						specularMaterialProperties->setSpecularColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
					}
					// shininess
					for (auto xmlShininess: getChildrenByTagName(xmlTechniqueNode, "shininess"))
					for (auto xmlFloat: getChildrenByTagName(xmlShininess, "float")) {
						specularMaterialProperties->setShininess(Float::parseFloat(string(AVOID_NULLPTR_STRING(xmlFloat->GetText()))));
					}
				}
				// normal/bump texture
				for (auto xmlBumpExtra: getChildrenByTagName(xmlTechnique, "extra"))
				for (auto xmlBumpTechnique: getChildrenByTagName(xmlBumpExtra, "technique"))
				for (auto xmlBumpTechniqueBump: getChildrenByTagName(xmlBumpTechnique, "bump"))
				for (auto xmlBumpTexture: getChildrenByTagName(xmlBumpTechniqueBump, "texture")) {
					xmlBumpTextureId = string(AVOID_NULLPTR_STRING(xmlBumpTexture->Attribute("texture")));

					auto sample2SurfaceIt = samplerSurfaceMapping.find(xmlBumpTextureId);
					string sample2Surface;
					if (sample2SurfaceIt != samplerSurfaceMapping.end()) {
						sample2Surface = sample2SurfaceIt->second;
					}
					if (sample2Surface.length() == 0) continue;

					string surface2Image;
					auto surface2ImageIt = surfaceImageMapping.find(sample2Surface);
					if (surface2ImageIt != surfaceImageMapping.end()) {
						surface2Image = surface2ImageIt->second;
					}

					if (surface2Image.length() > 0) xmlBumpTextureId = surface2Image;
				}
			}
		}
	}

	// diffuse transparency texture
	string xmlTransparencyTextureFilename;
	if (xmlDiffuseTextureId.length() > 0) {
		xmlTransparencyTextureFilename = getTextureFileNameById(xmlRoot, xmlTransparencyTextureId);
		// do we have a file name
		if (xmlTransparencyTextureFilename.length() > 0) {
			// add texture
			xmlTransparencyTextureFilename = makeFileNameRelative(xmlTransparencyTextureFilename);
		}
	}

	// diffuse texture
	string xmlDiffuseTextureFilename;
	if (xmlDiffuseTextureId.length() > 0) {
		xmlDiffuseTextureFilename = getTextureFileNameById(xmlRoot, xmlDiffuseTextureId);
		// do we have a file name
		if (xmlDiffuseTextureFilename.length() > 0) {
			xmlDiffuseTextureFilename = makeFileNameRelative(xmlDiffuseTextureFilename);
			// add texture
			specularMaterialProperties->setDiffuseTexture(pathName, xmlDiffuseTextureFilename, pathName, xmlTransparencyTextureFilename);
		}
	}

	// specular texture
	string xmlSpecularTextureFilename;
	if (xmlSpecularTextureId.length() > 0) {
		xmlSpecularTextureFilename = getTextureFileNameById(xmlRoot, xmlSpecularTextureId);
		// do we have a file name
		if (xmlSpecularTextureFilename.length() > 0) {
			xmlSpecularTextureFilename = makeFileNameRelative(xmlSpecularTextureFilename);
			// add texture
			specularMaterialProperties->setSpecularTexture(pathName, xmlSpecularTextureFilename);
		}
	}

	// normal/bump texture
	string xmlBumpTextureFilename;
	if (xmlBumpTextureId.length() > 0) {
		xmlBumpTextureFilename = getTextureFileNameById(xmlRoot, xmlBumpTextureId);
		// do we have a file name
		if (xmlBumpTextureFilename.length() > 0) {
			xmlBumpTextureFilename = makeFileNameRelative(xmlBumpTextureFilename);
			// add texture
			specularMaterialProperties->setNormalTexture(pathName, xmlBumpTextureFilename);
		}
	}

	/*
	// determine displacement map file name
	string xmlDisplacementFilename;
	// add texture
	if (xmlDisplacementFilename.length() > 0) {
		specularMaterialProperties->setDisplacementTexture(pathName, xmlDisplacementFilename);
	}
	*/

	// adjust ambient light with blender
	if (model->getAuthoringTool() == Model::AUTHORINGTOOL_BLENDER && specularMaterialProperties->getAmbientColor().equals(BLENDER_AMBIENT_NONE)) {
		specularMaterialProperties->setAmbientColor(
			Color4(
				specularMaterialProperties->getDiffuseColor().getRed() * BLENDER_AMBIENT_FROM_DIFFUSE_SCALE,
				specularMaterialProperties->getDiffuseColor().getGreen() * BLENDER_AMBIENT_FROM_DIFFUSE_SCALE,
				specularMaterialProperties->getDiffuseColor().getBlue() * BLENDER_AMBIENT_FROM_DIFFUSE_SCALE,
				1.0f
			)
		);
		specularMaterialProperties->setDiffuseColor(
			Color4(
				specularMaterialProperties->getDiffuseColor().getRed() * BLENDER_DIFFUSE_SCALE,
				specularMaterialProperties->getDiffuseColor().getGreen() * BLENDER_DIFFUSE_SCALE,
				specularMaterialProperties->getDiffuseColor().getBlue() * BLENDER_DIFFUSE_SCALE,
				specularMaterialProperties->getDiffuseColor().getAlpha()
			)
		);
	}

	// add specular material properties
	material->setSpecularMaterialProperties(specularMaterialProperties);

	// add material to library
	model->getMaterials()[material->getId()] = material;

	//
	return material;
}

const string DAEReader::makeFileNameRelative(const string& fileName)
{
	// check if absolute path
	if (StringTools::startsWith(fileName, "/") == true ||
		StringTools::regexMatch(fileName, "^[A-Z]\\:\\\\.*$") == true) {
		int indexSlash = fileName.find_last_of(L'/');
		int indexBackslash = fileName.find_last_of(L'\\');
		if (indexSlash != -1 || indexBackslash != -1) {
			if (indexSlash > indexBackslash) {
				return StringTools::substring(fileName, indexSlash + 1);
			} else {
				return StringTools::substring(fileName, indexBackslash + 1);
			}
		}
	}
	return fileName;
}

const string DAEReader::getTextureFileNameById(TiXmlElement* xmlRoot, const string& xmlTextureId)
{
	string xmlTextureFilename;
	auto xmlLibraryImages = getChildrenByTagName(xmlRoot, "library_images").at(0);
	for (auto xmlImage: getChildrenByTagName(xmlLibraryImages, "image")) {
		if (string(AVOID_NULLPTR_STRING(xmlImage->Attribute("id"))) == xmlTextureId) {
			xmlTextureFilename = string(AVOID_NULLPTR_STRING(getChildrenByTagName(xmlImage, "init_from").at(0)->GetText()));
			if (StringTools::startsWith(xmlTextureFilename, "file://") == true) {
				xmlTextureFilename = StringTools::substring(xmlTextureFilename, 7);
			}
			break;
		}
	}
	return xmlTextureFilename;
}

const vector<TiXmlElement*> DAEReader::getChildrenByTagName(TiXmlElement* parent, const char* name)
{
	vector<TiXmlElement*> elementList;
	for (auto *child = parent->FirstChildElement(name); child != nullptr; child = child->NextSiblingElement(name)) {
		elementList.push_back(child);
	}
	return elementList;
}

const vector<TiXmlElement*> DAEReader::getChildren(TiXmlElement* parent)
{
	vector<TiXmlElement*> elementList;
	for (auto *child = parent->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		elementList.push_back(child);
	}
	return elementList;
}
