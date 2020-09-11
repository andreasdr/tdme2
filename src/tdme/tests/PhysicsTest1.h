
#pragma once

#include <tdme/tdme.h>
#include <tdme/application/Application.h>
#include <tdme/application/InputEventHandler.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/tests/fwd-tdme.h>
#include <tdme/utilities/ObjectDeleter.h>

using tdme::application::Application;
using tdme::application::InputEventHandler;
using tdme::engine::Engine;
using tdme::engine::Object3DModel;
using tdme::engine::model::Model;
using tdme::engine::physics::World;
using tdme::engine::primitives::BoundingVolume;
using tdme::utilities::ObjectDeleter;

/** 
 * Engine with physics test 1
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::tests::PhysicsTest1 final
	: public virtual Application, public virtual InputEventHandler
{

private:
	static constexpr int32_t RIGID_TYPEID_STANDARD { 1 };
	static constexpr int32_t BOX_COUNT { 5 };
	static constexpr int32_t BOXSTACK_COUNT { 2 };
	static constexpr int32_t CAPSULE_COUNT { 10 };
	static constexpr int32_t SPHERE_COUNT { 10 };
	Engine* engine { nullptr };
	bool keyLeft;
	bool keyRight;
	bool keyUp;
	bool keyDown;
	bool keyW;
	bool keyA;
	bool keyS;
	bool keyD;
	World* world { nullptr };
	ObjectDeleter<Model> modelDeleter;
	ObjectDeleter<BoundingVolume> bvDeleter;
	ObjectDeleter<Object3DModel> object3DModelDeleter;

public:

	/** 
	 * Main
	 * @param argc argument count
	 * @param argv argument values
	 */
	static void main(int argc, char** argv);

public:
	/**
	 * Public constructor
	 */
	PhysicsTest1();

	/**
	 * Public destructor
	 */
	~PhysicsTest1();

	void display() override;
	void dispose() override;
	void initialize() override;
	void reshape(int32_t width, int32_t height) override;

	void onChar(unsigned int key, int x, int y) override;
	void onKeyDown (unsigned char key, int x, int y) override;
	void onKeyUp(unsigned char key, int x, int y) override;
	void onSpecialKeyDown (int key, int x, int y) override;
	void onSpecialKeyUp(int key, int x, int y) override;
	void onMouseDragged(int x, int y) override;
	void onMouseMoved(int x, int y) override;
	void onMouseButton(int button, int state, int x, int y) override;
	void onMouseWheel(int button, int direction, int x, int y) override;

};
