#include <tdme/engine/PartitionNone.h>

#include <vector>

#include <tdme/engine/Entity.h>
#include <tdme/utilities/VectorIteratorMultiple.h>

using std::vector;

using tdme::engine::PartitionNone;
using tdme::engine::Entity;
using tdme::utilities::VectorIteratorMultiple;

PartitionNone::PartitionNone()
{
	arrayListIteratorMultiple.addVector(&entities);
}

void PartitionNone::reset()
{
	entities.clear();
}

void PartitionNone::addEntity(Entity* entity)
{
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i] == entity) return;
	}

	entities.push_back(entity);
}

void PartitionNone::updateEntity(Entity* entity)
{
}

void PartitionNone::removeEntity(Entity* entity)
{
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i] == entity) {
			entities.erase(entities.begin() + i);
			return;
		}
	}
}

const vector<Entity*>& PartitionNone::getVisibleEntities(Frustum* frustum)
{
	return entities;
}
