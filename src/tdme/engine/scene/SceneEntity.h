#pragma once

#include <string>

#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/prototype/PrototypeProperties.h>
#include <tdme/engine/scene/fwd-tdme.h>
#include <tdme/engine/Transformations.h>

using std::string;

using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeProperties;
using tdme::engine::Transformations;

/**
 * Scene entity definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::scene::SceneEntity final
	: public PrototypeProperties
{
private:
	string id;
	string description;
	Transformations transformations;
	Prototype* prototype { nullptr };
	string reflectionEnvironmentMappingId;

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param description description
	 * @param transformations transformations
	 * @param prototype prototype
	 */
	SceneEntity(const string& id, const string& description, const Transformations& transformations, Prototype* prototype);

	/**
	 * Destructor
	 */
	~SceneEntity();

	/**
	 * @return id
	 */
	inline const string& getId() {
		return id;
	}

	/**
	 * Set id
	 * @param id id
	 */
	inline void setId(const string& id) {
		this->id = id;
	}

	/**
	 * @return description
	 */
	inline const string& getDescription() {
		return description;
	}

	/**
	 * Set description
	 * @param description description
	 */
	inline void setDescription(const string& description) {
		this->description = description;
	}

	/**
	 * @return transformations
	 */
	inline Transformations& getTransformations() {
		return transformations;
	}

	/**
	 * @return prototype
	 */
	inline Prototype* getPrototype() {
		return prototype;
	}

	/**
	 * Set up prototype
	 * @param prototype prototype
	 */
	inline void setPrototype(Prototype* prototype) {
		this->prototype = prototype;
	}

	/**
	 * @return reflection environment mapping id
	 */
	inline const string& getReflectionEnvironmentMappingId() {
		return reflectionEnvironmentMappingId;
	}

	/**
	 * @return reflection environment mapping id
	 */
	inline void setReflectionEnvironmentMappingId(const string& reflectionEnvironmentMappingId) {
		this->reflectionEnvironmentMappingId = reflectionEnvironmentMappingId;
	}

	/**
	 * @return merged properties from entity and object
	 */
	PrototypeProperties* getTotalProperties();

};
