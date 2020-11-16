#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/model/Node.h>
#include <tdme/math/Vector3.h>

using std::vector;
using std::string;

using tdme::engine::model::Face;
using tdme::engine::model::Node;
using tdme::engine::model::Material;
using tdme::engine::model::TextureCoordinate;
using tdme::math::Vector3;

/**
 * Node faces entity
 * A node can have multiple entities containing faces and a applied material
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::model::FacesEntity final
{
private:
	string id;
	Node* node;
	Material* material;
	vector<Face> faces;
	bool textureCoordinatesAvailable;
	bool tangentBitangentAvailable;

	/**
	 * Determine features
	 */
	void determineFeatures();

public:
	/**
	 * Public constructor
	 */
	FacesEntity();

	/**
	 * Public constructor
	 * @param node node
	 * @param id id
	 */
	FacesEntity(Node* node, const string& id);

	/**
	 * @return faces entity id
	 */
	inline const string& getId() const {
		return id;
	}

	/**
	 * Set up the entity's material
	 * @param material material
	 */
	inline void setMaterial(Material* material) {
		this->material = material;
	}

	/**
	 * @return entity's material
	 */
	inline const Material* getMaterial() const {
		return material;
	}

	/**
	 * @return entity's faces
	 */
	inline const vector<Face>& getFaces() const {
		return faces;
	}

	/**
	 * Set up entity's faces
	 * @param faces faces
	 */
	void setFaces(const vector<Face>& faces);

	/**
	 * @return if texture coordinates are available for the whole entity
	 */
	inline bool isTextureCoordinatesAvailable() const {
		return textureCoordinatesAvailable == true && node->getTextureCoordinates().size() > 0;
	}

	/**
	 * @return if tangents and bitangents are available for the whole entity
	 */
	inline bool isTangentBitangentAvailable() const {
		return tangentBitangentAvailable == true && node->getTangents().size() > 0 && node->getBitangents().size() > 0;
	}

};
