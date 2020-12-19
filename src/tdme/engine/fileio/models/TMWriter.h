#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fileio/models/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/os/filesystem/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

#include <tdme/engine/fileio/models/ModelFileIOException.h>
#include <tdme/os/filesystem/FileSystemException.h>

using std::array;
using std::map;
using std::string;
using std::vector;

using tdme::engine::fileio::models::ModelFileIOException;
using tdme::engine::model::Animation;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Joint;
using tdme::engine::model::JointWeight;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::Skinning;
using tdme::engine::model::TextureCoordinate;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystemException;

namespace tdme {
namespace engine {
namespace fileio {
namespace models {

/**
 * TM writer output stream
 * @author Andreas Drewke
 * @version $Id$
 */
class TMWriterOutputStream {
private:
	vector<uint8_t> data;
public:

	/**
	 * Constructor
	 */
	inline TMWriterOutputStream() {
	}

	/**
	 * Get data
	 * @return data
	 */
	inline vector<uint8_t>* getData() {
		return &data;
	}

	/**
	 * Writes a boolean to output stream
	 * @throws model file IO exception
	 * @param b boolean
	 */
	inline void writeBoolean(bool b) {
		writeByte(b == true?1:0);
	}

	/**
	 * Writes a byte to output stream
	 * @throws model file IO exception
	 * @param b byte
	 */
	inline void writeByte(int8_t b) {
		data.push_back(b);
	}

	/**
	 * Writes a integer to output stream
	 * @throws model file IO exception
	 * @param i int
	 */
	inline void writeInt(int32_t i) {
		writeByte((i >> 24) & 0xFF);
		writeByte((i >> 16) & 0xFF);
		writeByte((i >> 8) & 0xFF);
		writeByte((i >> 0) & 0xFF);
	}

	/**
	 * Writes a float to output stream
	 * @param f float
	 * @throws model file IO exception
	 */
	inline void writeFloat(float f) {
		int value = *((int*)&f);
		writeInt(value);
	}

	/**
	 * Writes a string to output stream
	 * @param s string
	 * @throws model file IO exception
	 */
	inline void writeString(const string& s) {
		if (s.size() == 0) {
			writeBoolean(false);
		} else {
			writeBoolean(true);
			writeInt(s.size());
			for (auto i = 0; i < s.size(); i++) {
				writeByte(static_cast< uint8_t >(s[i]));
			}
		}
	}

	/**
	 * Writes a float array to output stream
	 * @param f float array
	 * @throws model file IO exception
	 */
	inline void writeFloatArray(const array<float,2>& f) {
		writeInt(f.size());
		for (auto i = 0; i < f.size(); i++) {
			writeFloat(f[i]);
		}
	}

	/**
	 * Writes a float array to output stream
	 * @param f float array
	 * @throws model file IO exception
	 */
	inline void writeFloatArray(const array<float,3>& f) {
		writeInt(f.size());
		for (auto i = 0; i < f.size(); i++) {
			writeFloat(f[i]);
		}
	}

	/**
	 * Writes a float array to output stream
	 * @param f float array
	 * @throws model file IO exception
	 */
	inline void writeFloatArray(const array<float,4>& f) {
		writeInt(f.size());
		for (auto i = 0; i < f.size(); i++) {
			writeFloat(f[i]);
		}
	}

	/**
	 * Writes a float array to output stream
	 * @param f float array
	 * @throws model file IO exception
	 */
	inline void writeFloatArray(const array<float,9>& f) {
		writeInt(f.size());
		for (auto i = 0; i < f.size(); i++) {
			writeFloat(f[i]);
		}
	}

	/**
	 * Writes a float array to output stream
	 * @param f float array
	 * @throws model file IO exception
	 */
	inline void writeFloatArray(const array<float,16>& f) {
		writeInt(f.size());
		for (auto i = 0; i < f.size(); i++) {
			writeFloat(f[i]);
		}
	}

	/**
	 * Writes a float array to output stream
	 * @param f float array
	 * @throws model file IO exception
	 */
	inline void writeFloatArray(const vector<float>& f) {
		writeInt(f.size());
		for (auto i = 0; i < f.size(); i++) {
			writeFloat(f[i]);
		}
	}

};

};
};
};
};

/**
 * TDME model writer
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::fileio::models::TMWriter
{
public:

	/**
	 * TDME model format writer
	 * @param model model
	 * @param pathName path name
	 * @param fileName file name
	 * @throws tdme::os::filesystem::FileSystemException
	 * @throws tdme::engine::fileio::models::ModelFileIOException
	 */
	static void write(Model* model, const string& pathName, const string& fileName);

private:

	/**
	 * Write material
	 * @param os output stream
	 * @param m material
	 * @throws model file IO exception
	 */
	static void writeMaterial(TMWriterOutputStream* os, Material* m);

	/**
	 * Write animation setup
	 * @param os output stream
	 * @param animationSetup animation setup
	 * @throws model file IO exception
	 */
	static void writeAnimationSetup(TMWriterOutputStream* os, AnimationSetup* animationSetup);

	/**
	 * Write vertices to output stream
	 * @param os output stream
	 * @param v vertices
	 * @throws model file IO exception
	 */
	static void writeVertices(TMWriterOutputStream* os, const vector<Vector3>& v);

	/**
	 * Write texture coordinates to output stream
	 * @param os output stream
	 * @param tc texture coordinates
	 * @throws model file IO exception
	 */
	static void writeTextureCoordinates(TMWriterOutputStream* os, const vector<TextureCoordinate>& tc);

	/**
	 * Write indices to output stream
	 * @param os output stream
	 * @param indices indices
	 * @throws model file IO exception
	 */
	static void writeIndices(TMWriterOutputStream* os, const array<int32_t, 3>& indices);

	/**
	 * Write animation to output stream
	 * @param os output stream
	 * @param a animation
	 * @throws model file IO exception
	 */
	static void writeAnimation(TMWriterOutputStream* os, Animation* a);

	/**
	 * Write faces entities to output stream
	 * @param os output stream
	 * @param facesEntities faces entities
	 * @throws model file IO exception
	 */
	static void writeFacesEntities(TMWriterOutputStream* os, const vector<FacesEntity>& facesEntities);

	/**
	 * Write skinning joint
	 * @param os output stream
	 * @param joint joint
	 * @throws model file IO exception
	 */
	static void writeSkinningJoint(TMWriterOutputStream* os, const Joint& joint);

	/**
	 * Write skinning joint weight
	 * @param os output stream
	 * @param jointWeight joint
	 * @throws model file IO exception
	 */
	static void writeSkinningJointWeight(TMWriterOutputStream* os, const JointWeight& jointWeight);

	/**
	 * Write skinning to output stream
	 * @param os output stream
	 * @param skinning skinning
	 * @throws model file IO exception
	 */
	static void writeSkinning(TMWriterOutputStream* os, Skinning* skinning);

	/**
	 * Write sub nodes
	 * @param os output stream
	 * @param subNodes sub nodes
	 * @throws model file IO exception
	 */
	static void writeSubNodes(TMWriterOutputStream* os, const map<string, Node*>& subNodes);

	/**
	 * Write node to output stream
	 * @param os output stream
	 * @param g node
	 * @throws model file IO exception
	 */
	static void writeNode(TMWriterOutputStream* os, Node* g);
};
