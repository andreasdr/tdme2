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

inline const Vector3 Terrain::computeTerrainVertexNormal(const vector<Vector3>& terrainVerticesVector, int vertexIdx, int verticesPerX) {
	Vector3 vertexNormal;
	if (vertexIdx == -1) return vertexNormal.set(0.0f, 1.0f, 0.0f);
	auto verticesPerZ = terrainVerticesVector.size() / verticesPerX;
	auto topVertexIdx = getTerrainModelTopVertexIdx(vertexIdx, verticesPerX, verticesPerZ);
	auto topLeftVertexIdx = getTerrainModelLeftVertexIdx(topVertexIdx, verticesPerX, verticesPerZ);
	auto leftVertexIdx = getTerrainModelLeftVertexIdx(vertexIdx, verticesPerX, verticesPerZ);
	auto bottomVertexIdx = getTerrainModelBottomVertexIdx(vertexIdx, verticesPerX, verticesPerZ);
	auto rightVertexIdx = getTerrainModelRightVertexIdx(vertexIdx, verticesPerX, verticesPerZ);
	auto bottomRightVertexIdx = getTerrainModelRightVertexIdx(bottomVertexIdx, verticesPerX, verticesPerZ);
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

void Terrain::applyBrushToTerrainModel(
	Model* terrainModel,
	vector<Vector3>& terrainVerticesVector,
	const Vector3& brushCenterPosition,
	Texture* brushTexture,
	float brushScale,
	float brushStrength,
	BrushOperation brushOperation,
	float flattenHeight
) {
	// check if we have a texture
	if (brushTexture == nullptr) return;
	// check if we have a model
	if (terrainModel == nullptr) return;
	// get terrain node
	auto terrainNode = terrainModel->getNodeById("terrain");
	if (terrainNode == nullptr) return;

	// apply brush
	auto terrainVertices = terrainNode->getVertices();
	auto terrainNormals = terrainNode->getNormals();
	auto verticesPerZ = static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getMax().getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE);
	auto verticesPerX = static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getMax().getX() - terrainModel->getBoundingBox()->getMin().getX()) / STEP_SIZE);
	auto textureData = brushTexture->getTextureData();
	auto textureWidth = brushTexture->getTextureWidth();
	auto textureHeight = brushTexture->getTextureHeight();
	auto textureBytePerPixel = brushTexture->getDepth() == 32?4:3;
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
			if (terrainModelX < 0 || terrainModelX >= verticesPerX ||
				terrainModelZ < 0 || terrainModelZ >= verticesPerZ) continue;
			auto vertexIdx = terrainModelZ * verticesPerX + terrainModelX;
			auto terrainVertexHeight = terrainVerticesVector[vertexIdx][1];
			switch(brushOperation) {
				case BRUSHOPERATION_ADD:
					terrainVertexHeight+= appliedStrength;
					break;
				case BRUSHOPERATION_SUBTRACT:
					terrainVertexHeight-= appliedStrength;
					break;
				case BRUSHOPERATION_FLATTEN:
					terrainVertexHeight = terrainVertexHeight * (1.0f - Math::clamp(appliedStrength, 0.0f, 1.0f)) + flattenHeight * Math::clamp(appliedStrength, 0.0f, 1.0f);
					break;
				case BRUSHOPERATION_DELETE:
					terrainVertexHeight = terrainVertexHeight * (1.0f - Math::clamp(appliedStrength, 0.0f, 1.0f)) + 0.0f * Math::clamp(appliedStrength, 0.0f, 1.0f);
					break;
				case BRUSHOPERATION_SMOOTH:
					auto terrainVertexHeightNeighbours = 0.0f;
					auto terrainVertexHeightNeighbourCount = 0;
					auto topVertexIdx = getTerrainModelTopVertexIdx(vertexIdx, verticesPerX, verticesPerZ);
					auto leftVertexIdx = getTerrainModelLeftVertexIdx(vertexIdx, verticesPerX, verticesPerZ);
					auto bottomVertexIdx = getTerrainModelBottomVertexIdx(vertexIdx, verticesPerX, verticesPerZ);
					auto rightVertexIdx = getTerrainModelRightVertexIdx(vertexIdx, verticesPerX, verticesPerZ);
					if (topVertexIdx != -1) {
						terrainVertexHeightNeighbourCount++;
						terrainVertexHeightNeighbours+= terrainVerticesVector[topVertexIdx][1];
					}
					if (leftVertexIdx != -1) {
						terrainVertexHeightNeighbourCount++;
						terrainVertexHeightNeighbours+= terrainVerticesVector[leftVertexIdx][1];
					}
					if (bottomVertexIdx != -1) {
						terrainVertexHeightNeighbourCount++;
						terrainVertexHeightNeighbours+= terrainVerticesVector[bottomVertexIdx][1];
					}
					if (rightVertexIdx != -1) {
						terrainVertexHeightNeighbourCount++;
						terrainVertexHeightNeighbours+= terrainVerticesVector[rightVertexIdx][1];
					}
					if (terrainVertexHeightNeighbourCount > 0) {
						auto terrainVertexHeightSmoothed = terrainVertexHeightNeighbours / static_cast<float>(terrainVertexHeightNeighbourCount);
						terrainVertexHeight = terrainVertexHeight * (1.0f - Math::clamp(appliedStrength, 0.0f, 1.0f)) + terrainVertexHeightSmoothed * Math::clamp(appliedStrength, 0.0f, 1.0f);
					}
					break;
			}
			terrainVerticesVector[terrainModelZ * verticesPerX + terrainModelX][1] = terrainVertexHeight;
			auto terrainVerticesIdx = (terrainModelZ * verticesPerX * 4) + (terrainModelX * 4);
			terrainVertices[terrainVerticesIdx + 3][1] = terrainVertexHeight; // original
			if (terrainModelZ < verticesPerZ - 1) terrainVertices[terrainVerticesIdx + (verticesPerX * 4) + 0][1] = terrainVertexHeight; // top
			if (terrainModelZ < verticesPerZ - 1 && terrainModelX < verticesPerX - 1) terrainVertices[terrainVerticesIdx + (verticesPerX * 4) + (1 * 4) + 1][1] = terrainVertexHeight; // top left
			if (terrainModelX < verticesPerX - 1) terrainVertices[terrainVerticesIdx + (1 * 4) + 2][1] = terrainVertexHeight; // left
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
			if (terrainModelX < 0 || terrainModelX >= verticesPerX ||
				terrainModelZ < 0 || terrainModelZ >= verticesPerZ) continue;
			auto vertexIdx = terrainModelZ * verticesPerX + terrainModelX;
			auto topVertexIdx = getTerrainModelTopVertexIdx(vertexIdx, verticesPerX, verticesPerZ);
			auto topLeftVertexIdx = getTerrainModelLeftVertexIdx(topVertexIdx, verticesPerX, verticesPerZ);
			auto leftVertexIdx = getTerrainModelLeftVertexIdx(vertexIdx, verticesPerX, verticesPerZ);
			auto normalIdx = (terrainModelZ * verticesPerX * 4) + (terrainModelX * 4);
			terrainNormals[normalIdx + 0] = computeTerrainVertexNormal(terrainVerticesVector, topVertexIdx, verticesPerX);
			terrainNormals[normalIdx + 1] = computeTerrainVertexNormal(terrainVerticesVector, topLeftVertexIdx, verticesPerX);
			terrainNormals[normalIdx + 2] = computeTerrainVertexNormal(terrainVerticesVector, leftVertexIdx, verticesPerX);
			terrainNormals[normalIdx + 3] = computeTerrainVertexNormal(terrainVerticesVector, vertexIdx, verticesPerX);
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
}

bool Terrain::getTerrainModelFlattenHeight(
	Model* terrainModel,
	vector<Vector3>& terrainVerticesVector,
	const Vector3& brushCenterPosition,
	float& flattenHeight
) {
	// check if we have a model
	if (terrainModel == nullptr) return false;
	// get terrain node
	auto terrainNode = terrainModel->getNodeById("terrain");
	if (terrainNode == nullptr) return false;

	// apply brush
	auto verticesPerZ = static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getMax().getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE);
	auto verticesPerX = static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getMax().getX() - terrainModel->getBoundingBox()->getMin().getX()) / STEP_SIZE);
	auto terrainModelX = static_cast<int>(Math::ceil((brushCenterPosition.getX() - terrainModel->getBoundingBox()->getMin().getX()) / STEP_SIZE));
	auto terrainModelZ = static_cast<int>(Math::ceil((brushCenterPosition.getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE));
	if (terrainModelX < 0 || terrainModelX >= verticesPerX ||
		terrainModelZ < 0 || terrainModelZ >= verticesPerZ) return false;
	flattenHeight = terrainVerticesVector[terrainModelZ * verticesPerZ + terrainModelX][1];
	return true;
}
