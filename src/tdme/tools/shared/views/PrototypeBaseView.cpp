#include <tdme/tools/shared/views/PrototypeBaseView.h>

#include <string>

#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeProperty.h>
#include <tdme/engine/scene/ScenePropertyPresets.h>
#include <tdme/tools/shared/controller/PrototypeBaseSubScreenController.h>

using std::string;

using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::scene::ScenePropertyPresets;
using tdme::tools::shared::controller::PrototypeBaseSubScreenController;
using tdme::tools::shared::views::PrototypeBaseView;

PrototypeBaseView::PrototypeBaseView(PrototypeBaseSubScreenController* prototypeBaseSubScreenController)
{
	this->prototypeBaseSubScreenController = prototypeBaseSubScreenController;
}

void PrototypeBaseView::initialize()
{
	prototypeBaseSubScreenController->setPrototypePresetIds(ScenePropertyPresets::getInstance()->getEntityPropertiesPresets());
}

void PrototypeBaseView::prototypePropertiesPreset(Prototype* prototype, const string& presetId)
{
	if (prototype == nullptr)
		return;

	prototype->clearProperties();
	auto& entityPropertiesPresets = ScenePropertyPresets::getInstance()->getEntityPropertiesPresets();
	const vector<PrototypeProperty*>* entityPropertyPresetArrayList = nullptr;
	auto entityPropertyPresetArrayListIt = entityPropertiesPresets.find(presetId);
	if (entityPropertyPresetArrayListIt != entityPropertiesPresets.end()) {
		entityPropertyPresetArrayList = &entityPropertyPresetArrayListIt->second;
	}
	if (entityPropertyPresetArrayList != nullptr) {
		for (auto entityPropertyPreset: *entityPropertyPresetArrayList) {
			prototype->addProperty(entityPropertyPreset->getName(), entityPropertyPreset->getValue());
		}
	}
	prototypeBaseSubScreenController->setPrototypeProperties(prototype, presetId, "");
}

bool PrototypeBaseView::prototypePropertySave(Prototype* prototype, const string& oldName, const string& name, const string& value)
{
	if (prototype == nullptr)
		return false;

	if (prototype->updateProperty(oldName, name, value) == true) {
		prototypeBaseSubScreenController->setPrototypeProperties(prototype, "", name);
		return true;
	}
	return false;
}

bool PrototypeBaseView::prototypePropertyAdd(Prototype* prototype)
{
	if (prototype == nullptr)
		return false;

	if (prototype->addProperty("new.property", "new.value")) {
		prototypeBaseSubScreenController->setPrototypeProperties(prototype, "", "new.property");
		return true;
	}
	return false;
}

bool PrototypeBaseView::prototypePropertyRemove(Prototype* prototype, const string& name)
{
	if (prototype == nullptr)
		return false;

	auto idx = prototype->getPropertyIndex(name);
	if (idx != -1 && prototype->removeProperty(name) == true) {
		auto property = prototype->getPropertyByIndex(idx);
		if (property == nullptr) {
			property = prototype->getPropertyByIndex(idx - 1);
		}
		prototypeBaseSubScreenController->setPrototypeProperties(prototype, "", property == nullptr ? "" : property->getName());
		return true;
	}
	return false;
}

void PrototypeBaseView::setPrototypeData(Prototype* prototype, const string& name, const string& description)
{
	if (prototype == nullptr)
		return;

	prototype->setName(name);
	prototype->setDescription(description);
}
