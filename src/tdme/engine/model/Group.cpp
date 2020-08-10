#include <tdme/engine/model/Group.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Skinning.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using std::map;
using std::vector;
using std::string;

using tdme::engine::model::Group;
using tdme::engine::model::Animation;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Model;
using tdme::engine::model::Skinning;
using tdme::engine::model::TextureCoordinate;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

Group::Group(Model* model, Group* parentGroup, const string& id, const string& name)
{
	this->model = model;
	this->parentGroup = parentGroup;
	this->id = id;
	this->name = name;
	this->transformationsMatrix.identity();
	this->animation = nullptr;
	this->skinning = nullptr;
	this->joint = false;
}

Group::~Group() {
	if (animation != nullptr) delete animation;
	if (skinning != nullptr) delete skinning;
}

void Group::setVertices(const vector<Vector3>& vertices)
{
	this->vertices.resize(vertices.size());
	auto i = 0;
	for (auto& vertex: vertices) {
		this->vertices[i++] = vertex;
	}
}

void Group::setNormals(const vector<Vector3>& normals)
{
	this->normals.resize(normals.size());
	auto i = 0;
	for (auto& normal: normals) {
		this->normals[i++] = normal;
	}
}

void Group::setTextureCoordinates(const vector<TextureCoordinate>& textureCoordinates)
{
	this->textureCoordinates.resize(textureCoordinates.size());
	auto i = 0;
	for (auto& textureCoordinate: textureCoordinates) {
		this->textureCoordinates[i++] = textureCoordinate;
	}
}

void Group::setTangents(const vector<Vector3>& tangents)
{
	this->tangents.resize(tangents.size());
	auto i = 0;
	for (auto& tangent: tangents) {
		this->tangents[i++] = tangent;
	}
}

void Group::setBitangents(const vector<Vector3>& bitangents)
{
	this->bitangents.resize(bitangents.size());
	auto i = 0;
	for (auto& bitangent: bitangents) {
		this->bitangents[i++] = bitangent;
	}
}

void Group::setAnimation(Animation* animation) {
	if (this->animation != nullptr && this->animation != animation) delete this->animation;
	this->animation = animation;
}

void Group::setSkinning(Skinning* skinning)
{
	if (this->skinning != nullptr && this->skinning != skinning) delete this->skinning;
	if (skinning != nullptr) model->setHasSkinning(true);
	this->skinning = skinning;
}

int32_t Group::getFaceCount() const
{
	auto faceCount = 0;
	for (auto& facesEntity : facesEntities) {
		faceCount += facesEntity.getFaces().size();
	}
	return faceCount;
}

void Group::setFacesEntities(const vector<FacesEntity>& facesEntities)
{
	this->facesEntities.resize(facesEntities.size());
	auto i = 0;
	for (auto& facesEntity: facesEntities) {
		this->facesEntities[i++] = facesEntity;
	}
}

void Group::setOrigins(const vector<Vector3>& origins) {
	this->origins.resize(origins.size());
	auto i = 0;
	for (auto& origin: origins) {
		this->origins[i++] = origin;
	}
}

Group* Group::getSubGroupById(const string& groupId)
{
	auto groupIt = subGroups.find(groupId);
	if (groupIt != subGroups.end()) {
		return groupIt->second;
	}
	return nullptr;
}
