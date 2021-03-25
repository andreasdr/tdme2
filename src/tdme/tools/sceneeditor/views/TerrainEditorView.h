#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/tools/sceneeditor/views/fwd-tdme.h>
#include <tdme/tools/shared/views/fwd-tdme.h>
#include <tdme/tools/shared/views/SharedTerrainEditorView.h>

using std::string;

using tdme::engine::prototype::Prototype;
using tdme::math::Vector3;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::SharedTerrainEditorView;

/**
 * Terrain editor view
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::tools::sceneeditor::views::TerrainEditorView final: public SharedTerrainEditorView
{
public:
	/**
	 * Public constructor
	 * @param popUps pop ups view
	 */
	TerrainEditorView(PopUps* popUps);

	// overridden methods
	virtual void onSetPrototypeData() override;
	virtual void onLoadTerrain(Prototype* oldEntity, Prototype* entity) override;
	virtual void onInitAdditionalScreens() override;

private:
	// overridden methods
	virtual Prototype* loadTerrainPrototype(const string& pathName, const string& fileName) override;

};
