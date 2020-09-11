#include <tdme/engine/fileio/models/ModelFileIOException.h>

#include <tdme/utilities/ExceptionBase.h>

using tdme::engine::fileio::models::ModelFileIOException;

using tdme::utilities::ExceptionBase;

ModelFileIOException::ModelFileIOException(const string& message) throw() : ExceptionBase(message) {
}
