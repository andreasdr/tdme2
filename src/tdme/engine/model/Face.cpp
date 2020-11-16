#include <tdme/engine/model/Face.h>

#include <tdme/engine/model/Node.h>

using tdme::engine::model::Face;
using tdme::engine::model::Node;

Face::Face()
{
}

Face::Face(Node* node, int32_t vi0, int32_t vi1, int32_t vi2, int32_t ni0, int32_t ni1, int32_t ni2)
{
	this->node = node;
	vertexIndices[0] = vi0;
	vertexIndices[1] = vi1;
	vertexIndices[2] = vi2;
	normalIndices[0] = ni0;
	normalIndices[1] = ni1;
	normalIndices[2] = ni2;
}

Face::Face(Node* node, int32_t vi0, int32_t vi1, int32_t vi2, int32_t ni0, int32_t ni1, int32_t ni2, int32_t vt0, int32_t vt1, int32_t vt2)
{
	this->node = node;
	vertexIndices[0] = vi0;
	vertexIndices[1] = vi1;
	vertexIndices[2] = vi2;
	normalIndices[0] = ni0;
	normalIndices[1] = ni1;
	normalIndices[2] = ni2;
	textureCoordinateIndices[0] = vt0;
	textureCoordinateIndices[1] = vt1;
	textureCoordinateIndices[2] = vt2;
}

void Face::setNormalIndices(int32_t ni0, int32_t ni1, int32_t ni2)
{
	normalIndices[0] = ni0;
	normalIndices[1] = ni1;
	normalIndices[2] = ni2;
}

void Face::setTextureCoordinateIndices(int32_t vt0, int32_t vt1, int32_t vt2)
{
	textureCoordinateIndices[0] = vt0;
	textureCoordinateIndices[1] = vt1;
	textureCoordinateIndices[2] = vt2;
}

void Face::setTangentIndices(int32_t ti0, int32_t ti1, int32_t ti2)
{
	tangentIndices[0] = ti0;
	tangentIndices[1] = ti1;
	tangentIndices[2] = ti2;
}

void Face::setBitangentIndices(int32_t bi0, int32_t bi1, int32_t bi2)
{
	bitangentIndices[0] = bi0;
	bitangentIndices[1] = bi1;
	bitangentIndices[2] = bi2;
}

void Face::setIndexedRenderingIndices(const array<int32_t, 3>& indices)
{
	vertexIndices = indices;
	normalIndices = indices;
	textureCoordinateIndices = indices;
	tangentIndices = indices;
	bitangentIndices = indices;
}

