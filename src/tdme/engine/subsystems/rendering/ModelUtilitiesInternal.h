#pragma once

#include <map>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/ModelStatistics.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::string;

using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::rendering::ModelStatistics;
using tdme::engine::subsystems::rendering::Object3DModelInternal;
using tdme::math::Matrix4x4;

/**
 * Model utilities
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::ModelUtilitiesInternal
{
public:
	/**
	 * Creates a bounding box from given model
	 * @param model model
	 * @param overriddenNodeTransformationsMatrices overridden node transformations matrices
	 * @return axis aligned bounding box
	 */
	static BoundingBox* createBoundingBox(Model* model, const map<string, Matrix4x4*> overriddenNodeTransformationsMatrices = map<string, Matrix4x4*>());

	/**
	 * Invert normals of a model
	 * @param model model
	 */
	static void invertNormals(Model* model);

	/**
	 * Compute model statistics
	 * @param model model
	 * @param modelStatistics model statistics
	 */
	static void computeModelStatistics(Model* model, ModelStatistics* modelStatistics);

	/**
	 * Compute model statistics
	 * @param object3DModelInternal object 3d model internal
	 * @param modelStatistics model statistics
	 */
	static void computeModelStatistics(Object3DModelInternal* object3DModelInternal, ModelStatistics* modelStatistics);

	/**
	 * Compute if model 1 equals model 2
	 * @param model1 model 1
	 * @param model2 model 2
	 * @return model1 equals model2
	 */
	static bool equals(Model* model1, Model* model2);

	/**
	 * Compute if model 1 equals model 2
	 * @param object3DModel1Internal model 1
	 * @param object3DModel2Internal model 2
	 * @return model1 equals model2
	 */
	static bool equals(Object3DModelInternal* object3DModel1Internal, Object3DModelInternal* object3DModel2Internal);

private:
	/**
	 * Creates a bounding box from given object3d model
	 * @param object3DModelInternal model
	 * @return axis aligned bounding box
	 */
	static BoundingBox* createBoundingBox(Object3DModelInternal* object3DModelInternal);

	/**
	 * Creates a bounding box from given object3d model without mesh
	 * @param object3DModelInternal model
	 * @return boundingBox axis aligned bounding box
	 */
	static BoundingBox* createBoundingBoxNoMesh(Object3DModelInternal* object3DModelInternal);

	/**
	 * Invert normals recursive
	 * @param nodes nodes
	 */
	static void invertNormals(const map<string, Node*>& nodes);

};
