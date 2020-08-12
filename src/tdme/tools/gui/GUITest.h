#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/application/Application.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/tools/gui/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/gui/events/GUIChangeListener.h>

using std::string;

using tdme::application::Application;
using tdme::engine::Engine;
using tdme::gui::events::GUIActionListener;
using tdme::gui::events::GUIActionListener_Type;
using tdme::gui::events::GUIChangeListener;
using tdme::tools::shared::views::PopUps;


/** 
 * GUI Test
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::gui::GUITest final
	: public virtual Application, public virtual GUIActionListener, public virtual GUIChangeListener
{
private:
	string screenFileName;
	Engine* engine { nullptr };
	PopUps* popUps { nullptr };

public:
	/**
	 * Public constructor
	 * @param screenFileName screen file name
	 */
	GUITest(const string& screenFileName);

	/**
	 * Public destructor
	 */
	~GUITest();

	void initialize() override;
	void dispose() override;
	void reshape(int32_t width, int32_t height) override;
	void display() override;
	void onActionPerformed(GUIActionListener_Type* type, GUIElementNode* node) override;
	void onValueChanged(GUIElementNode* node) override;

	/** 
	 * Main
	 * @param argc argument count
	 * @param argv argument values
	 */
	static void main(int argc, char** argv);
};
