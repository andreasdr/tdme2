#version 100

precision highp float;

// attributes
attribute vec3 inVertex;
attribute vec2 inTextureSpriteIndex;
attribute vec4 inColor;
attribute float inPointSize;
attribute vec2 inSpriteSheetDimensions;
attribute vec4 inEffectColorMul;
attribute vec4 inEffectColorAdd;

// uniforms
uniform mat4 mvpMatrix;
uniform int viewPortWidth;
uniform int viewPortHeight;
uniform float projectionMatrixXx;
uniform float projectionMatrixYy;

// will be passed to fragment shader
varying float vsTextureIndex;
varying float vsSpriteIndex;
varying vec4 vsColor;
varying vec2 vsSpriteSheetDimension;
varying vec4 vsEffectColorMul;
varying vec4 vsEffectColorAdd;

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
	gl_PointSize = (float(viewPortWidth) > float(viewPortHeight)?float(viewPortWidth) * projectionMatrixXx * inPointSize:float(viewPortHeight) * projectionMatrixYy * inPointSize) / gl_Position.w;
}
