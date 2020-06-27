#version 330

// passed from geometry shader
in vec2 vsFragTextureUV;
in vec3 vsPosition;

// out
out vec4 outColor;

{$DEFINITIONS}

uniform sampler2D diffuseTextureUnit;
uniform int diffuseTextureAvailable;
uniform int diffuseTextureMaskedTransparency;
uniform float diffuseTextureMaskedTransparencyThreshold;

void main(void) {
	vec4 diffuseTextureColor;
	if (diffuseTextureAvailable == 1) {
		// fetch from texture
		diffuseTextureColor = texture(diffuseTextureUnit, vsFragTextureUV);
		// check if to handle diffuse texture masked transparency
		if (diffuseTextureMaskedTransparency == 1) {
			// discard if beeing transparent
			if (diffuseTextureColor.a < diffuseTextureMaskedTransparencyThreshold) discard;
			// set to opqaue
			diffuseTextureColor.a = 1.0;
		}
	}
	outColor = vec4(0.0, 0.0, 0.0, 1.0);
}
