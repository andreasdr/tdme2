// based on:
/*
 * Copyright (C)2015-2016 Haxe Foundation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <tdme/utilities/HashLink.h>

#if defined(HASHLINK)
	extern "C" {
		#include <hl.h>
		#include <hlmodule.h>
	}
#endif

#include <string>
#include <vector>

#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>

using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;

using tdme::utilities::Console;

using std::string;
using std::to_string;
using std::vector;

using tdme::utilities::HashLink;

bool HashLink::execute(const string& pathName, const string& fileName) {
	#if defined(HASHLINK)
		Console::println("HashLink::execute(): " + pathName + "/" + fileName + ": init");

		//
		typedef struct {
			hl_code* code;
			hl_module* m;
			vdynamic* ret;
			vclosure c;
			char* file;
			int file_time;
		} main_context;
		main_context ctx;
		bool hadException = false;

		//
		char* errorMessage;
		vector<uint8_t> byteCode;
		FileSystem::getInstance()->getContent(pathName, fileName, byteCode);

		//
		int argc = 1;
		char* argv[] = {"test script"};
		hl_global_init();
		hl_sys_init((void**)argv, argc, nullptr);
		hl_register_thread(&ctx);
		ctx.code = hl_code_read((unsigned char*)byteCode.data(), byteCode.size(), &errorMessage);
		if (ctx.code == nullptr) {
			Console::println("HashLink::execute(): " + pathName + "/" + fileName + ": failed to load code: " + errorMessage);
		};
		ctx.m = hl_module_alloc(ctx.code);
		if (ctx.m == nullptr) {
			Console::println("HashLink::execute(): " + pathName + "/" + fileName + ": failed allocate");
		}
		if (!hl_module_init(ctx.m, false)) return 3;
		hl_code_free(ctx.code);
		ctx.c.t = ctx.code->functions[ctx.m->functions_indexes[ctx.m->code->entrypoint]].type;
		ctx.c.fun = ctx.m->functions_ptrs[ctx.m->code->entrypoint];
		ctx.c.hasValue = 0;
		ctx.ret = hl_dyn_call_safe(&ctx.c, nullptr, 0, &hadException);
		if (hadException == true) {
			varray *a = hl_exception_stack();
			int i;
			Console::println("HashLink::execute(): " + pathName + "/" + fileName + ": Uncaught exception: " + (char*)hl_to_string(ctx.ret));
			for (i = 0;i < a->size; i++) Console::println(string("Called from ") + (char*)hl_aptr(a,uchar*)[i]);
			hl_debug_break();
			hl_global_free();
			return 1;
		}
		hl_module_free(ctx.m);
		hl_free(&ctx.code->alloc);
		hl_global_free();

		//
		Console::println("HashLink::execute(): " + pathName + "/" + fileName + ": done");

		//
		return true;
	#else
		Console::println("HashLink::execute(): " + pathName + "/" + fileName + ": TDME2 has been compiled without hashlink support");
		return false;
	#endif
}
