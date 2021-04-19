CFLAGS = -Wno-multichar
STACKFLAGS =
OFLAGS =
LDFLAGS =
INCLUDES := $(INCLUDES) -Isrc -Iext -Iext/src -I. -Iext/reactphysics3d/src/ -Iext/rapidjson/include
CPPVERSION = -std=gnu++11
EXTRAFLAGS =

#EXTRAFLAGS := -D_GLIBCXX_DEBUG

# set platform specific flags
OSSHORT := $(shell sh -c 'uname -o 2>/dev/null')
OS := $(shell sh -c 'uname -s 2>/dev/null')
ARCH := $(shell sh -c 'uname -m 2>/dev/null')
ifeq ($(OS), Darwin)
	# MacOSX, Vulkan
	ifeq ($(VULKAN), YES)
		EXTRAFLAGS := -DVULKAN
		EXTRA_LIBS := -Lext/fbx/macosx/lib -lfbxsdk -ltdme2 -ltdme2-ext -Lext/glfw3/macosx/lib -l glfw3 -l vulkan.1 -framework Cocoa -framework IOKit -framework Carbon -framework OpenAL -pthread
	else
		# MacOSX, GL
		EXTRAFLAGS := -DGLFW3
		EXTRA_LIBS := -Lext/fbx/macosx/lib -lfbxsdk -ltdme2 -ltdme2-ext -Lext/glfw3/macosx/lib -l glfw3 -framework Cocoa -framework OpenGL -framework OpenCL -framework IOKit -framework Carbon -framework OpenAL -pthread
	endif
	INCLUDES := $(INCLUDES) -Iext/fbx/macosx/include -Iext/glfw3/include
	OFLAGS := -O2
else ifeq ($(OS), FreeBSD)
	# FreeBSD, Vulkan
	ifeq ($(VULKAN), YES)
		EXTRAFLAGS := -DVULKAN
		EXTRA_LIBS := -ltdme2 -ltdme2-ext -L/usr/local/lib -lglfw -lvulkan -lopenal -pthread
	else
		# FreeBSD, GL
		EXTRAFLAGS := -DGLFW3
		EXTRA_LIBS := -ltdme2 -ltdme2-ext -L/usr/local/lib -lglfw -lGLEW -lGL -lopenal -pthread
	endif
	INCLUDES := $(INCLUDES) -I/usr/local/include
	OFLAGS := -O2
else ifeq ($(OS), Linux)
	# Linux, Vulkan
	ifeq ($(VULKAN), YES)
		EXTRAFLAGS := -DVULKAN
		EXTRA_LIBS := -ltdme2 -ltdme2-ext -L/usr/lib64 -lglfw -lvulkan -lopenal -pthread
	else ifeq ($(GLES2), YES)
		# Linux, GLES2
		EXTRAFLAGS := -DGLFW3 -DGLES2
		EXTRA_LIBS := -ltdme2 -ltdme2-ext -L/usr/lib64 -lGLESv2 -lEGL -lglfw -lopenal -pthread 
	else
		# Linux, GL
		EXTRAFLAGS := -DGLFW3
		EXTRA_LIBS := -ltdme2 -ltdme2-ext -L/usr/lib64 -lGLEW -lGL -lglfw -lopenal -pthread
	endif
	OFLAGS := -O2
else
    INCLUDES := $(INCLUDES) -I/mingw64/include/
	ifeq ($(VULKAN), YES)
		EXTRAFLAGS := -DVULKAN
		EXTRA_LIBS := -L/mingw64/lib -ltdme2 -ltdme2-ext -lws2_32 -Lext/vulkan/runtime/mingw64 -lvulkan-1 -lglfw3 -lopenal -ldbghelp
	else
		EXTRAFLAGS := -DGLFW3
		EXTRA_LIBS := -L/mingw64/lib -ltdme2 -ltdme2-ext -lws2_32 -lglew32 -lopengl32 -lglfw3 -lopenal -ldbghelp
	endif 
	OFLAGS := -O2
endif

# set default application build type if not yet done
ifeq ($(APP_BUILD_TYPE),)
	override APP_BUILD_TYPE=0
endif

OFLAGS := 
CPPFLAGS := $(CPPFLAGS) $(INCLUDES)
CFLAGS := $(CFLAGS) $(OFLAGS) $(EXTRAFLAGS) -g -pipe -MMD -MP -DAPP_BUILD_TYPE=$(APP_BUILD_TYPE) -DNDEBUG
#CFLAGS := $(CFLAGS) $(OFLAGS) $(EXTRAFLAGS) -g -pipe -MMD -MP -DAPP_BUILD_TYPE=$(APP_BUILD_TYPE)
#CFLAGS := $(CFLAGS) $(OFLAGS) $(EXTRAFLAGS) -pipe -MMD -MP -DAPP_BUILD_TYPE=$(APP_BUILD_TYPE) -DNDEBUG
CXXFLAGS := $(CFLAGS) $(CPPVERSION)

BIN := bin
OBJ := obj

LIBS = lib
LIB = tdme2

SRC = src

SRCS = {$source-files}

MAIN_SRCS = {$main-source-files}

MAINS = $(MAIN_SRCS:$(SRC)/%-main.cpp=$(BIN)/%)
OBJS = $(SRCS:$(SRC)/%.cpp=$(OBJ)/%.o)

all: $(OBJS)

define cpp-command
@mkdir -p $(dir $@); 
@echo Compile $<; $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
endef

$(OBJS):$(OBJ)/%.o: $(SRC)/%.cpp | print-opts
	$(cpp-command)

ifeq ($(OSSHORT), Msys)
$(MAINS):$(BIN)/%:$(SRC)/%-main.cpp $(OBJS)
	@mkdir -p $(dir $@);
	@EXECUTABLE=$$(echo $1 | grep -o '[a-zA-Z0-9]*-main' | sed -e 's/\-main//');
	@../tdme2/scripts/windows-mingw-create-executable-rc.sh "$<" $@.rc
	@windres $@.rc -o coff -o $@.rc.o
	$(CXX) $(STACKFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -L$(BIN) -L$(LIBS) $(OBJS) -o $@ $@.rc.o $< -l$(LIB) $(EXTRA_LIBS)
else
$(MAINS):$(BIN)/%:$(SRC)/%-main.cpp $(OBJS)
	@mkdir -p $(dir $@);
	$(CXX) $(STACKFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -L$(BIN) -L$(LIBS) $(OBJS) -o $@ $< -l$(LIB) $(EXTRA_LIBS)
endif

mains: $(MAINS)

clean:
	rm -rf $(OBJ) $(BIN)

print-opts:
	@echo Building with \"$(CXX) $(CPPFLAGS) $(CXXFLAGS)\"
	
.PHONY: all mains clean print-opts

-include $(OBJS:%.o=%.d)
