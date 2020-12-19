#include <tdme/engine/subsystems/manager/VBOManager_VBOManaged.h>

#include <string>

#include <tdme/engine/subsystems/manager/VBOManager.h>

using std::string;

using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::manager::VBOManager_VBOManaged;

VBOManager_VBOManaged::VBOManager_VBOManaged(const string& id, vector<int32_t>& vboIds)
{
	this->id = id;
	this->vboIds = vboIds;
}
