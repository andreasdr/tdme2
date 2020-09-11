#pragma once

#include <array>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>

using std::array;

/** 
 * Renderer specular material data entity
 */
struct tdme::engine::subsystems::renderer::Renderer_SpecularMaterial
{
	array<float, 4> ambient {{ 0.2f, 0.2f, 0.2f, 1.0f }};
	array<float, 4> diffuse {{ 0.8f, 0.8f, 0.8f, 1.0f }};
	array<float, 4> specular {{ 0.0f, 0.0f, 0.0f, 1.0f }};
	array<float, 4> emission {{ 0.0f, 0.0f, 0.0f, 1.0f }};
	float shininess { 0.0f };
	int diffuseTextureMaskedTransparency { 0 };
	float diffuseTextureMaskedTransparencyThreshold { 0.0f };
	int textureAtlasSize { 1 };
	array<float, 2>  textureAtlasPixelDimension { 0.0f, 0.0f };
};
