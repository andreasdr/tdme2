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

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::shadowmapping::ShadowMapCreationShaderImplementation;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;

/**
 * Shadow mapping shader to create a shadow map
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::shadowmapping::ShadowMapCreationShader final
{
private:
	struct ShadowMapCreationShaderContext {
		ShadowMapCreationShaderImplementation* implementation { nullptr };
	};
	map<string, ShadowMapCreationShaderImplementation*> shader;
	bool running { false };
	Engine* engine { nullptr };
	Renderer* renderer { nullptr };
	vector<ShadowMapCreationShaderContext> contexts;

public:
	/**
	 * Constructor
	 * @param renderer renderer
	 */
	ShadowMapCreationShader(Renderer* renderer);

	/**
	 * Destructor
	 */
	~ShadowMapCreationShader();

	/**
	 * @return if initialized and ready to use
	 */
	bool isInitialized();

	/**
	 * Init shadow map creation shader program
	 */
	void initialize();

	/**
	 * Use shadow map creation shader program
	 * @param engine engine
	 */
	void useProgram(Engine* engine);

	/**
	 * Unuse shadow map creation shader program
	 */
	void unUseProgram();

	/**
	 * Set up matrices
	 * @param context context
	 */
	void updateMatrices(void* context);

	/**
	 * Set up texture matrix
	 * @param context context
	 */
	void updateTextureMatrix(void* context);

	/**
	 * Update material
	 * @param context context
	 */
	void updateMaterial(void* context);

	/**
	 * Bind texture
	 * @param context context
	 * @param textureId texture id
	 */
	void bindTexture(void* context, int32_t textureId);

	/**
	 * Set shader
	 * @param context
	 * @param id shader id
	 */
	void setShader(void* context, const string& id);

};
