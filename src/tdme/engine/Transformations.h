#pragma once

#include <vector>

#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/Rotation.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Quaternion.h>

using std::vector;

using tdme::engine::Rotation;
using tdme::engine::model::RotationOrder;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::math::Quaternion;

/**
 * Transformations which contain scale, rotations and translation
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::Transformations {
private:
	Vector3 translation;
	Vector3 scale;
	Vector3 pivot;
	Quaternion rotationsQuaternion;
	vector<Rotation> rotations;
	Matrix4x4 transformationsMatrix;

public:
	/**
	 * Public constructor
	 */
	Transformations();

	/**
	 * Destructor
	 */
	virtual ~Transformations();

	/**
	 * @return object translation
	 */
	inline const Vector3& getTranslation() const {
		return translation;
	}

	/**
	 * Set translation
	 * @param translation translation
	 */
	inline void setTranslation(const Vector3& translation) {
		this->translation.set(translation);
	}

	/**
	 * @return object scale
	 */
	inline const Vector3& getScale() const {
		return scale;
	}

	/**
	 * Set scale
	 * @param scale scale
	 */
	inline void setScale(const Vector3& scale) {
		this->scale.set(scale);
	}

	/**
	 * @return pivot or center of rotations
	 */
	inline const Vector3& getPivot() const {
		return pivot;
	}

	/**
	 * Set pivot
	 * @param pivot pivot
	 */
	inline void setPivot(const Vector3& pivot) {
		this->pivot.set(pivot);
	}

	/**
	 * @return rotation count
	 */
	inline const int getRotationCount() const {
		return rotations.size();
	}

	/**
	 * Get rotation at given index
	 * @param idx rotation index
	 * @return rotation
	 */
	inline Rotation& getRotation(const int idx) {
		return rotations[idx];
	}

	/**
	 * Add rotation
	 * @param axis axis
	 * @param angle angle
	 */
	inline void addRotation(const Vector3& axis, const float angle) {
		rotations.push_back(Rotation(axis, angle));
	}

	/**
	 * Remove rotation
	 * @param idx index
	 */
	inline void removeRotation(const int idx) {
		rotations.erase(rotations.begin() + idx);
	}

	/**
	 * @param idx rotation index
	 * @return rotation axis for rotation with given index
	 */
	inline const Vector3& getRotationAxis(const int idx) const {
		return rotations[idx].getAxis();
	}

	/**
	 * Set rotation axis
	 * @param idx rotation index
	 * @param axis rotation axis
	 */
	inline void setRotationAxis(const int idx, const Vector3& axis) {
		return rotations[idx].setAxis(axis);
	}

	/**
	 * @param idx rotation index
	 * @return rotation angle for rotation with given index
	 */
	inline const float getRotationAngle(const int idx) const {
		return rotations[idx].getAngle();
	}

	/**
	 * @param idx rotation index
	 * @param angle rotation angle
	 * @return rotation angle for rotation with given index
	 */
	inline void setRotationAngle(const int idx, const float angle) {
		rotations[idx].setAngle(angle);
	}

	/**
	 * @return rotations quaternion
	 */
	inline const Quaternion& getRotationsQuaternion() const {
		return rotationsQuaternion;
	}

	/**
	 * @return this transformations matrix
	 */
	inline const Matrix4x4& getTransformationsMatrix() const {
		return transformationsMatrix;
	}

	/**
	 * Set up this transformations from given transformations
	 * @param transformations transformations
	 */
	virtual void fromTransformations(const Transformations& transformations);

	/**
	 * Set up this transformations from given matrix and rotation order
	 * @param matrix matrix
	 * @param rotationOrder rotation order
	 */
	virtual void fromMatrix(const Matrix4x4& matrix, RotationOrder* rotationOrder);

	/**
	 * Computes transformation matrix
	 * @param parentTransformations parent transformations
	 */
	virtual void update();

	/**
	 * Apply parent transformations
	 * @param parentTransformations parent transformations
	 */
	virtual void applyParentTransformations(const Transformations& parentTransformations);

	/**
	 * Invert this transformations
	 */
	virtual void invert();

};
