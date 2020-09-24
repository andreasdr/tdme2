#include <tdme/engine/subsystems/rendering/Object3DAnimation.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/engine/Engine.h>
#include <tdme/engine/Timing.h>
#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/model/Joint.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Skinning.h>
#include <tdme/engine/subsystems/rendering/AnimationState.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>

using std::map;
using std::vector;
using std::string;
using std::to_string;

using tdme::engine::subsystems::rendering::Object3DAnimation;
using tdme::engine::Engine;
using tdme::engine::Timing;
using tdme::engine::model::Animation;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Joint;
using tdme::engine::model::Model;
using tdme::engine::model::Skinning;
using tdme::engine::subsystems::rendering::AnimationState;
using tdme::utilities::Console;
using tdme::math::Math;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

Object3DAnimation::Object3DAnimation(Model* model, Engine::AnimationProcessingTarget animationProcessingTarget)
{
	this->animationProcessingTarget = animationProcessingTarget;
	this->model = model;
	// skinning
	hasSkinning = false;
	if (model->hasSkinning() == true) {
		hasSkinning = true;
		skinningGroups.resize(determineSkinnedGroupCount(model->getSubGroups()));
		determineSkinnedGroups(model->getSubGroups(), skinningGroups, 0);
		skinningGroupsMatrices.resize(skinningGroups.size());
		for (auto i = 0; i < skinningGroups.size(); i++) {
			vector<FlattenedGroup> groupListIgnored;
			createTransformationsMatrices(skinningGroupsMatrices[i], groupListIgnored, model->getSubGroups());
		}
	}
	hasAnimations = model->hasAnimations();
	//
	baseAnimationIdx = 0;
	// animation
	setAnimation(Model::ANIMATIONSETUP_DEFAULT);
	// create transformations matrices
	transformationsMatrices.push_back(map<string, Matrix4x4*>());
	groupLists.push_back(vector<FlattenedGroup>());
	createTransformationsMatrices(transformationsMatrices[0], groupLists[0], model->getSubGroups());
	// calculate transformations matrices
	computeTransformationsMatrices(groupLists[0], model->getImportTransformationsMatrix(), baseAnimations.size() == 0?nullptr:&baseAnimations[0]);
	// skinning ...
	if (hasSkinning == true) {
		for (auto i = 0; i < skinningGroups.size(); i++) {
			skinningGroupsGroupSkinningJoints.push_back(vector<GroupSkinningJoint>());
			for (auto& skinningJoint: skinningGroups[i]->getSkinning()->getJoints()) {
				auto transformationsMatrixIt = transformationsMatrices[0].find(skinningJoint.getGroupId());
				if (transformationsMatrixIt == transformationsMatrices[0].end()) continue;
				auto skinningGroupMatrixIt = skinningGroupsMatrices[i].find(skinningJoint.getGroupId());
				if (skinningGroupMatrixIt == skinningGroupsMatrices[i].end()) continue;
				skinningGroupsGroupSkinningJoints[i].push_back({
					.joint = &skinningJoint,
					.groupTransformationsMatrix = transformationsMatrixIt->second,
					.skinningGroupTransformationsMatrix = skinningGroupMatrixIt->second
				});
			}
		}
		updateSkinningTransformationsMatrices();
	}
	// reset animation
	if (baseAnimations.size() == 0) baseAnimations.push_back(AnimationState());
	baseAnimations[baseAnimationIdx].endAtTime = -1LL;
	baseAnimations[baseAnimationIdx].lastAtTime = Timing::UNDEFINED;
	baseAnimations[baseAnimationIdx].currentAtTime = 0LL;
	baseAnimations[baseAnimationIdx].time = 0.0f;
	baseAnimations[baseAnimationIdx].finished = false;
}

Object3DAnimation::~Object3DAnimation() {
	for (auto baseAnimationTransformationsMatrices: transformationsMatrices) {
		for (auto it: baseAnimationTransformationsMatrices) {
			delete it.second;
		}
	}
	for (auto skinningGroupMatricies: skinningGroupsMatrices) {
		for (auto it: skinningGroupMatricies) {
			delete it.second;
		}
	}
	for (auto overridenTransformationsMatrixIt: overridenTransformationsMatrices) {
		delete overridenTransformationsMatrixIt.second;
	}
}

