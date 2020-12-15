#pragma once

#include <tdme/engine/subsystems/lighting/fwd-tdme.h>

/**
 * Lighting shader constants
 * @author Andreas Drewke
 * @version $Id$
 */
struct tdme::engine::subsystems::lighting::LightingShaderConstants final
{
	static constexpr int32_t SPECULAR_TEXTUREUNIT_DIFFUSE { 0 };
	static constexpr int32_t SPECULAR_TEXTUREUNIT_SPECULAR { 1 };
	static constexpr int32_t SPECULAR_TEXTUREUNIT_NORMAL { 2 };
	static constexpr int32_t SPECULAR_TEXTUREUNIT_ENVIRONMENT { 3 };
	static constexpr int32_t SPECULAR_TEXTUREUNIT_TERRAIN_GRAS { 4 };
	static constexpr int32_t SPECULAR_TEXTUREUNIT_TERRAIN_DIRT { 5 };
	static constexpr int32_t SPECULAR_TEXTUREUNIT_TERRAIN_STONE { 6 };
	static constexpr int32_t SPECULAR_TEXTUREUNIT_TERRAIN_SNOW { 7 };

	static constexpr int32_t PBR_TEXTUREUNIT_BASECOLOR { 0 };
	static constexpr int32_t PBR_TEXTUREUNIT_METALLICROUGHNESS { 1 };
	static constexpr int32_t PBR_TEXTUREUNIT_NORMAL { 2 };
	static constexpr int32_t PBR_TEXTUREUNIT_ENVIRONMENT_DIFFUSE { 3 };
	static constexpr int32_t PBR_TEXTUREUNIT_ENVIRONMENT_SPECULAR { 4 };
	static constexpr int32_t PBR_TEXTUREUNIT_ENVIRONMENT_BRDF { 5 };

};
