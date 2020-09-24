#pragma once

#include <array>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/ParticleSystemEntity.h>
#include <tdme/engine/Light.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/subsystems/earlyzrejection/fwd-tdme.h>
#include <tdme/engine/subsystems/framebuffer/fwd-tdme.h>
#include <tdme/engine/subsystems/lighting/fwd-tdme.h>
#include <tdme/engine/subsystems/lines/fwd-tdme.h>
#include <tdme/engine/subsystems/manager/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer_InstancedRenderFunctionParameters.h>
#include <tdme/engine/subsystems/particlesystem/fwd-tdme.h>
#include <tdme/engine/subsystems/postprocessing/fwd-tdme.h>
#include <tdme/engine/subsystems/postprocessing/PostProcessingProgram.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/fwd-tdme.h>
#include <tdme/engine/subsystems/skinning/fwd-tdme.h>
#include <tdme/engine/subsystems/texture2D/fwd-tdme.h>
#include <tdme/gui/fwd-tdme.h>
#include <tdme/gui/nodes/fwd-tdme.h>
#include <tdme/gui/renderer/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix2D3x3.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/os/threading/Thread.h>

using std::array;
using std::map;
using std::vector;
using std::string;
using std::unordered_map;
using std::to_string;

using tdme::engine::Camera;
using tdme::engine::Entity;
using tdme::engine::EntityHierarchy;
using tdme::engine::EntityPickingFilter;
using tdme::engine::FogParticleSystem;
using tdme::engine::FrameBuffer;
using tdme::engine::Light;
using tdme::engine::LinesObject3D;
using tdme::engine::LODObject3D;
using tdme::engine::Object3D;
using tdme::engine::Object3DRenderGroup;
using tdme::engine::ObjectParticleSystem;
using tdme::engine::ParticleSystemEntity;
using tdme::engine::ParticleSystemGroup;
using tdme::engine::Partition;
using tdme::engine::PointsParticleSystem;
using tdme::engine::Timing;
using tdme::engine::model::Color4;
using tdme::engine::model::Group;
using tdme::engine::model::Material;
using tdme::engine::subsystems::earlyzrejection::EZRShaderPre;
using tdme::engine::subsystems::framebuffer::FrameBufferRenderShader;
using tdme::engine::subsystems::lighting::LightingShader;
using tdme::engine::subsystems::lines::LinesShader;
using tdme::engine::subsystems::manager::MeshManager;
using tdme::engine::subsystems::manager::TextureManager;
using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::rendering::EntityRenderer_InstancedRenderFunctionParameters;
using tdme::engine::subsystems::rendering::TransparentRenderFacesPool;
using tdme::engine::subsystems::particlesystem::ParticlesShader;
using tdme::engine::subsystems::postprocessing::PostProcessing;
using tdme::engine::subsystems::postprocessing::PostProcessingProgram;
using tdme::engine::subsystems::postprocessing::PostProcessingShader;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapping;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderPre;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRender;
using tdme::engine::subsystems::skinning::SkinningShader;
using tdme::engine::subsystems::texture2D::Texture2DRenderShader;
using tdme::gui::GUI;
using tdme::gui::renderer::GUIRenderer;
using tdme::gui::renderer::GUIShader;
using tdme::math::Matrix2D3x3;
using tdme::math::Matrix4x4;
using tdme::math::Vector2;
using tdme::math::Vector3;
using tdme::os::threading::Thread;

