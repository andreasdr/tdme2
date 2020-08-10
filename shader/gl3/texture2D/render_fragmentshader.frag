#version 330

// uniforms
uniform sampler2D textureUnit;

// passed from vertex shader
in vec2 vsFragTextureUV;

// passed out
out vec4 outColor;

// main
void main(void) {
	outColor = texture(textureUnit, vsFragTextureUV);
	gl_FragDepth = 1.0;
}
