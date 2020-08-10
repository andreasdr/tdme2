#version 330

{$DEFINITIONS}

#if defined(HAVE_4K)
	#define MAP_LOOKUPS_NEAR		4
	#define MAP_LOOKUPS_FAR		8
#else
	#define MAP_LOOKUPS_NEAR		2
	#define MAP_LOOKUPS_FAR		4
#endif

#define DISTANCE_NEAR	0.96
#define DISTANCE_FAR		0.98
#define DISTANCE_MAX		0.985

// uniforms
uniform sampler2D colorBufferTextureUnit;
uniform sampler2D depthBufferTextureUnit;
uniform float bufferTexturePixelWidth;
uniform float bufferTexturePixelHeight;

// passed from vertex shader
in vec2 vsFragTextureUV;

// passed out
out vec4 outColor;

// main
void main(void) {
	float depth = texture(depthBufferTextureUnit, vsFragTextureUV).r;
	vec4 originalColor = texture(colorBufferTextureUnit, vsFragTextureUV);
	originalColor.a = 1.0;
	vec4 blurredColor = vec4(0.0, 0.0, 0.0, 1.0);
	outColor = originalColor;
	if (depth == 0.0) {
		for (int y = 0; y < MAP_LOOKUPS_FAR; y++)
		for (int x = 0; x < MAP_LOOKUPS_FAR; x++) {
			vec2 textureUV = vsFragTextureUV.xy +
				vec2(
					(-MAP_LOOKUPS_FAR / 2.0 + 0.5 + x) * bufferTexturePixelWidth,
					(-MAP_LOOKUPS_FAR / 2.0 + 0.5 + y) * bufferTexturePixelHeight
				);
			textureUV.x = clamp(textureUV.x, 0.0, 1.0 - bufferTexturePixelWidth);
			textureUV.y = clamp(textureUV.y, 0.0, 1.0 - bufferTexturePixelHeight);
			if (texture(depthBufferTextureUnit, textureUV).r == 0.0) {
				depth = 1.0;
			}
		}
	}
	if (depth > DISTANCE_NEAR) {
		float intensity = clamp((depth - DISTANCE_NEAR) * 1.0 / (DISTANCE_FAR - DISTANCE_NEAR), 0.0, 1.0);
		for (int y = 0; y < MAP_LOOKUPS_NEAR; y++)
		for (int x = 0; x < MAP_LOOKUPS_NEAR; x++) {
			vec2 textureUV = vsFragTextureUV.xy +
				vec2(
					(-MAP_LOOKUPS_NEAR / 2.0 + 0.5 + x) * bufferTexturePixelWidth,
					(-MAP_LOOKUPS_NEAR / 2.0 + 0.5 + y) * bufferTexturePixelHeight
				);
			textureUV.x = clamp(textureUV.x, 0.0, 1.0 - bufferTexturePixelWidth);
			textureUV.y = clamp(textureUV.y, 0.0, 1.0 - bufferTexturePixelHeight);
			blurredColor+= texture(colorBufferTextureUnit, textureUV);
		}
		blurredColor/= MAP_LOOKUPS_NEAR * MAP_LOOKUPS_NEAR;
		blurredColor*= intensity;
		blurredColor+= originalColor * (1.0 - intensity);
		blurredColor.a = 1.0;
		outColor = blurredColor;
	}
	if (depth > DISTANCE_FAR) {
		originalColor = blurredColor;
		blurredColor = vec4(0.0, 0.0, 0.0, 0.0);
		float intensity = clamp((depth - DISTANCE_FAR) * 1.0 / (DISTANCE_MAX - DISTANCE_FAR), 0.0, 1.0);
		for (int y = 0; y < MAP_LOOKUPS_FAR; y++)
		for (int x = 0; x < MAP_LOOKUPS_FAR; x++) {
			vec2 textureUV = vsFragTextureUV.xy +
				vec2(
					(-MAP_LOOKUPS_FAR / 2.0 + 0.5 + x) * bufferTexturePixelWidth,
					(-MAP_LOOKUPS_FAR / 2.0 + 0.5 + y) * bufferTexturePixelHeight
				);
			textureUV.x = clamp(textureUV.x, 0.0, 1.0 - bufferTexturePixelWidth);
			textureUV.y = clamp(textureUV.y, 0.0, 1.0 - bufferTexturePixelHeight);
			blurredColor+= texture(colorBufferTextureUnit, textureUV);
		}
		blurredColor/= MAP_LOOKUPS_FAR * MAP_LOOKUPS_FAR;
		blurredColor*= intensity;
		blurredColor+= originalColor * (1.0 - intensity);
		blurredColor.a = 1.0;
		outColor = blurredColor;
	}
	gl_FragDepth = depth;
}
