#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/prototype/fwd-tdme.h>
#include <tdme/engine/Transformations.h>

using std::unordered_map;
using std::unordered_set;
using std::vector;

using tdme::engine::prototype::Prototype;

using tdme::engine::Transformations;

/**
 * Prototype terrain definition
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::prototype::PrototypeTerrain final
{
private:
	float width { -1 };
	float depth { -1 };
	vector<float> terrainHeightVector;
	int waterPositionMapIdx { 0 };
	unordered_map<int, float> waterPositionMapsHeight;
	unordered_map<int, unordered_map<int, unordered_set<int>>> waterPositionMaps;
	int foliagePrototypeMapIdx { 0 };
	unordered_map<Prototype*, int> foliagePrototypeFoliageMap;
	unordered_map<int, Prototype*> foliageFoliagePrototypeMap;
	vector<unordered_map<int, vector<Transformations>>> foliageMaps;

public:

	/**
	 * Public constructor
	 */
	inline PrototypeTerrain() {
	}

	/**
	 * Destructor
	 */
	~PrototypeTerrain() {
		for (auto& foliagePrototypeFoliageMapIt: foliagePrototypeFoliageMap) {
			delete foliagePrototypeFoliageMapIt.first;
		}
	}

	/**
	 * @return width
	 */
	inline float getWidth() const {
		return width;
	}

	/**
	 * Set width
	 * @param width width
	 */
	inline void setWidth(float width) {
		this->width = width;
	}

	/**
	 * @return depth
	 */
	inline float getDepth() const {
		return depth;
	}

	/**
	 * Set depth
	 * @param depth depth
	 */
	inline void setDepth(float depth) {
		this->depth = depth;
	}

	/**
	 * @return terrain height vector
	 */
	inline vector<float>& getHeightVector() {
		return terrainHeightVector;
	}

	/**
	 * Get water position maps indices
	 * @return water position maps indices
	 */
	inline vector<int> getWaterPositionMapsIndices() {
		vector<int> waterPositionMapsIndices;
		for (auto waterPositionMapsIt: waterPositionMaps) {
			waterPositionMapsIndices.push_back(waterPositionMapsIt.first);
		}
		return waterPositionMapsIndices;
	}

	/**
	 * @return allocated water position map idx
	 */
	inline int allocateWaterPositionMapIdx() {
		return waterPositionMapIdx++;
	}

	/**
	 * Get water position map height at given water position map index
	 * @param idx water position map index
	 * @return water position map height at given water position map index
	 */
	inline float getWaterPositionMapHeight(int idx) {
		return waterPositionMapsHeight[idx];
	}

	/**
	 * Set water position map height at given water position map index
	 * @param idx water position map index
	 * @param waterHeight waterHeight water height
	 */
	inline void setWaterPositionMapHeight(int idx, float waterHeight) {
		waterPositionMapsHeight[idx] = waterHeight;
	}

	/**
	 * Get water position map at given index
	 * @param idx index
	 * @return water position map
	 */
	inline unordered_map<int, unordered_set<int>>& getWaterPositionMap(int idx) {
		return waterPositionMaps[idx];
	}

	/**
	 * Remove water position map at given water position map index
	 * @param idx water position map index
	 */
	inline void removeWaterPositionMap(int idx) {
		waterPositionMapsHeight.erase(idx);
		waterPositionMaps.erase(idx);
	}

	/**
	 * Get or allocate a foliage prototype index
	 * @param prototype prototype
	 * @return prototype index
	 */
	inline int getFoliagePrototypeIndex(Prototype* prototype) {
		auto foliagePrototypeIt = foliagePrototypeFoliageMap.find(prototype);
		if (foliagePrototypeIt == foliagePrototypeFoliageMap.end()) {
			foliagePrototypeFoliageMap[prototype] = foliagePrototypeMapIdx;
			foliageFoliagePrototypeMap[foliagePrototypeMapIdx] = prototype;
			return foliagePrototypeMapIdx++;
		}
		return foliagePrototypeIt->second;
	}

	/**
	 * Get foliage prototype indices
	 * @return foliage prototype indices
	 */
	inline vector<int> getFoliagePrototypeIndices() {
		vector<int> foliagePrototypeIndices;
		for (auto& foliageFoliagePrototypeMapIt: foliageFoliagePrototypeMap) {
			foliagePrototypeIndices.push_back(foliageFoliagePrototypeMapIt.first);
		}
		return foliagePrototypeIndices;
	}

	/**
	 * Get foliage prototype by given index
	 * @param idx index
	 * @return prototype or nullptr
	 */
	inline Prototype* getFoliagePrototype(int idx) {
		auto foliageFoliagePrototypeIt = foliageFoliagePrototypeMap.find(idx);
		if (foliageFoliagePrototypeIt == foliageFoliagePrototypeMap.end()) {
			return nullptr;
		}
		return foliageFoliagePrototypeIt->second;
	}

	/**
	 * @return foliage maps
	 */
	inline vector<unordered_map<int, vector<Transformations>>>& getFoliageMaps() {
		return foliageMaps;
	}

};
