#include <tdme/utilities/Terrain.h>

#include <array>
#include <string>
#include <vector>

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

using std::array;
using std::string;
using std::to_string;
using std::vector;

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

void Terrain::createTerrainModels(float width, float depth, float y, vector<float>& terrainHeightVector, BoundingBox& terrainBoundingBox, vector<Model*>& terrainModels)
{
	vector<vector<Vector3>> partitionTerrainVertices;
	vector<vector<Vector3>> partitionTerrainNormals;
	vector<vector<array<int, 6>>> partitionTerrainFaces;
	auto partitionsX = static_cast<int>(Math::ceil(width / PARTITION_SIZE));
	auto partitionsZ = static_cast<int>(Math::ceil(depth / PARTITION_SIZE));
	auto partitionCount = partitionsX * partitionsZ;
	partitionTerrainVertices.resize(partitionCount);
	partitionTerrainNormals.resize(partitionCount);
	partitionTerrainFaces.resize(partitionCount);
	if (terrainHeightVector.empty() == true) {
		for (float z = 0.0f; z < depth; z+= STEP_SIZE) {
			for (float x = 0.0f; x < width; x+= STEP_SIZE) {
				terrainHeightVector.push_back(y);
			}
		}
	}
	auto terrainHeightVectorVerticesPerX = static_cast<int>(Math::ceil(width / STEP_SIZE));
	auto terreinHeightVectorVerticesPerZ = static_cast<int>(Math::ceil(depth / STEP_SIZE));
	for (float z = 0.0f; z < depth; z+= STEP_SIZE) {
		for (float x = 0.0f; x < width; x+= STEP_SIZE) {

			auto partitionX = static_cast<int>(x / PARTITION_SIZE);
			auto partitionZ = static_cast<int>(z / PARTITION_SIZE);
			auto partitionIdx = partitionZ * partitionsX + partitionX;

			auto& terrainVertices = partitionTerrainVertices[partitionIdx];
			auto& terrainNormals = partitionTerrainNormals[partitionIdx];
			auto& terrainFaces = partitionTerrainFaces[partitionIdx];

			int normalIdx = terrainNormals.size();
			int vertexIdx = terrainVertices.size();

			auto terrainHeightVectorX = static_cast<int>(x / STEP_SIZE) + 1;
			auto terrainHeightVectorZ = static_cast<int>(z / STEP_SIZE) + 1;

			Vector3 topVertex;
			Vector3 topLeftVertex;
			Vector3 leftVertex;
			Vector3 vertex;

			getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX, terrainHeightVectorZ - 1, topVertex);
			getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX - 1, terrainHeightVectorZ - 1, topLeftVertex);
			getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX - 1, terrainHeightVectorZ, leftVertex);
			getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX, terrainHeightVectorZ, vertex);

			terrainVertices.push_back(topVertex);
			terrainVertices.push_back(topLeftVertex);
			terrainVertices.push_back(leftVertex);
			terrainVertices.push_back(vertex);

			auto normal = computeTerrainVertexNormal(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX, terrainHeightVectorZ);
			auto topNormal = computeTerrainVertexNormal(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX, terrainHeightVectorZ - 1);
			auto topLeftNormal = computeTerrainVertexNormal(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX - 1, terrainHeightVectorZ - 1);
			auto leftNormal = computeTerrainVertexNormal(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX - 1, terrainHeightVectorZ);

			terrainNormals.push_back(topNormal);
			terrainNormals.push_back(topLeftNormal);
			terrainNormals.push_back(leftNormal);
			terrainNormals.push_back(normal);

			terrainFaces.push_back(
				{
					vertexIdx + 0,
					vertexIdx + 1,
					vertexIdx + 2,
					normalIdx + 0,
					normalIdx + 1,
					normalIdx + 2
				}
			);
			terrainFaces.push_back(
				{
					vertexIdx + 2,
					vertexIdx + 3,
					vertexIdx + 0,
					normalIdx + 2,
					normalIdx + 3,
					normalIdx + 0
				}
			);
		}
	}
	auto partitionIdx = 0;
	for (auto& terrainFaces: partitionTerrainFaces) {
		auto modelId = "terrain" + to_string(partitionIdx);
		auto terrainModel = new Model(modelId, modelId, UpVector::Y_UP, RotationOrder::ZYX, nullptr);
		auto terrainMaterial = new Material("terrain");
		terrainMaterial->setSpecularMaterialProperties(new SpecularMaterialProperties());
		// TODO: Fix me! The textures seem to be much too dark
		terrainMaterial->getSpecularMaterialProperties()->setAmbientColor(Color4(2.0f, 2.0f, 2.0f, 0.0f));
		terrainMaterial->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
		terrainMaterial->getSpecularMaterialProperties()->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 0.0f));
		terrainModel->getMaterials()[terrainMaterial->getId()] = terrainMaterial;
		auto terrainNode = new Node(terrainModel, nullptr, "terrain", "terrain");
		FacesEntity nodeFacesEntityTerrain(terrainNode, "terrain.facesentity");
		nodeFacesEntityTerrain.setMaterial(terrainMaterial);
		vector<FacesEntity> nodeFacesEntities;
		vector<Face> nodeFaces;
		for (auto faceIndices: partitionTerrainFaces[partitionIdx]) {
			nodeFaces.push_back(
				Face(
					terrainNode,
					faceIndices[0],
					faceIndices[1],
					faceIndices[2],
					faceIndices[3],
					faceIndices[4],
					faceIndices[5]
				)
			);
		};
		nodeFacesEntityTerrain.setFaces(nodeFaces);
		nodeFacesEntities.push_back(nodeFacesEntityTerrain);
		terrainNode->setVertices(partitionTerrainVertices[partitionIdx]);
		terrainNode->setNormals(partitionTerrainNormals[partitionIdx]);
		terrainNode->setFacesEntities(nodeFacesEntities);
		terrainModel->getNodes()[terrainNode->getId()] = terrainNode;
		terrainModel->getSubNodes()[terrainNode->getId()] = terrainNode;
		if (partitionIdx == 0) {
			terrainBoundingBox = *terrainModel->getBoundingBox();
		} else {
			terrainBoundingBox.extend(terrainModel->getBoundingBox());
		}
		terrainModels.push_back(terrainModel);
		partitionIdx++;
	}
}

