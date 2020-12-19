#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/earlyzrejection/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::subsystems::earlyzrejection::EZRShaderImplementation;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::math::Matrix4x4;

/**
 * Early z rejection shader
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::earlyzrejection::EZRShader final
{
private:
	struct EZRShaderContext {
		EZRShaderImplementation* implementation { nullptr };
	};

	map<string, EZRShaderImplementation*> shader;
	bool running { false };
	Engine* engine { nullptr };
	Renderer* renderer { nullptr };
	vector<EZRShaderContext> contexts;

public:
	/**
	 * Constructor
	 * @param renderer renderer
	 */
	EZRShader(Renderer* renderer);

	/**
	 * Destructor
	 */
	~EZRShader();

	/**
	 * @return if initialized and ready to use
	 */
	bool isInitialized();

	/**
	 * Init EZR shader
	 */
	void initialize();

	/**
	 * Use EZR render shader program
	 * @param engine engine
	 */
	void useProgram(Engine* engine);

	/**
	 * Unuse EZR render shader program
	 */
	void unUseProgram();

	/**
	 * Update program matrices
	 * @param renderer renderer
	 * @param context context
	 */
	void updateMatrices(void* context);

	/**
	 * Set up program texture matrix
	 * @param context context
	 * @param renderer renderer
	 */
	void updateTextureMatrix(void* context);

	/**
	 * Update material
	 * @param renderer renderer
	 * @param context context
	 */
	void updateMaterial(void* context);

	/**
	 * Bind texture
	 * @param renderer renderer
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
