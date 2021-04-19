NAME = tdme2
LIB := lib$(NAME).a
EXT_LIB := lib$(NAME)-ext.a

BIN = bin
LIB_DIR = lib
OBJ = obj
OBJ_DEBUG = obj-debug

CPPVERSION = -std=gnu++11
SRCS_PLATFORM =
EXT_GLSLANG_PLATFORM_SRCS =
SRCS_DEBUG =
OFLAGS =
EXTRAFLAGS =
LDFLAGS =
INCLUDES = -Isrc -Iext -I. -Iext/v-hacd/src/VHACD_Lib/inc/ -Iext/reactphysics3d/src/ -Iext/rapidjson/include

# set platform specific flags
OSSHORT := $(shell sh -c 'uname -o 2>/dev/null')
OS := $(shell sh -c 'uname -s 2>/dev/null')
ARCH := $(shell sh -c 'uname -m 2>/dev/null')
ifeq ($(OS), Darwin)
	# Mac OS X
	INCLUDES := $(INCLUDES) -Iext/fbx/macosx/include -Iext/glfw3/include
	# MacOSX, Metal via Vulkan
	ifeq ($(VULKAN), YES)
		EXTRAFLAGS := -DVULKAN -DHAVE_UNISTD_H
		INCLUDES := $(INCLUDES) -Iext\vulkan\vma\src
		SRCS_PLATFORM := $(SRCS_PLATFORM) \
				src/tdme/os/network/platform/bsd/KernelEventMechanism.cpp \
				src/tdme/engine/EngineVKRenderer.cpp \
				src/tdme/engine/subsystems/renderer/VKRenderer.cpp \
				src/tdme/engine/fileio/models/FBXReader.cpp \
				src/tdme/engine/fileio/models/ModelReaderFBX.cpp
		EXT_GLSLANG_PLATFORM_SRCS = \
			ext/vulkan/glslang/OSDependent/Unix/ossource.cpp
		EXTRA_LIBS := -Lext/fbx/macosx/lib -lfbxsdk -Lext/glfw3/macosx/lib -l glfw3 -l vulkan.1 -l$(NAME)-ext -framework Cocoa -framework IOKit -framework Carbon -framework OpenAL
	else
		# MacOSX, GL
		EXTRAFLAGS := -DGLFW3 -DHAVE_UNISTD_H
		SRCS_PLATFORM := $(SRCS_PLATFORM) \
			src/tdme/os/network/platform/bsd/KernelEventMechanism.cpp \
			src/tdme/engine/EngineGL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/SingleThreadedRenderer.cpp \
			src/tdme/engine/fileio/models/FBXReader.cpp \
			src/tdme/engine/fileio/models/ModelReaderFBX.cpp
		EXTRA_LIBS := -Lext/fbx/macosx/lib -lfbxsdk -l$(NAME)-ext -Lext/glfw3/macosx/lib -l glfw3 -framework Cocoa -framework OpenGL -framework OpenCL -framework IOKit -framework Carbon -framework OpenAL
	endif
	OFLAGS := -O2
else ifeq ($(OS), FreeBSD)
	# FreeBSD
	INCLUDES := $(INCLUDES) -I/usr/local/include
	# FreeBSD, Vulkan
	ifeq ($(VULKAN), YES)
		EXTRAFLAGS := -DVULKAN
		INCLUDES := $(INCLUDES) -Iext\vulkan\vma\src
		SRCS_PLATFORM := $(SRCS_PLATFORM) \
				src/tdme/os/network/platform/bsd/KernelEventMechanism.cpp \
				src/tdme/engine/EngineVKRenderer.cpp \
				src/tdme/engine/subsystems/renderer/VKRenderer.cpp \
				src/tdme/engine/fileio/models/ModelReader.cpp
		EXT_GLSLANG_PLATFORM_SRCS = \
			ext/vulkan/glslang/OSDependent/Unix/ossource.cpp
		EXTRA_LIBS := -l$(NAME) -l$(NAME)-ext -l$(NAME) -l$(NAME)-ext -L/usr/local/lib -lglfw -lvulkan -lopenal -pthread
	else
		EXTRAFLAGS := -DGLFW3
		#FreeBSD, GL
		SRCS_PLATFORM := $(SRCS_PLATFORM) \
			src/tdme/os/network/platform/bsd/KernelEventMechanism.cpp \
			src/tdme/engine/EngineGL2Renderer.cpp \
			src/tdme/engine/EngineGL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL2Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/SingleThreadedRenderer.cpp \
			src/tdme/engine/fileio/models/ModelReader.cpp
		EXTRA_LIBS := -l$(NAME) -l$(NAME)-ext -l$(NAME) -l$(NAME)-ext -L/usr/local/lib -lglfw -lGLEW -lGL -lopenal -pthread
	endif
	OFLAGS := -O2
else ifeq ($(OS), NetBSD)
	# NetBSD
	INCLUDES := $(INCLUDES) -I/usr/X11R7/include -I/usr/pkg/include
	SRCS_PLATFORM := $(SRCS_PLATFORM) \
			src/tdme/os/network/platform/bsd/KernelEventMechanism.cpp \
			src/tdme/engine/EngineGL2Renderer.cpp \
			src/tdme/engine/EngineGL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL2Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/SingleThreadedRenderer.cpp \
			src/tdme/engine/fileio/models/ModelReader.cpp
	EXTRA_LIBS := -l$(NAME) -l$(NAME)-ext -l$(NAME) -l$(NAME)-ext -L/usr/X11R7/lib -L/usr/pkg/lib -lGLEW -lGL -lfreeglut -lopenal -pthread
	OFLAGS := -O2
else ifeq ($(OS), OpenBSD)
	# OpenBSD
	INCLUDES := $(INCLUDES) -I/usr/X11R6/include -I/usr/local/include
	EXTRAFLAGS := -DGLFW3
	SRCS_PLATFORM := $(SRCS_PLATFORM) \
			src/tdme/os/network/platform/bsd/KernelEventMechanism.cpp \
			src/tdme/engine/EngineGL2Renderer.cpp \
			src/tdme/engine/EngineGL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL2Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/SingleThreadedRenderer.cpp \
			src/tdme/engine/fileio/models/ModelReader.cpp
	EXTRA_LIBS := -l$(NAME) -l$(NAME)-ext -l$(NAME) -l$(NAME)-ext -L/usr/X11R6/lib -L/usr/local/lib -lm -lstdc++ -lglfw -lGLEW -lGL -lopenal -pthread
	OFLAGS := -O2
else ifeq ($(OS), Haiku)
	# Haiku
	INCLUDES := $(INCLUDES) -I/boot/system/develop/headers
	SRCS_PLATFORM := $(SRCS_PLATFORM) \
			src/tdme/os/network/platform/fallback/KernelEventMechanism.cpp \
			src/tdme/engine/EngineGL2Renderer.cpp \
			src/tdme/engine/EngineGL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL2Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/SingleThreadedRenderer.cpp \
			src/tdme/engine/fileio/models/ModelReader.cpp
	EXTRA_LIBS := -l$(NAME) -l$(NAME)-ext -l$(NAME) -l$(NAME)-ext -lGLEW -lGL -lglut -lopenal -lnetwork
	OFLAGS := -O2
