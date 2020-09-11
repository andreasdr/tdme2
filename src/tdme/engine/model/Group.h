#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::vector;
using std::string;

using tdme::engine::model::Animation;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Model;
using tdme::engine::model::Skinning;
using tdme::engine::model::TextureCoordinate;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Model group
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::model::Group final
{
private:
	Model* model;
	Group* parentGroup;
	string id;
	string name;
	bool joint;
	Matrix4x4 transformationsMatrix;
	vector<Vector3> vertices;
	vector<Vector3> normals;
	vector<TextureCoordinate> textureCoordinates;
	vector<Vector3> tangents;
	vector<Vector3> bitangents;
	Animation* animation;
	Skinning* skinning;
	vector<FacesEntity> facesEntities;
	vector<Vector3> origins;
	map<string, Group*> subGroups;
public:
	/**
	 * Public constructor
	 * @param model model
	 * @param parentGroup parent group
	 * @param id id
	 * @param name name
	 */
	Group(Model* model, Group* parentGroup, const string& id, const string& name);

	/**
	 * Destructor
	 */
	~Group();

	/**
	 * @return model
	 */
	inline Model* getModel() {
		return model;
	}

	/**
	 * @return parent group
	 */
	inline Group* getParentGroup() {
		return parentGroup;
	}

	/**
	 * Returns id
	 * @return id
	 */
	inline const string& getId() {
		return id;
	}

	/**
	 * @return group's name
	 */
	inline const string& getName() {
		return name;
	}

	/**
	 * @return if this group is a joint/bone
	 */
	inline bool isJoint() const {
		return joint;
	}

	/**
	 * Sets up if this group is a joint or not 
	 * @param isJoint isbone
	 */
	inline void setJoint(bool isJoint) {
		joint = isJoint;
	}

	/**
	 * @return transformations matrix related to parent group
	 */
	inline const Matrix4x4& getTransformationsMatrix() const {
		return transformationsMatrix;
	}

	/**
	 * @return transformations matrix related to parent group
	 */
	inline void setTransformationsMatrix(const Matrix4x4& transformationsMatrix) {
		this->transformationsMatrix = transformationsMatrix;
	}

	/**
	 * @return vertices
	 */
	inline const vector<Vector3>& getVertices() const {
		return vertices;
	}

	/**
	 * Set vertices
	 * @param vertices vertices
	 */
	void setVertices(const vector<Vector3>& vertices);

	/**
	 * @return normals
	 */
	inline const vector<Vector3>& getNormals() const {
		return normals;
	}

	/**
	 * Set normals
	 * @param normals normals
	 */
	void setNormals(const vector<Vector3>& normals);

	/**
	 * @return texture coordinates or null (optional)
	 */
	inline const vector<TextureCoordinate>& getTextureCoordinates() const {
		return textureCoordinates;
	}

	/**
	 * Set texture coordinates
	 * @param textureCoordinates texture coordinates
	 */
	void setTextureCoordinates(const vector<TextureCoordinate>& textureCoordinates);

	/**
	 * @return tangents
	 */
	inline const vector<Vector3>& getTangents() const {
		return tangents;
	}

	/**
	 * Set tangents
	 * @param tangents tangents
	 */
	void setTangents(const vector<Vector3>& tangents);

	/**
	 * @return bitangents
	 */
	inline const vector<Vector3>& getBitangents() const {
		return bitangents;
	}

	/**
	 * Set bitangents
	 * @param bitangents bitangents
	 */
	void setBitangents(const vector<Vector3>& bitangents);

	/**
	 * @return animation
	 */
	inline Animation* getAnimation() {
		return animation;
	}

	/**
	 * Sets animation object
	 * @param animation animation
	 */
	void setAnimation(Animation* animation);

	/**
	 * @return skinning or null
	 */
	inline Skinning* getSkinning() {
		return skinning;
	}

	/**
	 * Sets skinning object
	 * @param skinning skinning
	 */
	void setSkinning(Skinning* skinning);

	/**
	 * @return number of faces in group
	 */
	int32_t getFaceCount() const;

	/**
	 * @return faces entities
	 */
	inline const vector<FacesEntity>& getFacesEntities() const {
		return facesEntities;
	}

	/**
	 * Find faces entity by id
	 * @param id id
	 * @return faces entity
	 */
	FacesEntity* getFacesEntity(const string& id);

	/**
	 * Set up faces entities
	 * @param facesEntities faces entity
	 */
	void setFacesEntities(const vector<FacesEntity>& facesEntities);

	/**
	 * @return origins
	 */
	const vector<Vector3>& getOrigins() const {
		return origins;
	}

	/**
	 * Set origins
	 * @param origins render group object origins
	 */
	void setOrigins(const vector<Vector3>& origins);

	/**
	 * @return sub sub groups of this group
	 */
	inline map<string, Group*>& getSubGroups() {
		return subGroups;
	}

	/**
	 * Returns a sub group by id
	 * @param groupId groupId
	 * @return sub group or null
	 */
	Group* getSubGroupById(const string& groupId);
};
