// based on http://fabiensanglard.net/shadowmapping/index.php, modified by me

#version 100

precision highp float;
precision highp sampler2D;

#define SHADOWMAP_LOOKUPS	2

uniform sampler2D textureUnit;
uniform float texturePixelWidth;
uniform float texturePixelHeight;

uniform sampler2D diffuseTextureUnit;
uniform int diffuseTextureAvailable;
uniform int diffuseTextureMaskedTransparency;
uniform float diffuseTextureMaskedTransparencyThreshold;

uniform vec3 lightDirection;

uniform int textureAtlasSize;
uniform vec2 textureAtlasPixelDimension;

// passed from vertex shader
varying vec2 vsFragTextureUV;
varying vec4 vsShadowCoord;
varying float vsShadowIntensity;

void main() {
	// retrieve diffuse texture color value
	if (diffuseTextureAvailable == 1) {
		// texture coordinate, also take atlas into account
		vec2 fragTextureUV;
		if (textureAtlasSize > 1) {
			#define ATLAS_TEXTURE_BORDER	32
			vec2 diffuseTextureAtlasIdx = floor(vsFragTextureUV / 1000.0);
			vec2 diffuseTextureAtlasCoord = vsFragTextureUV - 500.0 - diffuseTextureAtlasIdx * 1000.0;
			vec2 diffuseTextureAtlasTextureDimensions = vec2(1.0 / float(textureAtlasSize));
			fragTextureUV =
				mod(diffuseTextureAtlasCoord, vec2(1.0 - textureAtlasPixelDimension)) /
				float(textureAtlasSize) *
				vec2((diffuseTextureAtlasTextureDimensions - (float(ATLAS_TEXTURE_BORDER) * 2.0 * textureAtlasPixelDimension)) / diffuseTextureAtlasTextureDimensions) +
				vec2(float(ATLAS_TEXTURE_BORDER) * textureAtlasPixelDimension) +
				diffuseTextureAtlasTextureDimensions * diffuseTextureAtlasIdx;
		} else {
			fragTextureUV = vsFragTextureUV;
		}
		// fetch from texture
		vec4 diffuseTextureColor = texture2D(diffuseTextureUnit, fragTextureUV);
		// check if to handle diffuse texture masked transparency
		if (diffuseTextureMaskedTransparency == 1) {
			// discard if beeing transparent
			if (diffuseTextureColor.a < diffuseTextureMaskedTransparencyThreshold) discard;
		}
	}

	// do not process samples out of frustum, or out of shadow map
	if (vsShadowCoord.w == 0.0 ||
		vsShadowCoord.x < 0.0 || vsShadowCoord.x > 1.0 ||
		vsShadowCoord.y < 0.0 || vsShadowCoord.y > 1.0) {
		// return color to be blended with framebuffer
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	} else {
		// depth bias
		float depthBias = 0.0;

		// determine visibility
		float visibility = 0.0;
		for (int y = 0; y < SHADOWMAP_LOOKUPS; y++)
		for (int x = 0; x < SHADOWMAP_LOOKUPS; x++) {
			visibility+= texture2D(
				textureUnit,
				vsShadowCoord.xy +
					vec2(
						(float(-SHADOWMAP_LOOKUPS) / 2.0 + 0.5 + float(x)) * texturePixelWidth,
						(float(-SHADOWMAP_LOOKUPS) / 2.0 + 0.5 + float(y)) * texturePixelHeight
					)
			).x < vsShadowCoord.z + depthBias?0.50:0.0;
		}
		visibility = visibility / (float(SHADOWMAP_LOOKUPS) * float(SHADOWMAP_LOOKUPS));

		// return color to be blended with framebuffer
		gl_FragColor = vec4(0.0, 0.0, 0.0, visibility * vsShadowIntensity * 0.5);
	}
}
