#include <tdme/utilities/Terrain.h>

#include <array>
#include <map>
#include <set>
#include <string>
#include <unordered_set>
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
#include <tdme/engine/primitives/LineSegment.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/ModelTools.h>

using std::array;
using std::map;
using std::set;
using std::string;
using std::to_string;
using std::unordered_set;
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
using tdme::engine::primitives::LineSegment;
using tdme::engine::Rotation;
using tdme::engine::Transformations;
using tdme::math::Math;
using tdme::tools::shared::tools::Tools;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::Integer;
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

			auto terrainHeightVectorX = static_cast<int>(x / STEP_SIZE);
			auto terrainHeightVectorZ = static_cast<int>(z / STEP_SIZE);

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
	for (auto partitionIdx = 0; partitionIdx < partitionCount; partitionIdx++) {
		if (partitionTerrainFaces[partitionIdx].empty() == true) continue;
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

void Terrain::applyBrushToTerrainModels(
	BoundingBox& terrainBoundingBox,
	vector<Model*>& terrainModels,
	vector<float>& terrainHeightVector,
	const Vector3& brushCenterPosition,
	Texture* brushTexture,
	float brushScale,
	float brushStrength,
	BrushOperation brushOperation,
	float brushHeight
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

	// water
	if (brushOperation == BRUSHOPERATION_WATER) return;

	// other operations
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
					terrainVertexHeight = terrainVertexHeight * (1.0f - Math::clamp(appliedStrength, 0.0f, 1.0f)) + brushHeight * Math::clamp(appliedStrength, 0.0f, 1.0f);
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

bool Terrain::computeWaterPositionMap(BoundingBox& terrainBoundingBox, const vector<float>& terrainHeightVector, const Vector3& brushCenterPosition, float waterHeight, unordered_map<int, unordered_set<int>>& waterPositionMap) {
	auto terrainHeightVectorVerticesPerX = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX() / STEP_SIZE));
	auto terreinHeightVectorVerticesPerZ = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getZ() / STEP_SIZE));

	auto brushPosition = brushCenterPosition;
	auto terrainHeightVectorXCenter = static_cast<int>((brushPosition.getX() - terrainBoundingBox.getMin().getX()) / STEP_SIZE);
	auto terrainHeightVectorZCenter = static_cast<int>((brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / STEP_SIZE);

	//
	waterPositionMap[terrainHeightVectorZCenter].insert(terrainHeightVectorXCenter);

	//
	Console::println("Terrain::determineWaterPositionSet: " + to_string(terrainHeightVectorXCenter) + " / " + to_string(terrainHeightVectorZCenter) + " @ " + to_string(waterHeight));

	//
	determineWaterXPositionSet(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, terrainHeightVectorXCenter, terrainHeightVectorZCenter, waterHeight, waterPositionMap[terrainHeightVectorZCenter]);

	//
	{
		auto zLast = 0;
		auto zMin = -1;
		while (true == true) {
			auto terrainHeightVectorZLast = terrainHeightVectorZCenter + zLast;
			auto terrainHeightVectorZ = terrainHeightVectorZCenter + zMin;
			for (auto zLastWaterXPosition: waterPositionMap[terrainHeightVectorZLast]) {
				if (determineWater(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, zLastWaterXPosition, terrainHeightVectorZ, waterHeight) == true) {
					determineWaterXPositionSet(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, zLastWaterXPosition, terrainHeightVectorZ, waterHeight, waterPositionMap[terrainHeightVectorZ]);
				}
			}
			if (waterPositionMap[terrainHeightVectorZ].empty() == true) break;
			zLast = zMin;
			zMin--;
		}
	}

	//
	{
		auto zLast = 0;
		auto zMax = 1;
		while (true == true) {
			auto terrainHeightVectorZLast = terrainHeightVectorZCenter + zLast;
			auto terrainHeightVectorZ = terrainHeightVectorZCenter + zMax;
			for (auto zLastWaterXPosition: waterPositionMap[terrainHeightVectorZLast]) {
				if (determineWater(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, zLastWaterXPosition, terrainHeightVectorZ, waterHeight) == true) {
					determineWaterXPositionSet(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, zLastWaterXPosition, terrainHeightVectorZ, waterHeight, waterPositionMap[terrainHeightVectorZ]);
				}
			}
			if (waterPositionMap[terrainHeightVectorZ].empty() == true) break;
			zLast = zMax;
			zMax++;
		}
	}

	{
		auto waterPositionMapCopy = waterPositionMap;
		auto zMin = waterPositionMap.begin()->first;
		auto zMax = waterPositionMap.begin()->first;
		do {
			waterPositionMapCopy = waterPositionMap;
			for (auto& waterPositionMapIt: waterPositionMap) {
				auto z = waterPositionMapIt.first;
				for (auto x: waterPositionMapIt.second) {
					if (hasWaterPosition(waterPositionMap, x, z - 1) == false &&
						determineWater(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, x, z - 1, waterHeight) == true) {
						computeWaterPositionMap(
							terrainBoundingBox,
							terrainHeightVector,
							Vector3(
								terrainBoundingBox.getMin().getX() + x * STEP_SIZE,
								waterHeight,
								terrainBoundingBox.getMin().getZ() + ((z - 1) * STEP_SIZE)
							),
							waterHeight,
							waterPositionMap
						);
					} else
					if (hasWaterPosition(waterPositionMap, x, z + 1) == false &&
						determineWater(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, x, z + 1, waterHeight) == true) {
						computeWaterPositionMap(
							terrainBoundingBox,
							terrainHeightVector,
							Vector3(
								terrainBoundingBox.getMin().getX() + x * STEP_SIZE,
								waterHeight,
								terrainBoundingBox.getMin().getZ() + ((z + 1) * STEP_SIZE)
							),
							waterHeight,
							waterPositionMap
						);
					}
				}
			}
		} while (waterPositionMapCopy != waterPositionMap);
		waterPositionMap[zMax] = waterPositionMap[zMax - 1];
	}

	//
	auto haveWaterPositionSet = waterPositionMap.empty() == false;
	Console::println("Terrain::determineWaterPositionSet: Have water position set: " + to_string(haveWaterPositionSet));
	return haveWaterPositionSet;
}

