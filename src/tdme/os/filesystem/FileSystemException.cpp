#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/utilities/ExceptionBase.h>

using tdme::os::filesystem::FileSystemException;

FileSystemException::FileSystemException(const string& message) throw() : ExceptionBase(message) {
}
