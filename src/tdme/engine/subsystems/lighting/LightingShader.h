#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/lighting/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::subsystems::lighting::LightingShaderImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;

/**
 * Interface to lighting shader program
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::lighting::LightingShader final
{
private:
	struct LightingShaderContext {
		LightingShaderImplementation* implementation { nullptr };
	};
	map<string, LightingShaderImplementation*> shader;
	Renderer* renderer { nullptr };
	Engine* engine { nullptr };
	bool running { false };
	vector<LightingShaderContext> contexts;

public:
	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	LightingShader(Renderer* renderer);

	/**
	 * Destructor
	 */
	~LightingShader();

	/**
	 * @return initialized and ready to be used
	 */
	bool isInitialized();

	/**
	 * Initialize renderer
	 */
	void initialize();

	/**
	 * Use lighting program
	 * @param engine engine
	 */
	void useProgram(Engine* engine);

	/**
	 * Unuse lighting program
	 */
	void unUseProgram();

	/**
	 * Update effect to program
	 * @param context context
	 */
	void updateEffect(void* context);

	/**
	 * Update material to program
	 * @param context context
	 */
	void updateMaterial(void* context);

	/**
	 * Update light to program
	 * @param context context
	 * @param lightId light id
	 */
	void updateLight(void* context, int32_t lightId);

	/**
	 * Update matrices to program
	 * @param context context
	 */
	void updateMatrices(void* context);

	/**
	 * Update texture matrix to program
	 * @param context context
	 */
	void updateTextureMatrix(void* context);

	/**
	 * Update shader parameters
	 * @param context
	 */
	void updateShaderParameters(void* context);

	/**
	 * Set shader
	 * @param context context
	 * @param id id
	 */
	void setShader(void* context, const string& id);

	/**
	 * Bind texture
	 * @param context context
	 * @param textureId texture id
	 */
	void bindTexture(void* context, int32_t textureId);

};
