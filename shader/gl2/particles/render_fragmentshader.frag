#version 100

precision highp float;
precision highp sampler2D;

uniform sampler2D diffuseTextureUnits[8];

varying float vsTextureIndex;
varying float vsSpriteIndex;
varying vec4 vsColor;
varying vec2 vsSpriteSheetDimension;
varying vec4 vsEffectColorMul;
varying vec4 vsEffectColorAdd;

void main(void) {
	vec2 spriteCoord =
		gl_PointCoord / floor(vsSpriteSheetDimension) +
		vec2(
			(1.0 / floor(vsSpriteSheetDimension.x)) * floor(mod(floor(vsSpriteIndex), floor(vsSpriteSheetDimension.x))),
			1.0 - ((1.0 / floor(vsSpriteSheetDimension.y)) * floor(floor(vsSpriteIndex) / floor(vsSpriteSheetDimension.y)))
		);
	int textureIndex = int(vsTextureIndex);
	if (textureIndex == 0)
		gl_FragColor = clamp(vsEffectColorAdd + texture2D(diffuseTextureUnits[0], spriteCoord) * vsColor * vsEffectColorMul, 0.0, 1.0); else
	if (textureIndex == 1)
		gl_FragColor = clamp(vsEffectColorAdd + texture2D(diffuseTextureUnits[1], spriteCoord) * vsColor * vsEffectColorMul, 0.0, 1.0); else
	if (textureIndex == 2)
		gl_FragColor = clamp(vsEffectColorAdd + texture2D(diffuseTextureUnits[2], spriteCoord) * vsColor * vsEffectColorMul, 0.0, 1.0); else
	if (textureIndex == 3)
		gl_FragColor = clamp(vsEffectColorAdd + texture2D(diffuseTextureUnits[3], spriteCoord) * vsColor * vsEffectColorMul, 0.0, 1.0); else
	if (textureIndex == 4)
		gl_FragColor = clamp(vsEffectColorAdd + texture2D(diffuseTextureUnits[4], spriteCoord) * vsColor * vsEffectColorMul, 0.0, 1.0); else
	if (textureIndex == 5)
		gl_FragColor = clamp(vsEffectColorAdd + texture2D(diffuseTextureUnits[5], spriteCoord) * vsColor * vsEffectColorMul, 0.0, 1.0); else
	if (textureIndex == 6)
		gl_FragColor = clamp(vsEffectColorAdd + texture2D(diffuseTextureUnits[6], spriteCoord) * vsColor * vsEffectColorMul, 0.0, 1.0); else
	if (textureIndex == 7)
		gl_FragColor = clamp(vsEffectColorAdd + texture2D(diffuseTextureUnits[7], spriteCoord) * vsColor * vsEffectColorMul, 0.0, 1.0);
}
