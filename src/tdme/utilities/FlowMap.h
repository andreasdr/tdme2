#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/FlowMapCell.h>
#include <tdme/utilities/ReferenceCounter.h>

using std::map;
using std::string;
using std::to_string;
using std::vector;

using tdme::math::Math;
using tdme::math::Vector3;
using tdme::utilities::Console;
using tdme::utilities::FlowMapCell;
using tdme::utilities::ReferenceCounter;

/**
 * Flow map
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::utilities::FlowMap final: public ReferenceCounter {
friend class PathFinding;
private:
	bool complete;
	float stepSize;
	map<string, FlowMapCell> cells;
	vector<Vector3> endPositions;
	vector<Vector3> path;

	/**
	 * Private destructor
	 */
	inline ~FlowMap() {
	}

	/**
	 * Set flow map complete flag
	 * @param complete complete
	 */
	inline void setComplete(bool complete) {
		this->complete = complete;
	}

	/**
	 * Adds a cell to flow map
	 * @param id id
	 * @param position position
	 * @param walkable walkable
	 * @param direction direction
	 */
	inline void addCell(const string& id, const Vector3& position, bool walkable, const Vector3& direction) {
		cells[id] = FlowMapCell(position, walkable, direction);
	}

	/**
	 * Checks if a cell exists in flow map
	 * @param id id
	 */
	inline bool hasCell(const string& id) const {
		auto cellIt = cells.find(id);
		return cellIt != cells.end();
	}

public:
	/**
	 * Return string representation of given x,z for flow map cell id
	 * @param x x
	 * @param z z
	 * @return string representation
	 */
	inline string toId(float x, float z) const {
		return toId(x, z, stepSize);
	}

	/**
	 * Return string representation of given x,z for flow map cell id
	 * @param x x
	 * @param z z
	 * @param stepSize step size
	 * @return string representation
	 */
	inline static string toId(float x, float z, float stepSize) {
		string result;
		int32_t value = 0;
		result.reserve(sizeof(value) * 2);
		value = static_cast<int>(Math::ceil(x / stepSize));
		result.append((char*)&value, sizeof(value));
		value = static_cast<int>(Math::ceil(z / stepSize));
		result.append((char*)&value, sizeof(value));
		return result;
	}

	/**
	 * Align position component
	 * @param value value which is usually a position vector 3 position component
	 * @param stepSize step size
	 */
	inline static float alignPositionComponent(float value, float stepSize) {
		return Math::floor(value / stepSize) * stepSize;
	}

	/**
	 * Align position component
	 * @param value value which is usually a position vector 3 position component
	 */
	inline float alignPositionComponent(float value) const {
		return alignPositionComponent(value, stepSize);
	}

	/**
	 * Returns integer position component
	 * @param value value
	 * @return integer position component
	 */
	inline int getIntegerPositionComponent(float value) const {
		return static_cast<int>(alignPositionComponent(value, stepSize) / stepSize);
	}

	/**
	 * Returns integer position component
	 * @param value value
	 * @param stepSize step size
	 * @return integer position component
	 */
	inline static int getIntegerPositionComponent(float value, float stepSize) {
		return static_cast<int>(alignPositionComponent(value, stepSize) / stepSize);
	}

	/**
	 * Return string representation of given x,z integer flow map position representation for flow map cell id
	 * @param x x
	 * @param z z
	 * @return string representation
	 */
	inline static string toIdInt(int x, int z) {
		string result;
		int32_t value = 0;
		result.reserve(sizeof(x) * 2);
		result.append((char*)&x, sizeof(x));
		result.append((char*)&z, sizeof(z));
		return result;
	}

	/**
	 * Constructor
	 * @param path path
	 * @param endPositions end positions
	 * @param stepSize step size
	 */
	inline FlowMap(const vector<Vector3>& path, const vector<Vector3>& endPositions, float stepSize, bool complete = true): path(path), endPositions(endPositions), stepSize(stepSize), complete(complete) {
	}

	/**
	 * @return if flow map is complete
	 */
	inline bool isComplete() const {
		return complete;
	}

	/**
	 * @return step size
	 */
	inline float getStepSize() const {
		return stepSize;
	}

	/**
	 * Returns end positions
	 * @return end positions
	 */
	inline const vector<Vector3>& getEndPositions() const {
		return endPositions;
	}

	/**
	 * Returns path flow map is generated on
	 * @return path
	 */
	inline const vector<Vector3>& getPath() const {
		return path;
	}

	/**
	 * Get cell by id
	 * @param id id
	 * @return cell
	 */
	inline const FlowMapCell* getCell(const string& id) const {
		auto cellIt = cells.find(id);
		if (cellIt == cells.end()) return nullptr;
		return &cellIt->second;
	}

	/**
	 * Get cell by id
	 * @param id id
	 * @return cell
	 */
	inline FlowMapCell* getCell(const string& id) {
		auto cellIt = cells.find(id);
		if (cellIt == cells.end()) return nullptr;
		return &cellIt->second;
	}

	/**
	 * Get cell by position
	 * @param x x
	 * @param z z
	 * @return cell
	 */
	inline const FlowMapCell* getCell(float x, float z) const {
		auto id = toId(
			alignPositionComponent(x),
			alignPositionComponent(z)
		);
		auto cellIt = cells.find(id);
		if (cellIt == cells.end()) return nullptr;
		return &cellIt->second;
	}

	/**
	 * Get cell by position
	 * @param x x
	 * @param z z
	 * @return cell
	 */
	inline FlowMapCell* getCell(float x, float z) {
		auto cellId = toId(
			alignPositionComponent(x),
			alignPositionComponent(z)
		);
		auto cellIt = cells.find(cellId);
		if (cellIt == cells.end()) return nullptr;
		return &cellIt->second;
	}

	/**
	 * Cell map getter
	 * @returns cell map
	 */
	inline const map<string, FlowMapCell>& getCellMap() const {
		return cells;
	}

	/**
	 * Merge given flow map into this flow map, please note that given flow map step size needs to match this flow maps step size
	 * This only applies to a series of flow maps created sequentially and in correct order along a path
	 * @param flowMap flow map
	 */
	inline void merge(const FlowMap* flowMap) {
		// complete
		complete = flowMap->complete;
		// add path
		for (auto& pathNode: flowMap->path) {
			path.push_back(pathNode);
		}
		// add cells
		for (auto& cellIt: flowMap->cells) {
			cells[cellIt.first] = cellIt.second;
		}
		// end positions
		endPositions = flowMap->endPositions;
	}

};