else ifeq ($(OS), Linux)
	SRCS_PLATFORM := $(SRCS_PLATFORM) \
		src/tdme/os/network/platform/linux/KernelEventMechanism.cpp \
		src/tdme/engine/fileio/models/ModelReader.cpp
	# Linux, Vulkan
	ifeq ($(VULKAN), YES)
		EXTRAFLAGS := -DVULKAN
		INCLUDES := $(INCLUDES) -Iext\vulkan\vma\src
		SRCS_PLATFORM := $(SRCS_PLATFORM) \
			src/tdme/engine/EngineVKRenderer.cpp \
			src/tdme/engine/subsystems/renderer/VKRenderer.cpp
		EXT_GLSLANG_PLATFORM_SRCS = \
			ext/vulkan/glslang/OSDependent/Unix/ossource.cpp
		EXTRA_LIBS := -l$(NAME) -l$(NAME)-ext -l$(NAME) -l$(NAME)-ext -L/usr/lib64 -lglfw -lvulkan -lopenal -pthread
	# Linux, GLES2
	else ifeq ($(GLES2), YES)
		EXTRAFLAGS := -DGLFW3 -DGLES2
		# Linux, ARM, GL
		SRCS_PLATFORM := $(SRCS_PLATFORM) \
			src/tdme/engine/EngineGLES2Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GLES2Renderer.cpp \
			src/tdme/engine/subsystems/renderer/SingleThreadedRenderer.cpp
		EXTRA_LIBS := -l$(NAME) -l$(NAME)-ext -l$(NAME) -l$(NAME)-ext -L/usr/lib64 -L/usr/local/lib -lGLESv2 -lEGL -lglfw -lopenal -pthread 
	else
		# Linux, GL
		#EXTRAFLAGS = -D_GLIBCXX_DEBUG
		EXTRAFLAGS := -DGLFW3
		SRCS_PLATFORM:= $(SRCS_PLATFORM) \
			src/tdme/engine/EngineGL2Renderer.cpp \
			src/tdme/engine/EngineGL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL2Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/SingleThreadedRenderer.cpp
		EXTRA_LIBS := -l$(NAME) -l$(NAME)-ext -l$(NAME) -l$(NAME)-ext -L/usr/lib64 -lGLEW -lGL -lglfw -lopenal -pthread
	endif
	OFLAGS := -O2
else
	# Windows, VULKAN
	# TODO: No console flags: -Wl,-subsystem,windows
	ifeq ($(VULKAN), YES)
		EXTRAFLAGS = -DVULKAN
		INCLUDES := $(INCLUDES) -Iext\vulkan\vma\src
		#-D_GLIBCXX_DEBUG
		SRCS_PLATFORM:= $(SRCS_PLATFORM) \
				src/tdme/os/network/platform/fallback/KernelEventMechanism.cpp \
				src/tdme/engine/EngineVKRenderer.cpp \
				src/tdme/engine/subsystems/renderer/VKRenderer.cpp \
				src/tdme/engine/fileio/models/ModelReader.cpp
		EXT_GLSLANG_PLATFORM_SRCS = \
				ext/vulkan/glslang/OSDependent/Windows/ossource.cpp
		INCLUDES := $(INCLUDES) -Isrc -Iext -Iext/src -I/mingw64/include
		EXTRA_LIBS := -L/mingw64/lib -lws2_32 -Lext/vulkan/runtime/mingw64 -lvulkan-1 -lglfw3 -lopenal -ldbghelp -l$(NAME) -l$(NAME)-ext
	else
		EXTRAFLAGS = -DGLFW3
		# Windows, GL
		SRCS_PLATFORM:= $(SRCS_PLATFORM) \
			src/tdme/os/network/platform/fallback/KernelEventMechanism.cpp \
			src/tdme/engine/EngineGL2Renderer.cpp \
			src/tdme/engine/EngineGL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL2Renderer.cpp \
			src/tdme/engine/subsystems/renderer/GL3Renderer.cpp \
			src/tdme/engine/subsystems/renderer/SingleThreadedRenderer.cpp \
			src/tdme/engine/fileio/models/ModelReader.cpp
		INCLUDES := $(INCLUDES) -Isrc -Iext -Iext/src -I/mingw64/include/
		EXTRA_LIBS := -L/mingw64/lib -lws2_32 -lglew32 -lopengl32 -lglfw3 -lopenal -ldbghelp -l$(NAME) -l$(NAME)-ext
	endif
	OFLAGS := -O2
endif

ifeq ($(HASHLINK), YES)
	EXT_HL_SRCS = \
		ext/hashlink/src/code.c \
		ext/hashlink/src/jit.c \
		ext/hashlink/src/module.c \
		ext/hashlink/src/debugger.c \

	HL_CFLAGS = -O2 -std=c11 -I ext/hashlink/src -I include/pcre -I include/mikktspace -g -D LIBHL_EXPORTS
	EXTRAFLAGS := $(EXTRAFLAGS) -DHASHLINK
	INCLUDES := $(INCLUDES) -I ext/hashlink/src
	EXTRA_LIBS := $(EXTRA_LIBS) -ldl -L. -lhl
	LDFLAGS := $(LDFLAGS) -lm -Wl,-rpath,. -Wl,--export-dynamic -Wl,--no-undefined
else
	EXT_HL_SRCS =
	HL_CFLAGS =
endif

CPPFLAGS := $(INCLUDES)
CFLAGS := -g $(OFLAGS) $(EXTRAFLAGS) -pipe -MMD -MP -DNDEBUG
#CFLAGS := -g $(OFLAGS) $(EXTRAFLAGS) -pipe -MMD -MP
#CFLAGS := $(OFLAGS) $(EXTRAFLAGS) -pipe -MMD -MP -DNDEBUG
CFLAGS_EXT_RP3D := -g $(OFLAGS) -pipe -MMD -MP -DNDEBUG
#CFLAGS_EXT_RP3D := -g $(OFLAGS) -pipe -MMD -MP
#CFLAGS_EXT_RP3D := $(OFLAGS) -pipe -MMD -MP -DNDEBUG
CFLAGS_DEBUG := -g -pipe -MMD -MP
CXXFLAGS := $(CFLAGS) $(CPPVERSION)
CXXFLAGS_DEBUG := $(CFLAGS_DEBUG) $(CPPVERSION)
CXXFLAGS_EXT_RP3D = $(CFLAGS_EXT_RP3D) $(CPPVERSION)

#TODO
ifeq ($(LIBS-SHARED), YES)
	OBJ := obj/shared
	OBJ_DEBUG := obj-debug/shared
	CXX := $(CXX) -fPIC
	LIB := lib$(NAME).so
	EXT_LIB := lib$(NAME)-ext.so
else
	OBJ := obj/static
	OBJ_DEBUG := obj-debug/static
endif

LIBS := $(LIB_DIR)/$(LIB) $(LIB_DIR)/$(EXT_LIB)

SRC = src
TINYXML = tinyxml
ZLIB = zlib
LIBPNG = libpng
VORBIS = vorbis
OGG = ogg
REACTPHYSICS3D = reactphysics3d
SHA256 = sha256
VHACD = v-hacd
SPIRV = vulkan/spirv
GLSLANG = vulkan/glslang
OGLCOMPILERSDLL = vulkan/OGLCompilersDLL
VMA = vulkan/vma
HL = hashlink

