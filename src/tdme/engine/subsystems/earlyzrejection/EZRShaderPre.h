#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/earlyzrejection/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>

using std::vector;
using std::map;
using std::string;

using tdme::engine::Engine;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::earlyzrejection::EZRShaderPreImplementation;
using tdme::math::Matrix4x4;

/**
 * Early z rejection shader
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::earlyzrejection::EZRShaderPre final
{
private:
	struct EZRShaderPreContext {
		EZRShaderPreImplementation* implementation { nullptr };
	};

	map<string, EZRShaderPreImplementation*> shader;
	bool running { false };
	Engine* engine { nullptr };
	Renderer* renderer { nullptr };
	vector<EZRShaderPreContext> contexts;

public:
	/**
	 * Constructor
	 * @param renderer renderer
	 */
	EZRShaderPre(Renderer* renderer);

	/**
	 * Destructor
	 */
	~EZRShaderPre();

	/**
	 * @return if initialized and ready to use
	 */
	bool isInitialized();

	/**
	 * Init shadow mapping
	 */
	void initialize();

	/**
	 * Use pre render shadow mapping program
	 * @param engine engine
	 */
	void useProgram(Engine* engine);

	/**
	 * Un use pre render shadow mapping program
	 */
	void unUseProgram();

	/**
	 * Set up pre program mvp matrix
	 * @param renderer renderer
	 * @param context context
	 */
	void updateMatrices(void* context);

	/**
	 * Set up pre program texture matrix
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
