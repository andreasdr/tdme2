#pragma once

#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/fwd-tdme.h>

using std::vector;

using tdme::engine::Engine;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMap;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::math::Vector4;

/**
 * Shadow mapping module
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::shadowmapping::ShadowMapping final
{
	friend class ShadowMap;

private:
	enum ShadowMapping_RunState {NONE, PRE, RENDER};

	Renderer* renderer { nullptr };
	EntityRenderer* object3DRenderer { nullptr };

	Matrix4x4 shadowTransformationsMatrix;
	Matrix4x4 depthBiasMVPMatrix;

	Engine* engine { nullptr };

	vector<ShadowMap*> shadowMaps;
	ShadowMapping_RunState runState { NONE };

	vector<Object3D*> visibleObjectsReceivingShadows;

public:
	/**
	 * Constructor
	 * @param engine engine
	 * @param renderer renderer
	 * @param object3DRenderer object 3d renderer
	 */
	ShadowMapping(Engine* engine, Renderer* renderer, EntityRenderer* object3DRenderer);

	/**
	 * Destructor
	 */
	~ShadowMapping();

	/**
	 * @return engine
	 */
	Engine* getEngine();

	/**
	 * Reshape shadow maps
	 * @param width width
	 * @param height height
	 */
	void reshape(int32_t width, int32_t height);

	/**
	 * Create shadow maps
	 */
	void createShadowMaps();

	/**
	 * @return shadow map
	 * @param idx index
	 */
	ShadowMap* getShadowMap(int idx);

	/**
	 * Render shadow maps
	 * @param visibleObjects visible objects
	 */
	void renderShadowMaps(const vector<Object3D*>& visibleObjects);

	/**
	 * Dispose shadow maps
	 */
	void dispose();

	/**
	 * Start object transformations
	 * @param context context
	 * @param transformationsMatrix transformations matrix
	 */
	void startObjectTransformations(void* context, Matrix4x4& transformationsMatrix);

	/**
	 * End object transformations
	 */
	void endObjectTransformations();

	/**
	 * Update model view and projection matrix
	 * @param context context
	 */
	void updateMatrices(void* context);

	/**
	 * Update texture matrix
	 * @param context context
	 */
	void updateTextureMatrix(void* context);

	/**
	 * Update material
	 * @param context context
	 */
	void updateMaterial(void* context);

	/**
	 * Set shader
	 * @param context context
	 * @param id shader id
	 */
	void setShader(void* context, const string& id);

	/**
	 * Upload light
	 * @param lightId light id
	 */
	void updateLight(void* context, int32_t lightId);

	/**
	 * Bind texture
	 * @param textureId texture id
	 */
	void bindTexture(void* context, int32_t textureId);

	/**
	 * Update depth bias mvp matrix with given matrix
	 * @param context context
	 * @param depthBiasMVPMatrix depth bias MVP matrix
	 */
	void updateDepthBiasMVPMatrix(void* context, Matrix4x4& depthBiasMVPMatrix);

	/**
	 * Update depth bias mvp matrix / upload only
	 * @param context context
	 */
	void updateDepthBiasMVPMatrix(void* context);

};