SRCS = \
	src/tdme/audio/Audio.cpp \
	src/tdme/audio/AudioBufferManager.cpp \
	src/tdme/audio/AudioBufferManager_AudioBufferManaged.cpp \
	src/tdme/audio/AudioEntity.cpp \
	src/tdme/audio/AudioStream.cpp \
	src/tdme/audio/VorbisAudioStream.cpp \
	src/tdme/audio/Sound.cpp \
	src/tdme/audio/decoder/AudioDecoder.cpp \
	src/tdme/audio/decoder/AudioDecoderException.cpp \
	src/tdme/audio/decoder/VorbisDecoder.cpp \
	src/tdme/application/Application.cpp \
	src/tdme/application/InputEventHandler.cpp \
	src/tdme/engine/Camera.cpp \
	src/tdme/engine/Engine.cpp \
	src/tdme/engine/EntityHierarchy.cpp \
	src/tdme/engine/EntityShaderParameters.cpp \
	src/tdme/engine/EnvironmentMapping.cpp \
	src/tdme/engine/FogParticleSystem.cpp \
	src/tdme/engine/FrameBuffer.cpp \
	src/tdme/engine/Frustum.cpp \
	src/tdme/engine/Light.cpp \
	src/tdme/engine/LinesObject3D.cpp \
	src/tdme/engine/LODObject3D.cpp \
	src/tdme/engine/Object3D.cpp \
	src/tdme/engine/Object3DModel.cpp \
	src/tdme/engine/Object3DRenderGroup.cpp \
	src/tdme/engine/ObjectParticleSystem.cpp \
	src/tdme/engine/ParticleSystemGroup.cpp \
	src/tdme/engine/PartitionNone.cpp \
	src/tdme/engine/PartitionOctTree.cpp \
	src/tdme/engine/PointsParticleSystem.cpp \
	src/tdme/engine/Rotation.cpp \
	src/tdme/engine/SceneConnector.cpp \
	src/tdme/engine/Timing.cpp \
	src/tdme/engine/Transformations.cpp \
	src/tdme/engine/fileio/models/DAEReader.cpp \
	src/tdme/engine/fileio/models/GLTFReader.cpp \
	src/tdme/engine/fileio/models/ModelFileIOException.cpp \
	src/tdme/engine/fileio/models/TMReader.cpp \
	src/tdme/engine/fileio/models/TMWriter.cpp \
	src/tdme/engine/fileio/models/WFObjWriter.cpp \
	src/tdme/engine/fileio/prototypes/PrototypeWriter.cpp \
	src/tdme/engine/fileio/prototypes/PrototypeReader.cpp \
	src/tdme/engine/fileio/scenes/SceneWriter.cpp \
	src/tdme/engine/fileio/scenes/SceneReader.cpp \
	src/tdme/engine/fileio/textures/PNGTextureWriter.cpp \
	src/tdme/engine/fileio/textures/Texture.cpp \
	src/tdme/engine/fileio/textures/TextureReader.cpp \
	src/tdme/engine/model/Animation.cpp \
	src/tdme/engine/model/AnimationSetup.cpp \
	src/tdme/engine/model/Face.cpp \
	src/tdme/engine/model/FacesEntity.cpp \
	src/tdme/engine/model/Node.cpp \
	src/tdme/engine/model/Joint.cpp \
	src/tdme/engine/model/JointWeight.cpp \
	src/tdme/engine/model/Material.cpp \
	src/tdme/engine/model/Model.cpp \
	src/tdme/engine/model/PBRMaterialProperties.cpp \
	src/tdme/engine/model/UpVector.cpp \
	src/tdme/engine/model/RotationOrder.cpp \
	src/tdme/engine/model/Skinning.cpp \
	src/tdme/engine/model/ShaderModel.cpp \
	src/tdme/engine/model/SpecularMaterialProperties.cpp \
	src/tdme/engine/model/TextureCoordinate.cpp \
	src/tdme/engine/physics/Body.cpp \
	src/tdme/engine/physics/World.cpp \
	src/tdme/engine/primitives/BoundingBox.cpp \
	src/tdme/engine/primitives/BoundingVolume.cpp \
	src/tdme/engine/primitives/Capsule.cpp \
	src/tdme/engine/primitives/ConvexMesh.cpp \
	src/tdme/engine/primitives/HeightMap.cpp \
	src/tdme/engine/primitives/LineSegment.cpp \
	src/tdme/engine/primitives/OrientedBoundingBox.cpp \
	src/tdme/engine/primitives/Sphere.cpp \
	src/tdme/engine/primitives/TerrainMesh.cpp \
	src/tdme/engine/primitives/Triangle.cpp \
	src/tdme/engine/prototype/Prototype.cpp \
	src/tdme/engine/prototype/PrototypeAudio.cpp \
	src/tdme/engine/prototype/PrototypeBoundingVolume.cpp \
	src/tdme/engine/prototype/PrototypeLODLevel.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem_BoundingBoxParticleEmitter.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem_CircleParticleEmitter.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem_CircleParticleEmitterPlaneVelocity.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem_Emitter.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem_FogParticleSystem.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem_ObjectParticleSystem.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem_PointParticleEmitter.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem_PointParticleSystem.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem_SphereParticleEmitter.cpp \
	src/tdme/engine/prototype/PrototypeParticleSystem_Type.cpp \
	src/tdme/engine/prototype/PrototypePhysics.cpp \
	src/tdme/engine/prototype/PrototypePhysics_BodyType.cpp \
	src/tdme/engine/prototype/PrototypeProperties.cpp \
	src/tdme/engine/prototype/PrototypeProperty.cpp \
	src/tdme/engine/prototype/Prototype_Type.cpp \
	src/tdme/engine/scene/Scene.cpp \
	src/tdme/engine/scene/SceneEntity.cpp \
	src/tdme/engine/scene/SceneLibrary.cpp \
	src/tdme/engine/scene/SceneLight.cpp \
	src/tdme/engine/scene/ScenePropertyPresets.cpp \
	src/tdme/engine/subsystems/earlyzrejection/EZRShader.cpp \
	src/tdme/engine/subsystems/earlyzrejection/EZRShaderBaseImplementation.cpp \
	src/tdme/engine/subsystems/earlyzrejection/EZRShaderDefaultImplementation.cpp \
	src/tdme/engine/subsystems/environmentmapping/EnvironmentMappingRenderer.cpp \
	src/tdme/engine/subsystems/framebuffer/FrameBufferRenderShader.cpp \
	src/tdme/engine/subsystems/lighting/LightingShader.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderBaseImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderDefaultImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderFoliageImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderLightScatteringDefaultImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderLightScatteringFoliageImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderLightScatteringTreeImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderPBRBaseImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderPBRDefaultImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderSkyImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderTerrainEditorImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderTerrainImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderTreeImplementation.cpp \
	src/tdme/engine/subsystems/lighting/LightingShaderWaterImplementation.cpp \
	src/tdme/engine/subsystems/lines/LinesShader.cpp \
	src/tdme/engine/subsystems/lines/LinesObject3DInternal.cpp \
	src/tdme/engine/subsystems/manager/MeshManager.cpp \
	src/tdme/engine/subsystems/manager/MeshManager_MeshManaged.cpp \
	src/tdme/engine/subsystems/manager/TextureManager.cpp \
	src/tdme/engine/subsystems/manager/TextureManager_TextureManaged.cpp \
	src/tdme/engine/subsystems/manager/VBOManager.cpp \
	src/tdme/engine/subsystems/manager/VBOManager_VBOManaged.cpp \
	src/tdme/engine/subsystems/rendering/BatchRendererPoints.cpp \
	src/tdme/engine/subsystems/rendering/BatchRendererTriangles.cpp \
	src/tdme/engine/subsystems/rendering/ModelUtilitiesInternal.cpp \
	src/tdme/engine/subsystems/rendering/Object3DAnimation.cpp \
	src/tdme/engine/subsystems/rendering/Object3DBase.cpp \
	src/tdme/engine/subsystems/rendering/Object3DBase_TransformedFacesIterator.cpp \
	src/tdme/engine/subsystems/rendering/Object3DNode.cpp \
	src/tdme/engine/subsystems/rendering/Object3DNodeMesh.cpp \
	src/tdme/engine/subsystems/rendering/Object3DNodeRenderer.cpp \
	src/tdme/engine/subsystems/rendering/Object3DInternal.cpp \
	src/tdme/engine/subsystems/rendering/Object3DModelInternal.cpp \
	src/tdme/engine/subsystems/rendering/EntityRenderer.cpp \
	src/tdme/engine/subsystems/rendering/EntityRenderer_TransparentRenderFacesGroupPool.cpp \
	src/tdme/engine/subsystems/rendering/ObjectBuffer.cpp \
	src/tdme/engine/subsystems/rendering/RenderTransparentRenderPointsPool.cpp \
	src/tdme/engine/subsystems/rendering/TransparentRenderFacesGroup.cpp \
	src/tdme/engine/subsystems/rendering/TransparentRenderFacesPool.cpp \
	src/tdme/engine/subsystems/rendering/TransparentRenderFacesPool_TransparentRenderFacesPool.cpp \
	src/tdme/engine/subsystems/rendering/TransparentRenderPointsPool.cpp \
	src/tdme/engine/subsystems/particlesystem/BoundingBoxParticleEmitter.cpp \
	src/tdme/engine/subsystems/particlesystem/CircleParticleEmitter.cpp \
	src/tdme/engine/subsystems/particlesystem/CircleParticleEmitterPlaneVelocity.cpp \
	src/tdme/engine/subsystems/particlesystem/FogParticleSystemInternal.cpp \
	src/tdme/engine/subsystems/particlesystem/ObjectParticleSystemInternal.cpp \
	src/tdme/engine/subsystems/particlesystem/ParticlesShader.cpp \
	src/tdme/engine/subsystems/particlesystem/PointParticleEmitter.cpp \
	src/tdme/engine/subsystems/particlesystem/PointsParticleSystemInternal.cpp \
	src/tdme/engine/subsystems/particlesystem/SphereParticleEmitter.cpp \
	src/tdme/engine/subsystems/renderer/Renderer.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessing.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessingProgram.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessingShader.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessingShaderBaseImplementation.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessingShaderBlurImplementation.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessingShaderDefaultImplementation.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessingShaderDesaturationImplementation.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessingShaderLightScatteringImplementation.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessingShaderSSAOImplementation.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessingShaderSSAOMapImplementation.cpp \
	src/tdme/engine/subsystems/postprocessing/PostProcessingShaderVignetteImplementation.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMap.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapping.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapCreationShader.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderBaseImplementation.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderDefaultImplementation.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderFoliageImplementation.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapCreationShaderTreeImplementation.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapRenderShader.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderBaseImplementation.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderDefaultImplementation.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderFoliageImplementation.cpp \
	src/tdme/engine/subsystems/shadowmapping/ShadowMapRenderShaderTreeImplementation.cpp \
	src/tdme/engine/subsystems/skinning/SkinningShader.cpp \
	src/tdme/engine/subsystems/texture2D/Texture2DRenderShader.cpp \
	src/tdme/gui/GUI.cpp \
	src/tdme/gui/GUIParser.cpp \
	src/tdme/gui/GUIParserException.cpp \
	src/tdme/gui/effects/GUIColorEffect.cpp \
	src/tdme/gui/effects/GUIEffect.cpp \
	src/tdme/gui/effects/GUIPositionEffect.cpp \
	src/tdme/gui/elements/GUIButton.cpp \
	src/tdme/gui/elements/GUIButtonController.cpp \
	src/tdme/gui/elements/GUICheckbox.cpp \
	src/tdme/gui/elements/GUICheckboxController.cpp \
	src/tdme/gui/elements/GUIDropDown.cpp \
	src/tdme/gui/elements/GUIDropDownController.cpp \
	src/tdme/gui/elements/GUIDropDownOption.cpp \
	src/tdme/gui/elements/GUIDropDownOptionController.cpp \
	src/tdme/gui/elements/GUIElement.cpp \
	src/tdme/gui/elements/GUIImageButton.cpp \
	src/tdme/gui/elements/GUIInput.cpp \
	src/tdme/gui/elements/GUIInputController.cpp \
	src/tdme/gui/elements/GUIKnob.cpp \
	src/tdme/gui/elements/GUIKnobController.cpp \
	src/tdme/gui/elements/GUIMenuHeader.cpp \
	src/tdme/gui/elements/GUIMenuHeaderController.cpp \
	src/tdme/gui/elements/GUIMenuHeaderItem.cpp \
	src/tdme/gui/elements/GUIMenuHeaderItemController.cpp \
	src/tdme/gui/elements/GUIMenuItem.cpp \
	src/tdme/gui/elements/GUIMenuItemController.cpp \
	src/tdme/gui/elements/GUIMenuSeparator.cpp \
	src/tdme/gui/elements/GUIProgressBar.cpp \
	src/tdme/gui/elements/GUIProgressBarController.cpp \
	src/tdme/gui/elements/GUIRadioButton.cpp \
	src/tdme/gui/elements/GUIRadioButtonController.cpp \
	src/tdme/gui/elements/GUIScrollArea.cpp \
	src/tdme/gui/elements/GUIScrollAreaController.cpp \
	src/tdme/gui/elements/GUISelectBox.cpp \
	src/tdme/gui/elements/GUISelectBoxController.cpp \
	src/tdme/gui/elements/GUISelectBoxOption.cpp \
	src/tdme/gui/elements/GUISelectBoxOptionController.cpp \
	src/tdme/gui/elements/GUISelectBoxParentOption.cpp \
	src/tdme/gui/elements/GUISelectBoxParentOptionController.cpp \
	src/tdme/gui/elements/GUISliderH.cpp \
	src/tdme/gui/elements/GUISliderHController.cpp \
	src/tdme/gui/elements/GUISliderV.cpp \
	src/tdme/gui/elements/GUISliderVController.cpp \
	src/tdme/gui/elements/GUITab.cpp \
	src/tdme/gui/elements/GUITabContent.cpp \
	src/tdme/gui/elements/GUITabContentController.cpp \
	src/tdme/gui/elements/GUITabController.cpp \
	src/tdme/gui/elements/GUITabs.cpp \
	src/tdme/gui/elements/GUITabsContent.cpp \
	src/tdme/gui/elements/GUITabsController.cpp \
	src/tdme/gui/elements/GUITabsHeader.cpp \
	src/tdme/gui/elements/GUITabsHeaderController.cpp \
	src/tdme/gui/events/GUIKeyboardEvent.cpp \
	src/tdme/gui/events/GUIMouseEvent.cpp \
	src/tdme/gui/nodes/GUIColor.cpp \
	src/tdme/gui/nodes/GUIElementController.cpp \
	src/tdme/gui/nodes/GUIElementIgnoreEventsController.cpp \
	src/tdme/gui/nodes/GUIElementNode.cpp \
	src/tdme/gui/nodes/GUIHorizontalScrollbarInternalController.cpp \
	src/tdme/gui/nodes/GUIHorizontalScrollbarInternalNode.cpp \
	src/tdme/gui/nodes/GUIImageNode.cpp \
	src/tdme/gui/nodes/GUIInputInternalController.cpp \
	src/tdme/gui/nodes/GUIInputInternalNode.cpp \
	src/tdme/gui/nodes/GUILayerNode.cpp \
	src/tdme/gui/nodes/GUILayoutNode.cpp \
	src/tdme/gui/nodes/GUILayoutNode_Alignment.cpp \
	src/tdme/gui/nodes/GUIMultilineTextNode.cpp \
	src/tdme/gui/nodes/GUINode.cpp \
	src/tdme/gui/nodes/GUINodeConditions.cpp \
	src/tdme/gui/nodes/GUINodeController.cpp \
	src/tdme/gui/nodes/GUINode_AlignmentHorizontal.cpp \
	src/tdme/gui/nodes/GUINode_AlignmentVertical.cpp \
	src/tdme/gui/nodes/GUINode_Flow.cpp \
	src/tdme/gui/nodes/GUINode_RequestedConstraints_RequestedConstraintsType.cpp \
	src/tdme/gui/nodes/GUIPanelNode.cpp \
	src/tdme/gui/nodes/GUIParentNode.cpp \
	src/tdme/gui/nodes/GUIParentNode_Overflow.cpp \
	src/tdme/gui/nodes/GUIScreenNode.cpp \
	src/tdme/gui/nodes/GUISpaceNode.cpp \
	src/tdme/gui/nodes/GUITableNode.cpp \
	src/tdme/gui/nodes/GUITableCellNode.cpp \
	src/tdme/gui/nodes/GUITableRowNode.cpp \
	src/tdme/gui/nodes/GUITextNode.cpp \
	src/tdme/gui/nodes/GUIVerticalScrollbarInternalController.cpp \
	src/tdme/gui/nodes/GUIVerticalScrollbarInternalNode.cpp \
	src/tdme/gui/renderer/GUIFont.cpp \
	src/tdme/gui/renderer/GUIFont_CharacterDefinition.cpp \
	src/tdme/gui/renderer/GUIRenderer.cpp \
	src/tdme/gui/renderer/GUIShader.cpp \
	src/tdme/network/httpclient/HTTPClient.cpp \
	src/tdme/network/httpclient/HTTPClientException.cpp \
	src/tdme/network/httpclient/HTTPDownloadClient.cpp \
	src/tdme/network/udpclient/NetworkClientException.cpp \
	src/tdme/network/udpclient/UDPClient.cpp \
	src/tdme/network/udpclient/UDPClientMessage.cpp \
	src/tdme/network/udpserver/NetworkServerException.cpp \
	src/tdme/network/udpserver/ServerClient.cpp \
	src/tdme/network/udpserver/ServerClientRequestHandlerHubException.cpp \
	src/tdme/network/udpserver/ServerRequest.cpp \
	src/tdme/network/udpserver/ServerWorkerThread.cpp \
	src/tdme/network/udpserver/ServerWorkerThreadPool.cpp \
	src/tdme/network/udpserver/UDPServer.cpp \
	src/tdme/network/udpserver/UDPServerClient.cpp \
	src/tdme/network/udpserver/UDPServerIOThread.cpp \
	src/tdme/os/filesystem/ArchiveFileSystem.cpp \
	src/tdme/os/filesystem/FileSystem.cpp \
	src/tdme/os/filesystem/FileSystemException.cpp \
	src/tdme/os/filesystem/StandardFileSystem.cpp \
	src/tdme/os/network/Network.cpp \
	src/tdme/os/network/NetworkException.cpp \
	src/tdme/os/network/NetworkIOException.cpp \
	src/tdme/os/network/NetworkSocketClosedException.cpp \
	src/tdme/os/network/NetworkKEMException.cpp \
	src/tdme/os/network/NetworkSocket.cpp \
	src/tdme/os/network/NetworkSocketException.cpp \
	src/tdme/os/network/UDPSocket.cpp \
	src/tdme/os/network/TCPSocket.cpp \
	src/tdme/os/threading/Barrier.cpp \
	src/tdme/os/threading/Condition.cpp \
	src/tdme/os/threading/Mutex.cpp \
	src/tdme/os/threading/ReadWriteLock.cpp \
	src/tdme/os/threading/SpinLock.cpp \
	src/tdme/os/threading/Thread.cpp \
	src/tdme/tests/AngleTest.cpp \
	src/tdme/tests/AudioTest.cpp \
	src/tdme/tests/CrashTest.cpp \
	src/tdme/tests/EngineTest.cpp \
	src/tdme/tests/EntityHierarchyTest.cpp \
	src/tdme/tests/LODTest.cpp \
	src/tdme/tests/FlowMapTest.cpp \
	src/tdme/tests/FoliageTest.cpp \
	src/tdme/tests/MathOperatorTest.cpp \
	src/tdme/tests/PathFindingTest.cpp \
	src/tdme/tests/PivotTest.cpp \
	src/tdme/tests/PhysicsTest1.cpp \
	src/tdme/tests/PhysicsTest2.cpp \
	src/tdme/tests/PhysicsTest3.cpp \
	src/tdme/tests/PhysicsTest4.cpp \
	src/tdme/tests/RayTracingTest.cpp \
	src/tdme/tests/ThreadingTest_ConsumerThread.cpp \
	src/tdme/tests/ThreadingTest_ProducerThread.cpp \
	src/tdme/tests/ThreadingTest_TestThread.cpp \
	src/tdme/tests/UDPServerTest_UDPServer.cpp \
	src/tdme/tests/UDPServerTest_UDPServerClient.cpp \
	src/tdme/tests/SkinningTest.cpp \
	src/tdme/tests/TreeTest.cpp \
	src/tdme/tests/WaterTest.cpp \
	src/tdme/tools/gui/GUITest.cpp \
	src/tdme/tools/installer/Installer.cpp \
	src/tdme/tools/modeleditor/TDMEModelEditor.cpp \
	src/tdme/tools/particlesystem/TDMEParticleSystem.cpp \
	src/tdme/tools/sceneeditor/TDMESceneEditor.cpp \
	src/tdme/tools/sceneeditor/controller/EmptyScreenController.cpp \
	src/tdme/tools/sceneeditor/controller/EnvironmentMappingScreenController.cpp \
	src/tdme/tools/sceneeditor/controller/SceneEditorLibraryScreenController.cpp \
	src/tdme/tools/sceneeditor/controller/SceneEditorScreenController.cpp \
	src/tdme/tools/sceneeditor/controller/TriggerScreenController.cpp \
	src/tdme/tools/sceneeditor/views/EmptyView.cpp \
	src/tdme/tools/sceneeditor/views/EnvironmentMappingView.cpp \
	src/tdme/tools/sceneeditor/views/ModelEditorView.cpp \
	src/tdme/tools/sceneeditor/views/ParticleSystemView.cpp \
	src/tdme/tools/sceneeditor/views/SceneEditorView.cpp \
	src/tdme/tools/sceneeditor/views/SceneEditorView_EntityColor.cpp \
	src/tdme/tools/sceneeditor/views/TerrainEditorView.cpp \
	src/tdme/tools/sceneeditor/views/TriggerView.cpp \
	src/tdme/tools/shared/controller/FileDialogPath.cpp \
	src/tdme/tools/shared/controller/FileDialogScreenController.cpp \
	src/tdme/tools/shared/controller/InfoDialogScreenController.cpp \
	src/tdme/tools/shared/controller/ModelEditorScreenController.cpp \
	src/tdme/tools/shared/controller/ParticleSystemScreenController.cpp \
	src/tdme/tools/shared/controller/ProgressBarScreenController.cpp \
	src/tdme/tools/shared/controller/PrototypeBaseSubScreenController.cpp \
	src/tdme/tools/shared/controller/PrototypeDisplaySubScreenController.cpp \
	src/tdme/tools/shared/controller/PrototypePhysicsSubScreenController.cpp \
	src/tdme/tools/shared/controller/PrototypePhysicsSubScreenController_BoundingVolumeType.cpp \
	src/tdme/tools/shared/controller/PrototypePhysicsSubScreenController_GenerateConvexMeshes.cpp \
	src/tdme/tools/shared/controller/PrototypeSoundsSubScreenController.cpp \
	src/tdme/tools/shared/controller/TerrainEditorScreenController.cpp \
	src/tdme/tools/shared/tools/Tools.cpp \
	src/tdme/tools/shared/views/CameraInputHandler.cpp \
	src/tdme/tools/shared/views/CameraRotationInputHandler.cpp \
	src/tdme/tools/shared/views/Gizmo.cpp \
	src/tdme/tools/shared/views/PopUps.cpp \
	src/tdme/tools/shared/views/PrototypeBaseView.cpp \
	src/tdme/tools/shared/views/PrototypePhysicsView.cpp \
	src/tdme/tools/shared/views/PrototypeDisplayView.cpp \
	src/tdme/tools/shared/views/PrototypeSoundsView.cpp \
	src/tdme/tools/shared/views/SharedModelEditorView.cpp \
	src/tdme/tools/shared/views/SharedParticleSystemView.cpp \
	src/tdme/tools/shared/views/SharedTerrainEditorView.cpp \
	src/tdme/tools/terraineditor/TDMETerrainEditor.cpp \
	src/tdme/utilities/Base64EncDec.cpp \
	src/tdme/utilities/Character.cpp \
	src/tdme/utilities/Console.cpp \
	src/tdme/utilities/ExceptionBase.cpp \
	src/tdme/utilities/Enum.cpp \
	src/tdme/utilities/Float.cpp \
	src/tdme/utilities/HashLink.cpp \
	src/tdme/utilities/HexEncDec.cpp \
	src/tdme/utilities/Integer.cpp \
	src/tdme/utilities/IntEncDec.cpp \
	src/tdme/utilities/MutableString.cpp \
	src/tdme/utilities/ModelTools.cpp \
	src/tdme/utilities/PathFinding.cpp \
	src/tdme/utilities/Primitives.cpp \
	src/tdme/utilities/Properties.cpp \
	src/tdme/utilities/Reference.cpp \
	src/tdme/utilities/RTTI.cpp \
	src/tdme/utilities/StringTools.cpp \
	src/tdme/utilities/StringTokenizer.cpp \
	src/tdme/utilities/Terrain.cpp \
	$(SRCS_PLATFORM)