Vector3 Terrain::computeWaterReflectionEnvironmentMappingPosition(const unordered_map<int, unordered_set<int>>& waterPositionMap, float waterHeight) {
	// determine reflection environment mapping position
	auto zMin = Integer::MAX_VALUE;
	auto zMax = Integer::MIN_VALUE;
	auto xMin = Integer::MAX_VALUE;
	auto xMax = Integer::MIN_VALUE;
	for (auto& mIt: waterPositionMap) {
		auto z = mIt.first;
		if (z < zMin) zMin = z;
		if (z > zMax) zMax = z;
		for (auto x: mIt.second) {
			if (x < xMin) xMin = x;
			if (x > xMax) xMax = x;
		}
	}

	return Vector3(
		(static_cast<float>(xMin + xMax) / 2.0f) * STEP_SIZE,
		waterHeight + 2.0f,
		(static_cast<float>(zMin + zMax) / 2.0f) * STEP_SIZE
	);
}

void Terrain::createWaterModels(
	BoundingBox& terrainBoundingBox,
	const unordered_map<int, unordered_set<int>>& waterPositionMap,
	float waterHeight,
	int waterModelIdx,
	vector<Model*>& waterModels
) {
	//
	for (auto& mIt: waterPositionMap) {
		Console::print(to_string(mIt.first) + ": ");
		for (auto waterXPosition: mIt.second) Console::print(to_string(waterXPosition) + " ");
		Console::println();
	}

	auto width = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX()));
	auto depth = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getZ()));
	auto partitionsX = static_cast<int>(Math::ceil(width / PARTITION_SIZE));
	auto partitionsZ = static_cast<int>(Math::ceil(depth / PARTITION_SIZE));
	auto partitionCount = partitionsX * partitionsZ;
	vector<vector<Vector3>> partitionTerrainVertices;
	vector<vector<Vector3>> partitionTerrainNormals;
	vector<vector<array<int, 6>>> partitionWaterFaces;
	partitionTerrainVertices.resize(partitionCount);
	partitionTerrainNormals.resize(partitionCount);
	partitionWaterFaces.resize(partitionCount);
	for (float z = 0.0f; z < depth; z+= STEP_SIZE) {
		for (float x = 0.0f; x < width; x+= STEP_SIZE) {
			auto terrainHeightVectorX = static_cast<int>(x / STEP_SIZE);
			auto terrainHeightVectorZ = static_cast<int>(z / STEP_SIZE);

			Vector3 topVertex;
			Vector3 topLeftVertex;
			Vector3 leftVertex;
			Vector3 vertex;

			auto hasTopLeft = hasWaterPosition(waterPositionMap, terrainHeightVectorX - 1, terrainHeightVectorZ - 1);
			auto hasTop = hasWaterPosition(waterPositionMap, terrainHeightVectorX, terrainHeightVectorZ - 1);
			auto hasLeft = hasWaterPosition(waterPositionMap, terrainHeightVectorX - 1, terrainHeightVectorZ);
			auto hasOrigin = hasWaterPosition(waterPositionMap, terrainHeightVectorX, terrainHeightVectorZ);

			auto haveVertexCount = 0;
			if (hasTop == true) haveVertexCount++;
			if (hasTopLeft == true) haveVertexCount++;
			if (hasLeft == true) haveVertexCount++;
			if (hasOrigin == true) haveVertexCount++;
			if (haveVertexCount < 3) continue;

			auto partitionX = static_cast<int>(x / PARTITION_SIZE);
			auto partitionZ = static_cast<int>(z / PARTITION_SIZE);
			auto partitionIdx = partitionZ * partitionsX + partitionX;

			auto& terrainVertices = partitionTerrainVertices[partitionIdx];
			auto& terrainNormals = partitionTerrainNormals[partitionIdx];
			auto& terrainFaces = partitionWaterFaces[partitionIdx];

			int normalIdx = terrainNormals.size();
			int vertexIdx = terrainVertices.size();

			getWaterVertex(terrainHeightVectorX, terrainHeightVectorZ - 1, waterHeight, topVertex);
			getWaterVertex(terrainHeightVectorX - 1, terrainHeightVectorZ - 1, waterHeight, topLeftVertex);
			getWaterVertex(terrainHeightVectorX - 1, terrainHeightVectorZ, waterHeight, leftVertex);
			getWaterVertex(terrainHeightVectorX, terrainHeightVectorZ, waterHeight, vertex);

			if (hasTop == true) terrainVertices.push_back(topVertex);
			if (hasTopLeft == true) terrainVertices.push_back(topLeftVertex);
			if (hasLeft == true) terrainVertices.push_back(leftVertex);
			if (hasOrigin == true) terrainVertices.push_back(vertex);

			auto normal = Vector3(0.0f, 1.0f, 0.0f);
			auto topNormal = Vector3(0.0f, 1.0f, 0.0f);
			auto topLeftNormal = Vector3(0.0f, 1.0f, 0.0f);
			auto leftNormal = Vector3(0.0f, 1.0f, 0.0f);

			if (hasTop == true) terrainNormals.push_back(topNormal);
			if (hasTopLeft == true) terrainNormals.push_back(topLeftNormal);
			if (hasLeft == true) terrainNormals.push_back(leftNormal);
			if (hasOrigin == true) terrainNormals.push_back(normal);

			if (hasTopLeft == false ||
				hasTop == false ||
				hasLeft == false ||
				hasOrigin == false) {
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
			} else {
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
	}
	for (auto partitionIdx = 0; partitionIdx < partitionCount; partitionIdx++) {
		if (partitionWaterFaces[partitionIdx].empty() == true) continue;
		auto modelId = "water." + to_string(waterModelIdx) + "." + to_string(partitionIdx);
		auto waterModel = new Model(modelId, modelId, UpVector::Y_UP, RotationOrder::ZYX, nullptr);
		auto waterMaterial = new Material("water");
		waterMaterial->setSpecularMaterialProperties(new SpecularMaterialProperties());
		// TODO: Fix me! The textures seem to be much too dark
		waterMaterial->getSpecularMaterialProperties()->setAmbientColor(Color4(0.022f, 0.13f, 0.56f, 1.0f));
		waterMaterial->getSpecularMaterialProperties()->setDiffuseColor(Color4(0.026f, 0.15f, 0.64f, 1.0f));
		waterMaterial->getSpecularMaterialProperties()->setSpecularColor(Color4(1.0f, 1.0f, 1.0f, 1.0f));
		waterMaterial->getSpecularMaterialProperties()->setShininess(100.0f);
		waterModel->getMaterials()[waterMaterial->getId()] = waterMaterial;
		auto waterNode = new Node(waterModel, nullptr, "water", "water");
		FacesEntity nodeFacesEntityWater(waterNode, "water.facesentity");
		nodeFacesEntityWater.setMaterial(waterMaterial);
		vector<FacesEntity> nodeFacesEntities;
		vector<Face> nodeFaces;
		for (auto faceIndices: partitionWaterFaces[partitionIdx]) {
			nodeFaces.push_back(
				Face(
					waterNode,
					faceIndices[0],
					faceIndices[1],
					faceIndices[2],
					faceIndices[3],
					faceIndices[4],
					faceIndices[5]
				)
			);
		};
		nodeFacesEntityWater.setFaces(nodeFaces);
		nodeFacesEntities.push_back(nodeFacesEntityWater);
		waterNode->setVertices(partitionTerrainVertices[partitionIdx]);
		waterNode->setNormals(partitionTerrainNormals[partitionIdx]);
		waterNode->setFacesEntities(nodeFacesEntities);
		waterModel->getNodes()[waterNode->getId()] = waterNode;
		waterModel->getSubNodes()[waterNode->getId()] = waterNode;
		ModelTools::prepareForIndexedRendering(waterModel);
		waterModels.push_back(waterModel);
	}
}

bool Terrain::getTerrainModelsHeight(
	BoundingBox& terrainBoundingBox,
	vector<Model*>& terrainModels,
	vector<float>& terrainHeightVector,
	const Vector3& brushCenterPosition,
	float& brushHeight
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
	brushHeight = terrainHeightVector[terrainHeightVectorZ * terreinHeightVectorVerticesPerZ + terrainHeightVectorX];

	//
	return true;
}

void Terrain::createFoliageMaps(
	BoundingBox& terrainBoundingBox,
	vector<unordered_map<int, vector<Transformations>>>& foliageMaps
) {
	//
	auto width = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX()));
	auto depth = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getZ()));
	auto partitionsX = static_cast<int>(Math::ceil(width / PARTITION_SIZE));
	auto partitionsZ = static_cast<int>(Math::ceil(depth / PARTITION_SIZE));
	auto partitionCount = partitionsX * partitionsZ;
	foliageMaps.resize(partitionCount);
	for (auto& foliageMap: foliageMaps) foliageMap.clear();
}

