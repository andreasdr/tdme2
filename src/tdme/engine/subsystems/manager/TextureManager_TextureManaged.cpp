#include <tdme/engine/subsystems/manager/TextureManager_TextureManaged.h>

#include <string>

#include <tdme/engine/subsystems/manager/TextureManager.h>

using std::string;

using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::manager::TextureManager_TextureManaged;

TextureManager_TextureManaged::TextureManager_TextureManaged(const string& id, int32_t rendererId)
{
	this->id = id;
	this->rendererId = rendererId;
}