EXT_TINYXML_SRCS = \
	ext/tinyxml/tinystr.cpp \
	ext/tinyxml/tinyxml.cpp \
	ext/tinyxml/tinyxmlerror.cpp \
	ext/tinyxml/tinyxmlparser.cpp

EXT_ZLIB_SRCS = \
	ext/zlib/adler32.c \
	ext/zlib/crc32.c \
	ext/zlib/deflate.c \
	ext/zlib/infback.c \
	ext/zlib/inffast.c \
	ext/zlib/inflate.c \
	ext/zlib/inftrees.c \
	ext/zlib/trees.c \
	ext/zlib/zutil.c \
	ext/zlib/compress.c \
	ext/zlib/uncompr.c \
	ext/zlib/gzclose.c \
	ext/zlib/gzlib.c \
	ext/zlib/gzread.c \
	ext/zlib/gzwrite.c

EXT_LIBPNG_SRCS = \
	ext/libpng/pngrio.c \
	ext/libpng/pngwio.c \
	ext/libpng/pngmem.c \
	ext/libpng/pngwtran.c \
	ext/libpng/pngtrans.c \
	ext/libpng/pngerror.c \
	ext/libpng/pngpread.c \
	ext/libpng/pngget.c \
	ext/libpng/pngset.c \
	ext/libpng/pngwrite.c \
	ext/libpng/pngwutil.c \
	ext/libpng/pngread.c \
	ext/libpng/pngrutil.c \
	ext/libpng/png.c \
	ext/libpng/pngrtran.c