void Object3DAnimation::setAnimation(const string& id, float speed)
{
	auto _animationActiveSetup = model->getAnimationSetup(id);

	// only switch animation if we have one
	if (_animationActiveSetup != nullptr) {
		AnimationState baseAnimation;
		baseAnimation.setup = _animationActiveSetup;
		baseAnimation.endAtTime = -1LL;
		baseAnimation.lastAtTime = Timing::UNDEFINED;
		baseAnimation.currentAtTime = 0LL;
		baseAnimation.time = 0.0f;
		baseAnimation.speed = speed;
		baseAnimation.finished = false;
		if (baseAnimations.size() == 0) {
			baseAnimations.push_back(baseAnimation);
			baseAnimationIdx = 0;
		} else
		if (baseAnimations.size() == 1) {
			baseAnimations.push_back(baseAnimation);
			baseAnimationIdx = 1;
			transformationsMatrices.push_back(map<string, Matrix4x4*>());
			groupLists.push_back(vector<FlattenedGroup>());
			createTransformationsMatrices(transformationsMatrices[1], groupLists[1], model->getSubGroups());
			transformationsMatrices.push_back(map<string, Matrix4x4*>());
			groupLists.push_back(vector<FlattenedGroup>());
			createTransformationsMatrices(transformationsMatrices[2], groupLists[2], model->getSubGroups());
		} else {
			baseAnimationIdx = (baseAnimationIdx + 1) % 2;
			baseAnimations[baseAnimationIdx] = baseAnimation;
		}
		if (baseAnimations.size() > 1) {
			auto baseAnimationIdxLast = (baseAnimationIdx + 1) % 2;
			auto& baseAnimationLast = baseAnimations[baseAnimationIdxLast];
			baseAnimationLast.endAtTime = baseAnimationLast.currentAtTime;
		}
	} else {
		Console::println("Object3DAnimation::setAnimation(): " + model->getId() + ": missing animation: " + id);
	}
}

void Object3DAnimation::setAnimationSpeed(float speed) {
	if (baseAnimations.size() == 0) return;
	baseAnimations[baseAnimationIdx].speed = speed;
}

void Object3DAnimation::addOverlayAnimation(const string& id)
{
	// remove active overlay animation with given ids
	removeOverlayAnimation(id);
	// check overlay animation
	auto animationSetup = model->getAnimationSetup(id);
	if (animationSetup == nullptr) return;
	if (animationSetup->getOverlayFromGroupId().size() == 0) return;
	// create animation state
	auto animationState = new AnimationState();
	animationState->setup = animationSetup;
	animationState->lastAtTime = Timing::UNDEFINED;
	animationState->currentAtTime = 0LL;
	animationState->time = 0.0f;
	animationState->speed = 1.0f;
	animationState->finished = false;
	// register overlay animation
	overlayAnimationsById[id] = animationState;
	overlayAnimationsByJointId[animationSetup->getOverlayFromGroupId()] = animationState;
	//
	updateGroupLists();
}

void Object3DAnimation::removeOverlayAnimation(const string& id)
{
	auto animationStateIt = overlayAnimationsById.find(id);
	if (animationStateIt == overlayAnimationsById.end()) return;
	auto animationState = animationStateIt->second;
	overlayAnimationsById.erase(animationStateIt);
	auto overlayAnimationsByJointIdIt = overlayAnimationsByJointId.find(animationState->setup->getOverlayFromGroupId());
	if (overlayAnimationsByJointIdIt == overlayAnimationsByJointId.end() || overlayAnimationsByJointIdIt->second->setup != animationState->setup) {
		delete animationState;
		return;
	}
	overlayAnimationsByJointId.erase(overlayAnimationsByJointIdIt);
	delete animationState;
	//
	updateGroupLists();
}