inline const Vector3 Terrain::computeTerrainVertexNormal(const vector<float>& terrainHeightVector, int verticesPerX, int verticesPerZ, int x, int z) {
	Vector3 vertexNormal;

	Vector3 vertex;
	auto haveVertex = getTerrainVertex(terrainHeightVector, verticesPerX, verticesPerZ, x, z - 1, vertex);
	if (haveVertex == false) return Vector3(0.0f, 1.0f, 0.0f);

	Vector3 topVertex;
	Vector3 topLeftVertex;
	Vector3 leftVertex;
	Vector3 bottomVertex;
	Vector3 rightVertex;
	Vector3 bottomRightVertex;

	auto haveTopVertex = getTerrainVertex(terrainHeightVector, verticesPerX, verticesPerZ, x, z - 1, topVertex);
	auto haveTopLeftVertex = getTerrainVertex(terrainHeightVector, verticesPerX, verticesPerZ, x - 1, z - 1, topLeftVertex);
	auto haveLeftVertex = getTerrainVertex(terrainHeightVector, verticesPerX, verticesPerZ, x - 1, z, leftVertex);
	auto haveBottomVertex = getTerrainVertex(terrainHeightVector, verticesPerX, verticesPerZ, x, z + 1, bottomVertex);
	auto haveRightVertex = getTerrainVertex(terrainHeightVector, verticesPerX, verticesPerZ, x + 1, z, rightVertex);
	auto haveBottomRightVertex = getTerrainVertex(terrainHeightVector, verticesPerX, verticesPerZ, x + 1, z + 1, bottomRightVertex);

	Vector3 triangleNormal;
	int normalCount = 0;
	if (haveTopVertex == true && haveTopLeftVertex == true) {
		ModelTools::computeNormal(
			{
				topVertex,
				topLeftVertex,
				vertex
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (haveTopLeftVertex == true && haveLeftVertex == true) {
		ModelTools::computeNormal(
			{
				topLeftVertex,
				leftVertex,
				vertex
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (haveLeftVertex == true && haveBottomVertex == true) {
		ModelTools::computeNormal(
			{
				leftVertex,
				bottomVertex,
				vertex
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (haveBottomVertex == true && haveBottomRightVertex == true) {
		ModelTools::computeNormal(
			{
				bottomVertex,
				bottomRightVertex,
				vertex
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (haveBottomRightVertex == true && haveRightVertex == true) {
		ModelTools::computeNormal(
			{
				bottomRightVertex,
				rightVertex,
				vertex
			},
			triangleNormal
		);
		vertexNormal.add(triangleNormal);
		normalCount++;
	}
	if (haveRightVertex == true && haveTopVertex == true) {
		ModelTools::computeNormal(
			{
				rightVertex,
				topVertex,
				vertex
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
	BoundingBox& terrainBoundingBox,
	vector<Model*> terrainModels,
	vector<float>& terrainHeightVector,
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
	if (terrainModels.empty() == true) return;

	// apply brush
	vector<vector<Vector3>> partitionTerrainVertices;
	vector<vector<Vector3>> partitionTerrainNormals;
	partitionTerrainVertices.resize(terrainModels.size());
	partitionTerrainNormals.resize(terrainModels.size());
	auto partitionsX = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX() / PARTITION_SIZE));
	auto terrainHeightVectorVerticesPerX = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX() / STEP_SIZE));
	auto terreinHeightVectorVerticesPerZ = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getZ() / STEP_SIZE));
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
			auto terrainHeightVectorX = static_cast<int>((brushPosition.getX() - terrainBoundingBox.getMin().getX()) / STEP_SIZE);
			auto terrainHeightVectorZ = static_cast<int>((brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / STEP_SIZE);
			if (terrainHeightVectorX < 0 || terrainHeightVectorX >= terrainHeightVectorVerticesPerX ||
				terrainHeightVectorZ < 0 || terrainHeightVectorZ >= terreinHeightVectorVerticesPerZ) continue;
			auto vertexIdx = terrainHeightVectorZ * terrainHeightVectorVerticesPerX + terrainHeightVectorX;
			auto terrainVertexHeight = terrainHeightVector[vertexIdx];
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
					Vector3 topVertex;
					Vector3 leftVertex;
					Vector3 bottomVertex;
					Vector3 rightVertex;
					auto haveTopVertex = getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX, terrainHeightVectorZ - 1, topVertex);
					auto haveLeftVertex = getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX - 1, terrainHeightVectorZ, leftVertex);
					auto haveBottomVertex = getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX, terrainHeightVectorZ + 1, bottomVertex);
					auto haveRightVertex = getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX + 1, terrainHeightVectorZ, rightVertex);
					if (haveTopVertex == true) {
						terrainVertexHeightNeighbourCount++;
						terrainVertexHeightNeighbours+= topVertex[1];
					}
					if (haveLeftVertex == true) {
						terrainVertexHeightNeighbourCount++;
						terrainVertexHeightNeighbours+= leftVertex[1];
					}
					if (haveBottomVertex == true) {
						terrainVertexHeightNeighbourCount++;
						terrainVertexHeightNeighbours+= bottomVertex[1];
					}
					if (haveRightVertex == true) {
						terrainVertexHeightNeighbourCount++;
						terrainVertexHeightNeighbours+= rightVertex[1];
					}
					if (terrainVertexHeightNeighbourCount > 0) {
						auto terrainVertexHeightSmoothed = terrainVertexHeightNeighbours / static_cast<float>(terrainVertexHeightNeighbourCount);
						terrainVertexHeight = terrainVertexHeight * (1.0f - Math::clamp(appliedStrength, 0.0f, 1.0f)) + terrainVertexHeightSmoothed * Math::clamp(appliedStrength, 0.0f, 1.0f);
					}
					break;
			}
			terrainHeightVector[terrainHeightVectorZ * terrainHeightVectorVerticesPerX + terrainHeightVectorX] = terrainVertexHeight;

			// original
			{
				auto _brushPosition = brushPosition;
				auto partitionX = static_cast<int>((_brushPosition.getX() - terrainBoundingBox.getMin().getX()) / PARTITION_SIZE);
				auto partitionZ = static_cast<int>((_brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / PARTITION_SIZE);
				auto partitionIdx = partitionZ * partitionsX + partitionX;
				auto terrainModel = partitionIdx < terrainModels.size()?terrainModels[partitionIdx]:nullptr;
				auto terrainNode = terrainModel != nullptr?terrainModel->getNodeById("terrain"):nullptr;

				auto terrainModelX = terrainNode != nullptr?static_cast<int>((_brushPosition.getX() - terrainModel->getBoundingBox()->getMin().getX()) / STEP_SIZE):-1;
				auto terrainModelZ = terrainNode != nullptr?static_cast<int>((_brushPosition.getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE):-1;
				auto terrainModelVerticesPerZ = terrainNode != nullptr?static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getDimensions().getZ() / STEP_SIZE)):-1;
				auto terrainModelVerticesPerX = terrainNode != nullptr?static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getDimensions().getX() / STEP_SIZE)):-1;

				if (terrainNode != nullptr &&
					terrainModelX >= 0 &&
					terrainModelX < terrainModelVerticesPerX &&
					terrainModelZ >= 0 &&
					terrainModelZ < terrainModelVerticesPerZ) {
					if (partitionTerrainVertices[partitionIdx].empty() == true) {
						partitionTerrainVertices[partitionIdx] = terrainNode->getVertices();
					}
					auto& terrainVertices = partitionTerrainVertices[partitionIdx];
					auto vertexIdx = (terrainModelZ * terrainModelVerticesPerX * 4) + (terrainModelX * 4);
					terrainVertices[vertexIdx + 3][1] = terrainVertexHeight;
				}
			}

			// top
			{
				auto _brushPosition = brushPosition.clone().sub(Vector3(0.0f, 0.0f, -STEP_SIZE));
				auto partitionX = static_cast<int>((_brushPosition.getX() - terrainBoundingBox.getMin().getX()) / PARTITION_SIZE);
				auto partitionZ = static_cast<int>((_brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / PARTITION_SIZE);
				auto partitionIdx = partitionZ * partitionsX + partitionX;
				auto terrainModel = partitionIdx < terrainModels.size()?terrainModels[partitionIdx]:nullptr;
				auto terrainNode = terrainModel != nullptr?terrainModel->getNodeById("terrain"):nullptr;

				auto terrainModelX = terrainNode != nullptr?static_cast<int>((_brushPosition.getX() - terrainModel->getBoundingBox()->getMin().getX()) / STEP_SIZE):-1;
				auto terrainModelZ = terrainNode != nullptr?static_cast<int>((_brushPosition.getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE):-1;
				auto terrainModelVerticesPerZ = terrainNode != nullptr?static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getDimensions().getZ() / STEP_SIZE)):-1;
				auto terrainModelVerticesPerX = terrainNode != nullptr?static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getDimensions().getX() / STEP_SIZE)):-1;

				if (terrainNode != nullptr &&
					terrainModelX >= 0 &&
					terrainModelX < terrainModelVerticesPerX &&
					terrainModelZ >= 0 &&
					terrainModelZ < terrainModelVerticesPerZ) {
					if (partitionTerrainVertices[partitionIdx].empty() == true) {
						partitionTerrainVertices[partitionIdx] = terrainNode->getVertices();
					}
					auto& terrainVertices = partitionTerrainVertices[partitionIdx];
					auto vertexIdx = (terrainModelZ * terrainModelVerticesPerX * 4) + (terrainModelX * 4);
					terrainVertices[vertexIdx + 0][1] = terrainVertexHeight;
				}
			}

			// top, left
			{
				auto _brushPosition = brushPosition.clone().sub(Vector3(-STEP_SIZE, 0.0f, -STEP_SIZE));
				auto partitionX = static_cast<int>((_brushPosition.getX() - terrainBoundingBox.getMin().getX()) / PARTITION_SIZE);
				auto partitionZ = static_cast<int>((_brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / PARTITION_SIZE);
				auto partitionIdx = partitionZ * partitionsX + partitionX;
				auto terrainModel = partitionIdx < terrainModels.size()?terrainModels[partitionIdx]:nullptr;
				auto terrainNode = terrainModel != nullptr?terrainModel->getNodeById("terrain"):nullptr;

				auto terrainModelX = terrainNode != nullptr?static_cast<int>((_brushPosition.getX() - terrainModel->getBoundingBox()->getMin().getX()) / STEP_SIZE):-1;
				auto terrainModelZ = terrainNode != nullptr?static_cast<int>((_brushPosition.getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE):-1;
				auto terrainModelVerticesPerZ = terrainNode != nullptr?static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getDimensions().getZ() / STEP_SIZE)):-1;
				auto terrainModelVerticesPerX = terrainNode != nullptr?static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getDimensions().getX() / STEP_SIZE)):-1;

				if (terrainNode != nullptr &&
					terrainModelX >= 0 &&
					terrainModelX < terrainModelVerticesPerX &&
					terrainModelZ >= 0 &&
					terrainModelZ < terrainModelVerticesPerZ) {
					if (partitionTerrainVertices[partitionIdx].empty() == true) {
						partitionTerrainVertices[partitionIdx] = terrainNode->getVertices();
					}
					auto& terrainVertices = partitionTerrainVertices[partitionIdx];
					auto vertexIdx = (terrainModelZ * terrainModelVerticesPerX * 4) + (terrainModelX * 4);
					terrainVertices[vertexIdx + 1][1] = terrainVertexHeight;
				}
			}

			// left
			{
				auto _brushPosition = brushPosition.clone().sub(Vector3(-STEP_SIZE, 0.0f, 0.0f));
				auto partitionX = static_cast<int>((_brushPosition.getX() - terrainBoundingBox.getMin().getX()) / PARTITION_SIZE);
				auto partitionZ = static_cast<int>((_brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / PARTITION_SIZE);
				auto partitionIdx = partitionZ * partitionsX + partitionX;
				auto terrainModel = partitionIdx < terrainModels.size()?terrainModels[partitionIdx]:nullptr;
				auto terrainNode = terrainModel != nullptr?terrainModel->getNodeById("terrain"):nullptr;

				auto terrainModelX = terrainNode != nullptr?static_cast<int>((_brushPosition.getX() - terrainModel->getBoundingBox()->getMin().getX()) / STEP_SIZE):-1;
				auto terrainModelZ = terrainNode != nullptr?static_cast<int>((_brushPosition.getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE):-1;
				auto terrainModelVerticesPerZ = terrainNode != nullptr?static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getDimensions().getZ() / STEP_SIZE)):-1;
				auto terrainModelVerticesPerX = terrainNode != nullptr?static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getDimensions().getX() / STEP_SIZE)):-1;

				if (terrainNode != nullptr &&
					terrainModelX >= 0 &&
					terrainModelX < terrainModelVerticesPerX &&
					terrainModelZ >= 0 &&
					terrainModelZ < terrainModelVerticesPerZ) {
					if (partitionTerrainVertices[partitionIdx].empty() == true) {
						partitionTerrainVertices[partitionIdx] = terrainNode->getVertices();
					}
					auto& terrainVertices = partitionTerrainVertices[partitionIdx];
					auto vertexIdx = (terrainModelZ * terrainModelVerticesPerX * 4) + (terrainModelX * 4);
					terrainVertices[vertexIdx + 2][1] = terrainVertexHeight;
				}
			}

			//
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
			auto partitionX = static_cast<int>((brushPosition.getX() - terrainBoundingBox.getMin().getX()) / PARTITION_SIZE);
			auto partitionZ = static_cast<int>((brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / PARTITION_SIZE);
			auto partitionIdx = partitionZ * partitionsX + partitionX;
			auto terrainModel = partitionIdx < terrainModels.size()?terrainModels[partitionIdx]:nullptr;
			auto terrainNode = terrainModel != nullptr?terrainModel->getNodeById("terrain"):nullptr;

			auto terrainHeightVectorX = static_cast<int>((brushPosition.getX() - terrainBoundingBox.getMin().getX()) / STEP_SIZE);
			auto terrainHeightVectorZ = static_cast<int>((brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / STEP_SIZE);
			auto terrainModelX = terrainNode != nullptr?static_cast<int>((brushPosition.getX() - terrainModel->getBoundingBox()->getMin().getX()) / STEP_SIZE):-1;
			auto terrainModelZ = terrainNode != nullptr?static_cast<int>((brushPosition.getZ() - terrainModel->getBoundingBox()->getMin().getZ()) / STEP_SIZE):-1;
			auto terrainModelVerticesPerZ = terrainNode != nullptr?static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getDimensions().getZ() / STEP_SIZE)):-1;
			auto terrainModelVerticesPerX = terrainNode != nullptr?static_cast<int>(Math::ceil(terrainModel->getBoundingBox()->getDimensions().getX() / STEP_SIZE)):-1;

			if (terrainNode != nullptr &&
				terrainModelX >= 0 &&
				terrainModelX < terrainModelVerticesPerX &&
				terrainModelZ >= 0 &&
				terrainModelZ < terrainModelVerticesPerZ) {
				if (partitionTerrainNormals[partitionIdx].empty() == true) {
					partitionTerrainNormals[partitionIdx] = terrainNode->getNormals();
				}
				auto& terrainNormals = partitionTerrainNormals[partitionIdx];

				auto topNormal = computeTerrainVertexNormal(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX, terrainHeightVectorZ - 1);
				auto topLeftNormal = computeTerrainVertexNormal(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX - 1, terrainHeightVectorZ - 1);
				auto leftNormal = computeTerrainVertexNormal(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX - 1, terrainHeightVectorZ);
				auto normal = computeTerrainVertexNormal(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorX, terrainHeightVectorZ);
				auto normalIdx = (terrainModelZ * terrainModelVerticesPerX * 4) + (terrainModelX * 4);

				terrainNormals[normalIdx + 0] = topNormal;
				terrainNormals[normalIdx + 1] = topLeftNormal;
				terrainNormals[normalIdx + 2] = leftNormal;
				terrainNormals[normalIdx + 3] = normal;
			}

			//
			brushPosition.add(
				Vector3(
					STEP_SIZE,
					0.0f,
					0.0f
				)
			);
		}
	}

	// set terrain model vertices
	{
		auto partitionIdx = 0;
		for (auto& terrainVertices: partitionTerrainVertices) {
			if (terrainVertices.empty() == false) {
				auto terrainNode = terrainModels[partitionIdx]->getNodeById("terrain");
				if (terrainNode != nullptr) {
					terrainNode->setVertices(terrainVertices);
				}
			}
			partitionIdx++;
		}
	}
	// set terrain model normals
	{
		auto partitionIdx = 0;
		for (auto& terrainNormals: partitionTerrainNormals) {
			if (terrainNormals.empty() == false) {
				auto terrainNode = terrainModels[partitionIdx]->getNodeById("terrain");
				if (terrainNode != nullptr) {
					terrainNode->setNormals(terrainNormals);
				}
			}
			partitionIdx++;
		}
	}
}