EXT_VORBIS_SRCS = \
	ext/vorbis/analysis.c \
	ext/vorbis/bitrate.c \
	ext/vorbis/block.c \
	ext/vorbis/codebook.c \
	ext/vorbis/envelope.c \
	ext/vorbis/floor0.c \
	ext/vorbis/floor1.c \
	ext/vorbis/info.c \
	ext/vorbis/lookup.c \
	ext/vorbis/lpc.c \
	ext/vorbis/lsp.c \
	ext/vorbis/mapping0.c \
	ext/vorbis/mdct.c \
	ext/vorbis/misc.c \
	ext/vorbis/psy.c \
	ext/vorbis/registry.c \
	ext/vorbis/res0.c \
	ext/vorbis/sharedbook.c \
	ext/vorbis/smallft.c \
	ext/vorbis/synthesis.c \
	ext/vorbis/vorbisenc.c \
	ext/vorbis/vorbisfile.c \
	ext/vorbis/window.c

EXT_OGG_SRCS = \
	ext/ogg/bitwise.c \
	ext/ogg/framing.c

EXT_SHA256_SRCS = \
	ext/sha256/sha256.cpp

EXT_VHACD_SRCS = \
	ext/v-hacd/src/VHACD_Lib/src/FloatMath.cpp \
	ext/v-hacd/src/VHACD_Lib/src/VHACD-ASYNC.cpp \
	ext/v-hacd/src/VHACD_Lib/src/VHACD.cpp \
	ext/v-hacd/src/VHACD_Lib/src/btAlignedAllocator.cpp \
	ext/v-hacd/src/VHACD_Lib/src/btConvexHullComputer.cpp \
	ext/v-hacd/src/VHACD_Lib/src/vhacdICHull.cpp \
	ext/v-hacd/src/VHACD_Lib/src/vhacdManifoldMesh.cpp \
	ext/v-hacd/src/VHACD_Lib/src/vhacdMesh.cpp \
	ext/v-hacd/src/VHACD_Lib/src/vhacdRaycastMesh.cpp \
	ext/v-hacd/src/VHACD_Lib/src/vhacdVolume.cpp

