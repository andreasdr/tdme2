#include <tdme/engine/fileio/models/TMReader.h>

#include <array>
#include <map>
#include <string>
#include <vector>

#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/Joint.h>
#include <tdme/engine/model/JointWeight.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/engine/model/PBRMaterialProperties.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/ShaderModel.h>
#include <tdme/engine/model/Skinning.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/math/Matrix2D3x3.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>

using std::array;
using std::map;
using std::vector;
using std::string;
using std::to_string;
using std::to_string;

using tdme::engine::fileio::models::TMReader;
using tdme::engine::fileio::models::TMReaderInputStream;
using tdme::engine::fileio::models::ModelFileIOException;
using tdme::engine::model::Animation;
using tdme::engine::model::Color4;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Node;
using tdme::engine::model::Joint;
using tdme::engine::model::JointWeight;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::utilities::ModelTools;
using tdme::engine::model::PBRMaterialProperties;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::ShaderModel;
using tdme::engine::model::Skinning;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::model::UpVector;
using tdme::engine::primitives::BoundingBox;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

Model* TMReader::read(const string& pathName, const string& fileName)
{
	vector<uint8_t> content;
	FileSystem::getInstance()->getContent(pathName, fileName, content);
	TMReaderInputStream is(&content);
	auto fileId = is.readString();
	if (fileId.length() == 0 || fileId != "TDME Model") {
		throw ModelFileIOException(
			"File is not a TDME model file, file id = '" +
			(fileId) +
			"'"
		);
	}
	array<uint8_t, 3> version;
	version[0] = is.readByte();
	version[1] = is.readByte();
	version[2] = is.readByte();
	if ((version[0] != 1 || version[1] != 0 || version[2] != 0) &&
		(version[0] != 1 || version[1] != 9 || version[2] != 9) &&
		(version[0] != 1 || version[1] != 9 || version[2] != 10) &&
		(version[0] != 1 || version[1] != 9 || version[2] != 11) &&
		(version[0] != 1 || version[1] != 9 || version[2] != 12) &&
		(version[0] != 1 || version[1] != 9 || version[2] != 13) &&
		(version[0] != 1 || version[1] != 9 || version[2] != 14) &&
		(version[0] != 1 || version[1] != 9 || version[2] != 15)) {
		throw ModelFileIOException(
			"Version mismatch, should be 1.0.0, 1.9.9, 1.9.10, 1.9.11, 1.9.12, 1.9.13, 1.9.14, 1.9.15 but is " +
			to_string(version[0]) +
			"." +
			to_string(version[1]) +
			"." +
			to_string(version[2])
		);
	}
	auto name = is.readString();
	auto upVector = UpVector::valueOf(is.readString());
	auto rotationOrder = RotationOrder::valueOf(is.readString());
	auto shaderModel = ShaderModel::SPECULAR;
	if ((version[0] == 1 && version[1] == 9 && version[2] == 14) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 15)) {
		shaderModel = ShaderModel::valueOf(is.readString());
	}
	array<float, 3> boundingBoxMinXYZ;
	is.readFloatArray(boundingBoxMinXYZ);
	array<float, 3> boundingBoxMaxXYZ;
	is.readFloatArray(boundingBoxMaxXYZ);
	auto boundingBox = new BoundingBox(Vector3(boundingBoxMinXYZ), Vector3(boundingBoxMaxXYZ));
	auto model = new Model(
		pathName + "/" + fileName,
		fileName,
		upVector,
		rotationOrder,
		boundingBox
	);
	model->setShaderModel(shaderModel);
	model->setFPS(is.readFloat());
	array<float, 16> importTransformationsMatrixArray;
	is.readFloatArray(importTransformationsMatrixArray);
	model->setImportTransformationsMatrix(importTransformationsMatrixArray);
	auto materialCount = is.readInt();
	for (auto i = 0; i < materialCount; i++) {
		auto material = readMaterial(pathName, &is, version);
		model->getMaterials()[material->getId()] = material;
	}
	readSubNodes(&is, model, nullptr, model->getSubNodes());
	auto animationSetupCount = is.readInt();
	for (auto i = 0; i < animationSetupCount; i++) {
		readAnimationSetup(&is, model, version);
	}
	if (model->getAnimationSetup(Model::ANIMATIONSETUP_DEFAULT) == nullptr) {
		model->addAnimationSetup(Model::ANIMATIONSETUP_DEFAULT, 0, 0, true);
	}
	return model;
}

