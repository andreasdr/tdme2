#pragma once

#include <algorithm>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/utilities/fwd-tdme.h>

using std::find;
using std::vector;

namespace tdme {
namespace utilities {

/**
 * Object deleter
 * @author Andreas Drewke
 * @version $Id$
 */
template<typename T>
class ObjectDeleter final
{
private:
	vector<T*> objects;

public:
	/**
	 * Public constructor
	 */
	ObjectDeleter() {
	}

	/**
	 * Public destructor
	 */
	~ObjectDeleter() {
		for (auto object: objects) delete object;
		objects.clear();
	}

	/**
	 * Adds vector to iterator
	 * @param vector vector
	 */
	T* add(T* object) {
		if (find(objects.begin(), objects.end(), object) != objects.end()) return object;
		objects.push_back(object);
		return object;
	}

};

};
};
