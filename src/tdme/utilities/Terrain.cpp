#include <tdme/utilities/Terrain.h>

#include <string>

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
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ModelTools.h>

using std::string;
using std::to_string;

using tdme::utilities::Terrain;

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
using tdme::engine::model::UpVector;
using tdme::engine::primitives::BoundingBox;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ModelTools;

Model* Terrain::createTerrainModel(float width, float depth, float y, vector<Vector3>& terrainVerticesVector)
{
	auto modelId = "terrain" + to_string(static_cast<int>(width * 100)) + "x" + to_string(static_cast<int>(depth * 100)) + "@" + to_string(static_cast<int>(y * 100));
	auto terrainModel = new Model(modelId, modelId, UpVector::Y_UP, RotationOrder::ZYX, nullptr);
	auto terrainMaterial = new Material("terrain");
	terrainMaterial->setSpecularMaterialProperties(new SpecularMaterialProperties());
	// TODO: Fix me! The textures seem to be much too dark
	terrainMaterial->getSpecularMaterialProperties()->setAmbientColor(Color4(2.0f, 2.0f, 2.0f, 0.0f));
	terrainMaterial->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
	terrainMaterial->getSpecularMaterialProperties()->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 0.0f));
	terrainModel->getMaterials()[terrainMaterial->getId()] = terrainMaterial;
	auto terrainNode = new Node(terrainModel, nullptr, "terrain", "terrain");
	vector<Vector3> terrainVertices;
	vector<Vector3> terrainNormals;
	vector<Face> terrainFaces;
	#define STEP_SIZE	0.5f
	auto verticesPerZ = static_cast<int>(Math::ceil(width / STEP_SIZE));
	auto zMax = static_cast<int>(Math::ceil(depth / STEP_SIZE));
	for (float z = 0.0f; z < depth; z+= STEP_SIZE) {
		for (float x = 0.0f; x < width; x+= STEP_SIZE) {
			auto normalIdx = terrainNormals.size();
			auto vertexIdx = terrainVertices.size();

			terrainVerticesVector.push_back(Vector3(x, y, z));

			terrainVertices.push_back(Vector3(x, y, z - STEP_SIZE));
			terrainVertices.push_back(Vector3(x - STEP_SIZE, y, z - STEP_SIZE));
			terrainVertices.push_back(Vector3(x - STEP_SIZE, y, z));
			terrainVertices.push_back(Vector3(x, y, z));

			terrainNormals.push_back(Vector3(0.0f, 1.0f, 0.0f));
			terrainNormals.push_back(Vector3(0.0f, 1.0f, 0.0f));
			terrainNormals.push_back(Vector3(0.0f, 1.0f, 0.0f));
			terrainNormals.push_back(Vector3(0.0f, 1.0f, 0.0f));

			terrainFaces.push_back(
				Face(
					terrainNode,
					vertexIdx + 0,
					vertexIdx + 1,
					vertexIdx + 2,
					normalIdx + 0,
					normalIdx + 1,
					normalIdx + 2
				)
			);
			terrainFaces.push_back(
				Face(
					terrainNode,
					vertexIdx + 2,
					vertexIdx + 3,
					vertexIdx + 0,
					normalIdx + 2,
					normalIdx + 3,
					normalIdx + 0
				)
			);
		}
	}
	FacesEntity nodeFacesEntityTerrain(terrainNode, "terrain.facesentity");
	nodeFacesEntityTerrain.setMaterial(terrainMaterial);
	vector<FacesEntity> nodeFacesEntities;
	nodeFacesEntityTerrain.setFaces(terrainFaces);
	nodeFacesEntities.push_back(nodeFacesEntityTerrain);
	terrainNode->setVertices(terrainVertices);
	terrainNode->setNormals(terrainNormals);
	terrainNode->setFacesEntities(nodeFacesEntities);
	terrainModel->getNodes()[terrainNode->getId()] = terrainNode;
	terrainModel->getSubNodes()[terrainNode->getId()] = terrainNode;
	auto boundingBox = terrainModel->getBoundingBox();
	return terrainModel;
}

