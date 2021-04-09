#if defined(VULKAN) || defined(GLFW3)
	#include <tdme/application/Application.h>
	using tdme::application::Application;
#else
	#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__linux__) || defined(_WIN32)
		#include <GL/freeglut.h>
	#elif defined(__APPLE__)
		#include <GLUT/glut.h>
	#elif defined(__HAIKU__)
		#include <GL/glut.h>
	#endif
#endif

#include "InputEventHandler.h"

using tdme::application::InputEventHandler;

InputEventHandler::~InputEventHandler() {
}

int InputEventHandler::getKeyboardModifiers() {
	#if defined(VULKAN) || defined(GLFW3)
		return 0;
	#else
		return glutGetModifiers();
	#endif
}
