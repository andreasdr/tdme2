#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/tools/leveleditor/views/fwd-tdme.h>
#include <tdme/tools/shared/model/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/SharedParticleSystemView.h>

using std::string;

using tdme::tools::shared::views::SharedParticleSystemView;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::views::PopUps;


/**
 * Particle System View
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::leveleditor::views::ParticleSystemView final
	: public SharedParticleSystemView
{
public:
	/**
	 * Public constructor
	 * @param popUps pop ups
	 */
	ParticleSystemView(PopUps* popUps);

	// overridden methods
	virtual void onInitAdditionalScreens() override;
	virtual void onSetEntityData() override;
	virtual void onLoadParticleSystem(LevelEditorEntity* oldEntity, LevelEditorEntity* newEntity) override;

private:
	// overridden methods
	virtual LevelEditorEntity* loadParticleSystem(const string& name, const string& description, const string& pathName, const string& fileName) override;

};
