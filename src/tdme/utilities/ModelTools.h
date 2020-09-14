#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Group.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/tools/shared/files/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::array;
using std::map;
using std::vector;
using std::string;

using tdme::engine::Transformations;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::model::Group;
using tdme::engine::model::Model;
using tdme::engine::model::Skinning;
using tdme::math::Vector3;
using tdme::tools::shared::files::ProgressCallback;

/** 
 * Model tools functions class
 * @author Andreas Drewke
 */
class tdme::utilities::ModelTools final
{

public:
	enum VertexOrder { VERTEXORDER_CLOCKWISE, VERTEXORDER_COUNTERCLOCKWISE };

	/** 
	 * Determines vertex order of face
	 * @param vertices vertices
	 * @return vertex order
	 */
	static VertexOrder determineVertexOrder(const vector<Vector3>& vertices);

	/** 
	 * Computes face normal for given face vertices
	 * @param vertices face vertices
	 * @param normal face normal
	 * @return computed face normal
	 */
	static void computeNormal(const array<Vector3,3>& vertices, Vector3& normal) {
		// face normal
		Vector3::computeCrossProduct(
			(vertices)[1].clone().sub((vertices)[0]),
			(vertices)[2].clone().sub((vertices)[0]),
			normal
		);
		// check if zero?
		if (normal.computeLengthSquared() < Math::EPSILON * Math::EPSILON) {
			// take up vector
			normal.set(0.0f, 1.0f, 0.0f);
		} else {
			// otherwise normalize
			normal.normalize();
		}
	}

	/** 
	 * Computes face normals for given face vertices
	 * these normals will not be smooth
	 * @param vertices face vertices
	 * @param normals computed face normals
	 */
	static void computeNormals(const array<Vector3,3>& vertices, array<Vector3,3>& normals) {
		// face normal
		Vector3 normal;
		computeNormal(vertices, normal);

		// compute vertex normal
		for (auto i = 0; i < vertices.size(); i++) {
			normals[i].set(normal);
		}
	}

	/** 
	 * Prepare for indexed rendering
	 * @param model model
	 */
	static void prepareForIndexedRendering(Model* model);

private:

	/** 
	 * Prepares this group for indexed rendering
	 * @param groups groups
	 */
	static void prepareForIndexedRendering(const map<string, Group*>& groups);

	/** 
	 * Maps original vertices to new vertice mapping
	 * @param skinning skinning
	 * @param vertexMapping vertice mapping / new vertex index to old vertex index
	 * @param vertices vertice count
	 */
	static void prepareForIndexedRendering(Skinning* skinning, const vector<int32_t>& vertexMapping, int32_t vertices);

public:

	/** 
	 * Set up joints for skinning groups
	 * @param model model
	 */
	static void setupJoints(Model* model);

private:

	/** 
	 * Sets up a group as joint taking all subgroups into account
	 * @param root group
	 */
	static void setJoint(Group* root);

public:

	/** 
	 * Fix animation length
	 * @param model model
	 */
	static void fixAnimationLength(Model* model);

private:

	/** 
	 * Fixes animation length as sometimes they are only given partially, which is not supported by engine
	 * @param root group
	 * @param frames frames
	 */
	static void fixAnimationLength(Group* root, int32_t frames);

public:

	/** 
	 * Check default animation
	 * @param model model
	 * @return if animation exists
	 */
	static bool hasDefaultAnimation(Model* model);

	/**
	 * Create default animation
	 * @param model model
	 * @param frames frames
	 */
	static void createDefaultAnimation(Model* model, int32_t frames);

	/**
	 * Clone material
	 * @param material material
	 * @param id new id to use
	 * @return material
	 */
	static Material* cloneMaterial(const Material* material, const string& id = string());

	/**
	 * Create model from source sub groups into target sub groups
	 * @param sourceGroup source group
	 * @param targetModel target model
	 * @param targetParentGroup target parent group
	 * @param cloneMesh clone mesh
	 */
	static void cloneGroup(Group* sourceGroup, Model* targetModel, Group* targetParentGroup = nullptr, bool cloneMesh = true);

private:

	/**
	 * Partition sub groups
	 * @param sourceGroup source group to partition
	 * @param modelsByPartition models by partition
	 * @param modelsPosition models position
	 * @param parentTransformationsMatrix parent transformations matrix
	 */
	static void partitionGroup(Group* sourceGroup, map<string, Model*>& modelsByPartition, map<string, Vector3>& modelsPosition, const Matrix4x4& parentTransformationsMatrix);

