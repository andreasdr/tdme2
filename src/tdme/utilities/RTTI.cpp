#include <tdme/utilities/RTTI.h>

#if defined(_WIN32) && defined(_MSC_VER)
	// no op
#else
	#include <cxxabi.h>
	#include <stdlib.h>
#endif

using tdme::utilities::RTTI;

const std::string RTTI::demangle(const char* name) {
	#if defined(_WIN32) && defined(_MSC_VER)
		auto demangledNameString = name;
		return demangledNameString;
	#else
		int status;
		char* demangledName = abi::__cxa_demangle(name, 0, 0, &status);
		std::string demangledNameString(demangledName == nullptr?name:demangledName);
		free(demangledName);
		return demangledNameString;
	#endif
}
