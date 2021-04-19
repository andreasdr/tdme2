#include <tdme/engine/model/Node.h>

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
using std::string;
using std::vector;

using tdme::engine::model::Animation;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::Skinning;
using tdme::engine::model::TextureCoordinate;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

Node::Node(Model* model, Node* parentNode, const string& id, const string& name)
{
	this->model = model;
	this->parentNode = parentNode;
	this->id = id;
	this->name = name;
	this->transformationsMatrix.identity();
	this->animation = nullptr;
	this->skinning = nullptr;
	this->joint = false;
	this->verticesUpdated = false;
	this->normalsUpdated = false;
}

Node::~Node() {
	if (animation != nullptr) delete animation;
	if (skinning != nullptr) delete skinning;
}

void Node::setVertices(const vector<Vector3>& vertices)
{
	this->vertices.resize(vertices.size());
	auto i = 0;
	for (auto& vertex: vertices) {
		this->vertices[i++] = vertex;
	}
	this->verticesUpdated = true;
}

void Node::setNormals(const vector<Vector3>& normals)
{
	this->normals.resize(normals.size());
	auto i = 0;
	for (auto& normal: normals) {
		this->normals[i++] = normal;
	}
	this->normalsUpdated = true;
}

void Node::setTextureCoordinates(const vector<TextureCoordinate>& textureCoordinates)
{
	this->textureCoordinates.resize(textureCoordinates.size());
	auto i = 0;
	for (auto& textureCoordinate: textureCoordinates) {
		this->textureCoordinates[i++] = textureCoordinate;
	}
}

void Node::setTangents(const vector<Vector3>& tangents)
{
	this->tangents.resize(tangents.size());
	auto i = 0;
	for (auto& tangent: tangents) {
		this->tangents[i++] = tangent;
	}
}

void Node::setBitangents(const vector<Vector3>& bitangents)
{
	this->bitangents.resize(bitangents.size());
	auto i = 0;
	for (auto& bitangent: bitangents) {
		this->bitangents[i++] = bitangent;
	}
}

void Node::setAnimation(Animation* animation) {
	if (this->animation != nullptr && this->animation != animation) delete this->animation;
	this->animation = animation;
}

void Node::setSkinning(Skinning* skinning)
{
	if (this->skinning != nullptr && this->skinning != skinning) delete this->skinning;
	if (skinning != nullptr) model->setHasSkinning(true);
	this->skinning = skinning;
}

int32_t Node::getFaceCount() const
{
	auto faceCount = 0;
	for (auto& facesEntity : facesEntities) {
		faceCount += facesEntity.getFaces().size();
	}
	return faceCount;
}

FacesEntity* Node::getFacesEntity(const string& id) {
	for (auto& facesEntity: facesEntities) {
		if (facesEntity.getId() == id) return &facesEntity;
	}
	return nullptr;
}

void Node::setFacesEntities(const vector<FacesEntity>& facesEntities)
{
	this->facesEntities.resize(facesEntities.size());
	auto i = 0;
	for (auto& facesEntity: facesEntities) {
		this->facesEntities[i++] = facesEntity;
	}
}

void Node::setOrigins(const vector<Vector3>& origins) {
	this->origins.resize(origins.size());
	auto i = 0;
	for (auto& origin: origins) {
		this->origins[i++] = origin;
	}
}

Node* Node::getSubNodeById(const string& nodeId)
{
	auto nodeIt = subNodes.find(nodeId);
	if (nodeIt != subNodes.end()) {
		return nodeIt->second;
	}
	return nullptr;
}
