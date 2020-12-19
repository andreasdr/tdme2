
#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>

using std::string;

using tdme::engine::model::Model;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::prototype::Prototype;
using tdme::math::Vector3;

/**
 * Prototype bounding volume definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::prototype::PrototypeBoundingVolume final
{
private:
	static volatile unsigned int modelIdx;
	int id;
	Prototype* prototype { nullptr };
	string modelMeshFile;
	Model* model { nullptr };
	BoundingVolume* boundingVolume { nullptr };

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param prototype prototype
	 */
	PrototypeBoundingVolume(int id, Prototype* prototype);

	/**
	 * Destructor
	 */
	~PrototypeBoundingVolume();

	/**
	 * @return id
	 */
	inline int getId() {
		return id;
	}

	/**
	 * @return model mesh file
	 */
	inline const string& getModelMeshFile() {
		return modelMeshFile;
	}

	/**
	 * @return model
	 */
	inline Model* getModel() {
		return model;
	}

	/**
	 * @return bounding volume
	 */
	inline BoundingVolume* getBoundingVolume() {
		return boundingVolume;
	}

	/**
	 * @return bounding volume model index
	 */
	int allocateModelIdx();

	/**
	 * Setup bounding volume none
	 */
	void setupNone();

	/**
	 * Setup bounding volume sphere
	 * @param center center
	 * @param radius radius
	 */
	void setupSphere(const Vector3& center, float radius);

	/**
	 * Setup bounding volume capsule
	 * @param a a
	 * @param b b
	 * @param radius radius
	 */
	void setupCapsule(const Vector3& a, const Vector3& b, float radius);

	/**
	 * Setup bounding volume oriented bounding box
	 * @param center center
	 * @param axis0 axis 0
	 * @param axis1 axis 1
	 * @param axis2 axis 2
	 * @param halfExtension half extension
	 */
	void setupObb(const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, const Vector3& halfExtension);

	/**
	 * Setup bounding volume bounding box
	 * @param min min
	 * @param max max
	 */
	void setupAabb(const Vector3& min, const Vector3& max);

	/**
	 * Setup bounding volume sphere
	 * @param pathName path name
	 * @param fileName file name
	 */
	void setupConvexMesh(const string& pathName, const string& fileName);

private:

	/**
	 * Update prototype
	 */
	void updatePrototype();

};