/**
 * Engine main class
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::Engine final
{
	friend class EngineGL3Renderer;
	friend class EngineGL2Renderer;
	friend class EngineGLES2Renderer;
	friend class EngineVKRenderer;
	friend class EntityHierarchy;
	friend class FogParticleSystem;
	friend class FrameBuffer;
	friend class Object3D;
	friend class Object3DRenderGroup;
	friend class LinesObject3D;
	friend class LODObject3D;
	friend class ParticleSystemGroup;
	friend class ObjectParticleSystem;
	friend class PointsParticleSystem;
	friend class SkinnedObject3DRenderGroup;
	friend class tdme::engine::subsystems::framebuffer::FrameBufferRenderShader;
	friend class tdme::engine::subsystems::lines::LinesObject3DInternal;
	friend class tdme::engine::subsystems::rendering::BatchRendererPoints;
	friend class tdme::engine::subsystems::rendering::BatchRendererTriangles;
	friend class tdme::engine::subsystems::rendering::Object3DBase;
	friend class tdme::engine::subsystems::rendering::Object3DGroup;
	friend class tdme::engine::subsystems::rendering::Object3DGroupRenderer;
	friend class tdme::engine::subsystems::rendering::EntityRenderer;
	friend class tdme::engine::subsystems::rendering::Object3DInternal;
	friend class tdme::engine::subsystems::rendering::Object3DGroupMesh;
	friend class tdme::engine::subsystems::rendering::ObjectBuffer;
	friend class tdme::engine::subsystems::rendering::TransparentRenderFacesGroup;
	friend class tdme::engine::subsystems::particlesystem::FogParticleSystemInternal;
	friend class tdme::engine::subsystems::particlesystem::ParticlesShader;
	friend class tdme::engine::subsystems::particlesystem::PointsParticleSystemInternal;
	friend class tdme::engine::subsystems::postprocessing::PostProcessingProgram;
	friend class tdme::engine::subsystems::shadowmapping::ShadowMapping;
	friend class tdme::engine::subsystems::skinning::SkinningShader;
	friend class tdme::engine::subsystems::texture2D::Texture2DRenderShader;
	friend class tdme::gui::GUI;
	friend class tdme::gui::nodes::GUIImageNode;
	friend class tdme::gui::nodes::GUINode;
	friend class tdme::gui::renderer::GUIRenderer;
	friend class tdme::gui::renderer::GUIFont;

public:
	enum AnimationProcessingTarget { NONE, CPU, CPU_NORENDERING, GPU };
	enum ShaderType { OBJECT3D };
	enum EffectPass { EFFECTPASS_NONE, EFFECTPASS_LIGHTSCATTERING, EFFECTPASS_COUNT };
	static constexpr int LIGHTS_MAX { 8 };

protected:
	static Engine* currentEngine;

private:
	static Engine* instance;
	static Renderer* renderer;

	static TextureManager* textureManager;
	static VBOManager* vboManager;
	static MeshManager* meshManager;
	static GUIRenderer* guiRenderer;

	static AnimationProcessingTarget animationProcessingTarget;

	static EZRShaderPre* ezrShaderPre;
	static ShadowMappingShaderPre* shadowMappingShaderPre;
	static ShadowMappingShaderRender* shadowMappingShaderRender;
	static LightingShader* lightingShader;
	static ParticlesShader* particlesShader;
	static LinesShader* linesShader;
	static SkinningShader* skinningShader;
	static GUIShader* guiShader;
	static FrameBufferRenderShader* frameBufferRenderShader;
	static PostProcessing* postProcessing;
	static PostProcessingShader* postProcessingShader;
	static Texture2DRenderShader* texture2DRenderShader;
	static int threadCount;
	static bool have4K;
	static float animationBlendingTime;
	static int32_t shadowMapWidth;
	static int32_t shadowMapHeight;
	static int32_t shadowMapRenderLookUps;
	static float shadowMaplightEyeDistanceScale;
	static float transformationsComputingReduction1Distance;
	static float transformationsComputingReduction2Distance;
	static int32_t lightSourceTextureId;

	struct Shader {
		ShaderType type;
		string id;
		map<string, string> parameterTypes;
		map<string, string> parameterDefaults;
	};

	static map<string, Shader> shaders;

	int32_t width { -1 };
	int32_t height { -1 };
	int32_t scaledWidth { -1 };
	int32_t scaledHeight { -1 };
	GUI* gui { nullptr };
	Timing* timing { nullptr };
	Camera* camera { nullptr };

	Partition* partition { nullptr };

	array<Light, LIGHTS_MAX> lights;
	Color4 sceneColor;
	bool renderLightSourceEnabled;
	float lightSourceSize;
	Vector3 lightSourcePosition;
	bool fixedLightScatteringIntensity;
	float lightScatteringItensityValue;
	FrameBuffer* frameBuffer { nullptr };
	FrameBuffer* postProcessingFrameBuffer1 { nullptr };
	FrameBuffer* postProcessingFrameBuffer2{ nullptr };
	FrameBuffer* postProcessingTemporaryFrameBuffer { nullptr };
	array<FrameBuffer*, EFFECTPASS_COUNT - 1> effectPassFrameBuffers;
	array<bool, EFFECTPASS_COUNT - 1> effectPassSkip;
	ShadowMapping* shadowMapping { nullptr };

	map<string, Entity*> entitiesById;
	map<string, ParticleSystemEntity*> autoEmitParticleSystemEntities;
	map<string, Entity*> noFrustumCullingEntitiesById;

	vector<Object3D*> visibleObjects;
	vector<Object3D*> visibleObjectsPostPostProcessing;
	vector<Object3D*> visibleObjectsNoDepthTest;
	vector<LODObject3D*> visibleLODObjects;
	vector<ObjectParticleSystem*> visibleOpses;
	vector<Entity*> visiblePpses;
	vector<ParticleSystemGroup*> visiblePsgs;
	vector<LinesObject3D*> visibleLinesObjects;
	vector<Object3DRenderGroup*> visibleObjectRenderGroups;
	vector<EntityHierarchy*> visibleObjectEntityHierarchies;
	vector<Entity*> noFrustumCullingEntities;

	vector<Object3D*> visibleEZRObjects;

	EntityRenderer* entityRenderer { nullptr };

	static bool skinningShaderEnabled;
	bool shadowMappingEnabled;
	bool renderingInitiated;
	bool renderingComputedTransformations;

	vector<string> postProcessingPrograms;

	bool initialized;

	bool isUsingPostProcessingTemporaryFrameBuffer;

	class EngineThread: public Thread {
		friend class Engine;
	private:
		int idx;
		void* context;
	public:
		enum State { STATE_WAITING, STATE_TRANSFORMATIONS, STATE_RENDERING, STATE_SPINNING };

		Engine* engine;

		struct {
			EntityRenderer_InstancedRenderFunctionParameters parameters;
			unordered_map<string, unordered_map<string, vector<Object3D*>>> objectsByShadersAndModels;
			TransparentRenderFacesPool* transparentRenderFacesPool;
		} rendering;

		volatile State state { STATE_WAITING };

	private:
		/**
		 * Constructor
		 * @param idx thread index
		 * @param context context
		 */
		EngineThread(int idx, void* context);

		/**
		 * Run
		 */
		virtual void run();
	};

	static vector<EngineThread*> engineThreads;

	/**
	 * @return mesh manager
	 */
	inline static MeshManager* getMeshManager() {
		return meshManager;
	}

	/**
	 * @return shadow mapping or nullptr if disabled
	 */
	inline ShadowMapping* getShadowMapping() {
		return shadowMapping;
	}

	/**
	 * @return shadow mapping shader
	 */
	inline static ShadowMappingShaderPre* getShadowMappingShaderPre() {
		return shadowMappingShaderPre;
	}

	/**
	 * @return shadow mapping shader
	 */
	inline static ShadowMappingShaderRender* getShadowMappingShaderRender() {
		return shadowMappingShaderRender;
	}

	/**
	 * @return lighting shader
	 */
	inline static LightingShader* getLightingShader() {
		return lightingShader;
	}

	/**
	 * @return particles shader
	 */
	inline static ParticlesShader* getParticlesShader() {
		return particlesShader;
	}

	/**
	 * @return lines shader
	 */
	inline static LinesShader* getLinesShader() {
		return linesShader;
	}

	/**
	 * @return skinning shader
	 */
	inline static SkinningShader* getSkinningShader() {
		return skinningShader;
	}

	/**
	 * @return GUI shader
	 */
	inline static GUIShader* getGUIShader() {
		return guiShader;
	}

	/**
	 * @return frame buffer render shader
	 */
	inline static FrameBufferRenderShader* getFrameBufferRenderShader() {
		return frameBufferRenderShader;
	}

	/**
	 * @return post processing shader
	 */
	inline static PostProcessingShader* getPostProcessingShader() {
		return postProcessingShader;
	}

	/**
	 * @return object 3d renderer
	 */
	inline EntityRenderer* getEntityRenderer() {
		return entityRenderer;
	}

	/**
	 * Determine entity types
	 * @param entities given entities to investigate
	 * @param objects object
	 * @param objectsPostPostProcessing objects that will be rendered after post processing
	 * @param objectsNoDepthTest objects that will render without depth test
	 * @param lodObjects LOD objects
	 * @param opses object particle systems
	 * @param ppses point particle systems
	 * @param psgs particle system groups
	 * @param linesObjects lines objects
	 * @param objectRenderGroups object render groups
	 * @param entityHierarchies entity hierarchies
	 */
	void determineEntityTypes(
		const vector<Entity*>& entities,
		vector<Object3D*>& objects,
		vector<Object3D*>& objectsPostPostProcessing,
		vector<Object3D*>& objectsNoDepthTest,
		vector<LODObject3D*>& lodObjects,
		vector<ObjectParticleSystem*>& opses,
		vector<Entity*>& ppses,
		vector<ParticleSystemGroup*>& psgs,
		vector<LinesObject3D*>& linesObjects,
		vector<Object3DRenderGroup*>& objectRenderGroups,
		vector<EntityHierarchy*>& entityHierarchies
	);

	/**
	 * Computes visibility and transformations
	 * @param threadCount thread count
	 * @param threadIdx thread idx
	 */
	void computeTransformationsFunction(int threadCount, int threadIdx);

	/**
	 * Computes visibility and transformations
	 */
	void computeTransformations();

	/**
	 * Set up GUI mode rendering
	 */
	void initGUIMode();

	/**
	 * Set up GUI mode rendering
	 */
	void doneGUIMode();

	/**
	 * Initiates the rendering process
	 * updates timing, updates camera
	 */
	void initRendering();

	/**
	 * Private constructor
	 */
	Engine();
