#version 100

precision mediump float;

// uniforms
uniform sampler2D textureUnit;

// passed from vertex shader
varying vec2 vsFragTextureUV;

// main
void main(void) {
	gl_FragColor = texture2D(textureUnit, vsFragTextureUV);
}
