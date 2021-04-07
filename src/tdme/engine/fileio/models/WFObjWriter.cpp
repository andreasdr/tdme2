#include <tdme/engine/fileio/models/WFObjWriter.h>

#include <array>
#include <string>
#include <vector>

#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/ModelTools.h>

using std::array;
using std::string;
using std::to_string;
using std::vector;

using tdme::engine::fileio::models::ModelFileIOException;
using tdme::engine::fileio::models::WFObjWriter;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::ModelTools;

WFObjWriter::WFObjWriter()
{
}

void WFObjWriter::addVertex(const Vector3& vertex) {
	vertices.push_back(vertex);
}

void WFObjWriter::addFace(vector<int> faceVertexIndices) {
	if (faceVertexIndices.size() < 3) throw ModelFileIOException("a face must at least exist of 3 vertices");
	for (auto vertexIdx: faceVertexIndices) {
		if (vertexIdx < 0 || vertexIdx >= vertices.size()) throw ModelFileIOException("face vertex index out of bounds");
	}
	for (auto i = 0; i < faceVertexIndices.size(); i++) {
		for (auto j = i + 1; j < faceVertexIndices.size(); j++) {
			if (faceVertexIndices[i] == faceVertexIndices[j]) throw ModelFileIOException("duplicate face vertex index");
		}
	}
	array<Vector3, 3> minFaceVertices { vertices[faceVertexIndices[0]], vertices[faceVertexIndices[1]], vertices[faceVertexIndices[2]]};
	auto faceNormal = ModelTools::computeNormal(minFaceVertices);
	auto faceNormalIdx = 0;
	for (auto i = 0; i < normals.size(); i++) {
		if (faceNormal.equals(normals[i]) == true) break;
		faceNormalIdx++;
	}
	if (faceNormalIdx == normals.size()) normals.push_back(faceNormal);
	vector<array<int, 2>> face;
	for (auto i = 0; i < faceVertexIndices.size(); i++) {
		face.push_back({faceVertexIndices[i], faceNormalIdx});
	}
	faces.push_back(face);
}

void WFObjWriter::write(const string& pathName, const string& fileName) {
	{
		vector<string> content;
		content.push_back("mtllib " + fileName + ".mtl");
		content.push_back("o TDME2_WFOBJ_EXPORT");
		for (auto& vertex: vertices) {
			content.push_back("v " + to_string(vertex.getX()) + " " + to_string(vertex.getY()) + " " + to_string(vertex.getZ()));
		}
		for (auto& normal: normals) {
			content.push_back("vn " + to_string(normal.getX()) + " " + to_string(normal.getY()) + " " + to_string(normal.getZ()));
		}
		content.push_back("usemtl default");
		for (auto& faceVertexIndices: faces) {
			string faceString = "f ";
			for (auto faceIndices: faceVertexIndices) {
				faceString+= to_string(faceIndices[0] + 1) +"/" + to_string(faceIndices[1] + 1) + " ";
			}
			content.push_back(faceString);
		}
		FileSystem::getInstance()->setContentFromStringArray(pathName, fileName, content);
	}
	{
		vector<string> content;
		content.push_back("newmtl default");
		content.push_back("Ka 0.2 0.2 0.2");
 		content.push_back("Kd 0.8 0.8 0.8");
		FileSystem::getInstance()->setContentFromStringArray(pathName, fileName + ".mtl", content);
	}
}

