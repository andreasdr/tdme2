#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/shadowmapping/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::Engine;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRenderImplementation;
using tdme::math::Matrix4x4;

/**
 * Shadow mapping shader to render shadow maps
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::shadowmapping::ShadowMappingShaderRender final
{
private:
	struct ShadowMappingShaderRenderContext {
		ShadowMappingShaderRenderImplementation* implementation { nullptr };
	};
	map<string, ShadowMappingShaderRenderImplementation*> shader;
	bool running { false };
	Matrix4x4 depthBiasMVPMatrix;
	Engine* engine { nullptr };
	Renderer* renderer { nullptr };
	vector<ShadowMappingShaderRenderContext> contexts;
	int32_t lightId { -1 };

public:
	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	ShadowMappingShaderRender(Renderer* renderer);

	/**
	 * Destructor
	 */
	~ShadowMappingShaderRender();

	/**
	 * @return if initialized and ready to use
	 */
	bool isInitialized();

	/**
	 * Init shadow mapping
	 */
	void initialize();

	/**
	 * Use render shadow mapping program
	 * @param engine engine
	 */
	void useProgram(Engine* engine);

	/**
	 * Un use render shadow mapping program
	 */
	void unUseProgram();

	/**
	 * Update matrices
	 * @param context context
	 */
	void updateMatrices(void* context);

	/**
	 * Set up pre program texture matrix
	 * @param context context
	 */
	void updateTextureMatrix(void* context);

	/**
	 * Update material
	 * @param context context
	 */
	void updateMaterial(void* context);

	/**
	 * Update light
	 * @param context context
	 * @param lightId light id
	 */
	void updateLight(void* context, int32_t lightId);

	/**
	 * Bind texture
	 * @param context context
	 * @param textureId texture id
	 */
	void bindTexture(void* context, int32_t textureId);

	/**
	 * Set up program depth bias mvp matrix
	 * @param context context
	 * @param depthBiasMVPMatrix depth bias mvp matrix
	 */
	void setProgramDepthBiasMVPMatrix(void* context, const Matrix4x4& depthBiasMVPMatrix);

	/**
	 * Set light id
	 * @param lightId light id to render
	 */
	void setRenderLightId(int32_t lightId);

	/**
	 * Set shader
	 * @param id id
	 */
	void setShader(void* context, const string& id);

};