void Terrain::emptyFoliageMaps(
	vector<unordered_map<int, vector<Transformations>>>& foliageMaps
) {
	//
	for (auto& foliageMap: foliageMaps) foliageMap.clear();
}

void Terrain::applyFoliageBrush(
	BoundingBox& terrainBoundingBox,
	vector<float>& terrainHeightVector,
	const Vector3& brushCenterPosition,
	Texture* brushTexture,
	float brushScale,
	float brushDensity,
	array<int, 5> brushPrototypeIds,
	array<float, 5> brushPrototypeCount,
	array<array<float, 2>, 5> brushPrototypeScale,
	array<array<float, 6>, 5> brushPrototypeRotation,
	array<float, 5> brushPrototypeSlopeMax,
	array<float, 5> brushPrototypeHeightMax,
	BrushOperation brushOperation,
	vector<unordered_map<int, vector<Transformations>>>& foliageMaps,
	vector<unordered_map<int, vector<Transformations>>>& newFoliageMaps
) {
	// check if we have a texture
	if (brushTexture == nullptr) return;

	// apply brush
	auto partitionsX = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX() / PARTITION_SIZE));
	auto terrainHeightVectorVerticesPerX = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX() / STEP_SIZE));
	auto terreinHeightVectorVerticesPerZ = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getZ() / STEP_SIZE));

	// other operations
	auto textureData = brushTexture->getTextureData();
	auto textureWidth = brushTexture->getTextureWidth();
	auto textureHeight = brushTexture->getTextureHeight();
	auto textureBytePerPixel = brushTexture->getDepth() == 32?4:3;

	//
	vector<unordered_map<int, float>> brushMapCountMapTemplate;
	auto brushMapCountMapWidth = static_cast<int>(textureWidth * brushScale);
	auto brushMapCountMapDepth = static_cast<int>(textureHeight * brushScale);
	for (auto z = 0.0f; z < textureHeight * brushScale; z+= 1.0f) {
		for (auto x = 0.0f; x < textureWidth * brushScale; x+= 1.0f) {
			auto textureX = static_cast<int>(x / brushScale);
			auto textureY = static_cast<int>(z / brushScale);
			auto red = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 0);
			auto green = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 1);
			auto blue = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 2);
			auto alpha = textureBytePerPixel == 3?255:textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 3);
			auto appliedDensity = (static_cast<float>(red) + static_cast<float>(green) + static_cast<float>(blue)) / (255.0f * 3.0f) * brushDensity;
			unordered_map<int, float> brushMapCountMapEntity;
			for (auto i = 0; i < brushPrototypeIds.size(); i++) {
				auto prototypeId = brushPrototypeIds[i];
				if (prototypeId == -1) continue;
				auto foliageCount = brushPrototypeCount[i];
				brushMapCountMapEntity[prototypeId] = foliageCount * appliedDensity;
			}
			brushMapCountMapTemplate.push_back(brushMapCountMapEntity);
		}
	}

	//
	auto brushMapCountMap = brushMapCountMapTemplate;
	for (auto prototypeId: brushPrototypeIds) {
		if (prototypeId == -1) continue;
		float totalCount = 0.0f;
		for (auto i = 0; i < brushMapCountMap.size(); i++) {
			auto& brushMapCountMapEntity = brushMapCountMap[i];
			auto count = brushMapCountMapEntity[prototypeId];
			auto countFloor = Math::floor(count);
			totalCount+= count - countFloor;
			brushMapCountMapEntity[prototypeId] = countFloor;
			auto totalCountFloor = Math::floor(totalCount);
			if (totalCount >= 1.0f) brushMapCountMapEntity[prototypeId]+= totalCountFloor;
			totalCount-= totalCountFloor;
		}
	}

	// randomize
	unordered_map<int, unordered_map<int, vector<int>>> brushMapIdxPerDensityPerPrototype;
	for (auto prototypeId: brushPrototypeIds) {
		for (auto i = 0; i < brushMapCountMapTemplate.size(); i++) {
			auto brushMapPrototypeCount = brushMapCountMapTemplate[i][prototypeId];
			brushMapIdxPerDensityPerPrototype[prototypeId][static_cast<int>(brushMapPrototypeCount * 1000.0f)].push_back(i);
		}
	}
	for (auto prototypeId: brushPrototypeIds) {
		for (auto i = 0; i < brushMapCountMap.size(); i++) {
			auto brushMapPrototypeCountMapEntityITemplate = brushMapCountMapTemplate[i][prototypeId];
			auto brushMapPrototypeCountMapEntityI = brushMapCountMap[i][prototypeId];
			auto& brushMapIdxPerDensityPerPrototypeVector = brushMapIdxPerDensityPerPrototype[prototypeId][static_cast<int>(brushMapPrototypeCountMapEntityITemplate * 1000.0f)];
			auto j = brushMapIdxPerDensityPerPrototypeVector[static_cast<int>(brushMapIdxPerDensityPerPrototypeVector.size() - 1) * Math::random()];
			auto brushMapPrototypeCountMapEntityJ = brushMapCountMap[j][prototypeId];
			brushMapCountMap[j][prototypeId] = brushMapPrototypeCountMapEntityI;
			brushMapCountMap[i][prototypeId] = brushMapPrototypeCountMapEntityJ;
		}
	}

	//
	for (auto z = 0.0f; z < textureHeight * brushScale; z+= 1.0f) {
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
		for (auto x = 0.0f; x < textureWidth * brushScale; x+= 1.0f) {
			auto brushMapCountMapX = static_cast<int>(x);
			auto brushMapCountMapZ = static_cast<int>(z);
			auto brushMapCountMapEntity = brushMapCountMap[brushMapCountMapZ * brushMapCountMapWidth + brushMapCountMapX];
			auto terrainHeightVectorX = static_cast<int>((brushPosition.getX() - terrainBoundingBox.getMin().getX()) / STEP_SIZE);
			auto terrainHeightVectorZ = static_cast<int>((brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / STEP_SIZE);
			if (terrainHeightVectorX < 0 || terrainHeightVectorX >= terrainHeightVectorVerticesPerX ||
				terrainHeightVectorZ < 0 || terrainHeightVectorZ >= terreinHeightVectorVerticesPerZ) continue;

			//
			auto partitionX = static_cast<int>((brushPosition.getX() - terrainBoundingBox.getMin().getX()) / PARTITION_SIZE);
			auto partitionZ = static_cast<int>((brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / PARTITION_SIZE);
			auto partitionIdx = partitionZ * partitionsX + partitionX;

			//
			switch(brushOperation) {
				case BRUSHOPERATION_ADD:
					for (auto& brushMapCountMapEntityIt: brushMapCountMapEntity) {
						auto prototypeId = brushMapCountMapEntityIt.first;
						if (prototypeId == -1) continue;
						auto prototypeCount = brushMapCountMapEntityIt.second;

						auto prototypeIdx = -1;
						for (auto i = 0; i < brushPrototypeIds.size(); i++) {
							if (brushPrototypeIds[i] == prototypeId) prototypeIdx = i;
						}
						if (prototypeIdx == -1) continue;

						//
						for (auto i = 0; i < static_cast<int>(prototypeCount); i++) {
							auto prototypeScale = brushPrototypeScale[prototypeIdx][0] + ((brushPrototypeScale[prototypeIdx][1] - brushPrototypeScale[prototypeIdx][0]) * Math::random());

							//
							Vector3 translation(
								Math::floor(brushPosition.getX()) + Math::random(),
								0.0f,
								Math::floor(brushPosition.getZ()) + Math::random()
							);

							auto haveContact = false;
							Vector3 contact;
							Vector3 normal;
							float height;
							for (int _z = -1; _z < 2; _z++)
							for (int _x = -1; _x < 2; _x++) {
								Vector3 topVertex;
								Vector3 topLeftVertex;
								Vector3 leftVertex;
								Vector3 vertex;

								getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, _x + terrainHeightVectorX, _z + terrainHeightVectorZ - 1, topVertex);
								getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, _x + terrainHeightVectorX - 1, _z + terrainHeightVectorZ - 1, topLeftVertex);
								getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, _x + terrainHeightVectorX - 1, _z + terrainHeightVectorZ, leftVertex);
								getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, _x + terrainHeightVectorX, _z + terrainHeightVectorZ, vertex);

								if (LineSegment::doesLineSegmentCollideWithTriangle(topVertex, topLeftVertex, leftVertex, translation.clone().setY(-10000.0f), translation.clone().setY(+10000.0f), contact) == true) {
									haveContact = true;
									ModelTools::computeNormal({topVertex, topLeftVertex, leftVertex}, normal);
									height = (topVertex.getY() + topLeftVertex.getY() + leftVertex.getY()) / 3.0f;
									break;
								} else
								if (LineSegment::doesLineSegmentCollideWithTriangle(leftVertex, vertex, topVertex, translation.clone().setY(-10000.0f), translation.clone().setY(+10000.0f), contact) == true) {
									haveContact = true;
									ModelTools::computeNormal({leftVertex, vertex, topVertex}, normal);
									height = (leftVertex.getY() + vertex.getY() + topVertex.getY()) / 3.0f;
									break;
								}
							}

							// check height
							if (height > brushPrototypeHeightMax[prototypeIdx]) continue;

							//
							if (haveContact == false) {
								Console::println(
									"Terrain::applyFoliageBrush(): no contact@" +
									to_string(translation.getX()) + ", " +
									to_string(translation.getZ())
								);
								contact = translation;
								continue;
							}

							// slope
							auto slope = Math::abs(180.0f / 3.14f * Math::acos(Math::clamp(Vector3::computeDotProduct(normal, Vector3(0.0, 1.0, 0.0)), -1.0, 1.0)));
							if (slope > brushPrototypeSlopeMax[prototypeIdx]) continue;

							//
							Transformations transformations;
							transformations.setTranslation(translation);
							transformations.addRotation(Rotation::Z_AXIS, brushPrototypeRotation[prototypeIdx][4] + ((brushPrototypeRotation[prototypeIdx][5] - brushPrototypeRotation[prototypeIdx][4]) * Math::random()));
							transformations.addRotation(Rotation::Y_AXIS, brushPrototypeRotation[prototypeIdx][2] + ((brushPrototypeRotation[prototypeIdx][3] - brushPrototypeRotation[prototypeIdx][2]) * Math::random()));
							transformations.addRotation(Rotation::X_AXIS, brushPrototypeRotation[prototypeIdx][0] + ((brushPrototypeRotation[prototypeIdx][1] - brushPrototypeRotation[prototypeIdx][0]) * Math::random()));
							transformations.setScale(Vector3(prototypeScale, prototypeScale, prototypeScale));

							transformations.setTranslation(translation.clone().setY(contact.getY()));
							transformations.update();

							//
							foliageMaps[partitionIdx][prototypeId].push_back(transformations);
							newFoliageMaps[partitionIdx][prototypeId].push_back(transformations);
						}
					}
					break;
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
}

