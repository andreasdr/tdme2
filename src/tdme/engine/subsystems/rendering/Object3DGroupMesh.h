#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/skinning/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::vector;
using std::string;

using tdme::utilities::FloatBuffer;
using tdme::utilities::ShortBuffer;
using tdme::engine::Engine;
using tdme::engine::model::Group;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DGroupRenderer;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Object 3D group mesh specifically for rendering
 * @author Andreas Drewke
 */
class tdme::engine::subsystems::rendering::Object3DGroupMesh final
{
	friend class ModelUtilitiesInternal;
	friend class Object3DBase;
	friend class Object3DBase_TransformedFacesIterator;
	friend class Object3DGroup;
	friend class Object3DGroupRenderer;
	friend class EntityRenderer;
	friend class TransparentRenderFacesPool;
	friend class tdme::engine::subsystems::skinning::SkinningShader;

private:
	int instances;
	Object3DGroupRenderer* object3DGroupRenderer;
	Group* group;
	int32_t faceCount;
	const vector<Vector3>* vertices;
	const vector<Vector3>* normals;
	const vector<Vector3>* tangents;
	const vector<Vector3>* bitangents;
	const vector<TextureCoordinate>* textureCoordinates;
	vector<int32_t> indices;
	vector<Vector3> transformedVertices;
	vector<Vector3> transformedNormals;
	vector<Vector3> transformedTangents;
	vector<Vector3> transformedBitangents;
	vector<TextureCoordinate> transformedTextureCoordinates;
	vector<map<string, Matrix4x4*>*> skinningMatrices;
	Engine::AnimationProcessingTarget animationProcessingTarget;

	int32_t cSkinningMaxVertexWeights;
	vector<vector<float>> cSkinningJointWeight;

	Matrix4x4* cGroupTransformationsMatrix;

	vector<vector<vector<Matrix4x4*>>> cSkinningJointTransformationsMatrices;

	bool skinning;
	int32_t skinningJoints;

	bool recreatedBuffers;

	/**
	 * Public constructor
	 */
	Object3DGroupMesh();

	/** 
	 * Creates a object3d group mesh from group
	 * @param object3DGroupRenderer object 3D group renderer
	 * @param animationProcessingTarget animation processing target
	 * @param group group
	 * @param transformationMatrices instances transformationm matrices
	 * @param skinningMatrices instances skinning matrices
	 * @param instances instances
	 * @return object 3d group mesh
	 */
	static Object3DGroupMesh* createMesh(Object3DGroupRenderer* object3DGroupRenderer, Engine::AnimationProcessingTarget animationProcessingTarget, Group* group, const vector<map<string, Matrix4x4*>*>& transformationMatrices, const vector<map<string, Matrix4x4*>*>& skinningMatrices, int instances);

	/** 
	 * Computes mesh transformations
	 * @param context context
	 * @param object3DBase object 3d base
	 */
	void computeTransformations(void* context, Object3DBase* object3DBase);

	/** 
	 * Recreates group float buffers
	 */
	void recreateBuffers();

	/**
	 * @return has recreated buffers
	 */
	bool hasRecreatedBuffers() {
		return recreatedBuffers == true;
	}

	/** 
	 * @return if buffers has been recreated and unsets state
	 */
	bool getRecreatedBuffers();

	/** 
	 * Set up vertex indices buffer
	 * @param renderer renderer
	 * @param context context
	 * @param vboId vbo id
	 */
	void setupVertexIndicesBuffer(Renderer* renderer, void* context, int32_t vboId);

	/** 
	 * Set up texture coordinates buffer
	 * @param renderer renderer
	 * @param context context
	 * @param vboId vbo id
	 */
	void setupTextureCoordinatesBuffer(Renderer* renderer, void* context, int32_t vboId);

	/** 
	 * Set up vertices buffer
	 * @param renderer renderer
	 * @param context context
	 * @param vboId vbo id
	 */
	void setupVerticesBuffer(Renderer* renderer, void* context, int32_t vboId);

	/** 
	 * Set up normals buffer
	 * @param renderer renderer
	 * @param context context
	 * @param vboId vbo id
	 */
	void setupNormalsBuffer(Renderer* renderer, void* context, int32_t vboId);

	/** 
	 * Set up tangents buffer
	 * @param renderer renderer
	 * @param context context
	 * @param vboId vbo id
	 */
	void setupTangentsBuffer(Renderer* renderer, void* context, int32_t vboId);

	/** 
	 * Set up bitangents buffer
	 * @param renderer renderer
	 * @param context context
	 * @param vboId vbo id
	 */
	void setupBitangentsBuffer(Renderer* renderer, void* context, int32_t vboId);

	/**
	 * Set up render group object origins data buffer
	 * @param renderer renderer
	 * @param context context
	 * @param vboId vbo id
	 */
	void setupOriginsBuffer(Renderer* renderer, void* context, int32_t vboId);

};
