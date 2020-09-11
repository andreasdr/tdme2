#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/application/Application.h>
#include <tdme/tests/fwd-tdme.h>
#include <tdme/utilities/ObjectDeleter.h>

using tdme::application::Application;
using tdme::engine::Engine;
using tdme::engine::model::Model;
using tdme::utilities::ObjectDeleter;

/** 
 * Water shader test
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::tests::WaterTest final
	: public virtual Application
{
private:
	Engine* engine { nullptr };
	ObjectDeleter<Model> modelDeleter;

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
	WaterTest();

	// overriden methods
	void display() override;
	void dispose() override;
	void initialize() override;
	void reshape(int32_t width, int32_t height) override;
};