const string TMReader::getTexturePath(const string& modelPathName, const string& texturePathName, const string& textureFileName) {
	if (FileSystem::getInstance()->fileExists(texturePathName + "/" + textureFileName) == true) {
		return texturePathName;
	} else
	if (FileSystem::getInstance()->fileExists(modelPathName + "/" + textureFileName) == true) {
		return modelPathName;
	} else
	if (FileSystem::getInstance()->fileExists(FileSystem::getInstance()->getPathName(modelPathName) + "/" + textureFileName) == true) {
		return FileSystem::getInstance()->getPathName(modelPathName);
	} else {
		return texturePathName;
	}
}

Material* TMReader::readMaterial(const string& pathName, TMReaderInputStream* is, const array<uint8_t, 3>& version)
{
	auto id = is->readString();
	auto m = new Material(id);
	auto smp = new SpecularMaterialProperties();
	array<float, 4> colorRGBAArray;
	is->readFloatArray(colorRGBAArray);
	smp->setAmbientColor(Color4(colorRGBAArray));
	is->readFloatArray(colorRGBAArray);
	smp->setDiffuseColor(Color4(colorRGBAArray));
	is->readFloatArray(colorRGBAArray);
	smp->setSpecularColor(Color4(colorRGBAArray));
	is->readFloatArray(colorRGBAArray);
	smp->setEmissionColor(Color4(colorRGBAArray));
	smp->setShininess(is->readFloat());
	if (version[0] == 1 && version[1] == 9 && version[2] == 15) {
		smp->setTextureAtlasSize(is->readInt());
	}
	auto diffuseTexturePathName = is->readString();
	auto diffuseTextureFileName = is->readString();
	auto diffuseTransparencyTexturePathName = is->readString();
	auto diffuseTransparencyTextureFileName = is->readString();
	if (diffuseTextureFileName.size() != 0) {
		smp->setDiffuseTexture(
			getTexturePath(pathName, diffuseTexturePathName, diffuseTextureFileName),
			diffuseTextureFileName,
			getTexturePath(pathName, diffuseTransparencyTexturePathName, diffuseTransparencyTextureFileName),
			diffuseTransparencyTextureFileName
		);
	}
	auto specularTexturePathName = is->readString();
	auto specularTextureFileName = is->readString();
	if (specularTextureFileName.size() != 0) {
		smp->setSpecularTexture(
			getTexturePath(pathName, specularTexturePathName, specularTextureFileName),
			specularTextureFileName
		);
	}
	auto normalTexturePathName = is->readString();
	auto normalTextureFileName = is->readString();
	if (normalTextureFileName.size() != 0) {
		smp->setNormalTexture(
			getTexturePath(pathName, normalTexturePathName, normalTextureFileName),
			normalTextureFileName
		);
	}
	if ((version[0] == 1 && version[1] == 9 && version[2] == 9) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 10) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 11)) {
		auto displacementTexturePathName = is->readString();
		auto displacementTextureFileName = is->readString();
	}
	if ((version[0] == 1 && version[1] == 9 && version[2] == 15)) {
		smp->setDiffuseTextureTransparency(is->readBoolean());
	}
	smp->setDiffuseTextureMaskedTransparency(is->readBoolean());
	if ((version[0] == 1 && version[1] == 9 && version[2] == 9) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 10) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 11) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 12) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 13) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 14) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 15))  {
		smp->setDiffuseTextureMaskedTransparencyThreshold(is->readFloat());
	}
	if ((version[0] == 1 && version[1] == 9 && version[2] == 10) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 11) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 12) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 13) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 14) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 15)) {
		array<float, 9> textureMatrix;
		is->readFloatArray(textureMatrix);
		smp->setTextureMatrix(Matrix2D3x3(textureMatrix));
	}
	m->setSpecularMaterialProperties(smp);
	if ((version[0] == 1 && version[1] == 9 && version[2] == 13) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 14) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 15)) {
		if (is->readBoolean() == true) {
			auto pmp = new PBRMaterialProperties();
			is->readFloatArray(colorRGBAArray);
			pmp->setBaseColorFactor(Color4(colorRGBAArray));
			auto baseColorTexturePathName = is->readString();
			auto baseColorTextureFileName = is->readString();
			if (baseColorTextureFileName.size() != 0) {
				pmp->setBaseColorTexture(baseColorTexturePathName, baseColorTextureFileName);
			}
			pmp->setBaseColorTextureMaskedTransparency(is->readBoolean());
			pmp->setBaseColorTextureMaskedTransparency(is->readFloat());
			pmp->setMetallicFactor(is->readFloat());
			pmp->setRoughnessFactor(is->readFloat());
			auto metallicRoughnessTexturePathName = is->readString();
			auto metallicRoughnessTextureFileName = is->readString();
			if (metallicRoughnessTextureFileName.size() != 0) {
				pmp->setMetallicRoughnessTexture(metallicRoughnessTexturePathName, metallicRoughnessTextureFileName);
			}
			pmp->setNormalScale(is->readFloat());
			auto normalTexturePathName = is->readString();
			auto normalTextureFileName = is->readString();
			if (normalTextureFileName.size() != 0) {
				pmp->setNormalTexture(normalTexturePathName, normalTextureFileName);
			}
			pmp->setExposure(is->readFloat());
			m->setPBRMaterialProperties(pmp);
		}
	}
	return m;
}

