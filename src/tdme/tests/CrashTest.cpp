#include <tdme/tests/CrashTest.h>

#include <tdme/application/Application.h>
#include <tdme/utilities/Console.h>

using tdme::tests::CrashTest;

using tdme::application::Application;
using tdme::utilities::Console;

class CrashTestClass {
public:
	static void crash() {
		char* i = nullptr;
		Console::println(string("CrashTestClass::crash(): message: ") + i);
	}
};

static void crashTestFunction() {
	CrashTestClass::crash();
}

void CrashTest::main()
{
	Application::installExceptionHandler();
	Console::println("CrashTest::main(): init");
	crashTestFunction();
	Console::println("CrashTest::main(): done");
}
