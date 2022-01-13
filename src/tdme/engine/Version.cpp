#include <tdme/engine/Version.h>

#include <string>

#include <tdme/tdme.h>

using std::string;

using tdme::engine::Version;

string Version::getVersion() {
	return "1.1.115";
}

string Version::getCopyright() {
	return "Developed 2012-2021 by Andreas Drewke, Dominik Hepp, Mathias Lenz, Kolja Gumpert, drewke.net, mindty.com, github.com/mahula. Please see the license @ https://github.com/andreasdr/tdme2/blob/master/LICENSE";
}