/**
 * @version $Id: 4e34499120297e23dd0002a1f2f70a59cbfdfe7d $
 */

#include <tdme/os/network/NetworkSocketClosedException.h>

using tdme::os::network::NetworkSocketClosedException;

NetworkSocketClosedException::NetworkSocketClosedException(const string &message) throw() : NetworkIOException(message) {
}

