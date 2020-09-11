#pragma once

#include <map>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::string;

using std::string;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Group;
using tdme::engine::model::UpVector;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::ShaderModel;
using tdme::engine::primitives::BoundingBox;
using tdme::math::Matrix4x4;

/**
 * Representation of a 3d model
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::model::Model final
{
	friend class Group;

public:
	enum AuthoringTool {AUTHORINGTOOL_UNKNOWN, AUTHORINGTOOL_BLENDER};

	static string ANIMATIONSETUP_DEFAULT;
	static constexpr float FPS_DEFAULT { 30.0f };

private:
	AuthoringTool authoringTool;
	string id;
	string name;
	UpVector* upVector;
	RotationOrder* rotationOrder;
	ShaderModel* shaderModel;
	map<string, Material*> materials;
	map<string, Group*> groups;
	map<string, Group*> subGroups;
	bool skinning;
	float fps;
	map<string, AnimationSetup*> animationSetups;
	Matrix4x4 importTransformationsMatrix;
	BoundingBox* boundingBox;

	/**
	 * Delete sub groups
	 * @param subGroups sub groups
	 */
	void deleteSubGroups(const map<string, Group*>& subGroups);

	/**
	 * Set up if model has skinning
	 * @param hasSkinning has skinning
	 */
	inline void setHasSkinning(bool hasSkinning) {
		skinning = hasSkinning;
	}

	/**
	 * Computes a transformations matrix at a given frame for a given group id recursivly
	 * @param groups groups
	 * @param parentTransformationsMatrix parent transformations matrix
	 * @param frame frame
	 * @param groupId group id
	 * @param transformationsMatrix transformations matrix
	 * @return target group transformations
	 */
	bool computeTransformationsMatrix(const map<string, Group*>& groups, const Matrix4x4& parentTransformationsMatrix, int32_t frame, const string& groupId, Matrix4x4& transformationsMatrix);

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param name name
	 * @param upVector up vector
	 * @param rotationOrder rotation order
	 * @param boundingBox bounding box
	 * @param authoringTool authoring tool
	 */
	Model(const string& id, const string& name, UpVector* upVector, RotationOrder* rotationOrder, BoundingBox* boundingBox, AuthoringTool authoringTool = AUTHORINGTOOL_UNKNOWN);

	/**
	 * Deconstructor
	 */
	~Model();

	/**
	 * @return authoring tool
	 */
	inline AuthoringTool getAuthoringTool() {
		return authoringTool;
	}

	/**
	 * @return model id
	 */
	inline const string& getId() {
		return id;
	}

	/**
	 * @return model name
	 */
	inline const string& getName() {
		return name;
	}

	/**
	 * @return up vector
	 */
	inline UpVector* getUpVector() {
		return upVector;
	}

	/**
	 * Set up vector
	 * @param upVector up vector
	 */
	inline void setUpVector(UpVector* upVector) {
		this->upVector = upVector;
	}

	/**
	 * @return rotation order
	 */
	inline RotationOrder* getRotationOrder() {
		return rotationOrder;
	}

	/**
	 * @return preferred shader model
	 */
	inline ShaderModel* getShaderModel() {
		return shaderModel;
	}

	/**
	 * Set preferred shader model
	 * @param shaderModel preferred shader model
	 */
	inline void setShaderModel(ShaderModel* shaderModel) {
		this->shaderModel = shaderModel;
	}

	/**
	 * Returns all object materials
	 * @return materials
	 */
	inline map<string, Material*>& getMaterials() {
		return materials;
	}

	/**
	 * Returns all object's groups
	 * @return all groups
	 */
	inline map<string, Group*>& getGroups() {
		return groups;
	}

	/**
	 * Returns a group by given name or null
	 * @param id id
	 * @return
	 */
	Group* getGroupById(const string& id);

	/**
	 * Returns object's sub groups
	 * @return sub groups
	 */
	inline map<string, Group*>& getSubGroups() {
		return subGroups;
	}

	/**
	 * Returns a sub group by given name or null
	 * @param id id
	 * @return
	 */
	Group* getSubGroupById(const string& id);

	/**
	 * @return has skinning
	 */
	inline bool hasSkinning() {
		return skinning;
	}

	/**
	 * @return frames per seconds
	 */
	inline float getFPS() {
		return fps;
	}

	/**
	 * Set model animation frames per seconds
	 * @param fps fps
	 */
	inline void setFPS(float fps) {
		this->fps = fps;
	}

	/**
	 * Adds an base animation setup
	 * @param id id
	 * @param startFrame start frame
	 * @param endFrame end frame
	 * @param loop loop
	 * @param speed speed whereas 1.0 is default speed
	 * @return animation setup
	 */
	AnimationSetup* addAnimationSetup(const string& id, int32_t startFrame, int32_t endFrame, bool loop, float speed = 1.0f);

	/**
	 * Adds an overlay animation setup
	 * @param id id
	 * @param overlayFromGroupId overlay from group id
	 * @param startFrame start frame
	 * @param endFrame end frame
	 * @param loop loop
	 * @param speed speed whereas 1.0 is default speed
	 * @return animation setup
	 */
	AnimationSetup* addOverlayAnimationSetup(const string& id, const string& overlayFromGroupId, int32_t startFrame, int32_t endFrame, bool loop, float speed = 1.0f);

	/**
	 * @return animation setup for given id or null
	 */
	AnimationSetup* getAnimationSetup(const string& id);

	/**
	 * @return animation setup for given id or null
	 */
	inline map<string, AnimationSetup*>& getAnimationSetups() {
		return animationSetups;
	}

	/**
	 * @return if model has animations
	 */
	inline bool hasAnimations() {
		return
			animationSetups.size() == 0 ||
			(getAnimationSetup(ANIMATIONSETUP_DEFAULT) != nullptr &&
			getAnimationSetup(ANIMATIONSETUP_DEFAULT)->getFrames() > 0);
	}

	/**
	 * @return import transformations matrix like converting Z-UP to Y-UP
	 */
	inline const Matrix4x4& getImportTransformationsMatrix() {
		return importTransformationsMatrix;
	}

	/**
	 * Set import transformations matrix
	 * @param importTransformationsMatrix import transformations matrix like converting Z-UP to Y-UP
	 */
	void setImportTransformationsMatrix(const Matrix4x4& importTransformationsMatrix) {
		this->importTransformationsMatrix = importTransformationsMatrix;
	}

	/**
	 * @return bounding box
	 */
	BoundingBox* getBoundingBox();

	/**
	 * Computes a transformations matrix at a given frame for a given group id recursivly
	 * @param groupId group id
	 * @param parentTransformationsMatrix parent transformations matrix
	 * @param transformationsMatrix target group transformations matrix
	 * @param frame frame
	 */
	inline bool computeTransformationsMatrix(const string& groupId, const Matrix4x4& parentTransformationsMatrix, Matrix4x4& transformationsMatrix, int32_t frame = 0) {
		return computeTransformationsMatrix(subGroups, parentTransformationsMatrix, frame, groupId, transformationsMatrix);
	}

	/**
	 * Computes a transformations matrix at a given frame for a given group id recursivly
	 * @param groupId group id
	 * @param transformationsMatrix target group transformations matrix
	 * @param frame frame
	 */
	inline bool computeTransformationsMatrix(const string& groupId, Matrix4x4& transformationsMatrix, int32_t frame = 0) {
		return computeTransformationsMatrix(subGroups, importTransformationsMatrix, frame, groupId, transformationsMatrix);
	}

};