inline const Vector3 Terrain::computeTerrainVertexNormal(const vector<Vector3>& terrainVerticesVector, int vertexIdx, int verticesPerZ) {
	Vector3 vertexNormal;
	if (vertexIdx == -1) {
		Console::println("Terrain::computeTerrainVertexNormal(): no vertex normal available: invalid vertex idx");
		return vertexNormal.set(0.0f, 1.0f, 0.0f);
	}
	auto zMax = terrainVerticesVector.size() / verticesPerZ;
	auto topVertexIdx = getTerrainModelTopVertexIdx(vertexIdx, verticesPerZ, zMax);
	auto topLeftVertexIdx = getTerrainModelLeftVertexIdx(topVertexIdx, verticesPerZ, zMax);
	auto leftVertexIdx = getTerrainModelLeftVertexIdx(vertexIdx, verticesPerZ, zMax);
	auto bottomVertexIdx = getTerrainModelBottomVertexIdx(vertexIdx, verticesPerZ, zMax);
	auto rightVertexIdx = getTerrainModelRightVertexIdx(vertexIdx, verticesPerZ, zMax);
	auto bottomRightVertexIdx = getTerrainModelRightVertexIdx(bottomVertexIdx, verticesPerZ, zMax);
	Vector3 triangleNormal;
	int normalCount = 0;
	if (topVertexIdx != -1 && topLeftVertexIdx != -1) {
		ModelTools::computeNormal(
			{
				terrainVerticesVector[topVertexIdx],
				terrainVerticesVector[topLeftVertexIdx],
				terrainVerticesVector[vertexIdx]
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (topLeftVertexIdx != -1 && leftVertexIdx != -1) {
		ModelTools::computeNormal(
			{
				terrainVerticesVector[topLeftVertexIdx],
				terrainVerticesVector[leftVertexIdx],
				terrainVerticesVector[vertexIdx]
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (leftVertexIdx != -1 && bottomVertexIdx != -1) {
		ModelTools::computeNormal(
			{
				terrainVerticesVector[leftVertexIdx],
				terrainVerticesVector[bottomVertexIdx],
				terrainVerticesVector[vertexIdx]
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (bottomVertexIdx != -1 && bottomRightVertexIdx != -1) {
		ModelTools::computeNormal(
			{
				terrainVerticesVector[bottomVertexIdx],
				terrainVerticesVector[bottomRightVertexIdx],
				terrainVerticesVector[vertexIdx]
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (bottomRightVertexIdx != -1 && rightVertexIdx != -1) {
		ModelTools::computeNormal(
			{
				terrainVerticesVector[bottomRightVertexIdx],
				terrainVerticesVector[rightVertexIdx],
				terrainVerticesVector[vertexIdx]
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (rightVertexIdx != -1 && topVertexIdx != -1) {
		ModelTools::computeNormal(
			{
				terrainVerticesVector[rightVertexIdx],
				terrainVerticesVector[topVertexIdx],
				terrainVerticesVector[vertexIdx]
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (normalCount > 0) {
		return vertexNormal.normalize();
	}
	Console::println("Terrain::computeTerrainVertexNormal(): no vertex normal available: normal count == 0");
	return vertexNormal.set(0.0f, 1.0f, 0.0f);
}

void Terrain::updateTerrainModel(Model* terrainModel, vector<Vector3>& terrainVerticesVector, const Vector3& brushCenterPosition, const string& brushTextureFileName, float brushScale, float brushStrength) {
	// get terrain node
	auto terrainNode = terrainModel->getNodeById("terrain");
	if (terrainNode == nullptr) return;

	// load texture
	Texture* texture = nullptr;
	try {
		texture = TextureReader::read(Tools::getPathName(brushTextureFileName), Tools::getFileName(brushTextureFileName), false, false);
	} catch (Exception &exception) {
		Console::println(string("Terrain::updateTerrainModel(): An error occurred: ") + exception.what());
	}

	// apply brush
	if (texture != nullptr) {
		#define brushScale	0.5f
		auto terrainVertices = terrainNode->getVertices();
		auto terrainNormals = terrainNode->getNormals();
		auto verticesPerZ = static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getMax().getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE);
		auto normalsPerZ = static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getMax().getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE);
		auto zMax = terrainVerticesVector.size() / verticesPerZ;
		auto textureData = texture->getTextureData();
		auto textureWidth = texture->getTextureWidth();
		auto textureHeight = texture->getTextureHeight();
		auto textureBytePerPixel = texture->getDepth() == 32?4:3;
		for (auto z = 0.0f; z < textureHeight * brushScale; z+= STEP_SIZE) {
			auto brushPosition =
				brushCenterPosition.
				clone().
				sub(
					Vector3(
						(static_cast<float>(textureWidth) * brushScale) / 2.0f,
						0.0f,
						((static_cast<float>(textureHeight) * brushScale) / 2.0f)
					)
				).
				add(
					Vector3(
						0.0f,
						0.0f,
						z
					)
				);
			for (auto x = 0.0f; x < textureWidth * brushScale; x+= STEP_SIZE) {
				auto textureX = static_cast<int>(x / brushScale);
				auto textureY = static_cast<int>(z / brushScale);
				auto red = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 0);
				auto green = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 1);
				auto blue = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 2);
				auto alpha = textureBytePerPixel == 3?255:textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 3);
				auto appliedStrength = (static_cast<float>(red) + static_cast<float>(green) + static_cast<float>(blue)) / (255.0f * 3.0f) * brushStrength;
				auto terrainModelX = static_cast<int>(Math::ceil((brushPosition.getX() - terrainModel->getBoundingBox()->getMin().getX()) / STEP_SIZE));
				auto terrainModelZ = static_cast<int>(Math::ceil((brushPosition.getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE));
				auto terrainVertexHeight = terrainVerticesVector[terrainModelZ * verticesPerZ + terrainModelX].add(Vector3(0.0f, appliedStrength, 0.0f))[1];
				auto terrainVerticesIdx = (terrainModelZ * verticesPerZ * 4) + (terrainModelX * 4);
				terrainVertices[terrainVerticesIdx + 3][1] = terrainVertexHeight; // original
				terrainVertices[terrainVerticesIdx + (verticesPerZ * 4) + 0][1] = terrainVertexHeight; // top
				terrainVertices[terrainVerticesIdx + (verticesPerZ * 4) + (1 * 4) + 1][1] = terrainVertexHeight; // top left
				terrainVertices[terrainVerticesIdx + (1 * 4) + 2][1] = terrainVertexHeight; // left
				brushPosition.add(
					Vector3(
						STEP_SIZE,
						0.0f,
						0.0f
					)
				);
			}
		}
		for (auto z = -STEP_SIZE * 8.0f; z < textureHeight * brushScale + STEP_SIZE * 16.0f; z+= STEP_SIZE) {
			auto brushPosition =
				brushCenterPosition.
				clone().
				sub(
					Vector3(
						(static_cast<float>(textureWidth) * brushScale) / 2.0f,
						0.0f,
						(static_cast<float>(textureHeight) * brushScale) / 2.0f
					)
				).
				add(
					Vector3(
						-STEP_SIZE * 8.0f,
						0.0f,
						z
					)
				);
			for (auto x = -STEP_SIZE * 8.0f; x < textureWidth * brushScale + STEP_SIZE * 16.0f; x+= STEP_SIZE) {
				auto terrainModelX = static_cast<int>(Math::ceil((brushPosition.getX() - terrainModel->getBoundingBox()->getMin().getX()) / STEP_SIZE));
				auto terrainModelZ = static_cast<int>(Math::ceil((brushPosition.getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE));
				auto vertexIdx = terrainModelZ * verticesPerZ + terrainModelX;
				auto topVertexIdx = getTerrainModelTopVertexIdx(vertexIdx, verticesPerZ, zMax);
				auto topLeftVertexIdx = getTerrainModelLeftVertexIdx(topVertexIdx, verticesPerZ, zMax);
				auto leftVertexIdx = getTerrainModelLeftVertexIdx(vertexIdx, verticesPerZ, zMax);
				auto normalIdx = (terrainModelZ * normalsPerZ * 4) + (terrainModelX * 4);
				terrainNormals[normalIdx + 0] = computeTerrainVertexNormal(terrainVerticesVector, topVertexIdx, verticesPerZ);
				terrainNormals[normalIdx + 1] = computeTerrainVertexNormal(terrainVerticesVector, topLeftVertexIdx, verticesPerZ);
				terrainNormals[normalIdx + 2] = computeTerrainVertexNormal(terrainVerticesVector, leftVertexIdx, verticesPerZ);
				terrainNormals[normalIdx + 3] = computeTerrainVertexNormal(terrainVerticesVector, vertexIdx, verticesPerZ);
				brushPosition.add(
					Vector3(
						STEP_SIZE,
						0.0f,
						0.0f
					)
				);
			}
		}
		terrainNode->setVertices(terrainVertices);
		terrainNode->setNormals(terrainNormals);
		texture->releaseReference();
	}
}