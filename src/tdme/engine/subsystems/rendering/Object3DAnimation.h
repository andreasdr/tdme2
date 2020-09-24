#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/AnimationState.h>
#include <tdme/math/fwd-tdme.h>

using std::map;
using std::vector;
using std::string;

using tdme::engine::Engine;
using tdme::engine::Transformations;
using tdme::engine::model::Animation;
using tdme::engine::model::Group;
using tdme::engine::model::Joint;
using tdme::engine::model::Model;
using tdme::engine::subsystems::rendering::AnimationState;
using tdme::math::Matrix4x4;

/** 
 * Object3D animation class
 * @author Andreas Drewke
 */
class tdme::engine::subsystems::rendering::Object3DAnimation
{
	friend class tdme::engine::subsystems::rendering::ModelUtilitiesInternal;
	friend class tdme::engine::subsystems::rendering::Object3DBase;
	friend class tdme::engine::subsystems::rendering::Object3DGroup;
	friend class tdme::engine::subsystems::rendering::Object3DInternal;
private:
	/**
	 * Determine skinned group count
	 * @param groups groups
	 */
	int32_t determineSkinnedGroupCount(const map<string, Group*>& groups);

	/**
	 * Determine skinned group count
	 * @param groups groups
	 * @param count current count
	 */
	int32_t determineSkinnedGroupCount(const map<string, Group*>& groups, int32_t count);

	/**
	 * Determine skinned groups
	 * @param groups groups
	 * @param skinningGroups skinning groups
	 * @param idx idx
	 */
	int32_t determineSkinnedGroups(const map<string, Group*>& groups, vector<Group*>& skinningGroups, int32_t idx);

protected:
	struct FlattenedGroup {
		const string groupId;
		const Matrix4x4* groupTransformationsMatrix;
		Matrix4x4* groupOverridenTransformationsMatrix;
		const Animation* groupAnimation;
		AnimationState* groupAnimationState;
		const Matrix4x4* parentTransformationsMatrix;
		Matrix4x4* transformationsMatrix;
	};
	struct GroupSkinningJoint {
		const Joint* joint;
		const Matrix4x4* groupTransformationsMatrix;
		Matrix4x4* skinningGroupTransformationsMatrix;
	};

	Model* model;
	Engine::AnimationProcessingTarget animationProcessingTarget;
	map<string, Matrix4x4*> overridenTransformationsMatrices;
	vector<map<string, Matrix4x4*>> transformationsMatrices;
	bool hasSkinning;
	bool hasAnimations;
	vector<map<string, Matrix4x4*>> skinningGroupsMatrices;
	vector<Group*> skinningGroups;
	vector<vector<GroupSkinningJoint>> skinningGroupsGroupSkinningJoints;
	vector<AnimationState> baseAnimations;
	int baseAnimationIdx;
	map<string, AnimationState*> overlayAnimationsById;
	map<string, AnimationState*> overlayAnimationsByJointId;
	vector<vector<FlattenedGroup>> groupLists;

	/**
	 * Creates all groups transformation matrices
	 * @param matrices matrices
	 * @param groupList flattened group list
	 * @param groups groups
	 * @param parentTransformationsMatrix parent transformations matrix
	 * @param animationState animation state
	 */
	void createTransformationsMatrices(map<string, Matrix4x4*>& matrices, vector<FlattenedGroup>& groupList, const map<string, Group*>& groups, Matrix4x4* parentTransformationsMatrix = nullptr, AnimationState* animationState = nullptr);

	/**
	 * Update group list
	 * @param groupList flattened group list
	 * @param groupIdx group index
	 * @param groups groups
	 * @param animationState animation state
	 */
	void updateGroupList(vector<FlattenedGroup>& groupList, int& groupIdx, const map<string, Group*>& groups, AnimationState* animationState = nullptr);

	/**
	 * Update group list
	 * @param groupList flattened group list
	 */
	inline void updateGroupList(vector<FlattenedGroup>& groupList) {
		auto groupIdx = 0;
		updateGroupList(groupList, groupIdx, model->getSubGroups());
	}