EXT_REACTPHYSICS3D_SRCS = \
	ext/reactphysics3d/src/body/Body.cpp \
	ext/reactphysics3d/src/body/CollisionBody.cpp \
	ext/reactphysics3d/src/body/RigidBody.cpp \
	ext/reactphysics3d/src/collision/ContactManifoldInfo.cpp \
	ext/reactphysics3d/src/collision/broadphase/BroadPhaseAlgorithm.cpp \
	ext/reactphysics3d/src/collision/broadphase/DynamicAABBTree.cpp \
	ext/reactphysics3d/src/collision/narrowphase/DefaultCollisionDispatch.cpp \
	ext/reactphysics3d/src/collision/narrowphase/GJK/VoronoiSimplex.cpp \
	ext/reactphysics3d/src/collision/narrowphase/GJK/GJKAlgorithm.cpp \
	ext/reactphysics3d/src/collision/narrowphase/SAT/SATAlgorithm.cpp \
	ext/reactphysics3d/src/collision/narrowphase/CapsuleVsCapsuleAlgorithm.cpp \
	ext/reactphysics3d/src/collision/narrowphase/SphereVsCapsuleAlgorithm.cpp \
	ext/reactphysics3d/src/collision/narrowphase/SphereVsSphereAlgorithm.cpp \
	ext/reactphysics3d/src/collision/narrowphase/SphereVsConvexPolyhedronAlgorithm.cpp \
	ext/reactphysics3d/src/collision/narrowphase/CapsuleVsConvexPolyhedronAlgorithm.cpp \
	ext/reactphysics3d/src/collision/narrowphase/ConvexPolyhedronVsConvexPolyhedronAlgorithm.cpp \
	ext/reactphysics3d/src/collision/shapes/AABB.cpp \
	ext/reactphysics3d/src/collision/shapes/ConvexShape.cpp \
	ext/reactphysics3d/src/collision/shapes/ConvexPolyhedronShape.cpp \
	ext/reactphysics3d/src/collision/shapes/ConcaveShape.cpp \
	ext/reactphysics3d/src/collision/shapes/BoxShape.cpp \
	ext/reactphysics3d/src/collision/shapes/CapsuleShape.cpp \
	ext/reactphysics3d/src/collision/shapes/CollisionShape.cpp \
	ext/reactphysics3d/src/collision/shapes/ConvexMeshShape.cpp \
	ext/reactphysics3d/src/collision/shapes/SphereShape.cpp \
	ext/reactphysics3d/src/collision/shapes/TriangleShape.cpp \
	ext/reactphysics3d/src/collision/shapes/ConcaveMeshShape.cpp \
	ext/reactphysics3d/src/collision/shapes/HeightFieldShape.cpp \
	ext/reactphysics3d/src/collision/RaycastInfo.cpp \
	ext/reactphysics3d/src/collision/ProxyShape.cpp \
	ext/reactphysics3d/src/collision/TriangleVertexArray.cpp \
	ext/reactphysics3d/src/collision/PolygonVertexArray.cpp \
	ext/reactphysics3d/src/collision/TriangleMesh.cpp \
	ext/reactphysics3d/src/collision/PolyhedronMesh.cpp \
	ext/reactphysics3d/src/collision/HalfEdgeStructure.cpp \
	ext/reactphysics3d/src/collision/CollisionDetection.cpp \
	ext/reactphysics3d/src/collision/NarrowPhaseInfo.cpp \
	ext/reactphysics3d/src/collision/ContactManifold.cpp \
	ext/reactphysics3d/src/collision/ContactManifoldSet.cpp \
	ext/reactphysics3d/src/collision/MiddlePhaseTriangleCallback.cpp \
	ext/reactphysics3d/src/constraint/BallAndSocketJoint.cpp \
	ext/reactphysics3d/src/constraint/ContactPoint.cpp \
	ext/reactphysics3d/src/constraint/FixedJoint.cpp \
	ext/reactphysics3d/src/constraint/HingeJoint.cpp \
	ext/reactphysics3d/src/constraint/Joint.cpp \
	ext/reactphysics3d/src/constraint/SliderJoint.cpp \
	ext/reactphysics3d/src/engine/CollisionWorld.cpp \
	ext/reactphysics3d/src/engine/ConstraintSolver.cpp \
	ext/reactphysics3d/src/engine/ContactSolver.cpp \
	ext/reactphysics3d/src/engine/DynamicsWorld.cpp \
	ext/reactphysics3d/src/engine/Island.cpp \
	ext/reactphysics3d/src/engine/Material.cpp \
	ext/reactphysics3d/src/engine/OverlappingPair.cpp \
	ext/reactphysics3d/src/engine/Timer.cpp \
	ext/reactphysics3d/src/collision/CollisionCallback.cpp \
	ext/reactphysics3d/src/mathematics/mathematics_functions.cpp \
	ext/reactphysics3d/src/mathematics/Matrix2x2.cpp \
	ext/reactphysics3d/src/mathematics/Matrix3x3.cpp \
	ext/reactphysics3d/src/mathematics/Quaternion.cpp \
	ext/reactphysics3d/src/mathematics/Transform.cpp \
	ext/reactphysics3d/src/mathematics/Vector2.cpp \
	ext/reactphysics3d/src/mathematics/Vector3.cpp \
	ext/reactphysics3d/src/memory/MemoryManager.cpp \
	ext/reactphysics3d/src/memory/DefaultSingleFrameAllocator.cpp \
	ext/reactphysics3d/src/memory/DefaultPoolAllocator.cpp

