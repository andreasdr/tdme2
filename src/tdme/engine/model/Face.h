#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::array;

using tdme::engine::model::Node;

/**
 * Represents a model face, consisting of vertex, normal, tangent and bitangent vectors, indices and texture coordinate
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::model::Face final
{
	friend class tdme::utilities::ModelTools;

private:
	Node* node { nullptr };
	array<int32_t, 3> vertexIndices {{ -1, -1, -1 }};
	array<int32_t, 3> normalIndices {{ -1, -1, -1 }};
	array<int32_t, 3> textureCoordinateIndices {{ -1, -1, -1 }};
	array<int32_t, 3> tangentIndices {{ -1, -1, -1 }};
	array<int32_t, 3> bitangentIndices {{ -1, -1, -1 }};

	/**
	 * Prepared this face for indexed rendering
	 * @param indices indices
	 */
	void setIndexedRenderingIndices(const array<int32_t, 3>& indices);

public:
	/**
	 * Public constructor
	 */
	Face();

	/**
	 * Public constructor, requires vertex, normals indices
	 * we only support triangulated faces
	 * @param node model
	 * @param vi0 vertex index 0
	 * @param vi1 vertex index 1
	 * @param vi2 vertex index 2
	 * @param ni0 normal index 0
	 * @param ni1 normal index 1
	 * @param ni2 normal index 2
	 */
	Face(Node* node, int32_t vi0, int32_t vi1, int32_t vi2, int32_t ni0, int32_t ni1, int32_t ni2);

	/**
	 * Public constructor, requires vertex, normals indices, texture coordinate indices
	 * we only support triangulated faces
	 * @param node model
	 * @param vi0 vertex index 0
	 * @param vi1 vertex index 1
	 * @param vi2 vertex index 2
	 * @param ni0 normal index 0
	 * @param ni1 normal index 1
	 * @param ni2 normal index 2
	 * @param vt0 texture coordinate index 0
	 * @param vt1 texture coordinate index 1
	 * @param vt2 texture coordinate index 2
	 */
	Face(Node* node, int32_t vi0, int32_t vi1, int32_t vi2, int32_t ni0, int32_t ni1, int32_t ni2, int32_t vt0, int32_t vt1, int32_t vt2);

	/**
	 * @return node
	 */
	inline Node* getNode() const {
		return node;
	}

	/**
	 * @return vertex indices
	 */
	inline const array<int32_t, 3>& getVertexIndices() const {
		return vertexIndices;
	}

	/**
	 * @return normal indices
	 */
	inline const array<int32_t, 3>& getNormalIndices() const {
		return normalIndices;
	}

	/**
	 * Set normal indices
	 * @param ni0 ni0
	 * @param ni1 ni1
	 * @param ni2 ni2
	 */
	void setNormalIndices(int32_t ni0, int32_t ni1, int32_t ni2);

	/**
	 * Set up optional texture coordinate indices
	 * @param vt0 vtX
	 * @param vt1 vtY
	 * @param vt2 vtZ
	 */
	void setTextureCoordinateIndices(int32_t vt0, int32_t vt1, int32_t vt2);

	/**
	 * @return texture coordinate indices or null (optional)
	 */
	inline const array<int32_t, 3>& getTextureCoordinateIndices() const {
		return textureCoordinateIndices;
	}

	/**
	 * Set tangent indices
	 * @param ti0 ti0
	 * @param ti1 ti1
	 * @param ti2 ti2
	 */
	void setTangentIndices(int32_t ti0, int32_t ti1, int32_t ti2);

	/**
	 * @return tangent indices
	 */
	inline const array<int32_t, 3>& getTangentIndices() const {
		return tangentIndices;
	}

	/**
	 * Set bitangent indices
	 * @param bi0 bi0
	 * @param bi1 bi1
	 * @param bi2 bi2
	 */
	void setBitangentIndices(int32_t bi0, int32_t bi1, int32_t bi2);

	/**
	 * @return bi tangent indices
	 */
	inline const array<int32_t, 3>& getBitangentIndices() const {
		return bitangentIndices;
	}

};
