#include <tdme/gui/GUIParserException.h>

#include <tdme/utilities/ExceptionBase.h>

using tdme::gui::GUIParserException;
using tdme::utilities::ExceptionBase;

GUIParserException::GUIParserException(const string& message) : ExceptionBase(message)
{
}