void TMReader::readAnimationSetup(TMReaderInputStream* is, Model* model, const array<uint8_t, 3>& version) {
	auto id = is->readString();
	auto overlayFromNodeId = is->readString();
	auto startFrame = is->readInt();
	auto endFrame = is->readInt();
	auto loop = is->readBoolean();
	auto speed = 1.0f;
	if ((version[0] == 1 && version[1] == 9 && version[2] == 11) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 12) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 13) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 14) ||
		(version[0] == 1 && version[1] == 9 && version[2] == 15)) {
		speed = is->readFloat();
	}
	if (overlayFromNodeId.length() == 0) {
		model->addAnimationSetup(id, startFrame, endFrame, loop, speed);
	} else {
		model->addOverlayAnimationSetup(id, overlayFromNodeId, startFrame, endFrame, loop, speed);
	}
}

const vector<Vector3> TMReader::readVertices(TMReaderInputStream* is)
{
	vector<Vector3> v;
	array<float, 3> vXYZ;
	if (is->readBoolean() == true) {
		v.resize(is->readInt());
		for (auto i = 0; i < v.size(); i++) {
			is->readFloatArray(vXYZ);
			v[i].set(vXYZ);
		}
	}
	return v;
}

const vector<TextureCoordinate> TMReader::readTextureCoordinates(TMReaderInputStream* is)
{
	array<float, 2> tcUV;
	vector<TextureCoordinate> tc;
	if (is->readBoolean() == true) {
		tc.resize(is->readInt());
		for (auto i = 0; i < tc.size(); i++) {
			is->readFloatArray(tcUV);
			tc[i] = TextureCoordinate(tcUV);
		}
	}
	return tc;
}

bool TMReader::readIndices(TMReaderInputStream* is, array<int32_t, 3>* indices)
{
	if (is->readBoolean() == false) {
		return false;
	} else {
		auto length = is->readInt();
		if (length != indices->size()) {
			throw ModelFileIOException("Wrong indices array size");
		}
		for (auto i = 0; i < indices->size(); i++) {
			(*indices)[i] = is->readInt();
		}
		return true;
	}
}

Animation* TMReader::readAnimation(TMReaderInputStream* is, Node* g)
{
	if (is->readBoolean() == false) {
		return nullptr;
	} else {
		array<float, 16> matrixArray;
		auto frames = is->readInt();
		auto animation = new Animation();
		vector<Matrix4x4> transformationsMatrices;
		transformationsMatrices.resize(frames);
		for (auto i = 0; i < transformationsMatrices.size(); i++) {
			is->readFloatArray(matrixArray);
			transformationsMatrices[i].set(matrixArray);
		}
		animation->setTransformationsMatrices(transformationsMatrices);
		g->setAnimation(animation);
		return g->getAnimation();
	}
}