void Object3DAnimation::removeOverlayAnimationsFinished()
{
	// determine finished overlay animations
	vector<string> overlayAnimationsToRemove;
	for (auto it: overlayAnimationsById) {
		const string& id = it.first;
		AnimationState* animationState = it.second;
		{
			if (animationState->finished == true) {
				overlayAnimationsToRemove.push_back(id);
			}
		}
	}
	// remove them
	for (auto animationState: overlayAnimationsToRemove) {
		removeOverlayAnimation(animationState);
	}
}

void Object3DAnimation::removeOverlayAnimations()
{
	// remove them
	vector<string> overlayAnimationsToRemove;
	for (auto it: overlayAnimationsById) {
		overlayAnimationsToRemove.push_back(it.first);
	}
	for (auto animationState: overlayAnimationsToRemove) {
		removeOverlayAnimation(animationState);
	}
}

const string Object3DAnimation::getAnimation()
{
	return baseAnimations[baseAnimationIdx].setup == nullptr ? "none" : baseAnimations[baseAnimationIdx].setup->getId();
}

float Object3DAnimation::getAnimationTime()
{
	return baseAnimations[baseAnimationIdx].time;
}

bool Object3DAnimation::hasOverlayAnimation(const string& id)
{
	return overlayAnimationsById.find(id) != overlayAnimationsById.end();
}

float Object3DAnimation::getOverlayAnimationTime(const string& id)
{
	AnimationState* animationState = nullptr;
	auto animationStateIt = overlayAnimationsById.find(id);
	if (animationStateIt != overlayAnimationsById.end()) {
		animationState = animationStateIt->second;
	}
	return animationState == nullptr ? 1.0f : animationState->time;
}

const Matrix4x4 Object3DAnimation::getGroupTransformationsMatrix(const string& id)
{
	auto overridenTransformationsMatrixIt = overridenTransformationsMatrices.find(id);
	if (overridenTransformationsMatrixIt != overridenTransformationsMatrices.end()) {
		return *overridenTransformationsMatrixIt->second;
	} else {
		auto transformationMatrixIt = transformationsMatrices[0].find(id);
		if (transformationMatrixIt != transformationsMatrices[0].end()) {
			return *transformationMatrixIt->second;
		}
		Console::println("Object3DAnimation::getTransformationsMatrix(): " + id + ": group not found");
	}
	return Matrix4x4().identity();
}

void Object3DAnimation::setGroupTransformationsMatrix(const string& id, const Matrix4x4& matrix)
{
	auto overridenTransformationsMatrixIt = overridenTransformationsMatrices.find(id);
	if (overridenTransformationsMatrixIt != overridenTransformationsMatrices.end()) {
		*overridenTransformationsMatrixIt->second = matrix;
	} else {
		overridenTransformationsMatrices[id] = new Matrix4x4(matrix);
	}
	//
	updateGroupLists();
}

void Object3DAnimation::unsetGroupTransformationsMatrix(const string& id)
{
	auto overridenTransformationsMatrixIt = overridenTransformationsMatrices.find(id);
	if (overridenTransformationsMatrixIt != overridenTransformationsMatrices.end()) {
		delete overridenTransformationsMatrixIt->second;
		overridenTransformationsMatrices.erase(overridenTransformationsMatrixIt);
	}
	//
	updateGroupLists();
}

void Object3DAnimation::createTransformationsMatrices(map<string, Matrix4x4*>& matrices, vector<FlattenedGroup>& groupList, const map<string, Group*>& groups, Matrix4x4* parentTransformationsMatrix, AnimationState* animationState)
{
	// iterate through groups
	for (auto it: groups) {
		// put and associate transformation matrices with group
		auto group = it.second;
		auto matrix = new Matrix4x4();
		matrix->identity();
		matrices[group->getId()] = matrix;
		// overriden matrix
		Matrix4x4* overridenTransformationsMatrix = nullptr;
		auto overridenTransformationsMatrixIt = overridenTransformationsMatrices.find(group->getId());
		if (overridenTransformationsMatrixIt != overridenTransformationsMatrices.end()) overridenTransformationsMatrix = overridenTransformationsMatrixIt->second;
		// overlay animation
		auto overlayAnimationIt = overlayAnimationsByJointId.find(group->getId());
		if (overlayAnimationIt != overlayAnimationsByJointId.end()) {
			animationState = overlayAnimationIt->second;
		}
		groupList.push_back({
			.groupId = group->getId(),
			.groupTransformationsMatrix = &group->getTransformationsMatrix(),
			.groupOverridenTransformationsMatrix = overridenTransformationsMatrix,
			.groupAnimation = group->getAnimation(),
			.groupAnimationState = animationState,
			.parentTransformationsMatrix = parentTransformationsMatrix,
			.transformationsMatrix = matrix
		});
		// do sub groups
		auto& subGroups = group->getSubGroups();
		if (subGroups.size() > 0) {
			createTransformationsMatrices(matrices, groupList, subGroups, matrix, animationState);
		}
	}
}

