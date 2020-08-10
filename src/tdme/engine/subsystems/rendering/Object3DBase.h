#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Transformations.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/Object3DAnimation.h>
#include <tdme/engine/subsystems/rendering/Object3DGroup.h>
#include <tdme/engine/subsystems/skinning/fwd-tdme.h>
#include <tdme/utils/fwd-tdme.h>

using std::map;
using std::vector;
using std::string;

using tdme::engine::Engine;
using tdme::engine::Transformations;
using tdme::engine::model::Group;
using tdme::engine::model::Model;
using tdme::engine::primitives::Triangle;
using tdme::engine::subsystems::rendering::Object3DAnimation;
using tdme::engine::subsystems::rendering::Object3DBase_TransformedFacesIterator;
using tdme::engine::subsystems::rendering::Object3DGroup;
using tdme::engine::subsystems::rendering::Object3DGroupMesh;

/** 
 * Object3D base class
 * @author Andreas Drewke
 */
class tdme::engine::subsystems::rendering::Object3DBase
{
	friend class Object3DGroup;
	friend class Object3DGroupMesh;
	friend class Object3DBase_TransformedFacesIterator;
	friend class ModelUtilitiesInternal;
	friend class tdme::engine::subsystems::skinning::SkinningShader;

private:
	Object3DBase_TransformedFacesIterator* transformedFacesIterator { nullptr };

protected:
	Model* model;
	vector<Object3DGroup*> object3dGroups;
	bool usesManagers;
	int instances;
	int enabledInstances;
	vector<Object3DAnimation*> instanceAnimations;
	vector<bool> instanceEnabled;
	vector<Transformations> instanceTransformations;
	int currentInstance;
	Engine::AnimationProcessingTarget animationProcessingTarget;

	/**
	 * Private constructor
	 * @param model model
	 * @param useManagers use mesh and object 3d group renderer model manager
	 * @param animationProcessingTarget animation processing target
	 * @param instances instances to compute and render by duplication
	 */
	Object3DBase(Model* model, bool useManagers, Engine::AnimationProcessingTarget animationProcessingTarget, int instances);

	/**
	 * Destructor
	 */
	virtual ~Object3DBase();

public:

	/** 
	 * @return model
	 */
	inline Model* getModel() {
		return model;
	}

	/**
	 * Pre render step, computes transformations
	 * @param context context
	 * @param lastFrameAtTime time of last animation computation
	 * @param currentFrameAtTime time of current animation computation
	 */
	virtual inline void computeTransformations(void* context, int64_t lastFrameAtTime, int64_t currentFrameAtTime){
		enabledInstances = 0;
		for (auto i = 0; i < instances; i++) {
			if (instanceEnabled[i] == false) continue;
			instanceAnimations[i]->computeTransformations(context, instanceTransformations[i].getTransformationsMatrix(), lastFrameAtTime, currentFrameAtTime);
			enabledInstances++;
		}
		if (enabledInstances > 0) Object3DGroup::computeTransformations(context, object3dGroups);
	} 

	/**
	 * @return group count
	 */
	int getGroupCount() const;

	/** 
	 * Retrieves list of triangles of all or given groups
	 * @param triangles triangles
	 * @param groupIdx group index or -1 for all groups
	 */
	void getTriangles(vector<Triangle>& triangles, int groupIdx = -1);

	/** 
	 * @return transformed faces iterator
	 */
	Object3DBase_TransformedFacesIterator* getTransformedFacesIterator();

	/** 
	 * Returns object3d group mesh object
	 * @param groupId group id
	 * @return object3d group mesh object
	 */
	Object3DGroupMesh* getMesh(const string& groupId);

	/** 
	 * Initiates this object3d 
	 */
	virtual void initialize();

	/** 
	 * Disposes this object3d 
	 */
	virtual void dispose();

	/**
	 * @return maximum of instances
	 */
	inline int getInstances() {
		return instances;
	}