public:
	/**
	 * @return texture manager
	 */
	inline static TextureManager* getTextureManager() {
		return Engine::textureManager;
	}


	/**
	 * @return vertex buffer object manager
	 */
	inline static VBOManager* getVBOManager() {
		return vboManager;
	}

	/**
	 * @return engine thread count
	 */
	inline static int getThreadCount() {
		return Engine::threadCount;
	}

	/**
	 * Set engine thread count
	 * @param threadCount engine thread count
	 */
	inline static void setThreadCount(int threadCount) {
		Engine::threadCount = threadCount;
	}

	/**
	 * @return if having 4k
	 */
	inline static bool is4K() {
		return Engine::have4K;
	}

	/**
	 * Set if having 4k
	 * @param have4K have 4k
	 */
	inline static void set4K(bool have4K) {
		Engine::have4K = have4K;
	}

	/**
	 * @return animation blending time
	 */
	inline static float getAnimationBlendingTime() {
		return Engine::animationBlendingTime;
	}

	/**
	 * Set animation blending time
	 * @param animationBlendingTime animation blending time
	 */
	inline static void setAnimationBlendingTime(float animationBlendingTime) {
		Engine::animationBlendingTime = animationBlendingTime;
	}

	/**
	 * @return shadow map light eye distance scale
	 */
	inline static float getShadowMapLightEyeDistanceScale() {
		return Engine::shadowMaplightEyeDistanceScale;
	}

	/**
	 * Set shadow map light eye distance scale
	 * @param shadowMaplightEyeDistanceScale shadow map light eye distance scale
	 */
	inline static void setShadowMapLightEyeDistanceScale(float shadowMaplightEyeDistanceScale) {
		Engine::shadowMaplightEyeDistanceScale = shadowMaplightEyeDistanceScale;
	}

	/**
	 * @return shadow map width
	 */
	inline static int32_t getShadowMapWidth() {
		return Engine::shadowMapWidth;
	}

	/**
	 * @return shadow map width
	 */
	inline static int32_t getShadowMapHeight() {
		return Engine::shadowMapHeight;
	}

	/**
	 * @return shadow map render look ups
	 */
	inline static int32_t getShadowMapRenderLookUps() {
		return Engine::shadowMapRenderLookUps;
	}

	/**
	 * Set shadow map size
	 * @param width width
	 * @param height height
	 */
	inline static void setShadowMapSize(int32_t width, int32_t height) {
		Engine::shadowMapWidth = width;
		Engine::shadowMapHeight = height;
	}

	/**
	 * Set shadowmap look ups for each pixel when rendering
	 * @param shadow map render look ups
	 */
	inline static void setShadowMapRenderLookUps(int32_t shadowMapRenderLookUps) {
		Engine::shadowMapRenderLookUps = shadowMapRenderLookUps;
	}

	/**
	 * @return distance of animated object including skinned objects from which animation computation will be computed only every second frame
	 */
	inline static float getTransformationsComputingReduction1Distance() {
		return Engine::transformationsComputingReduction1Distance;
	}

	/**
	 * Set distance of animated object including skinned objects from camera which animation computation will be computed only every second frame
	 * @param skinningComputingReduction1Distance distance
	 */
	inline static void setTransformationsComputingReduction1Distance(float transformationsComputingReduction1Distance) {
		Engine::transformationsComputingReduction1Distance = transformationsComputingReduction1Distance;
	}

	/**
	 * @return distance of animated object including skinned objects from which animation computation will be computed only every forth frame
	 */
	inline static float getTransformationsComputingReduction2Distance() {
		return Engine::transformationsComputingReduction2Distance;
	}

	/**
	 * Set distance of animated object including skinned objects from camera which animation computation will be computed only every forth frame
	 * @param skinningComputingReduction2Distance distance
	 */
	inline static void setTransformationsComputingReduction2Distance(float transformationsComputingReduction2Distance) {
		Engine::transformationsComputingReduction2Distance = transformationsComputingReduction2Distance;
	}

	/**
	 * Returns registered shaders for given type
	 * @param type type
	 */
	static const vector<string> getRegisteredShader(ShaderType type);

	/**
	 * Register shader
	 * @param type shader type
	 * @param shaderId shader id
	 * @param parameterTypes parameter types
	 * @param parameterDefaults parameter defaults
	 */
	static void registerShader(ShaderType type, const string& shaderId, const map<string, string> parameterTypes, const map<string, string> parametersDefaults);

	/**
	 * Returns parameter types of shader with given id
	 * @param shaderId shader id
	 * @return shader parameter types
	 */
	static const map<string, string> getShaderParameterTypes(const string& shaderId);

	/**
	 * Returns parameter default value of shader with given id
	 * @param shaderId shader id
	 * @return shader parameter defaults
	 */
	static const map<string, string> getShaderParameterDefaults(const string& shaderId);

	/**
	 * Returns engine instance
	 * @return
	 */
	static Engine* getInstance();

	/**
	 * Creates an offscreen rendering instance
	 * Note:
	 * - the root engine must have been initialized before
	 * - the created offscreen engine must not be initialized
	 * @param width width
	 * @param height height
	 * @param enableShadowMapping enable shadow mapping
	 * @return off screen engine
	 */
	static Engine* createOffScreenInstance(int32_t width, int32_t height, bool enableShadowMapping);

	/**
	 * @return if initialized and ready to be used
	 */
	inline bool isInitialized() {
		return initialized;
	}

	/**
	 * @return width
	 */
	inline int32_t getWidth() {
		return width;
	}

	/**
	 * @return height
	 */
	inline int32_t getHeight() {
		return height;
	}

	/**
	 * @return scaled width or -1 if not in use
	 */
	inline int32_t getScaledWidth() {
		return scaledWidth;
	}

	/**
	 * @return scaled height -1 if not in use
	 */
	inline int32_t getScaledHeight() {
		return scaledHeight;
	}

	/**
	 * @return GUI
	 */
	inline GUI* getGUI() {
		return gui;
	}

	/**
	 * @return Timing
	 */
	inline Timing* getTiming() {
		return timing;
	}

	/**
	 * @return Camera
	 */
	inline Camera* getCamera() {
		return camera;
	}

	/**
	 * @return partition
	 */
	inline Partition* getPartition() {
		return partition;
	}

	/**
	 * Set partition
	 * @param partition partition
	 */
	void setPartition(Partition* partition);

	/**
	 * @return frame buffer or nullptr
	 */
	inline FrameBuffer* getFrameBuffer() {
		return frameBuffer;
	}

	/**
	 * @return count of lights
	 */
	inline int32_t getLightCount() {
		return lights.size();
	}

	/**
	 * Returns light at idx (0 <= idx < 8)
	 * @param idx idx
	 * @return Light
	 */
	inline Light* getLightAt(int32_t idx) {
		return &lights[idx];
	}

	/**
	 * @return scene / background color
	 */
	inline const Color4& getSceneColor() const {
		return sceneColor;
	}

	/**
	 * Set scene color
	 * @param sceneColor scene color
	 */
	inline void setSceneColor(const Color4& sceneColor) {
		this->sceneColor = sceneColor;
	}

	/**
	 * Returns if rendering light source is enabled
	 * @return rendering light source is enabled
	 */
	inline bool isRenderLightSource() const {
		return renderLightSourceEnabled;
	}

	/**
	 * Set rendering light source enabled/disabled
	 * @param renderLightSource render light source enabled
	 */
	inline void setRenderLightSource(bool renderLightSourceEnabled) {
		this->renderLightSourceEnabled = renderLightSourceEnabled;
	}

	/**
	 * Returns light source size
	 * TODO: this is a hack until we have shader properties
	 * @return light source size (moon, sun)
	 */
	inline float getLightSourceSize() const {
		return lightSourceSize;
	}

	/**
	 * Set light source size (moon, sun)
	 * TODO: this is a hack until we have shader properties
	 * @param lightSourceSize light source size
	 */
	inline void setLightSourceSize(float lightSourceSize) {
		this->lightSourceSize = lightSourceSize;
	}

	/**
	 * Returns light source position(moon, sun)
	 * TODO: this is a hack until we have shader properties
	 * @return light source position
	 */
	inline const Vector3& getLightSourcePosition() const {
		return lightSourcePosition;
	}

	/**
	 * Set light source position(moon, sun)
	 * TODO: this is a hack until we have shader properties
	 * @param lightSourcePosition light source position
	 */
	inline void setLightSourcePosition(const Vector3& lightSourcePosition) {
		this->lightSourcePosition = lightSourcePosition;
	}

	/**
	 * Returns if light scattering intensity is fixed
	 * // TODO: this is a hack until we got shader parameters
	 * @return if light scattering intensity is fixed
	 */
	inline bool isFixedLightScatteringIntensity() {
		return fixedLightScatteringIntensity;
	}

	/**
	 * Sets if light scattering intensity is fixed
	 * // TODO: this is a hack until we got shader parameters
	 * @param fixedLightScatteringIntensity light scattering intensity is fixed
	 */
	inline void setFixedLightScatteringIntensity(float fixedLightScatteringIntensity) {
		this->fixedLightScatteringIntensity = fixedLightScatteringIntensity;
	}

	/**
	 * Returns light scattering intensity (base) value
	 * // TODO: this is a hack until we got shader parameters
	 * @return light scattering intensity base value
	 */
	inline float getLightScatteringItensityValue() {
		return lightScatteringItensityValue;
	}

	/**
	 * Set light scattering intensity base value
	 * // TODO: this is a hack until we got shader parameters
	 * @param lightScatteringItensityValue light scattering intensity base value
	 */
	inline void setLightScatteringItensityValue(float lightScatteringItensityValue) {
		lightScatteringItensityValue = lightScatteringItensityValue;
	}

	/**
	 * @return entity count
	 */
	inline int32_t getEntityCount() {
		return entitiesById.size();
	}

	/**
	 * Returns a entity by given id
	 * @param id id
	 * @return entity or nullptr
	 */
	inline Entity* getEntity(const string& id) {
		auto entityByIdIt = entitiesById.find(id);
		if (entityByIdIt != entitiesById.end()) {
			return entityByIdIt->second;
		}
		return nullptr;
	}

	/**
	 * Adds an entity by id
	 * @param entity object
	 */
	void addEntity(Entity* entity);

	/**
	 * Removes an entity
	 * @param id id
	 */
	void removeEntity(const string& id);

	/**
	 * Removes all entities and caches
	 */
	void reset();

	/**
	 * Initialize render engine
	 */
	void initialize();

	/**
	 * Reshape
	 * @param width width
	 * @param height height
	 */
	void reshape(int32_t width, int32_t height);

	/**
	 * Scale which applies to main engine only
	 * @param width width
	 * @param height height
	 */
	void scale(int32_t width, int32_t height);

	/**
	 * Disable scaling, which applies to main engine only
	 */
	void unscale();

	/**
	 * Render scaled main engine to screen
	 */
	void renderToScreen();

	/**
	 * Renders the scene
	 */
	void display();

	/**
	 * Compute world coordinate from mouse position and z value
	 * @param mouseX mouse x
	 * @param mouseY mouse y
	 * @param z z
	 * @param worldCoordinate world coordinate
	 */
	void computeWorldCoordinateByMousePosition(int32_t mouseX, int32_t mouseY, float z, Vector3& worldCoordinate);

	/**
	 * Compute world coordinate from mouse position
	 * TODO:
	 * this does not work with GLES2
	 * @param mouseX mouse x
	 * @param mouseY mouse y
	 * @param worldCoordinate world coordinate
	 */
	void computeWorldCoordinateByMousePosition(int32_t mouseX, int32_t mouseY, Vector3& worldCoordinate);

	/**
	 * Retrieves entity by mouse position
	 * @param mouseX mouse x
	 * @param mouseY mouse y
	 * @param filter filter
	 * @param object3DGroup pointer to store group of Object3D to if appliable
	 * @param particleSystemEntity pointer to store sub particle system entity if having a particle system group
	 * @return entity or nullptr
	 */
	inline Entity* getEntityByMousePosition(int32_t mouseX, int32_t mouseY, EntityPickingFilter* filter = nullptr, Group** object3DGroup = nullptr, ParticleSystemEntity** particleSystemEntity = nullptr) {
		return
			getEntityByMousePosition(
				false,
				mouseX,
				mouseY,
				visibleObjects,
				visibleObjectsPostPostProcessing,
				visibleObjectsNoDepthTest,
				visibleLODObjects,
				visibleOpses,
				visiblePpses,
				visiblePsgs,
				visibleLinesObjects,
				visibleObjectEntityHierarchies,
				filter,
				object3DGroup,
				particleSystemEntity
			);
	}

	/**
	 * Retrieves entity by mouse position with contact point
	 * @param mouseX mouse x
	 * @param mouseY mouse y
	 * @param contactPoint world coordinate of contact point
	 * @param filter filter
	 * @param object3DGroup pointer to store group of Object3D to if appliable
	 * @param particleSystemEntity pointer to store sub particle system entity if having a particle system group
	 * @return entity or nullptr
	 */
	Entity* getEntityByMousePosition(int32_t mouseX, int32_t mouseY, Vector3& contactPoint, EntityPickingFilter* filter = nullptr, Group** object3DGroup = nullptr, ParticleSystemEntity** particleSystemEntity = nullptr);

	/**
	 * Does a ray casting of visible 3d object based entities
	 * @param startPoint start point
	 * @param endPoint end point
	 * @param contactPoint world coordinate of contact point
	 * @param filter filter
	 * @return entity or nullptr
	 */
	inline Entity* doRayCasting(
		const Vector3& startPoint,
		const Vector3& endPoint,
		Vector3& contactPoint,
		EntityPickingFilter* filter = nullptr
	) {
		return
			doRayCasting(
				false,
				visibleObjects,
				visibleObjectsPostPostProcessing,
				visibleObjectsNoDepthTest,
				visibleLODObjects,
				visibleObjectEntityHierarchies,
				startPoint,
				endPoint,
				contactPoint,
				filter
			);
	}

	/**
	 * Retrieves object by mouse position
	 * @param mouseX mouse x
	 * @param mouseY mouse y
	 * @param filter filter
	 * @return entity or nullptr
	 */
	Entity* getEntityContactPointByMousePosition(int32_t mouseX, int32_t mouseY, EntityPickingFilter* filter);

	/**
	 * Convert screen coordinate by world coordinate
	 * @param worldCoordinate world woordinate
	 * @param screenCoordinate screen coordinate
	 * @param width optional render target width
	 * @param height optional render target height
	 * @returns if world coordinate is within frustum
	 */
	bool computeScreenCoordinateByWorldCoordinate(const Vector3& worldCoordinate, Vector2& screenCoordinate, int width = -1, int height = -1);

	/**
	 * Shutdown the engine
	 */
	void dispose();

	/**
	 * Creates a PNG file from current screen(
	 * 	This does not seem to work with GLES2 and offscreen engines
	 * @param pathName path name 
	 * @param fileName file name
	 * @return success
	 */
	bool makeScreenshot(const string& pathName, const string& fileName);

	/**
	 * Clear post processing programs
	 */
	void resetPostProcessingPrograms();

	/**
	 * Add post processing program
	 * @param programId program id
	 */
	void addPostProcessingProgram(const string& programId);

	/**
	 * Destructor
	 */
	~Engine();