ifeq ($(VULKAN), YES)
	EXT_SPIRV_SRCS = \
		ext/vulkan/spirv/GlslangToSpv.cpp \
		ext/vulkan/spirv/InReadableOrder.cpp \
		ext/vulkan/spirv/Logger.cpp \
		ext/vulkan/spirv/SPVRemapper.cpp \
		ext/vulkan/spirv/SpvBuilder.cpp \
		ext/vulkan/spirv/SpvPostProcess.cpp \
		ext/vulkan/spirv/SpvTools.cpp \
		ext/vulkan/spirv/disassemble.cpp \
		ext/vulkan/spirv/doc.cpp

	EXT_GLSLANG_SRCS := \
		ext/vulkan/glslang/MachineIndependent/glslang_tab.cpp \
		ext/vulkan/glslang/MachineIndependent/attribute.cpp \
		ext/vulkan/glslang/MachineIndependent/Constant.cpp \
		ext/vulkan/glslang/MachineIndependent/iomapper.cpp \
		ext/vulkan/glslang/MachineIndependent/InfoSink.cpp \
		ext/vulkan/glslang/MachineIndependent/Initialize.cpp \
		ext/vulkan/glslang/MachineIndependent/IntermTraverse.cpp \
		ext/vulkan/glslang/MachineIndependent/Intermediate.cpp \
		ext/vulkan/glslang/MachineIndependent/ParseContextBase.cpp \
		ext/vulkan/glslang/MachineIndependent/ParseHelper.cpp \
		ext/vulkan/glslang/MachineIndependent/PoolAlloc.cpp \
		ext/vulkan/glslang/MachineIndependent/RemoveTree.cpp \
		ext/vulkan/glslang/MachineIndependent/Scan.cpp \
		ext/vulkan/glslang/MachineIndependent/ShaderLang.cpp \
		ext/vulkan/glslang/MachineIndependent/SymbolTable.cpp \
		ext/vulkan/glslang/MachineIndependent/Versions.cpp \
		ext/vulkan/glslang/MachineIndependent/intermOut.cpp \
		ext/vulkan/glslang/MachineIndependent/limits.cpp \
		ext/vulkan/glslang/MachineIndependent/linkValidate.cpp \
		ext/vulkan/glslang/MachineIndependent/parseConst.cpp \
		ext/vulkan/glslang/MachineIndependent/reflection.cpp \
		ext/vulkan/glslang/MachineIndependent/preprocessor/Pp.cpp \
		ext/vulkan/glslang/MachineIndependent/preprocessor/PpAtom.cpp \
		ext/vulkan/glslang/MachineIndependent/preprocessor/PpContext.cpp \
		ext/vulkan/glslang/MachineIndependent/preprocessor/PpScanner.cpp \
		ext/vulkan/glslang/MachineIndependent/preprocessor/PpTokens.cpp \
		ext/vulkan/glslang/MachineIndependent/propagateNoContraction.cpp \
		ext/vulkan/glslang/GenericCodeGen/CodeGen.cpp \
		ext/vulkan/glslang/GenericCodeGen/Link.cpp \
		$(EXT_GLSLANG_PLATFORM_SRCS)
    	
	EXT_OGLCOMPILERSDLL_SRCS = \
		ext/vulkan/OGLCompilersDLL/InitializeDll.cpp

	EXT_VMA_SRCS = \
		ext/vulkan/vma/src/VmaUsage.cpp

else
	EXT_SPIRV_SRCS =
	EXT_GLSLANG_SRCS =
	EXT_GLSLANG_SRCS =
endif

MAIN_SRCS = \
	src/tdme/tests/AngleTest-main.cpp \
	src/tdme/tests/AudioTest-main.cpp \
	src/tdme/tests/CrashTest-main.cpp \
	src/tdme/tests/EngineTest-main.cpp \
	src/tdme/tests/EntityHierarchyTest-main.cpp \
	src/tdme/tests/HashLinkTest-main.cpp \
	src/tdme/tests/HTTPClientTest-main.cpp \
	src/tdme/tests/HTTPDownloadClientTest-main.cpp \
	src/tdme/tests/LODTest-main.cpp \
	src/tdme/tests/FlowMapTest-main.cpp \
	src/tdme/tests/FoliageTest-main.cpp \
	src/tdme/tests/MathOperatorTest-main.cpp \
	src/tdme/tests/PathFindingTest-main.cpp \
	src/tdme/tests/PivotTest-main.cpp \
	src/tdme/tests/PhysicsTest1-main.cpp \
	src/tdme/tests/PhysicsTest2-main.cpp \
	src/tdme/tests/PhysicsTest3-main.cpp \
	src/tdme/tests/PhysicsTest4-main.cpp \
	src/tdme/tests/RayTracingTest-main.cpp \
	src/tdme/tests/SkinningTest-main.cpp \
	src/tdme/tests/ThreadingTest-main.cpp \
	src/tdme/tests/TreeTest-main.cpp \
	src/tdme/tests/UDPClientTest-main.cpp \
	src/tdme/tests/UDPServerTest-main.cpp \
	src/tdme/tests/WaterTest-main.cpp \
	src/tdme/tools/gui/GUITest-main.cpp \
	src/tdme/tools/installer/Installer-main.cpp \
	src/tdme/tools/modeleditor/TDMEModelEditor-main.cpp \
	src/tdme/tools/particlesystem/TDMEParticleSystem-main.cpp \
	src/tdme/tools/sceneeditor/TDMESceneEditor-main.cpp \
	src/tdme/tools/terraineditor/TDMETerrainEditor-main.cpp \
	src/tdme/tools/cli/archive-main.cpp \
	src/tdme/tools/cli/converttotm-main.cpp \
	src/tdme/tools/cli/copyanimationsetups-main.cpp \
	src/tdme/tools/cli/create-installer-main.cpp \
	src/tdme/tools/cli/fixdoxygen-main.cpp \
	src/tdme/tools/cli/generatelicenses-main.cpp \
	src/tdme/tools/cli/importtmodel-main.cpp \
	src/tdme/tools/cli/makefilegenerator-main.cpp \
	src/tdme/tools/cli/optimizemodel-main.cpp \
	src/tdme/tools/cli/scenefixmodelszup2yup-main.cpp \
	src/tdme/tools/cli/sortincludes-main.cpp