void Object3DAnimation::updateGroupList(vector<FlattenedGroup>& groupList, int& groupIdx, const map<string, Group*>& groups, AnimationState* animationState) {
	// iterate through groups
	for (auto it: groups) {
		// put and associate transformation matrices with group
		auto group = it.second;
		// overriden matrix
		Matrix4x4* overridenTransformationsMatrix = nullptr;
		auto overridenTransformationsMatrixIt = overridenTransformationsMatrices.find(group->getId());
		if (overridenTransformationsMatrixIt != overridenTransformationsMatrices.end()) overridenTransformationsMatrix = overridenTransformationsMatrixIt->second;
		// overlay animation
		auto overlayAnimationIt = overlayAnimationsByJointId.find(group->getId());
		if (overlayAnimationIt != overlayAnimationsByJointId.end()) {
			animationState = overlayAnimationIt->second;
		}
		// update group list
		groupList[groupIdx].groupOverridenTransformationsMatrix = overridenTransformationsMatrix;
		groupList[groupIdx].groupAnimationState = animationState;
		groupIdx++;
		// do sub groups
		auto& subGroups = group->getSubGroups();
		if (subGroups.size() > 0) {
			updateGroupList(groupList, groupIdx, subGroups, animationState);
		}
	}
}

void Object3DAnimation::computeTransformationsMatrices(vector<FlattenedGroup>& groupList, const Matrix4x4 parentTransformationsMatrix, AnimationState* animationState)
{
	// iterate through flattened groups
	Matrix4x4 transformationsMatrix;
	for (auto& flattenedGroup: groupList) {
		auto groupAnimationState = flattenedGroup.groupAnimationState != nullptr?flattenedGroup.groupAnimationState:animationState;
		// compute animation matrix if animation setups exist
		auto animation = flattenedGroup.groupAnimation;
		// TODO: check if its better to not compute animation matrix if finished
		if (animation != nullptr && groupAnimationState != nullptr && groupAnimationState->setup != nullptr) {
			auto& animationMatrices = animation->getTransformationsMatrices();
			auto frames = groupAnimationState->setup->getFrames();
			auto fps = model->getFPS();
			// determine current and last matrix
			auto frameAtLast = (groupAnimationState->lastAtTime / 1000.0f) * fps * groupAnimationState->setup->getSpeed() * groupAnimationState->speed;
			auto frameAtCurrent = (groupAnimationState->currentAtTime / 1000.0f) * fps * groupAnimationState->setup->getSpeed() * groupAnimationState->speed;
			// check if looping is disabled
			if (groupAnimationState->setup->isLoop() == false && frameAtCurrent >= frames) {
				frameAtLast = frames - 1;
				frameAtCurrent = frames - 1;
				groupAnimationState->finished = true;
			}
			auto matrixAtLast = static_cast<int32_t>(frameAtLast) % frames;
			auto matrixAtCurrent = static_cast<int32_t>(frameAtCurrent) % frames;
			groupAnimationState->time = frames <= 1?0.0f:static_cast<float>(matrixAtCurrent) / static_cast<float>((frames - 1));
			// compute animation transformations matrix
			auto t = frameAtCurrent - static_cast<float>(Math::floor(frameAtLast));
			if (t < 1.0f) {
				if (matrixAtLast == matrixAtCurrent) {
					matrixAtCurrent+= 1;
					if (matrixAtCurrent >= frames) {
						if (groupAnimationState->setup->isLoop() == true) {
							matrixAtCurrent = matrixAtCurrent % frames;
						} else {
							matrixAtCurrent = frames - 1;
						}
					}
				}
				Matrix4x4::interpolateLinear(
					animationMatrices[matrixAtLast + groupAnimationState->setup->getStartFrame()],
					animationMatrices[matrixAtCurrent + groupAnimationState->setup->getStartFrame()],
					t,
					transformationsMatrix
				);
			} else {
				transformationsMatrix.set(animationMatrices[matrixAtCurrent + groupAnimationState->setup->getStartFrame()]);
			}
		} else {
			if (flattenedGroup.groupOverridenTransformationsMatrix != nullptr) {
				transformationsMatrix.set(*flattenedGroup.groupOverridenTransformationsMatrix);
			} else {
				// no animation matrix, set up local transformation matrix up as group matrix
				transformationsMatrix.set(*flattenedGroup.groupTransformationsMatrix);
			}
		}
		// apply parent transformation matrix
		transformationsMatrix.multiply(flattenedGroup.parentTransformationsMatrix != nullptr?*flattenedGroup.parentTransformationsMatrix:parentTransformationsMatrix);
		// put and associate transformation matrices with group
		flattenedGroup.transformationsMatrix->set(transformationsMatrix);
	}
}

