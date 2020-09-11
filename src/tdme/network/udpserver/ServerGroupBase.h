#pragma once

#include <stdint.h>

#include <exception>
#include <string>
#include <sstream>

#include <tdme/tdme.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ReferenceCounter.h>

using std::string;

using tdme::utilities::Exception;
using tdme::utilities::ReferenceCounter;

namespace tdme {
namespace network {
namespace udpserver {

/**
 * Base class for network server group
 * @author Andreas Drewke
 */
class ServerGroupBase : public ReferenceCounter {
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
