#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/tools/sceneeditor/views/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/SharedParticleSystemView.h>

using std::string;

using tdme::engine::prototype::Prototype;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedParticleSystemView;


/**
 * Particle System View
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::sceneeditor::views::ParticleSystemView final
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
	virtual void onLoadParticleSystem(Prototype* oldEntity, Prototype* newEntity) override;

private:
	// overridden methods
	virtual Prototype* loadParticleSystemPrototype(const string& name, const string& description, const string& pathName, const string& fileName) override;

};
