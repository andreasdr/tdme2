#include <tdme/utilities/Properties.h>

#include <map>
#include <vector>
#include <string>

#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/StringTools.h>

using std::map;
using std::vector;
using std::string;

using tdme::utilities::Properties;

using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::StringTools;

Properties::Properties()
{
}

const string& Properties::get(const string& key, const string& defaultValue)
{
	auto it = properties.find(key);
	if (it == properties.end()) return defaultValue;
	return it->second;
}

void Properties::put(const string& key, const string& value)
{
	properties[key] = value;
}

void Properties::load(const string& pathName, const string& fileName, FileSystemInterface* fileSystem)
{
	properties.clear();
	vector<string> lines;
	if (fileSystem == nullptr) fileSystem = FileSystem::getInstance();
	fileSystem->getContentAsStringArray(pathName, fileName, lines);
	for (auto i = 0; i < lines.size(); i++) {
		string line = StringTools::trim(lines[i]);
		if (line.length() == 0 || StringTools::startsWith(line, "#")) continue;
		auto separatorPos = line.find(L'=');
		if (separatorPos == -1) continue;
		string key = StringTools::substring(line, 0, separatorPos);
		string value = StringTools::substring(line, separatorPos + 1);
		properties[key] = value;
	}
}

void Properties::store(const string& pathName, const string& fileName, FileSystemInterface* fileSystem) {
	vector<string> result;
	int32_t idx = 0;
	for (auto it = properties.begin(); it != properties.end(); ++it) {
		string key = it->first;
		string value = it->second;
		result.push_back(key + "=" + value);
	}
	if (fileSystem == nullptr) fileSystem = FileSystem::getInstance();
	fileSystem->setContentFromStringArray(pathName, fileName, result);
}
