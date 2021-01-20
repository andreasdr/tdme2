#include <tdme/engine/scene/SceneLibrary.h>

#include <algorithm>
#include <string>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/prototypes/PrototypeReader.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/Prototype_Type.h>
#include <tdme/engine/prototype/PrototypeBoundingVolume.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/scene/SceneLibrary.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/ExceptionBase.h>
#include <tdme/utilities/Primitives.h>
#include <tdme/utilities/StringTools.h>

using std::remove;
using std::string;
using std::to_string;

using tdme::engine::scene::SceneLibrary;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::prototypes::PrototypeReader;
using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::Prototype_Type;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::scene::Scene;
using tdme::math::Vector3;
using tdme::utilities::Console;
using tdme::utilities::ExceptionBase;
using tdme::utilities::Primitives;
using tdme::utilities::StringTools;

constexpr int SceneLibrary::ID_ALLOCATE;

SceneLibrary::SceneLibrary(Scene* scene)
{
	this->scene = scene;
	this->prototypeIdx = 0;
}

SceneLibrary::~SceneLibrary() {
	for (auto prototype: prototypes) {
		delete prototype;
	}
}

void SceneLibrary::clear()
{
	for (auto prototype: prototypes) {
		delete prototype;
	}
	this->prototypesById.clear();
	this->prototypes.clear();
	this->prototypeIdx = 0;
}

int SceneLibrary::allocatePrototypeId()
{
	return prototypeIdx++;
}

Prototype* SceneLibrary::addModel(int id, const string& name, const string& description, const string& pathName, const string& fileName, const Vector3& pivot)
{
	Prototype* prototype = nullptr;
	if (StringTools::endsWith(StringTools::toLowerCase(fileName), ".tmm") == true) {
		prototype = PrototypeReader::read(id == ID_ALLOCATE?allocatePrototypeId():id, pathName, fileName);
	} else {
		auto model = ModelReader::read(
			pathName,
			fileName
		);
		prototype = new Prototype(
			id == ID_ALLOCATE?allocatePrototypeId():id,
			Prototype_Type::MODEL,
			name,
			description,
			"",
			pathName + "/" + fileName,
			StringTools::replace(StringTools::replace(StringTools::replace(model->getId(), "\\", "_"), "/", "_"), ":", "_") + ".png",
			model,
			Vector3(0.0f, 0.0f, 0.0f)
		);
	}
	addPrototype(prototype);
	return prototype;
}

Prototype* SceneLibrary::addTrigger(int id, const string& name, const string& description, float width, float height, float depth)
{
	auto cacheId = "sceneeditor.trigger." + to_string(width) + "mx" + to_string(height) + "mx" + to_string(depth) + "m";
	Prototype* prototype = nullptr;
	auto boundingBox = new BoundingBox(Vector3(-width / 2.0f, 0.0f, -depth / 2.0f), Vector3(+width / 2.0f, height, +depth / 2.0f));
	auto modelId = cacheId + "_bv";
	prototype = new Prototype(
		id == ID_ALLOCATE?allocatePrototypeId():id,
		Prototype_Type::TRIGGER,
		name,
		description,
		"",
		cacheId,
		StringTools::replace(StringTools::replace(StringTools::replace(modelId, "\\", "_"), "/", "_"), ":", "_") + ".png",
		nullptr,
		Vector3()
	);
	prototype->addBoundingVolume(0, new PrototypeBoundingVolume(0, prototype));
	prototype->getBoundingVolume(0)->setupAabb(boundingBox->getMin(), boundingBox->getMax());
	addPrototype(prototype);
	return prototype;
}

Prototype* SceneLibrary::addEnvironmentMapping(int id, const string& name, const string& description, float width, float height, float depth)
{
	auto cacheId = "sceneeditor.environmentmapping." + to_string(width) + "mx" + to_string(height) + "mx" + to_string(depth) + "m";
	Prototype* prototype = nullptr;
	auto boundingBox = new BoundingBox(Vector3(-width / 2.0f, 0.0f, -depth / 2.0f), Vector3(+width / 2.0f, height, +depth / 2.0f));
	auto modelId = cacheId + "_bv";
	prototype = new Prototype(
		id == ID_ALLOCATE?allocatePrototypeId():id,
		Prototype_Type::ENVIRONMENTMAPPING,
		name,
		description,
		"",
		cacheId,
		StringTools::replace(StringTools::replace(StringTools::replace(cacheId, "\\", "_"), "/", "_"), ":", "_") + ".png",
		nullptr,
		Vector3()
	);
	prototype->addBoundingVolume(0, new PrototypeBoundingVolume(0, prototype));
	prototype->getBoundingVolume(0)->setupAabb(boundingBox->getMin(), boundingBox->getMax());
	addPrototype(prototype);
	return prototype;
}

Prototype* SceneLibrary::addEmpty(int id, const string& name, const string& description)
{
	auto cacheId = "sceneeditor.empty";
	Prototype* prototype = nullptr;
	auto model = ModelReader::read("resources/engine/tools/sceneeditor/models", "empty.dae");
	prototype = new Prototype(
		id == ID_ALLOCATE?allocatePrototypeId():id,
		Prototype_Type::EMPTY,
		name,
		description,
		"",
		cacheId,
		StringTools::replace(StringTools::replace(StringTools::replace(model->getId(), "\\", "_"), "/", "_"), ":", "_") + ".png",
		model,
		Vector3()
	);
	addPrototype(prototype);
	return prototype;
}

Prototype* SceneLibrary::addParticleSystem(int id, const string& name, const string& description)
{
	auto prototype = new Prototype(
		id == ID_ALLOCATE?allocatePrototypeId():id,
		Prototype_Type::PARTICLESYSTEM,
		name,
		description,
		"",
		"",
		"",
		nullptr,
		Vector3()
	);
	addPrototype(prototype);
	return prototype;
}

Prototype* SceneLibrary::addTerrain(int id, const string& name, const string& description)
{
	auto prototype = new Prototype(
		id == ID_ALLOCATE?allocatePrototypeId():id,
		Prototype_Type::TERRAIN,
		name,
		description,
		"",
		"",
		"",
		nullptr,
		Vector3()
	);
	addPrototype(prototype);
	return prototype;
}

void SceneLibrary::addPrototype(Prototype* prototype)
{

	auto prototypeByIdIt = prototypesById.find(prototype->getId());
	if (prototypeByIdIt != prototypesById.end()) {
		throw ExceptionBase("Prototype id already in use");
	}
	prototypes.push_back(prototype);
	prototypesById[prototype->getId()] = prototype;
	if (prototype->getId() >= prototypeIdx)
		prototypeIdx = prototype->getId() + 1;

}

Prototype* SceneLibrary::getPrototypeAt(int idx)
{
	return prototypes[idx];
}

Prototype* SceneLibrary::getPrototype(int id)
{
	auto prototypeByIdIt = prototypesById.find(id);
	if (prototypeByIdIt != prototypesById.end()) {
		return prototypeByIdIt->second;
	}
	return nullptr;
}

void SceneLibrary::removePrototype(int id)
{
	auto prototypeByIdIt = prototypesById.find(id);
	if (prototypeByIdIt != prototypesById.end()) {
		prototypes.erase(remove(prototypes.begin(), prototypes.end(), prototypeByIdIt->second), prototypes.end());
		delete prototypeByIdIt->second;
		prototypesById.erase(prototypeByIdIt);
	}
}

int SceneLibrary::getPrototypeCount()
{
	return prototypes.size();
}
