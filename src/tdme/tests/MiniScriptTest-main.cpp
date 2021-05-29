#include <tdme/utilities/MiniScript.h>

#include <tdme/utilities/Console.h>
#include <tdme/utilities/MiniScript.h>

using tdme::utilities::MiniScript;

using tdme::utilities::Console;

int main(int argc, char *argv[]) {
	Console::println("MiniScript");
	MiniScript* script = new MiniScript();
	script->loadScript("resources/tests/scripts", "test.tscript");
	while (script->isRunning() == true) {
		script->executeStateMachine();
	}
	delete script;
	return 0;
}