bool Terrain::getTerrainModelFlattenHeight(
	BoundingBox& terrainBoundingBox,
	vector<Model*> terrainModels,
	vector<float>& terrainHeightVector,
	const Vector3& brushCenterPosition,
	float& flattenHeight
) {
	// check if we have a model
	if (terrainModels.empty() == true) return false;

	// get height at brush position
	auto terrainHeightVectorVerticesPerX = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX() / STEP_SIZE));
	auto terreinHeightVectorVerticesPerZ = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getZ() / STEP_SIZE));
	auto terrainHeightVectorX = static_cast<int>((brushCenterPosition.getX() - terrainBoundingBox.getMin().getX()) / STEP_SIZE);
	auto terrainHeightVectorZ = static_cast<int>((brushCenterPosition.getZ() - terrainBoundingBox.getMin().getZ()) / STEP_SIZE);
	if (terrainHeightVectorX < 0 || terrainHeightVectorX >= terreinHeightVectorVerticesPerZ ||
			terrainHeightVectorZ < 0 || terrainHeightVectorZ >= terreinHeightVectorVerticesPerZ) return false;
	flattenHeight = terrainHeightVector[terrainHeightVectorZ * terreinHeightVectorVerticesPerZ + terrainHeightVectorX];

	//
	return true;
}