inline void Object3DAnimation::updateSkinningTransformationsMatrices() {
	for (auto& skinningGroupGroupSkinningJoints: skinningGroupsGroupSkinningJoints)
	for (auto& skinningGroupGroupSkinningJoint: skinningGroupGroupSkinningJoints) {
		skinningGroupGroupSkinningJoint.skinningGroupTransformationsMatrix->set(skinningGroupGroupSkinningJoint.joint->getBindMatrix()).multiply(*skinningGroupGroupSkinningJoint.groupTransformationsMatrix);
	}
}

void Object3DAnimation::computeTransformations(vector<FlattenedGroup>& groupList, const Matrix4x4& instanceTransformationsMatrix, AnimationState& baseAnimation, void* context, int64_t lastFrameAtTime, int64_t currentFrameAtTime)
{
	// do transformations if we have a animation
	if (baseAnimation.setup != nullptr) {
		// animation timing
		// do progress of base animation
		if (lastFrameAtTime != Timing::UNDEFINED && baseAnimation.lastAtTime != -1LL) {
			baseAnimation.currentAtTime+= currentFrameAtTime - lastFrameAtTime;
		}
		// do progress of overlay animations
		for (auto it: overlayAnimationsById) {
			AnimationState* overlayAnimationState = it.second;
			if (lastFrameAtTime != Timing::UNDEFINED && overlayAnimationState->lastAtTime != -1LL) {
				overlayAnimationState->currentAtTime+= currentFrameAtTime - lastFrameAtTime;
			}
			overlayAnimationState->lastAtTime = overlayAnimationState->currentAtTime;
		}
		// set up parent transformations matrix
		Matrix4x4 parentTransformationsMatrix;
		parentTransformationsMatrix.set(model->getImportTransformationsMatrix());
		if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
			parentTransformationsMatrix.multiply(instanceTransformationsMatrix);
		}
		// calculate transformations matrices
		computeTransformationsMatrices(groupList, parentTransformationsMatrix, &baseAnimation);
		//
		baseAnimation.lastAtTime = baseAnimation.currentAtTime;
	} else
	if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
		// set up parent transformations matrix
		Matrix4x4 parentTransformationsMatrix;
		parentTransformationsMatrix.set(model->getImportTransformationsMatrix());
		if (animationProcessingTarget == Engine::AnimationProcessingTarget::CPU_NORENDERING) {
			parentTransformationsMatrix.multiply(instanceTransformationsMatrix);
		}
		// calculate transformations matrices
		computeTransformationsMatrices(groupList, parentTransformationsMatrix, &baseAnimation);
	}
}