	/**
	 * Shrink to fit group
	 * @param group group
	 */
	static void shrinkToFit(Group* group);

	/**
	 * Find all faces that include vertex and compute the avarage normal
	 * @param group group
	 * @param vertex vertex
	 * @param normals normals
	 * @param normal normal
	 */
	inline static bool interpolateNormal(Group* group, const Vector3& vertex, const vector<Vector3>& normals, Vector3& normal) {
		array<Vector3, 3> vertices;
		auto normalCount = 0;
		normal.set(0.0f, 0.0f, 0.0f);
		for (auto& facesEntity: group->getFacesEntities()) {
			for (auto& face: facesEntity.getFaces()) {
				for (auto i = 0; i < vertices.size(); i++) {
					if (vertex.equals(group->getVertices()[face.getVertexIndices()[i]]) == true) {
						normal.add(normals[face.getNormalIndices()[0]]);
						normalCount++;
						break;
					}
				}
				if (normalCount == 6) break;
			}
			if (normalCount == 6) break;
		}
		if (normalCount > 1) {
			normal.normalize();
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Compute normals
	 * @param group group
	 */
	static float computeNormals(Group* group, ProgressCallback* progressCallback = nullptr, float incrementPerFace = 0.0f, float progress = 0.0f);

	/**
	 * Compute face count
	 * @param group group
	 */
	static int determineFaceCount(Group* group);

public:
	/**
	 * Partition model
	 * @param model model
	 * @param transformations transformations
	 * @param modelsByPartition models by partition
	 * @param modelsPosition models position
	 */
	static void partition(Model* model, const Transformations& transformations, map<string, Model*>& modelsByPartition, map<string, Vector3>& modelsPosition);

	/**
	 * Shrink to fit
	 * @param model model
	 */
	static void shrinkToFit(Model* model);

	/**
	 * Compute normals
	 * @param group group
	 */
	static void computeNormals(Model* model, ProgressCallback* progressCallback = nullptr);

	/**
	 * Prepare model for foliage shader
	 * @param model model
	 * @param shader shader
	 */
	static void prepareForShader(Model* model, const string& shader);

	/**
	 * Optimizes model in terms of material / group reduction
	 * @param model model
	 * @param texturePathName texturePathName
	 * @param excludeDiffuseTextureFileNamePatterns exclude diffuse texture file name patterns
	 */
	static Model* optimizeModel(Model* model, const string& texturePathName = string(), const vector<string>& excludeDiffuseTextureFileNamePatterns = vector<string>());
private:

	/**
	 * Prepare group for default shader
	 * @param group group
	 */
	static void prepareForDefaultShader(Group* group);

	/**
	 * Prepare group for foliage shader
	 * @param group group
	 * @param parentTransformationsMatrix parent transformations matrix
	 * @param shader shader
	 */
	static void prepareForFoliageTreeShader(Group* group, const Matrix4x4& parentTransformationsMatrix, const string& shader);

	/**
	 * Check for optimization
	 * @param group group
	 * @param materialUseCount material use count
	 * @param excludeDiffuseTextureFileNamePatterns exclude diffuse texture file name patterns
	 */
	static void checkForOptimization(Group* group, map<string, int>& materialUseCount, const vector<string>& excludeDiffuseTextureFileNamePatterns);

	/**
	 * Prepare for optimization
	 * @param group group
	 * @param parentTransformationsMatrix parent transformations matrix
	 * @param materialUseCount material use count
	 */
	static void prepareForOptimization(Group* group, const Matrix4x4& parentTransformationsMatrix);

	/**
	 * Create atlas texture
	 * @param id id
	 * @param textureAtlasTextures texture atlas textures
	 */
	static Texture* createAtlasTexture(const string& id, map<int, Texture*>& textureAtlasTextures);

	/**
	 * Prepare for optimization
	 * @param sourceGroup source group
	 * @param targetModel target model
	 * @param diffuseTextureAtlasSize diffuse texture atlas size
	 * @param diffuseTextureAtlasIndices diffuse texture atlas indices
	 * @param excludeDiffuseTextureFileNamePatterns exclude diffuse texture file name patterns
	 */
	static void optimizeGroup(Group* sourceGroup, Model* targetModel, int diffuseTextureAtlasSize, const map<string, int>& diffuseTextureAtlasIndices, const vector<string>& excludeDiffuseTextureFileNamePatterns);

};
