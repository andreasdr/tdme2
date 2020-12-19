#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/application/Application.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/scene/Scene.h>
#include <tdme/engine/Object3D.h>
#include <tdme/math/Vector3.h>
#include <tdme/tests/fwd-tdme.h>
#include <tdme/utilities/PathFinding.h>

using std::vector;

using tdme::application::Application;
using tdme::engine::physics::World;
using tdme::engine::prototype::Prototype;
using tdme::engine::scene::Scene;
using tdme::engine::Engine;
using tdme::engine::Object3D;
using tdme::math::Vector3;
using tdme::utilities::PathFinding;

/**
 * Engine with physics test 2
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::tests::PathFindingTest final
	: public virtual Application
{
private:
	World* world { nullptr };
	Engine* engine { nullptr };
	Scene scene;
	Prototype* playerModelPrototype { nullptr };
	Object3D* playerObject { nullptr };
	PathFinding* pathFinding { nullptr };
	vector<Vector3> pathPositions;
	vector<Vector3> path;
	int64_t timeLastUpdate;
	int32_t pathIdx;
	float playerXDirection;
	float playerZDirection;

	/**
	 * Determine player movement direction
	 */
	void determinePlayerMovementDirection();

	/**
	 * Determine id player completed current step in path for X axis
	 * @return completed
	 */
	bool determinePlayerCompletedStepX();

	/**
	 * Determine id player completed current step in path for Z axis
	 * @return completed
	 */
	bool determinePlayerCompletedStepZ();

	/**
	 * Do path finding
	 */
	void doPathFinding();
public:

	/**
	 * Main
	 * @param argc argument count
	 * @param argv argument values
	 */
	static void main(int argc, char** argv);

	/**
	 * Public constructor
	 */
	PathFindingTest();

	/**
	 * Public destructor
	 */
	~PathFindingTest();

	// overridden methods
	void display() override;
	void dispose() override;
	void initialize() override;
	void reshape(int32_t width, int32_t height) override;

};