void Terrain::applyFoliageDeleteBrush(
	BoundingBox& terrainBoundingBox, // TODO: constness
	const Vector3& brushCenterPosition,
	Texture* brushTexture,
	float brushScale,
	float brushDensity,
	BrushOperation brushOperation,
	vector<unordered_map<int, vector<Transformations>>>& foliageMaps,
	unordered_set<int>& recreateFoliagePartitions
) {
	// check if we have a texture
	if (brushTexture == nullptr) return;

	// apply brush
	auto partitionsX = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX() / PARTITION_SIZE));
	auto terrainHeightVectorVerticesPerX = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX() / STEP_SIZE));
	auto terreinHeightVectorVerticesPerZ = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getZ() / STEP_SIZE));

	// other operations
	auto textureData = brushTexture->getTextureData();
	auto textureWidth = brushTexture->getTextureWidth();
	auto textureHeight = brushTexture->getTextureHeight();
	auto textureBytePerPixel = brushTexture->getDepth() == 32?4:3;

	//
	for (auto z = 0.0f; z < textureHeight * brushScale; z+= 1.0f) {
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
		for (auto x = 0.0f; x < textureWidth * brushScale; x+= 1.0f) {
			auto textureX = static_cast<int>(x / brushScale);
			auto textureY = static_cast<int>(z / brushScale);
			auto red = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 0);
			auto green = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 1);
			auto blue = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 2);
			auto alpha = textureBytePerPixel == 3?255:textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 3);
			auto appliedDensity = (static_cast<float>(red) + static_cast<float>(green) + static_cast<float>(blue)) / (255.0f * 3.0f) * brushDensity;

			//
			auto terrainHeightVectorX = static_cast<int>((brushPosition.getX() - terrainBoundingBox.getMin().getX()) / STEP_SIZE);
			auto terrainHeightVectorZ = static_cast<int>((brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / STEP_SIZE);
			if (terrainHeightVectorX < 0 || terrainHeightVectorX >= terrainHeightVectorVerticesPerX ||
				terrainHeightVectorZ < 0 || terrainHeightVectorZ >= terreinHeightVectorVerticesPerZ) continue;

			//
			auto partitionX = static_cast<int>((brushPosition.getX() - terrainBoundingBox.getMin().getX()) / PARTITION_SIZE);
			auto partitionZ = static_cast<int>((brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / PARTITION_SIZE);
			auto partitionIdx = partitionZ * partitionsX + partitionX;

			//
			switch(brushOperation) {
				case BRUSHOPERATION_DELETE:
					{
						Vector3 topVertex;
						Vector3 topLeftVertex;
						Vector3 leftVertex;
						Vector3 vertex;

						getTerrainVertex(terrainHeightVectorX, terrainHeightVectorZ - 1, topVertex);
						getTerrainVertex(terrainHeightVectorX - 1, terrainHeightVectorZ - 1, topLeftVertex);
						getTerrainVertex(terrainHeightVectorX - 1, terrainHeightVectorZ, leftVertex);
						getTerrainVertex(terrainHeightVectorX, terrainHeightVectorZ, vertex);

						for (auto& foliageMapPartitionIt: foliageMaps[partitionIdx]) {
							auto& foliageMapPartitionPrototypeTransformations = foliageMapPartitionIt.second;
							for (auto i = 0; i < foliageMapPartitionPrototypeTransformations.size(); i++) {
								auto& translation = foliageMapPartitionPrototypeTransformations[i].getTranslation();
								if (appliedDensity > 0.0f &&
									translation.getX() >= leftVertex.getX() &&
									translation.getX() <= vertex.getX() &&
									translation.getZ() >= topVertex.getZ() &&
									translation.getZ() <= vertex.getZ()) {
									//
									foliageMapPartitionPrototypeTransformations.erase(foliageMapPartitionPrototypeTransformations.begin() + i);
									recreateFoliagePartitions.insert(partitionIdx);
									i--;
								}
							}
						}
					}
					break;
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
}

void Terrain::updateFoliageTerrainBrush(
	BoundingBox& terrainBoundingBox, // TODO: constness
	vector<float>& terrainHeightVector,
	const Vector3& brushCenterPosition,
	Texture* brushTexture,
	float brushScale,
	vector<unordered_map<int, vector<Transformations>>>& foliageMaps,
	unordered_set<int>& updateFoliagePartitions
) {
	// check if we have a texture
	if (brushTexture == nullptr) return;

	// apply brush
	auto partitionsX = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX() / PARTITION_SIZE));
	auto terrainHeightVectorVerticesPerX = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getX() / STEP_SIZE));
	auto terreinHeightVectorVerticesPerZ = static_cast<int>(Math::ceil(terrainBoundingBox.getDimensions().getZ() / STEP_SIZE));

	// other operations
	auto textureData = brushTexture->getTextureData();
	auto textureWidth = brushTexture->getTextureWidth();
	auto textureHeight = brushTexture->getTextureHeight();
	auto textureBytePerPixel = brushTexture->getDepth() == 32?4:3;

	//
	for (auto z = 0.0f; z < textureHeight * brushScale; z+= 1.0f) {
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
		for (auto x = 0.0f; x < textureWidth * brushScale; x+= 1.0f) {
			auto brushMapCountMapX = static_cast<int>(x);
			auto brushMapCountMapZ = static_cast<int>(z);
			auto terrainHeightVectorX = static_cast<int>((brushPosition.getX() - terrainBoundingBox.getMin().getX()) / STEP_SIZE);
			auto terrainHeightVectorZ = static_cast<int>((brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / STEP_SIZE);
			if (terrainHeightVectorX < 0 || terrainHeightVectorX >= terrainHeightVectorVerticesPerX ||
				terrainHeightVectorZ < 0 || terrainHeightVectorZ >= terreinHeightVectorVerticesPerZ) continue;

			//
			auto textureX = static_cast<int>(x / brushScale);
			auto textureY = static_cast<int>(z / brushScale);
			auto red = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 0);
			auto green = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 1);
			auto blue = textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 2);
			auto alpha = textureBytePerPixel == 3?255:textureData->get(textureY * textureWidth * textureBytePerPixel + textureX * textureBytePerPixel + 3);
			auto brushTextureDensity = (static_cast<float>(red) + static_cast<float>(green) + static_cast<float>(blue)) / (255.0f * 3.0f);

			//
			auto partitionX = static_cast<int>((brushPosition.getX() - terrainBoundingBox.getMin().getX()) / PARTITION_SIZE);
			auto partitionZ = static_cast<int>((brushPosition.getZ() - terrainBoundingBox.getMin().getZ()) / PARTITION_SIZE);
			auto partitionIdx = partitionZ * partitionsX + partitionX;

			//
			updateFoliagePartitions.insert(partitionIdx);

			//
			Vector3 topVertex;
			Vector3 topLeftVertex;
			Vector3 leftVertex;
			Vector3 vertex;

			//
			getTerrainVertex(terrainHeightVectorX, terrainHeightVectorZ - 1, topVertex);
			getTerrainVertex(terrainHeightVectorX - 1, terrainHeightVectorZ - 1, topLeftVertex);
			getTerrainVertex(terrainHeightVectorX - 1, terrainHeightVectorZ, leftVertex);
			getTerrainVertex(terrainHeightVectorX, terrainHeightVectorZ, vertex);

			//
			for (auto& foliageMapPartitionIt: foliageMaps[partitionIdx]) {
				auto prototypeId = foliageMapPartitionIt.first;
				if (prototypeId == -1) continue;
				auto& foliageMapPartitionPrototypeTransformations = foliageMapPartitionIt.second;

				//
				for (auto& transformations: foliageMapPartitionPrototypeTransformations) {
					auto& translation = transformations.getTranslation();
					if (brushTextureDensity > 0.0f &&
						translation.getX() >= leftVertex.getX() &&
						translation.getX() <= vertex.getX() &&
						translation.getZ() >= topVertex.getZ() &&
						translation.getZ() <= vertex.getZ()) {
						//
						auto haveContact = false;
						Vector3 contact;
						for (int _z = -1; _z < 2; _z++)
						for (int _x = -1; _x < 2; _x++) {
							Vector3 topVertex;
							Vector3 topLeftVertex;
							Vector3 leftVertex;
							Vector3 vertex;

							getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, _x + terrainHeightVectorX, _z + terrainHeightVectorZ - 1, topVertex);
							getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, _x + terrainHeightVectorX - 1, _z + terrainHeightVectorZ - 1, topLeftVertex);
							getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, _x + terrainHeightVectorX - 1, _z + terrainHeightVectorZ, leftVertex);
							getTerrainVertex(terrainHeightVector, terrainHeightVectorVerticesPerX, terreinHeightVectorVerticesPerZ, _x + terrainHeightVectorX, _z + terrainHeightVectorZ, vertex);

							if (LineSegment::doesLineSegmentCollideWithTriangle(topVertex, topLeftVertex, leftVertex, transformations.getTranslation().clone().setY(-10000.0f), transformations.getTranslation().clone().setY(+10000.0f), contact) == true) {
								haveContact = true;
								break;
							} else
							if (LineSegment::doesLineSegmentCollideWithTriangle(leftVertex, vertex, topVertex, transformations.getTranslation().clone().setY(-10000.0f), transformations.getTranslation().clone().setY(+10000.0f), contact) == true) {
								haveContact = true;
								break;
							}
						}

						//
						if (haveContact == false) {
							Console::println(
								"Terrain::applyFoliageBrush(): no contact@" +
								to_string(transformations.getTranslation().getX()) + ", " +
								to_string(transformations.getTranslation().getZ())
							);
							contact = transformations.getTranslation();
						}

						//
						transformations.setTranslation(transformations.getTranslation().clone().setY(contact.getY()));
						transformations.update();
					}
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
}