private:
	/**
	 * Retrieves entity by mouse position
	 * @param forcePicking override picking to be always enabled
	 * @param mouseX mouse x
	 * @param mouseY mouse y
	 * @param objects objects
	 * @param objectsPostPostProcessing objects that will be rendered after post processing
	 * @param objectsNoDepthTest objects that will render without depth test
	 * @param lodObjects LOD objects
	 * @param opses object particle systems
	 * @param ppses point particle systems
	 * @param psgs particle system groups
	 * @param linesObjects lines objects
	 * @param entityHierarchies entity hierarchies
	 * @param filter filter
	 * @param object3DGroup pointer to store group of Object3D to if appliable
	 * @param particleSystemEntity pointer to store sub particle system entity if having a particle system group
	 * @return entity or nullptr
	 */
	Entity* getEntityByMousePosition(
		bool forcePicking,
		int32_t mouseX,
		int32_t mouseY,
		const vector<Object3D*>& objects,
		const vector<Object3D*>& objectsPostPostProcessing,
		const vector<Object3D*>& objectsNoDepthTest,
		const vector<LODObject3D*>& lodObjects,
		const vector<ObjectParticleSystem*>& opses,
		const vector<Entity*>& ppses,
		const vector<ParticleSystemGroup*>& psgs,
		const vector<LinesObject3D*>& linesObjects,
		const vector<EntityHierarchy*>& entityHierarchies,
		EntityPickingFilter* filter = nullptr,
		Group** object3DGroup = nullptr,
		ParticleSystemEntity** particleSystemEntity = nullptr
	);

	/**
	 * Does a ray casting of visible 3d object based entities
	 * @param forcePicking override picking to be always enabled
	 * @param objects objects
	 * @param objectsPostPostProcessing objects that will be rendered after post processing
	 * @param objectsNoDepthTest objects that will render without depth test
	 * @param lodObjects LOD objects
	 * @param entityHierarchies entity hierarchies
	 * @param startPoint start point
	 * @param endPoint end point
	 * @param contactPoint world coordinate of contact point
	 * @param filter filter
	 * @return entity or nullptr
	 */
	Entity* doRayCasting(
		bool forcePicking,
		const vector<Object3D*>& objects,
		const vector<Object3D*>& objectsPostPostProcessing,
		const vector<Object3D*>& objectsNoDepthTest,
		const vector<LODObject3D*>& lodObjects,
		const vector<EntityHierarchy*>& entityHierarchies,
		const Vector3& startPoint,
		const Vector3& endPoint,
		Vector3& contactPoint,
		EntityPickingFilter* filter = nullptr
	);

	/**
	 * Removes a entity from internal lists, those entities can also be sub entities from entity hierarchy or particle system groups and such
	 * @param entity entity
	 */
	void deregisterEntity(Entity* entity);

	/**
	 * Adds a entity to internal lists, those entities can also be sub entities from entity hierarchy or particle system groups and such
	 * @param entity entity
	 */
	void registerEntity(Entity* entity);

	/**
	 * Do post processing
	 * @param renderPass render pass
	 * @param postProcessingFrameBuffers frame buffers to swap, input needs to live in postProcessingFrameBuffers[0]
	 * @param targetFrameBuffer target frame buffer
	 */
	void doPostProcessing(PostProcessingProgram::RenderPass renderPass, const array<FrameBuffer*, 2> postProcessingFrameBuffers, FrameBuffer* targetFrameBuffer);

	/**
	 * Do a render/effect pass
	 * @param effectPass effect pass
	 * @param shaderPrefix shader prefix
	 * @param useEZR if to use early Z rejection
	 * @param applyShadowMapping if to apply shadow mapping
	 * @param applyPostProcessing if to apply post processing
	 * @param doRenderLightSource do render light source
	 * @param doRenderParticleSystems if to render particle systems
	 * @param renderTypes render types
	 */
	void render(int32_t effectPass, const string& shaderPrefix, bool useEZR, bool applyShadowMapping, bool applyPostProcessing, bool doRenderLightSource, bool doRenderParticleSystems, int32_t renderTypes);

	/**
	 * Render light source
	 * @param width render target width
	 * @param height render target height
	 * @return if light source is visible
	 */
	bool renderLightSource(int width, int height);

};
