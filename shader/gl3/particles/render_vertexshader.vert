#version 330

precision highp float;
precision highp int;

// layout
layout (location = 0) in vec3 inVertex;
layout (location = 1) in uvec2 inTextureSpriteIndex;
layout (location = 3) in vec4 inColor;
layout (location = 5) in float inPointSize;
layout (location = 6) in uvec2 inSpriteSheetDimensions;
layout (location = 10) in vec4 inEffectColorMul;
layout (location = 11) in vec4 inEffectColorAdd;

// uniforms
uniform mat4 mvpMatrix;
uniform int viewPortWidth;
uniform int viewPortHeight;
uniform float projectionMatrixXx;
uniform float projectionMatrixYy;

// will be passed to fragment shader
flat out uint vsTextureIndex;
flat out uint vsSpriteIndex;
flat out vec4 vsColor;
flat out uvec2 vsSpriteSheetDimension;
flat out vec4 vsEffectColorMul;
flat out vec4 vsEffectColorAdd;

#if defined(HAVE_DEPTH_FOG)
	out float fragDepth;
#endif

void main(void) {
	//
	vsTextureIndex = inTextureSpriteIndex[0];
	vsSpriteIndex = inTextureSpriteIndex[1];
	vsColor = inColor;
	vsSpriteSheetDimension = inSpriteSheetDimensions;
	vsEffectColorMul = inEffectColorMul;
	vsEffectColorAdd = inEffectColorAdd;

	// compute gl position
	gl_Position = mvpMatrix * vec4(inVertex, 1.0);

	// point size
	gl_PointSize = (viewPortWidth > viewPortHeight?viewPortWidth * projectionMatrixXx * inPointSize:viewPortHeight * projectionMatrixYy * inPointSize) / gl_Position.w;

	//
	#if defined(HAVE_DEPTH_FOG)
		fragDepth = gl_Position.z;
	#endif
}
