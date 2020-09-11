#include <tdme/engine/model/FacesEntity.h>

#include <vector>
#include <string>

#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/math/Vector3.h>

using std::vector;
using std::string;

using tdme::engine::model::FacesEntity;
using tdme::engine::model::Face;
using tdme::engine::model::Group;
using tdme::engine::model::Material;
using tdme::engine::model::TextureCoordinate;
using tdme::math::Vector3;

FacesEntity::FacesEntity()
{
	this->id = "";
	this->group = nullptr;
	this->material = nullptr;
	this->textureCoordinatesAvailable = false;
	this->tangentBitangentAvailable = false;
}

FacesEntity::FacesEntity(Group* group, const string& id)
{
	this->id = id;
	this->group = group;
	this->material = nullptr;
	this->textureCoordinatesAvailable = false;
	this->tangentBitangentAvailable = false;
}

void FacesEntity::setFaces(const vector<Face>& faces)
{
	this->faces.clear();
	this->faces.resize(faces.size());
	int i = 0;
	for (auto& face: faces) {
		this->faces[i++] = face;
	}
	determineFeatures();
}

void FacesEntity::determineFeatures()
{
	textureCoordinatesAvailable = false;
	tangentBitangentAvailable = false;
	for (auto& face: faces) {
		auto& textureCoordinateIndices = face.getTextureCoordinateIndices();
		if (textureCoordinateIndices[0] != -1 && textureCoordinateIndices[1] != -1 && textureCoordinateIndices[2] != -1) textureCoordinatesAvailable = true;
		auto& tangentIndices = face.getTangentIndices();
		auto& biTangentIndices = face.getBitangentIndices();
		if (tangentIndices[0] != -1 && tangentIndices[1] != -1 && tangentIndices[2] != -1 &&
			biTangentIndices[0] != -1 && biTangentIndices[1] != -1 && biTangentIndices[2] != -1) tangentBitangentAvailable = true;
	}
}

