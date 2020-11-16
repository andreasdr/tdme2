#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/tools/leveleditor/views/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/SharedModelEditorView.h>

using std::string;

using tdme::tools::shared::views::SharedModelEditorView;
using tdme::math::Vector3;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::views::PopUps;

/**
 * Model editor view
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::leveleditor::views::ModelEditorView final
	: public SharedModelEditorView
{
public:
	/**
	 * Public constructor
	 * @param popUps pop ups view
	 */
	ModelEditorView(PopUps* popUps);

	// overridden methods
	virtual void onSetEntityData() override;
	virtual void onLoadModel(LevelEditorEntity* oldEntity, LevelEditorEntity* entity) override;
	virtual void onInitAdditionalScreens() override;

private:
	// overridden methods
	virtual LevelEditorEntity* loadModel(const string& name, const string& description, const string& pathName, const string& fileName, const Vector3& pivot) override;

};