void TMReader::readFacesEntities(TMReaderInputStream* is, Node* g)
{
	vector<FacesEntity> facesEntities;
	facesEntities.resize(is->readInt());
	for (auto i = 0; i < facesEntities.size(); i++) {
		facesEntities[i] = FacesEntity(g, is->readString());
		if (is->readBoolean() == true) {
			Material* material = nullptr;
			auto materialIt = g->getModel()->getMaterials().find(is->readString());
			if (materialIt != g->getModel()->getMaterials().end()) {
				material = materialIt->second;
			}
			facesEntities[i].setMaterial(material);
		}
		vector<Face> faces;
		faces.resize(is->readInt());
		array<int32_t,3> vertexIndices;
		array<int32_t,3> normalIndices;
		array<int32_t,3> textureCoordinateIndices;
		array<int32_t,3> tangentIndices;
		array<int32_t,3> bitangentIndices;
		bool haveTextureCoordinateIndices;
		bool haveTangentIndices;
		bool haveBitangentIndices;
		for (auto j = 0; j < faces.size(); j++) {
			readIndices(is, &vertexIndices);
			readIndices(is, &normalIndices);
			haveTextureCoordinateIndices = readIndices(is, &textureCoordinateIndices);
			haveTangentIndices = readIndices(is, &tangentIndices);
			haveBitangentIndices = readIndices(is, &bitangentIndices);
			faces[j] = Face(g,
				vertexIndices[0], vertexIndices[1], vertexIndices[2],
				normalIndices[0], normalIndices[1], normalIndices[2]
			);
			if (haveTextureCoordinateIndices == true) {
				faces[j].setTextureCoordinateIndices(
					textureCoordinateIndices[0], textureCoordinateIndices[1], textureCoordinateIndices[2]
				);
			}
			if (haveTangentIndices == true && haveBitangentIndices == true) {
				faces[j].setTangentIndices(tangentIndices[0], tangentIndices[1], tangentIndices[2]);
				faces[j].setBitangentIndices(bitangentIndices[0], bitangentIndices[1], bitangentIndices[2]);
			}
		}
		facesEntities[i].setFaces(faces);
	}
	g->setFacesEntities(facesEntities);
}

Joint TMReader::readSkinningJoint(TMReaderInputStream* is)
{
	array<float, 16> matrixArray;
	Joint joint(is->readString());
	is->readFloatArray(matrixArray);
	joint.setBindMatrix(Matrix4x4(matrixArray));
	return joint;
}

JointWeight TMReader::readSkinningJointWeight(TMReaderInputStream* is)
{

	int32_t jointIndex = is->readInt();
	int32_t weightIndex = is->readInt();
	return JointWeight(jointIndex, weightIndex);
}

void TMReader::readSkinning(TMReaderInputStream* is, Node* g)
{
	if (is->readBoolean() == true) {
		auto skinning = new Skinning();
		skinning->setWeights(is->readFloatVector());
		vector<Joint> joints;
		joints.resize(is->readInt());
		for (auto i = 0; i < joints.size(); i++) {
			joints[i] = readSkinningJoint(is);
		}
		skinning->setJoints(joints);
		vector<vector<JointWeight>> verticesJointsWeight;
		verticesJointsWeight.resize(is->readInt());
		for (auto i = 0; i < verticesJointsWeight.size(); i++) {
			verticesJointsWeight[i].resize(is->readInt());
			for (auto j = 0; j < verticesJointsWeight[i].size(); j++) {
				verticesJointsWeight[i][j] = readSkinningJointWeight(is);
			}
		}
		skinning->setVerticesJointsWeights(verticesJointsWeight);
		g->setSkinning(skinning);
	}
}

void TMReader::readSubNodes(TMReaderInputStream* is, Model* model, Node* parentNode, map<string, Node*>& subNodes)
{
	auto subNodeCount = is->readInt();
	for (auto i = 0; i < subNodeCount; i++) {
		auto subNode = readNode(is, model, parentNode);
		subNodes[subNode->getId()] = subNode;
		model->getNodes()[subNode->getId()] = subNode;
	}
}

Node* TMReader::readNode(TMReaderInputStream* is, Model* model, Node* parentNode)
{

	auto nodeId = is->readString();
	auto nodeName = is->readString();
	auto node = new Node(model, parentNode, nodeId, nodeName);
	node->setJoint(is->readBoolean());
	array<float, 16> matrixArray;
	is->readFloatArray(matrixArray);
	node->setTransformationsMatrix(Matrix4x4(matrixArray));
	vector<Vector3> vertices = readVertices(is);
	node->setVertices(vertices);
	vector<Vector3> normals = readVertices(is);
	node->setNormals(normals);
	vector<TextureCoordinate> textureCoordinates = readTextureCoordinates(is);
	node->setTextureCoordinates(textureCoordinates);
	vector<Vector3> tangents = readVertices(is);
	node->setTangents(tangents);
	vector<Vector3> bitangents = readVertices(is);
	node->setBitangents(bitangents);
	readAnimation(is, node);
	readSkinning(is, node);
	readFacesEntities(is, node);
	readSubNodes(is, model, parentNode, node->getSubNodes());
	return node;
}
