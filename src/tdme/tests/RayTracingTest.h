#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/application/Application.h>
#include <tdme/application/InputEventHandler.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/physics/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/tests/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/utilities/ObjectDeleter.h>

using tdme::application::Application;
using tdme::engine::Engine;
using tdme::engine::model::Model;
using tdme::engine::physics::World;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::model::Model;
using tdme::utilities::ObjectDeleter;
using tdme::tools::shared::model::LevelEditorEntity;

/**
 * Ray Tracing Test
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::tests::RayTracingTest final
	: public virtual Application, public virtual InputEventHandler
{
private:
	static constexpr int32_t RIGID_TYPEID_STANDARD { 1 };
	static constexpr int32_t BOX_COUNT { 4 };
	Engine* engine { nullptr };
	World* world { nullptr };

	bool keyLeft { false };
	bool keyRight { false };
	bool keyUp { false };
	bool keyDown { false };
	bool keyInfo { false };
	int mouseLastXDragged { -1 };
	int mouseLastYDragged { -1 };

	float rotationY { 0.0f };
	float rotationX { 0.0f };
	Model* entityBoundingVolumeModel { nullptr };
	ObjectDeleter<LevelEditorEntity> levelEditorEntityDeleter;
	ObjectDeleter<Model> modelDeleter;
	ObjectDeleter<BoundingVolume> bvDeleter;
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
	RayTracingTest();

	/**
	 * Public destructor
	 */
	~RayTracingTest();

	// overridden methods
	void display() override;
	void dispose() override;
	void initialize() override;
	void reshape(int32_t width, int32_t height) override;

	// override methods
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