	/**
	 * Update group lists
	 */
	inline void updateGroupLists() {
		for (auto& groupList: groupLists) updateGroupList(groupList);
	}

	/**
	 * Calculates all groups transformation matrices
	 * @param groupList flattened group list
	 * @param parentTransformationsMatrix parent transformations matrix
	 * @param animationState animation state
	 */
	void computeTransformationsMatrices(vector<FlattenedGroup>& groupList, const Matrix4x4 parentTransformationsMatrix, AnimationState* animationState);

	/**
	 * Compute transformations for given animation state into given transformations matrices
	 * @param groupList flattened group list
	 * @param instanceTransformationsMatrix object transformations matrix
	 * @param baseAnimation base animation
	 * @param context context
	 * @param lastFrameAtTime time of last animation computation
	 * @param currentFrameAtTime time of current animation computation
	 */
	void computeTransformations(vector<FlattenedGroup>& groupList, const Matrix4x4& instanceTransformationsMatrix, AnimationState& baseAnimation, void* context, int64_t lastFrameAtTime, int64_t currentFrameAtTime);

	/**
	 * Update skinning transformations matrices
	 */
	void updateSkinningTransformationsMatrices();

	/**
	 * Get skinning groups matrices
	 * @param group group
	 * @return matrices
	 */
	map<string, Matrix4x4*>* getSkinningGroupsMatrices(Group* group);

	/**
	 * Public constructor
	 * @param model model
	 * @param animationProcessingTarget animation processing target
	 */
	Object3DAnimation(Model* model, Engine::AnimationProcessingTarget animationProcessingTarget);

	/**
	 * Destructor
	 */
	virtual ~Object3DAnimation();

public:

	/** 
	 * Sets up a base animation to play
	 * @param id id
	 * @param speed speed whereas 1.0 is default speed
	 */
	void setAnimation(const string& id, float speed = 1.0f);

	/**
	 * Set up animation speed
	 * @param speed speed whereas 1.0 is default speed
	 */
	void setAnimationSpeed(float speed);

	/** 
	 * Overlays a animation above the base animation
	 * @param id id
	 */
	void addOverlayAnimation(const string& id);

	/** 
	 * Removes a overlay animation
	 * @param id id
	 */
	void removeOverlayAnimation(const string& id);

	/** 
	 * Removes all finished overlay animations
	 */
	void removeOverlayAnimationsFinished();

	/** 
	 * Removes all overlay animations
	 */
	void removeOverlayAnimations();

	/** 
	 * @return active animation setup id
	 */
	const string getAnimation();

	/** 
	 * Returns current base animation time 
	 * @return 0.0 <= time <= 1.0
	 */
	float getAnimationTime();

	/** 
	 * Returns if there is currently running a overlay animation with given id
	 * @param id id
	 * @return animation is running
	 */
	bool hasOverlayAnimation(const string& id);

	/** 
	 * Returns current overlay animation time
	 * @param id id 
	 * @return 0.0 <= time <= 1.0
	 */
	float getOverlayAnimationTime(const string& id);

	/** 
	 * Returns transformation matrix for given group
	 * @param id group id
	 * @return transformation matrix or identity matrix if not found
	 */
	const Matrix4x4 getGroupTransformationsMatrix(const string& id);

	/**
	 * Set transformation matrix for given group
	 * @param id group id
	 * @param matrix transformation matrix
	 */
	void setGroupTransformationsMatrix(const string& id, const Matrix4x4& matrix);

	/**
	 * Unset transformation matrix for given group
	 * @param id group id
	 */
	void unsetGroupTransformationsMatrix(const string& id);

	/**
	 * Pre render step, computes transformations
	 * @param context context
	 * @param objectTransformationsMatrix object transformations matrix
	 * @param lastFrameAtTime time of last animation computation
	 * @param currentFrameAtTime time of current animation computation
	 */
	void computeTransformations(void* context, const Matrix4x4& objectTransformationsMatrix, int64_t lastFrameAtTime, int64_t currentFrameAtTime);

};
