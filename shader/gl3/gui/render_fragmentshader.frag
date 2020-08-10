#version 330

// uniforms
uniform sampler2D diffuseTextureUnit;
uniform int diffuseTextureAvailable;
uniform sampler2D maskTextureUnit;
uniform int maskTextureAvailable;
uniform float maskMaxValue;
uniform vec4 effectColorMul;
uniform vec4 effectColorAdd;

// passed from vertex shader
in vec4 vsFragColor;
in vec2 vsFragTextureUV;

// passed out
out vec4 outColor;

// main
void main(void) {
	if (maskTextureAvailable == 1) {
		vec3 maskColor = texture(maskTextureUnit, vsFragTextureUV).rgb;
		if ((maskColor.r + maskColor.g + maskColor.b) / 3.0 > maskMaxValue) discard;
	}
	if (vsFragTextureUV.x < 0.0 || vsFragTextureUV.x > 1.0) discard;
	if (vsFragTextureUV.y < 0.0 || vsFragTextureUV.y > 1.0) discard;
	if (diffuseTextureAvailable == 1) {
		outColor = clamp((effectColorAdd + texture(diffuseTextureUnit, vsFragTextureUV) * vsFragColor * effectColorMul), 0.0, 1.0);
	} else {
		outColor = clamp(effectColorAdd + vsFragColor * effectColorMul, 0.0, 1.0);
	}
}
