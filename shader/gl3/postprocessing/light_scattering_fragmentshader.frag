#version 330

// see: https://fabiensanglard.net/lightScattering/index.php

#define EXPOSURE	0.003
#define DECAY		1.0
#define DENSITY		0.84
#define WEIGHT		5.65
#define SAMPLES		48

// uniforms
uniform sampler2D colorBufferTextureUnit;
uniform float bufferTexturePixelWidth;
uniform float bufferTexturePixelHeight;
uniform float textureLightPositionX;
uniform float textureLightPositionY;
uniform float intensity;

// passed from vertex shader
in vec2 vsFragTextureUV;

// passed out
out vec4 outColor;

// main
void main(void) {
	vec2 deltaTextureCoordinateTotal = vsFragTextureUV - vec2(textureLightPositionX, textureLightPositionY);
	vec2 deltaTextureCoordinate = deltaTextureCoordinateTotal;
	vec2 textureCoordinate = vsFragTextureUV.xy;
	deltaTextureCoordinate *= 1.0 /  float(SAMPLES) * DENSITY;
	float illuminationDecay = 1.0;
	outColor = vec4(0.0, 0.0, 0.0, 0.0);
	if (length(deltaTextureCoordinateTotal - deltaTextureCoordinate) < 0.5) {
		for(int i = 0; i < SAMPLES; i++) {
			float lengthAtFragment = length(deltaTextureCoordinateTotal - deltaTextureCoordinate);
			if (lengthAtFragment >= 0.5) {
				break;
			}
			vec4 sample = texture(colorBufferTextureUnit, textureCoordinate);
			sample*= illuminationDecay * WEIGHT;
			outColor+= sample * (1.0 - (clamp(lengthAtFragment, 0.0, 0.5) / 0.5));
			illuminationDecay *= DECAY;
			textureCoordinate -= deltaTextureCoordinate;
		}
	}
	outColor*= EXPOSURE;
	outColor.a = 0.5 * intensity;
}
