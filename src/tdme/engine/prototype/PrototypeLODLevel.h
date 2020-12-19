#pragma once

#include <string>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/LODObject3D.h>
#include <tdme/engine/LODObject3D.h>

using std::string;

using tdme::engine::model::Color4;
using tdme::engine::model::Model;
using tdme::engine::LODObject3D;

/**
 * Prototype LOD level definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::prototype::PrototypeLODLevel final
{
private:
	LODObject3D::LODLevelType type;
	string fileName;
	Model* model { nullptr };
	float minDistance;
	Color4 colorMul;
	Color4 colorAdd;

public:
	/**
	 * Public constructor
	 * @param type file name
	 * @param fileName model
	 * @param model min distance
	 * @param minDistance type
	 */
	PrototypeLODLevel(
		LODObject3D::LODLevelType type,
		const string& fileName,
		Model* model,
		float minDistance
	);

	/**
	 * Destructor
	 */
	~PrototypeLODLevel();

	/**
	 * @return type
	 */
	inline LODObject3D::LODLevelType getType() const {
		return type;
	}

	/**
	 * Set type
	 * @param type type
	 */
	inline void setType(LODObject3D::LODLevelType type) {
		this->type = type;
	}

	/**
	 * @return file name
	 */
	inline const string& getFileName() {
		return fileName;
	}

	/**
	 * Set file name
	 * @param fileName file name
	 */
	inline void setFileName(const string& fileName) {
		this->fileName = fileName;
	}

	/**
	 * @return model
	 */
	inline Model* getModel() {
		return model;
	}

	/**
	 * Set model
	 * @param model model
	 */
	inline void setModel(Model* model) {
		if (this->model == model) return;
		if (this->model != nullptr) delete this->model;
		this->model = model;
	}

	/**
	 * @return min distance
	 */
	inline float getMinDistance() const {
		return minDistance;
	}

	/**
	 * Set min distance
	 * @param minDistance min distance
	 */
	inline void setMinDistance(float minDistance) {
		this->minDistance = minDistance;
	}

	/**
	 * @return color add
	 */
	inline const Color4& getColorAdd() const {
		return colorAdd;
	}

	/**
	 * Set color add
	 * @param colorAdd color add
	 */
	inline void setColorAdd(const Color4& colorAdd) {
		this->colorAdd = colorAdd;
	}

	/**
	 * @return color mul
	 */
	inline const Color4& getColorMul() const {
		return colorMul;
	}

	/**
	 * Set color mul
	 * @param colorMul color mul
	 */
	inline void setColorMul(const Color4& colorMul) {
		this->colorMul = colorMul;
	}

};
