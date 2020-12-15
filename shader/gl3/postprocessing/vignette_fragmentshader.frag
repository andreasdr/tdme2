#version 330

{$DEFINITIONS}

// uniforms
uniform sampler2D colorBufferTextureUnit;
uniform sampler2D depthBufferTextureUnit;
uniform float bufferTexturePixelWidth;
uniform float bufferTexturePixelHeight;
uniform float intensity;

// passed from vertex shader
in vec2 vsFragTextureUV;

// passed out
out vec4 outColor;

// main
void main(void) {
	float depth = texture(depthBufferTextureUnit, vsFragTextureUV).r;
	float borderTransparency = 0.75;
	vec3 borderColor = vec3(1.0, 0.0, 0.0);
	vec4 originalColor = texture(colorBufferTextureUnit, vsFragTextureUV);
	float fadeArea = intensity;
	float fragmentIntensity = 1.0;
	float aspect = bufferTexturePixelHeight / bufferTexturePixelWidth; 
	if (vsFragTextureUV.x < fadeArea) fragmentIntensity*= vsFragTextureUV.x / fadeArea;
	if (vsFragTextureUV.x > 1.0 - fadeArea) fragmentIntensity*= (1.0 - vsFragTextureUV.x) / fadeArea;
	if (vsFragTextureUV.y < fadeArea * aspect) fragmentIntensity*= vsFragTextureUV.y / (fadeArea * aspect);
	if (vsFragTextureUV.y > 1.0 - fadeArea * aspect) fragmentIntensity*= (1.0 - vsFragTextureUV.y) / (fadeArea * aspect);
	outColor = vec4(
		originalColor.r * (fragmentIntensity / (1.0 / (1.0 - borderTransparency)) + borderTransparency) + ((1.0 - fragmentIntensity) * borderColor.r),
		originalColor.g * (fragmentIntensity / (1.0 / (1.0 - borderTransparency)) + borderTransparency) + ((1.0 - fragmentIntensity) * borderColor.g),
		originalColor.b * (fragmentIntensity / (1.0 / (1.0 - borderTransparency)) + borderTransparency) + ((1.0 - fragmentIntensity) * borderColor.b),
		originalColor.a
	);
	gl_FragDepth = depth;
}
