#include <tdme/tools/shared/controller/FileDialogPath.h>


using tdme::tools::shared::controller::FileDialogPath;

FileDialogPath::FileDialogPath()
{
	this->path = ".";
}

const string& FileDialogPath::getPath()
{
	return path;
}

void FileDialogPath::setPath(const string& path)
{
	this->path = path;
}