void Object3DAnimation::computeTransformations(void* context, const Matrix4x4& instanceTransformationsMatrix, int64_t lastFrameAtTime, int64_t currentFrameAtTime) {
	// compute last animation matrices if required
	auto baseAnimationIdxLast = transformationsMatrices.size() > 1?(baseAnimationIdx + 1) % 2:-1;
	if (baseAnimationIdxLast != -1 &&
		baseAnimations[baseAnimationIdxLast].lastAtTime != -1LL) {
		computeTransformations(groupLists[1 + baseAnimationIdxLast], instanceTransformationsMatrix, baseAnimations[baseAnimationIdxLast], context, lastFrameAtTime, currentFrameAtTime);
	} else {
		baseAnimationIdxLast = -1;
	}

	// compute current animation matrices
	computeTransformations(groupLists[groupLists.size() > 1?1 + baseAnimationIdx:baseAnimationIdx], instanceTransformationsMatrix, baseAnimations[baseAnimationIdx], context, lastFrameAtTime, currentFrameAtTime);

	// blend if required
	if (transformationsMatrices.size() > 1) {
		for (auto i = 0; i < groupLists[0].size(); i++) {
			if (baseAnimationIdxLast != -1 &&
				baseAnimations[baseAnimationIdxLast].endAtTime != -1LL) {
				auto blendingAnimationDuration = static_cast<float>(baseAnimations[baseAnimationIdxLast].currentAtTime - baseAnimations[baseAnimationIdxLast].endAtTime) / Engine::getAnimationBlendingTime();
				Matrix4x4::interpolateLinear(
					*groupLists[1 + baseAnimationIdxLast][i].transformationsMatrix,
					*groupLists[1 + baseAnimationIdx][i].transformationsMatrix,
					Math::min(
						blendingAnimationDuration,
						1.0f
					),
					*groupLists[0][i].transformationsMatrix
				);
				if (blendingAnimationDuration >= 1.0f) {
					auto& animationStateLast = baseAnimations[baseAnimationIdxLast];
					animationStateLast.setup = nullptr;
					animationStateLast.endAtTime = -1LL;
					animationStateLast.currentAtTime = -1LL;
					animationStateLast.lastAtTime = -1LL;
					animationStateLast.finished = true;
					animationStateLast.time = -1LL;
				}
			} else {
				groupLists[0][i].transformationsMatrix->set(*groupLists[1 + baseAnimationIdx][i].transformationsMatrix);
			}
		}
	}
	if (hasSkinning == true) updateSkinningTransformationsMatrices();
}

int32_t Object3DAnimation::determineSkinnedGroupCount(const map<string, Group*>& groups)
{
	return determineSkinnedGroupCount(groups, 0);
}

int32_t Object3DAnimation::determineSkinnedGroupCount(const map<string, Group*>& groups, int32_t count)
{
	// iterate through groups
	for (auto it: groups) {
		Group* group = it.second;
		//
		if (group->getSkinning() != nullptr)
			count++;
		// calculate sub groups
		auto& subGroups = group->getSubGroups();
		if (subGroups.size() > 0) {
			count = determineSkinnedGroupCount(subGroups, count);
		}
	}
	return count;
}

int32_t Object3DAnimation::determineSkinnedGroups(const map<string, Group*>& groups, vector<Group*>& skinningGroups, int32_t idx)
{
	// iterate through groups
	for (auto it: groups) {
		Group* group = it.second;
		// fetch skinning groups
		if (group->getSkinning() != nullptr) {
			skinningGroups[idx++] = group;
		}
		// calculate sub groups
		auto& subGroups = group->getSubGroups();
		if (subGroups.size() > 0) {
			idx = determineSkinnedGroups(subGroups, skinningGroups, idx);
		}
	}
	return idx;
}

map<string, Matrix4x4*>* Object3DAnimation::getSkinningGroupsMatrices(Group* group)
{
	if (hasSkinning == false) return nullptr;
	for (auto i = 0; i < skinningGroups.size(); i++) {
		if (skinningGroups[i] == group) {
			return &skinningGroupsMatrices[i];
		}
	}
	return nullptr;
}
