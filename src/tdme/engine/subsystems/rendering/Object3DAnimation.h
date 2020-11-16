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
using tdme::engine::model::Node;
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
	friend class tdme::engine::subsystems::rendering::Object3DNode;
	friend class tdme::engine::subsystems::rendering::Object3DInternal;
private:
	/**
	 * Determine skinned node count
	 * @param nodes nodes
	 */
	int32_t determineSkinnedNodeCount(const map<string, Node*>& nodes);

	/**
	 * Determine skinned node count
	 * @param nodes nodes
	 * @param count current count
	 */
	int32_t determineSkinnedNodeCount(const map<string, Node*>& nodes, int32_t count);

	/**
	 * Determine skinned nodes
	 * @param nodes nodes
	 * @param skinningNodes skinning nodes
	 * @param idx idx
	 */
	int32_t determineSkinnedNodes(const map<string, Node*>& nodes, vector<Node*>& skinningNodes, int32_t idx);

protected:
	struct FlattenedNode {
		const string nodeId;
		const Matrix4x4* nodeTransformationsMatrix;
		Matrix4x4* nodeOverriddenTransformationsMatrix;
		const Animation* nodeAnimation;
		AnimationState* nodeAnimationState;
		const Matrix4x4* parentTransformationsMatrix;
		Matrix4x4* transformationsMatrix;
	};
	struct NodeSkinningJoint {
		const Joint* joint;
		const Matrix4x4* nodeTransformationsMatrix;
		Matrix4x4* skinningNodeTransformationsMatrix;
	};

	Model* model;
	Engine::AnimationProcessingTarget animationProcessingTarget;
	map<string, Matrix4x4*> overriddenTransformationsMatrices;
	vector<map<string, Matrix4x4*>> transformationsMatrices;
	bool hasSkinning;
	bool hasAnimations;
	vector<map<string, Matrix4x4*>> skinningNodesMatrices;
	vector<Node*> skinningNodes;
	vector<vector<NodeSkinningJoint>> skinningNodesNodeSkinningJoints;
	vector<AnimationState> baseAnimations;
	int baseAnimationIdx;
	map<string, AnimationState*> overlayAnimationsById;
	map<string, AnimationState*> overlayAnimationsByJointId;
	vector<vector<FlattenedNode>> nodeLists;

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

	/**
	 * Creates all nodes transformation matrices
	 * @param matrices matrices
	 * @param nodeList flattened node list
	 * @param nodes nodes
	 * @param parentTransformationsMatrix parent transformations matrix
	 * @param animationState animation state
	 */
	void createTransformationsMatrices(map<string, Matrix4x4*>& matrices, vector<FlattenedNode>& nodeList, const map<string, Node*>& nodes, Matrix4x4* parentTransformationsMatrix = nullptr, AnimationState* animationState = nullptr);

	/**
	 * Update node list
	 * @param nodeList flattened node list
	 * @param nodeIdx node index
	 * @param nodes nodes
	 * @param animationState animation state
	 */
	void updateNodeList(vector<FlattenedNode>& nodeList, int& nodeIdx, const map<string, Node*>& nodes, AnimationState* animationState = nullptr);

	/**
	 * Update node list
	 * @param nodeList flattened node list
	 */
	inline void updateNodeList(vector<FlattenedNode>& nodeList) {
		auto nodeIdx = 0;
		updateNodeList(nodeList, nodeIdx, model->getSubNodes());
	}

	/**
	 * Update node lists
	 */
	inline void updateNodeLists() {
		for (auto& nodeList: nodeLists) updateNodeList(nodeList);
	}

	/**
	 * Calculates all nodes transformation matrices
	 * @param nodeList flattened node list
	 * @param parentTransformationsMatrix parent transformations matrix
	 * @param animationState animation state
	 */
	void computeTransformationsMatrices(vector<FlattenedNode>& nodeList, const Matrix4x4 parentTransformationsMatrix, AnimationState* animationState);

	/**
	 * Compute transformations for given animation state into given transformations matrices
	 * @param nodeList flattened node list
	 * @param instanceTransformationsMatrix object transformations matrix
	 * @param baseAnimation base animation
	 * @param context context
	 * @param lastFrameAtTime time of last animation computation
	 * @param currentFrameAtTime time of current animation computation
	 */
	void computeTransformations(vector<FlattenedNode>& nodeList, const Matrix4x4& instanceTransformationsMatrix, AnimationState& baseAnimation, void* context, int64_t lastFrameAtTime, int64_t currentFrameAtTime);

	/**
	 * Update skinning transformations matrices
	 */
	void updateSkinningTransformationsMatrices();

	/**
	 * Get skinning nodes matrices
	 * @param node node
	 * @return matrices
	 */
	map<string, Matrix4x4*>* getSkinningNodesMatrices(Node* node);

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
	 * Returns transformation matrix for given node
	 * @param id node id
	 * @return transformation matrix or identity matrix if not found
	 */
	const Matrix4x4 getNodeTransformationsMatrix(const string& id);

	/**
	 * Set transformation matrix for given node
	 * @param id node id
	 * @param matrix transformation matrix
	 */
	void setNodeTransformationsMatrix(const string& id, const Matrix4x4& matrix);

	/**
	 * Unset transformation matrix for given node
	 * @param id node id
	 */
	void unsetNodeTransformationsMatrix(const string& id);

	/**
	 * Pre render step, computes transformations
	 * @param context context
	 * @param objectTransformationsMatrix object transformations matrix
	 * @param lastFrameAtTime time of last animation computation
	 * @param currentFrameAtTime time of current animation computation
	 */
	void computeTransformations(void* context, const Matrix4x4& objectTransformationsMatrix, int64_t lastFrameAtTime, int64_t currentFrameAtTime);

};
