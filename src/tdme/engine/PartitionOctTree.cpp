#include <tdme/engine/PartitionOctTree.h>

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <tdme/math/Math.h>
#include <tdme/engine/Entity.h>
#include <tdme/engine/Frustum.h>
#include <tdme/engine/PartitionOctTree_PartitionTreeNode.h>
#include <tdme/engine/physics/CollisionDetection.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/VectorIteratorMultiple.h>
#include <tdme/utilities/Console.h>

using std::list;
using std::map;
using std::remove;
using std::vector;
using std::string;
using std::to_string;

using tdme::engine::PartitionOctTree;
using tdme::math::Math;
using tdme::engine::Entity;
using tdme::engine::Frustum;
using tdme::engine::PartitionOctTree_PartitionTreeNode;
using tdme::engine::physics::CollisionDetection;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::math::Vector3;
using tdme::utilities::VectorIteratorMultiple;
using tdme::utilities::Console;

constexpr float PartitionOctTree::PARTITION_SIZE_MIN;

constexpr float PartitionOctTree::PARTITION_SIZE_MAX;

PartitionOctTree::PartitionOctTree()
{
	reset();
}

void PartitionOctTree::reset()
{
	this->entityPartitionNodes.clear();
	this->visibleEntities.clear();
	this->treeRoot.partitionSize = -1;
	this->treeRoot.x = -1;
	this->treeRoot.y = -1;
	this->treeRoot.z = -1;
	this->treeRoot.parent = nullptr;
}

void PartitionOctTree::addEntity(Entity* entity)
{
	// update if already exists
	vector<PartitionOctTree_PartitionTreeNode*>* thisEntityPartitions = nullptr;
	auto thisEntityPartitionsIt = entityPartitionNodes.find(entity->getId());
	if (thisEntityPartitionsIt != entityPartitionNodes.end()) {
		thisEntityPartitions = &thisEntityPartitionsIt->second;
	}
	if (thisEntityPartitions != nullptr && thisEntityPartitions->empty() == false) {
		removeEntity(entity);
	}
    // frustum bounding box
	auto boundingBox = entity->getBoundingBoxTransformed();
	// find, create root nodes if not exists
	auto minXPartition = static_cast< int32_t >(Math::floor(boundingBox->getMin().getX() / PARTITION_SIZE_MAX));
	auto minYPartition = static_cast< int32_t >(Math::floor(boundingBox->getMin().getY() / PARTITION_SIZE_MAX));
	auto minZPartition = static_cast< int32_t >(Math::floor(boundingBox->getMin().getZ() / PARTITION_SIZE_MAX));
	auto maxXPartition = static_cast< int32_t >(Math::floor(boundingBox->getMax().getX() / PARTITION_SIZE_MAX));
	auto maxYPartition = static_cast< int32_t >(Math::floor(boundingBox->getMax().getY() / PARTITION_SIZE_MAX));
	auto maxZPartition = static_cast< int32_t >(Math::floor(boundingBox->getMax().getZ() / PARTITION_SIZE_MAX));
	for (auto yPartition = minYPartition; yPartition <= maxYPartition; yPartition++)
	for (auto xPartition = minXPartition; xPartition <= maxXPartition; xPartition++)
	for (auto zPartition = minZPartition; zPartition <= maxZPartition; zPartition++) {
		updatePartitionTree(&treeRoot, xPartition, yPartition, zPartition, PARTITION_SIZE_MAX, entity);
	}
}

void PartitionOctTree::removeEntity(Entity* entity)
{
	// check if we have entity in oct tree
	vector<PartitionOctTree_PartitionTreeNode*>* objectPartitionsVector = nullptr;
	auto objectPartitionsVectorIt = entityPartitionNodes.find(entity->getId());
	if (objectPartitionsVectorIt != entityPartitionNodes.end()) {
		objectPartitionsVector = &objectPartitionsVectorIt->second;
	}
	if (objectPartitionsVector == nullptr || objectPartitionsVector->empty() == true) {
		Console::println(
			"PartitionOctTree::removeEntity(): '" +
			entity->getId() +
			"' not registered"
		);
		return;
	}
	// remove object from assigned partitions
	// TODO: remove tree root sub nodes as well not only empty root nodes
	while (objectPartitionsVector->size() > 0) {
		auto lastIdx = objectPartitionsVector->size() - 1;
		auto partitionTreeNode = (*objectPartitionsVector)[lastIdx];
		auto& partitionObjects = partitionTreeNode->partitionEntities;
		partitionObjects.erase(remove(partitionObjects.begin(), partitionObjects.end(), entity), partitionObjects.end());
		objectPartitionsVector->erase(objectPartitionsVector->begin() + lastIdx);
		if (partitionObjects.empty() == true) {
			auto rootPartitionTreeNode = partitionTreeNode;
			while (rootPartitionTreeNode->parent != nullptr) rootPartitionTreeNode = rootPartitionTreeNode->parent;
			// check if whole top level partition is empty
			if (isPartitionNodeEmpty(rootPartitionTreeNode) == true) {
				// yep, remove it
				removePartitionNode(rootPartitionTreeNode);
				for (auto treeRootSubNodeIt = treeRoot.subNodes.begin(); treeRootSubNodeIt != treeRoot.subNodes.end(); ++treeRootSubNodeIt) {
					if ((void*)&treeRootSubNodeIt == (void*)rootPartitionTreeNode) {
						treeRoot.subNodes.erase(treeRootSubNodeIt);
						break;
					}
				}
				string key = to_string(rootPartitionTreeNode->x) + "," + to_string(rootPartitionTreeNode->y) + "," + to_string(rootPartitionTreeNode->z);
				treeRoot.subNodesByCoordinate.erase(key);
			}
		}
	}
	entityPartitionNodes.erase(objectPartitionsVectorIt);
}

const vector<Entity*>& PartitionOctTree::getVisibleEntities(Frustum* frustum)
{
	visibleEntities.clear();
	visibleEntitiesById.clear();
	auto lookUps = 0;
	for (auto& subNode: treeRoot.subNodes) {
		lookUps += doPartitionTreeLookUpVisibleObjects(frustum, &subNode);
	}
	return visibleEntities;
}

void PartitionOctTree::dumpNode(PartitionOctTree_PartitionTreeNode* node, int indent) {
	for (auto i = 0; i < indent; i++) Console::print("\t");
	Console::println(to_string(node->x) + "/" + to_string(node->y) + "/" + to_string(node->z) + ": ");
	for (auto entity: node->partitionEntities) {
		for (auto i = 0; i < indent + 1; i++) Console::print("\t");
		Console::println(entity->getId());
	}
	for (auto subNode: node->subNodes) dumpNode(&subNode, indent + 1);
}

void PartitionOctTree::dump() {
	dumpNode(&treeRoot, 0);
}
