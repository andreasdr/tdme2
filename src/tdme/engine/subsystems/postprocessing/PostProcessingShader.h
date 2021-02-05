#pragma once

#include <map>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/postprocessing/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>

using std::map;
using std::string;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Engine;
using tdme::engine::FrameBuffer;

/**
 * Post processing shader
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::postprocessing::PostProcessingShader final
{
private:
	map<string, PostProcessingShaderImplementation*> shader;
	PostProcessingShaderImplementation* implementation { nullptr };

	bool running { false };

public:
	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	PostProcessingShader(Renderer* renderer);

	/**
	 * Public destructor
	 */
	~PostProcessingShader();

	/**
	 * @return initialized and ready to be used
	 */
	bool isInitialized();

	/**
	 * Initialize renderer
	 */
	void initialize();

	/**
	 * Use program
	 */
	void useProgram();

	/**
	 * Unuse program
	 */
	void unUseProgram();

	/**
	 * Has post processing shader
	 * @param id shader id
	 * @return if shader exists and is initialized
	 */
	bool hasShader(const string& id);

	/**
	 * Set post processing shader
	 * @param context context
	 */
	void setShader(void* context, const string& id);

	/**
	 * Set source buffer pixel width
	 * @param context context
	 * @param pixelWidth pixel width
	 */
	void setBufferPixelWidth(void* context, float pixelWidth);

	/**
	 * Set source buffer pixel height
	 * @param context context
	 * @param pixelHeight pixel height
	 */
	void setBufferPixelHeight(void* context, float pixelHeight);

	/**
	 * Set shader parameters
	 * @param engine engine
	 */
	void setShaderParameters(void* context, Engine* engine);

};
