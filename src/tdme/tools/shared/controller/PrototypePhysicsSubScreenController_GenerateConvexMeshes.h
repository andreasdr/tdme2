#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/Triangle.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/tools/shared/controller/fwd-tdme.h>

using std::string;
using std::vector;

using tdme::engine::model::Model;
using tdme::engine::primitives::Triangle;
using tdme::engine::prototype::Prototype;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;

/**
 * Prototype physics sub screen controller convex mesh generation class
 */
class tdme::tools::shared::controller::PrototypePhysicsSubScreenController_GenerateConvexMeshes
{
public:
	/**
	 * Remove convex meshes
	 * @param prototypePhysicsSubScreenController entity physics sub screen controller
	 * @param entityFinal entity final
	 */
	static void removeConvexMeshes(PrototypePhysicsSubScreenController* prototypePhysicsSubScreenController, Prototype* entityFinal);

	/**
	 * Generateconvex meshes
	 * @param prototypePhysicsSubScreenController entity physics sub screen controller
	 * @param entityFinal entity final
	 */
	static void generateConvexMeshes(PrototypePhysicsSubScreenController* prototypePhysicsSubScreenController, Prototype* entityFinal);

private:

	/**
	 * Create model
	 * @param id id
	 * @param points points
	 * @param triangles triangles
	 * @param pointCount point count
	 * @param triangleCount triangle count
	 */
	static Model* createModel(const string& id, double* points, unsigned int* triangles, unsigned int pointCount, unsigned int triangleCount);

	/**
	 * Create model
	 * @param id id
	 * @param triangles triangles
	 */
	static Model* createModel(const string& id, vector<Triangle>& triangles);

	PrototypePhysicsSubScreenController* prototypePhysicsSubScreenController;
	Prototype* entityFinal;
};
