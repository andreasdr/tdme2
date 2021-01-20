#include <tdme/gui/events/GUIKeyboardEvent.h>

using tdme::gui::events::GUIKeyboardEvent;

int32_t GUIKeyboardEvent::getKeyCodeFromChar(char key) {
	#if defined(VULKAN) || defined(GLFW3)
		return -1;
	#else
		switch(key) {
			case(9): return KEYBOARD_KEYCODE_TAB;
			case(13): return KEYBOARD_KEYCODE_RETURN;
			case(25): return KEYBOARD_KEYCODE_TAB_SHIFT;
			case(27): return KEYBOARD_KEYCODE_ESCAPE;
			case(32): return KEYBOARD_KEYCODE_SPACE;
			#if defined(__APPLE__)
				case(8): return KEYBOARD_KEYCODE_DELETE;
				case(127): return KEYBOARD_KEYCODE_BACKSPACE;
			#else
				case(127): return KEYBOARD_KEYCODE_DELETE;
				case(8): return KEYBOARD_KEYCODE_BACKSPACE;
			#endif
			default: return -1;
		}
	#endif
}

GUIKeyboardEvent::GUIKeyboardEvent()
{
	this->time = -1LL;
	this->type = GUIKeyboardEventType::KEYBOARDEVENT_NONE;
	this->keyCode = -1;
	this->keyChar = 0;
	this->metaDown = false;
	this->controlDown = false;
	this->altDown = false;
	this->shiftDown = false;
	this->processed = false;
}

GUIKeyboardEvent::~GUIKeyboardEvent() {
}
