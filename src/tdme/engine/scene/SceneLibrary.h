#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/scene/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::prototype::Prototype;
using tdme::engine::scene::Scene;
using tdme::math::Vector3;

/**
 * Scene prototype library definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::scene::SceneLibrary final
{
public:
	static constexpr int ID_ALLOCATE { -1 };

private:
	Scene* scene { nullptr };
	map<int, Prototype*> prototypesById;
	vector<Prototype*> prototypes;
	int prototypeIdx;

	/**
	 * Allocata a unique prototype index
	 * @return index
	 */
	int allocatePrototypeId();

public:
	/**
	 * Public constructor
	 * @param scene scene
	 */
	SceneLibrary(Scene* scene);

	/**
	 * Destructor
	 */
	~SceneLibrary();

	/**
	 * Clears this scene prototype library
	 */
	void clear();

	/**
	 * Adds a model
	 * @param id id
	 * @param name name
	 * @param description description
	 * @param pathName path name
	 * @param fileName file name
	 * @param pivot pivot
	 * @return prototype
	 * @throws tdme::utilities::Exception
	 */
	Prototype* addModel(int id, const string& name, const string& description, const string& pathName, const string& fileName, const Vector3& pivot);

	/**
	 * Add a particle system
	 * @param id id
	 * @param name name
	 * @param description description
	 * @return prototype
	 * @throws tdme::utilities::Exception
	 */
	Prototype* addParticleSystem(int id, const string& name, const string& description);

	/**
	 * Add a trigger
	 * @param id id
	 * @param name name
	 * @param description description
	 * @param width width
	 * @param height height
	 * @param depth depth
	 * @return prototype
	 * @throws tdme::utilities::Exception
	 */
	Prototype* addTrigger(int id, const string& name, const string& description, float width, float height, float depth);

	/**
	 * Add a environment mapping
	 * @param id id
	 * @param name name
	 * @param description description
	 * @param width width
	 * @param height height
	 * @param depth depth
	 * @return prototype
	 * @throws tdme::utilities::Exception
	 */
	Prototype* addEnvironmentMapping(int id, const string& name, const string& description, float width, float height, float depth);

	/**
	 * Add a empty
	 * @param id id
	 * @param name name
	 * @param description description
	 * @return prototype
	 * @throws tdme::utilities::Exception
	 */
	Prototype* addEmpty(int id, const string& name, const string& description);

	/**
	 * Add a terrain
	 * @param id id
	 * @param name name
	 * @param description description
	 * @return prototype
	 * @throws tdme::utilities::Exception
	 */
	Prototype* addTerrain(int id, const string& name, const string& description);

	/**
	 * Add a prototype
	 * @param prototype prototype
	 */
	void addPrototype(Prototype* prototype);

	/**
	 * Get prototype at given index
	 * @param idx index
	 * @return prototype
	 */
	Prototype* getPrototypeAt(int idx);

	/**
	 * Get a prototype by given id
	 * @param id id
	 * @return prototype
	 */
	Prototype* getPrototype(int id);

	/**
	 * Remove a prototype
	 * @param id id
	 */
	void removePrototype(int id);

	/**
	 * @return prototype count
	 */
	int getPrototypeCount();

};