	/**
	 * @return current instance
	 */
	inline int getCurrentInstance() {
		return currentInstance;
	}

	/**
	 * Set current instance
	 * @param current instance
	 */
	inline void setCurrentInstance(int currentInstance) {
		if (currentInstance >= 0 && currentInstance < instances) this->currentInstance = currentInstance;
	}

	/**
	 * @return current instance enabled
	 */
	inline bool getInstanceEnabled() {
		return instanceEnabled[currentInstance];
	}

	/**
	 * Set current instance enabled
	 * @param enabled instance enabled
	 */
	inline void setInstanceEnabled(bool enabled) {
		instanceEnabled[currentInstance] = enabled;
	}

	/**
	 * Sets up a base animation to play
	 * @param id id
	 * @param speed speed whereas 1.0 is default speed
	 */
	inline void setAnimation(const string& id, float speed = 1.0f) {
		instanceAnimations[currentInstance]->setAnimation(id, speed);
	}

	/**
	 * Set up animation speed
	 * @param speed speed whereas 1.0 is default speed
	 */
	inline void setAnimationSpeed(float speed) {
		instanceAnimations[currentInstance]->setAnimationSpeed(speed);
	}

	/**
	 * Overlays a animation above the base animation
	 * @param id id
	 */
	inline void addOverlayAnimation(const string& id) {
		instanceAnimations[currentInstance]->addOverlayAnimation(id);
	}

	/**
	 * Removes a overlay animation
	 * @param id id
	 */
	inline void removeOverlayAnimation(const string& id) {
		instanceAnimations[currentInstance]->removeOverlayAnimation(id);
	}

	/**
	 * Removes all finished overlay animations
	 */
	inline void removeOverlayAnimationsFinished() {
		instanceAnimations[currentInstance]->removeOverlayAnimationsFinished();
	}

	/**
	 * Removes all overlay animations
	 */
	inline void removeOverlayAnimations() {
		instanceAnimations[currentInstance]->removeOverlayAnimations();
	}

	/**
	 * @return active animation setup id
	 */
	inline const string getAnimation() {
		return instanceAnimations[currentInstance]->getAnimation();
	}

	/**
	 * Returns current base animation time
	 * @return 0.0 <= time <= 1.0
	 */
	inline float getAnimationTime() {
		return instanceAnimations[currentInstance]->getAnimationTime();
	}

	/**
	 * Returns if there is currently running a overlay animation with given id
	 * @param id id
	 * @return animation is running
	 */
	inline bool hasOverlayAnimation(const string& id) {
		return instanceAnimations[currentInstance]->hasOverlayAnimation(id);
	}

	/**
	 * Returns current overlay animation time
	 * @param id id
	 * @return 0.0 <= time <= 1.0
	 */
	inline float getOverlayAnimationTime(const string& id) {
		return instanceAnimations[currentInstance]->getOverlayAnimationTime(id);
	}

	/**
	 * Returns transformation matrix for given group
	 * @param id group id
	 * @return transformation matrix or identity matrix if not found
	 */
	inline const Matrix4x4 getGroupTransformationsMatrix(const string& id) {
		return instanceAnimations[currentInstance]->getGroupTransformationsMatrix(id);
	}

	/**
	 * Set transformation matrix for given group
	 * @param id group id
	 * @param matrix transformation matrix
	 */
	inline void setGroupTransformationsMatrix(const string& id, const Matrix4x4& matrix) {
		instanceAnimations[currentInstance]->setGroupTransformationsMatrix(id, matrix);
	}

	/**
	 * Unset transformation matrix for given group
	 * @param id group id
	 */
	inline void unsetGroupTransformationsMatrix(const string& id) {
		instanceAnimations[currentInstance]->unsetGroupTransformationsMatrix(id);
	}

	/**
	 * @return this transformations matrix
	 */
	inline const Matrix4x4& getTransformationsMatrix() const {
		return instanceTransformations[currentInstance].getTransformationsMatrix();
	}

};
