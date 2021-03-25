#version 330

// based on: https://fabiensanglard.net/lightScattering/index.php

{$DEFINITIONS}

#define FALSE		0
#define MAX_LIGHTS	8

#define EXPOSURE	0.0075
#define DECAY		1.0
#define DENSITY		0.84
#define WEIGHT		5.65
#define SAMPLES		48

// light structure
struct Light {
	int enabled;
	vec2 position;
	float intensity;
};

// uniforms
uniform Light lights[MAX_LIGHTS];
uniform sampler2D colorBufferTextureUnit;
uniform float bufferTexturePixelWidth;
uniform float bufferTexturePixelHeight;

// passed from vertex shader
in vec2 vsFragTextureUV;

// passed out
out vec4 outColor;

// main
void main(void) {
	outColor = vec4(0.0, 0.0, 0.0, 0.0);
	for (int i = 0; i < MAX_LIGHTS; i++) {
		// skip on disabled lights
		if (lights[i].enabled == FALSE) continue;

		//
		vec2 deltaTextureCoordinateTotal = vsFragTextureUV - lights[i].position;
		vec2 deltaTextureCoordinate = deltaTextureCoordinateTotal;
		vec2 textureCoordinate = vsFragTextureUV;
		deltaTextureCoordinate *= 1.0 /  float(SAMPLES) * DENSITY;
		float illuminationDecay = 1.0;
		if (length(deltaTextureCoordinateTotal - deltaTextureCoordinate) < 0.5) {
			for(int j = 0; j < SAMPLES; j++) {
				float lengthAtFragment = length(deltaTextureCoordinateTotal - deltaTextureCoordinate);
				if (lengthAtFragment >= 0.5) {
					break;
				}
				vec4 textureSample = texture(colorBufferTextureUnit, textureCoordinate);
				textureSample *= illuminationDecay * WEIGHT;
				outColor+= textureSample * (1.0 - (clamp(lengthAtFragment, 0.0, 0.5) / 0.5)) * lights[i].intensity;
				illuminationDecay *= DECAY;
				textureCoordinate -= deltaTextureCoordinate;
			}
		}
	}
	outColor*= EXPOSURE;
	outColor.a = 1.0;
}
