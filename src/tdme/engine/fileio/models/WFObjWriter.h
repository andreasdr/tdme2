#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/models/fwd-tdme.h>
#include <tdme/math/Vector3.h>

using std::string;
using std::vector;

using tdme::math::Vector3;

/** 
 * Wavefront object model writer
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::fileio::models::WFObjWriter final
{
private:
	vector<Vector3> vertices;
	vector<Vector3> normals;
	vector<vector<array<int, 2>>> faces;
public:
	/**
	 * Public constructor
	 */
	WFObjWriter();

	/**
	 * Adds a vertex
	 * @param vertex vertex
	 */
	void addVertex(const Vector3& vertex);

	/**
	 * Adds a face
	 * @param faceVertexIndices face vertex indices
	 */
	void addFace(vector<int> faceVertexIndices);

	/** 
	 * Writes this wave front object file
	 * @param pathName path name
	 * @param fileName file name
	 * @return model
	 * @throws tdme::os::filesystem::FileSystemException
	 */
	void write(const string& pathName, const string& fileName);

};
