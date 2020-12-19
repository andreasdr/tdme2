#pragma once

#include <stdint.h>

#include <exception>
#include <sstream>
#include <string>

#include <tdme/tdme.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/Reference.h>

using std::string;

using tdme::utilities::Exception;
using tdme::utilities::Reference;

namespace tdme {
namespace network {
namespace udpserver {

/**
 * Base class for network server group
 * @author Andreas Drewke
 */
class ServerGroupBase : public Reference {
friend class ServerWorkerThread;

protected:
	/*
	 * @brief event method called if group will be initiated, will be called from worker
	 */
	virtual void onInit() = 0;

	/*
	 * @brief event method called if client will be closed, will be called from worker
	 */
	virtual void onClose() = 0;

	/*
	 * @brief custom event method called if fired, will be called from worker
	 */
	virtual void onCustomEvent(const string& type) = 0;
};

};
};
};
