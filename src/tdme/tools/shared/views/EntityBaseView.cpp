#include <tdme/tools/shared/views/EntityBaseView.h>

#include <string>

#include <tdme/tools/shared/controller/EntityBaseSubScreenController.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelPropertyPresets.h>
#include <tdme/tools/shared/model/PropertyModelClass.h>

using std::string;

using tdme::tools::shared::views::EntityBaseView;
using tdme::tools::shared::controller::EntityBaseSubScreenController;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelPropertyPresets;
using tdme::tools::shared::model::PropertyModelClass;

EntityBaseView::EntityBaseView(EntityBaseSubScreenController* entityBaseSubScreenController)
{
	this->entityBaseSubScreenController = entityBaseSubScreenController;
}

void EntityBaseView::initialize()
{
	entityBaseSubScreenController->setEntityPresetIds(LevelPropertyPresets::getInstance()->getObjectPropertiesPresets());
}

void EntityBaseView::entityPropertiesPreset(LevelEditorEntity* entity, const string& presetId)
{
	if (entity == nullptr)
		return;

	entity->clearProperties();
	auto& objectPropertiesPreset = LevelPropertyPresets::getInstance()->getObjectPropertiesPresets();
	const vector<PropertyModelClass*>* entityPropertyPresetArrayList = nullptr;
	auto entityPropertyPresetArrayListIt = objectPropertiesPreset.find(presetId);
	if (entityPropertyPresetArrayListIt != objectPropertiesPreset.end()) {
		entityPropertyPresetArrayList = &entityPropertyPresetArrayListIt->second;
	}
	if (entityPropertyPresetArrayList != nullptr) {
		for (auto entityPropertyPreset: *entityPropertyPresetArrayList) {
			entity->addProperty(entityPropertyPreset->getName(), entityPropertyPreset->getValue());
		}
	}
	entityBaseSubScreenController->setEntityProperties(entity, presetId, "");
}

bool EntityBaseView::entityPropertySave(LevelEditorEntity* entity, const string& oldName, const string& name, const string& value)
{
	if (entity == nullptr)
		return false;

	if (entity->updateProperty(oldName, name, value) == true) {
		entityBaseSubScreenController->setEntityProperties(entity, "", name);
		return true;
	}
	return false;
}

bool EntityBaseView::entityPropertyAdd(LevelEditorEntity* entity)
{
	if (entity == nullptr)
		return false;

	if (entity->addProperty("new.property", "new.value")) {
		entityBaseSubScreenController->setEntityProperties(entity, "", "new.property");
		return true;
	}
	return false;
}

bool EntityBaseView::entityPropertyRemove(LevelEditorEntity* entity, const string& name)
{
	if (entity == nullptr)
		return false;

	auto idx = entity->getPropertyIndex(name);
	if (idx != -1 && entity->removeProperty(name) == true) {
		auto property = entity->getPropertyByIndex(idx);
		if (property == nullptr) {
			property = entity->getPropertyByIndex(idx - 1);
		}
		entityBaseSubScreenController->setEntityProperties(entity, "", property == nullptr ? "" : property->getName());
		return true;
	}
	return false;
}

void EntityBaseView::setEntityData(LevelEditorEntity* entity, const string& name, const string& description)
{
	if (entity == nullptr)
		return;

	entity->setName(name);
	entity->setDescription(description);
}