MAINS = $(MAIN_SRCS:$(SRC)/%-main.cpp=$(BIN)/%)
OBJS = $(SRCS:$(SRC)/%.cpp=$(OBJ)/%.o)
OBJS_DEBUG = $(SRCS_DEBUG:$(SRC)/%.cpp=$(OBJ_DEBUG)/%.o)

EXT_TINYXML_OBJS = $(EXT_TINYXML_SRCS:ext/$(TINYXML)/%.cpp=$(OBJ)/%.o)
EXT_ZLIB_OBJS = $(EXT_ZLIB_SRCS:ext/$(ZLIB)/%.c=$(OBJ)/%.o)
EXT_LIBPNG_OBJS = $(EXT_LIBPNG_SRCS:ext/$(LIBPNG)/%.c=$(OBJ)/%.o)
EXT_VORBIS_OBJS = $(EXT_VORBIS_SRCS:ext/$(VORBIS)/%.c=$(OBJ)/%.o)
EXT_OGG_OBJS = $(EXT_OGG_SRCS:ext/$(OGG)/%.c=$(OBJ)/%.o)
EXT_SHA256_OBJS = $(EXT_SHA256_SRCS:ext/$(SHA256)/%.cpp=$(OBJ)/%.o)
EXT_VHACD_OBJS = $(EXT_VHACD_SRCS:ext/$(VHACD)/%.cpp=$(OBJ)/%.o)
EXT_REACTPHYSICS3D_OBJS = $(EXT_REACTPHYSICS3D_SRCS:ext/$(REACTPHYSICS3D)/%.cpp=$(OBJ)/%.o)
EXT_SPIRV_OBJS = $(EXT_SPIRV_SRCS:ext/$(SPIRV)/%.cpp=$(OBJ)/vulkan/%.o)
EXT_GLSLANG_OBJS = $(EXT_GLSLANG_SRCS:ext/$(GLSLANG)/%.cpp=$(OBJ)/vulkan/%.o)
EXT_OGLCOMPILERSDLL_OBJS = $(EXT_OGLCOMPILERSDLL_SRCS:ext/$(OGLCOMPILERSDLL)/%.cpp=$(OBJ)/vulkan/%.o)
EXT_VMA_OBJS = $(EXT_VMA_SRCS:ext/$(VMA)/%.cpp=$(OBJ)/vulkan/%.o)
EXT_HL_OBJS = $(EXT_HL_SRCS:ext/$(HL)/%.c=$(OBJ)/%.o)

all: $(LIBS)

define cpp-command
@mkdir -p $(dir $@);
@echo Compile $<; $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
endef

define cpp-command-debug
@mkdir -p $(dir $@);
@echo Compile $<; $(CXX) $(CPPFLAGS) $(CXXFLAGS_DEBUG) -c -o $@ $<
endef

define cpp-command-ext-rp3d
@mkdir -p $(dir $@);
@echo Compile $<; $(CXX) $(CPPFLAGS) $(CXXFLAGS_EXT_RP3D) -c -o $@ $<
endef

define c-command
@mkdir -p $(dir $@);
@echo Compile $<; $(CXX) -x c $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
endef

define c-command-hl
@mkdir -p $(dir $@);
@echo Compile $<; $(CXX) -x c $(HL_CFLAGS) -c -o $@ $<
endef

$(OBJS):$(OBJ)/%.o: $(SRC)/%.cpp | print-opts
	$(cpp-command)

$(OBJS_DEBUG):$(OBJ_DEBUG)/%.o: $(SRC)/%.cpp | print-opts
	$(cpp-command-debug)

$(EXT_TINYXML_OBJS):$(OBJ)/%.o: ext/$(TINYXML)/%.cpp | print-opts
	$(cpp-command)

$(EXT_ZLIB_OBJS):$(OBJ)/%.o: ext/$(ZLIB)/%.c | print-opts
	$(c-command)

$(EXT_LIBPNG_OBJS):$(OBJ)/%.o: ext/$(LIBPNG)/%.c | print-opts
	$(c-command)

$(EXT_VORBIS_OBJS):$(OBJ)/%.o: ext/$(VORBIS)/%.c | print-opts
	$(c-command)

$(EXT_OGG_OBJS):$(OBJ)/%.o: ext/$(OGG)/%.c | print-opts
	$(c-command)

$(EXT_SHA256_OBJS):$(OBJ)/%.o: ext/$(SHA256)/%.cpp | print-opts
	$(cpp-command)

$(EXT_VHACD_OBJS):$(OBJ)/%.o: ext/$(VHACD)/%.cpp | print-opts
	$(cpp-command)

$(EXT_REACTPHYSICS3D_OBJS):$(OBJ)/%.o: ext/$(REACTPHYSICS3D)/%.cpp | print-opts
	$(cpp-command-ext-rp3d)

$(EXT_SPIRV_OBJS):$(OBJ)/vulkan/%.o: ext/$(SPIRV)/%.cpp | print-opts
	$(cpp-command)

$(EXT_GLSLANG_OBJS):$(OBJ)/vulkan/%.o: ext/$(GLSLANG)/%.cpp | print-opts
	$(cpp-command)

$(EXT_OGLCOMPILERSDLL_OBJS):$(OBJ)/vulkan/%.o: ext/$(OGLCOMPILERSDLL)/%.cpp | print-opts
	$(cpp-command)

$(EXT_VMA_OBJS):$(OBJ)/vulkan/%.o: ext/$(VMA)/%.cpp | print-opts
	$(cpp-command)
$(EXT_HL_OBJS):$(OBJ)/%.o: ext/$(HL)/%.c | print-opts
	$(c-command-hl)

%.a:
	@echo Archive $@ created
	@mkdir -p $(dir $@)
	@rm -f $@
	@ar rcs $@ $^

%.so:
	@echo Shared library $@ created
	@mkdir -p $(dir $@)
	@rm -f $@
	$(CXX) -shared  $^ -o $@


$(LIB_DIR)/$(LIB): $(OBJS) $(OBJS_DEBUG)

$(LIB_DIR)/$(EXT_LIB): $(EXT_OBJS) $(EXT_TINYXML_OBJS) $(EXT_ZLIB_OBJS) $(EXT_LIBPNG_OBJS) $(EXT_VORBIS_OBJS) $(EXT_OGG_OBJS) $(EXT_SHA256_OBJS) $(EXT_VHACD_OBJS) $(EXT_REACTPHYSICS3D_OBJS) $(EXT_SPIRV_OBJS) $(EXT_GLSLANG_OBJS) $(EXT_OGLCOMPILERSDLL_OBJS) $(EXT_VMA_OBJS) $(EXT_HL_OBJS)

ifeq ($(OSSHORT), Msys)
$(MAINS):$(BIN)/%:$(SRC)/%-main.cpp $(LIBS)
	@mkdir -p $(dir $@);
	@EXECUTABLE=$$(echo $1 | grep -o '[a-zA-Z0-9]*-main' | sed -e 's/\-main//');
	@scripts/windows-mingw-create-executable-rc.sh "$<" $@.rc
	@windres $@.rc -o coff -o $@.rc.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -L$(LIB_DIR) -o $@ $@.rc.o $< -l$(NAME) $(EXTRA_LIBS)
else
$(MAINS):$(BIN)/%:$(SRC)/%-main.cpp $(LIBS)
	@mkdir -p $(dir $@);
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -L$(LIB_DIR) -o $@ $< -l$(NAME) $(EXTRA_LIBS)
endif

hashlink:
	(cd ext/hashlink && $(MAKE) clean && $(MAKE) libhl && mkdir -p ../../$(LIB_DIR) && cp libhl.so ../../$(LIB_DIR) && $(MAKE) clean)

mains: $(MAINS)

# TODO make sure that always directory obj and obj-debug are removed
clean:
	rm -rf obj obj-debug $(LIB_DIR) $(BIN)

print-opts:
	@echo Building with \"$(CXX) $(CPPFLAGS) $(CXXFLAGS)\"
	
.PHONY: all mains clean print-opts

-include $(OBJS:%.o=%.d)
-include $(OBJS_DEBUG:%.o=%.d)
