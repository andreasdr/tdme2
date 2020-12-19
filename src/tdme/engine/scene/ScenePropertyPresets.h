#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

#include <ext/tinyxml/tinyxml.h>

#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/scene/fwd-tdme.h>
#include <tdme/engine/scene/SceneLight.h>

using std::map;
using std::string;
using std::vector;

using tinyxml::TiXmlElement;

using tdme::engine::prototype::PrototypeProperties;
using tdme::engine::prototype::PrototypeProperty;
using tdme::engine::scene::Scene;
using tdme::engine::scene::SceneLight;
using tdme::engine::scene::ScenePropertyPresets;

/**
 * Scene Property Presets
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::scene::ScenePropertyPresets final
{

private:
	vector<PrototypeProperty*> scenePropertiesPreset;
	map<string, vector<PrototypeProperty*>> entityPropertiesPresets;
	map<string, SceneLight*> lightPresets;
	static ScenePropertyPresets* instance;

public:
	/**
	 * Constructor
	 * @param pathName path name
	 * @param fileName file name
	 */
	ScenePropertyPresets(const string& pathName, const string& fileName);

	/**
	 * Destructor
	 */
	~ScenePropertyPresets();

	/**
	 * @return scene property presets instance
	 */
	static ScenePropertyPresets* getInstance();

	/**
	 * Set default scene properties
	 * @param scene scene
	 */
	void setDefaultSceneProperties(Scene* scene);

	/**
	 * @return scene properties preset
	 */
	inline const vector<PrototypeProperty*>& getScenePropertiesPreset() const {
		return scenePropertiesPreset;
	}

	/**
	 * @return entity property presets
	 */
	inline const map<string, vector<PrototypeProperty*>>& getEntityPropertiesPresets() const {
		return entityPropertiesPresets;
	}

	/**
	 * @return light presets
	 */
	inline const map<string, SceneLight*>& getLightPresets() const {
		return lightPresets;
	}

private:

	/**
	 * Returns immediate children by tagnames of parent
	 * @param parent parent
	 * @param name name
	 * @return children with given name
	 */
	const vector<TiXmlElement*> getChildrenByTagName(TiXmlElement* parent, const char* name);

};
