#version 100

precision mediump float;

uniform vec4 effectColorAdd;

uniform sampler2D diffuseTextureUnit;
uniform int diffuseTextureAvailable;
uniform int diffuseTextureMaskedTransparency;
uniform float diffuseTextureMaskedTransparencyThreshold;

uniform samplerCube environmentMappingTextureUnit;
uniform int environmentMappingTextureAvailable;
uniform vec3 environmentMappingPosition;

uniform float materialReflection;

varying vec3 vsPosition;
varying vec3 vsNormal;
varying vec2 vsFragTextureUV;
varying vec4 vsFragColor;

{$DEFINITIONS}

#if defined(HAVE_TERRAIN_SHADER)
	#define TERRAIN_UV_SCALE			0.2
	#define TERRAIN_LEVEL_0				-4.0
	#define TERRAIN_LEVEL_1				10.0
	#define TERRAIN_HEIGHT_BLEND			4.0
	#define TERRAIN_SLOPE_BLEND			5.0

	varying vec3 terrainVertex;
	varying vec3 terrainNormal;
	varying float terrainHeight;
	varying float terrainSlope;

	uniform sampler2D grasTextureUnit;
	uniform sampler2D dirtTextureUnit;
	uniform sampler2D stoneTextureUnit;
	uniform sampler2D snowTextureUnit;

	vec4 readTerrainTextureGras(vec3 coords, vec3 blending, float scale) {
		// see: https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
		vec4 xAxis = texture2D(grasTextureUnit, coords.yz * scale);
		vec4 yAxis = texture2D(grasTextureUnit, coords.xz * scale);
		vec4 zAxis = texture2D(grasTextureUnit, coords.xy * scale);
		vec4 result = xAxis * blending.x + yAxis * blending.y + zAxis * blending.z;
		return result;
	}

	vec4 readTerrainTextureDirt(vec3 coords, vec3 blending, float scale) {
		// see: https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
		vec4 xAxis = texture2D(dirtTextureUnit, coords.yz * scale);
		vec4 yAxis = texture2D(dirtTextureUnit, coords.xz * scale);
		vec4 zAxis = texture2D(dirtTextureUnit, coords.xy * scale);
		vec4 result = xAxis * blending.x + yAxis * blending.y + zAxis * blending.z;
		return result;
	}

	vec4 readTerrainTextureStone(vec3 coords, vec3 blending, float scale) {
		// see: https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
		vec4 xAxis = texture2D(stoneTextureUnit, coords.yz * scale);
		vec4 yAxis = texture2D(stoneTextureUnit, coords.xz * scale);
		vec4 zAxis = texture2D(stoneTextureUnit, coords.xy * scale);
		vec4 result = xAxis * blending.x + yAxis * blending.y + zAxis * blending.z;
		return result;
	}

	vec4 readTerrainTextureSnow(vec3 coords, vec3 blending, float scale) {
		// see: https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
		vec4 xAxis = texture2D(snowTextureUnit, coords.yz * scale);
		vec4 yAxis = texture2D(snowTextureUnit, coords.xz * scale);
		vec4 zAxis = texture2D(snowTextureUnit, coords.xy * scale);
		vec4 result = xAxis * blending.x + yAxis * blending.y + zAxis * blending.z;
		return result;
	}

	#if defined(HAVE_TERRAIN_SHADER_EDITOR)
		uniform int brushEnabled;
		uniform vec2 brushPosition;
		uniform vec2 brushTextureDimension;
		uniform sampler2D brushTextureUnit;
		uniform mat3 brushTextureMatrix;
	#endif
#endif

void main (void) {
	// retrieve diffuse texture color value
	vec4 diffuseTextureColor;
	if (diffuseTextureAvailable == 1) {
		// fetch from texture
		diffuseTextureColor = texture2D(diffuseTextureUnit, vsFragTextureUV);
		// check if to handle diffuse texture masked transparency
		if (diffuseTextureMaskedTransparency == 1) {
			// discard if beeing transparent
			if (diffuseTextureColor.a < diffuseTextureMaskedTransparencyThreshold) discard;
			// set to opqaue
			diffuseTextureColor.a = 1.0;
		}
	}

	#if defined(HAVE_TERRAIN_SHADER)
		// see: https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
		vec3 uvMappingBlending = abs(terrainNormal);
		uvMappingBlending = normalize(max(uvMappingBlending, 0.00001)); // Force weights to sum to 1.0
		float b = (uvMappingBlending.x + uvMappingBlending.y + uvMappingBlending.z);
		uvMappingBlending /= vec3(b, b, b);

		vec4 terrainBlending = vec4(0.0, 0.0, 0.0, 0.0); // gras, dirt, stone, snow

		// terrainHeight
		if (terrainHeight > TERRAIN_LEVEL_1) {
			float blendFactorHeight = clamp((terrainHeight - TERRAIN_LEVEL_1) / TERRAIN_HEIGHT_BLEND, 0.0, 1.0);
			if (terrainSlope >= 45.0) {
				terrainBlending[2]+= blendFactorHeight; // stone
			} else
			if (terrainSlope >= 45.0 - TERRAIN_SLOPE_BLEND) {
				terrainBlending[2]+= blendFactorHeight * ((terrainSlope - (45.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // stone
				terrainBlending[3]+= blendFactorHeight * (1.0 - (terrainSlope - (45.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // snow
			} else {
				terrainBlending[3]+= blendFactorHeight; // snow
			}
		}
		if (terrainHeight >= TERRAIN_LEVEL_0 && terrainHeight < TERRAIN_LEVEL_1 + TERRAIN_HEIGHT_BLEND) {
			float blendFactorHeight = 1.0;
			if (terrainHeight > TERRAIN_LEVEL_1) {
				blendFactorHeight = 1.0 - clamp((terrainHeight - TERRAIN_LEVEL_1) / TERRAIN_HEIGHT_BLEND, 0.0, 1.0);
			} else
			if (terrainHeight < TERRAIN_LEVEL_0 + TERRAIN_HEIGHT_BLEND) {
				blendFactorHeight = clamp((terrainHeight - TERRAIN_LEVEL_0) / TERRAIN_HEIGHT_BLEND, 0.0, 1.0);
			}

			if (terrainSlope >= 45.0) {
				terrainBlending[2]+= blendFactorHeight; // stone
			} else
			if (terrainSlope >= 45.0 - TERRAIN_SLOPE_BLEND) {
				terrainBlending[2]+= blendFactorHeight * ((terrainSlope - (45.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // stone
				terrainBlending[1]+= blendFactorHeight * (1.0 - (terrainSlope - (45.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // dirt
			} else
			if (terrainSlope >= 26.0) {
				terrainBlending[1]+= blendFactorHeight; // dirt
			} else
			if (terrainSlope >= 26.0 - TERRAIN_SLOPE_BLEND) {
				terrainBlending[1]+= blendFactorHeight * ((terrainSlope - (26.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // dirt
				terrainBlending[0]+= blendFactorHeight * (1.0 - (terrainSlope - (26.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // gras
			} else {
				terrainBlending[0]+= blendFactorHeight; // gras
			}
		}
		if (terrainHeight < TERRAIN_LEVEL_0 + TERRAIN_HEIGHT_BLEND) {
			float blendFactorHeight = 1.0;
			if (terrainHeight > TERRAIN_LEVEL_0) {
				blendFactorHeight = 1.0 - clamp((terrainHeight - TERRAIN_LEVEL_0) / TERRAIN_HEIGHT_BLEND, 0.0, 1.0);
			}
			// 0- meter
			terrainBlending[1]+= blendFactorHeight; // dirt
		}

		//
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
		if (terrainBlending[0] > 0.001) gl_FragColor+= readTerrainTextureGras(terrainVertex, uvMappingBlending, TERRAIN_UV_SCALE) * terrainBlending[0];
		if (terrainBlending[1] > 0.001) gl_FragColor+= readTerrainTextureDirt(terrainVertex, uvMappingBlending, TERRAIN_UV_SCALE) * terrainBlending[1];
		if (terrainBlending[2] > 0.001) gl_FragColor+= readTerrainTextureStone(terrainVertex, uvMappingBlending, TERRAIN_UV_SCALE) * terrainBlending[2];
		if (terrainBlending[3] > 0.001) gl_FragColor+= readTerrainTextureSnow(terrainVertex, uvMappingBlending, TERRAIN_UV_SCALE) * terrainBlending[3];
		gl_FragColor*= vsFragColor;
		gl_FragColor = clamp(effectColorAdd + gl_FragColor, 0.0, 1.0);

		#if defined(HAVE_TERRAIN_SHADER_EDITOR)
			if (brushEnabled == 1) {
				vec2 brushTextureUV = vec2(brushTextureMatrix * vec3(((vsPosition.xz - brushPosition) / brushTextureDimension), 1.0));
				if (brushTextureUV.x >= 0.0 && brushTextureUV.x < 1.0 &&
					brushTextureUV.y >= 0.0 && brushTextureUV.y < 1.0) {
					gl_FragColor+= vec4(texture2D(brushTextureUnit, brushTextureUV).rgb * 0.25, 0.0);
				}
			}
		#endif
	#elif defined(HAVE_WATER_SHADER)
		vec4 envColor = vec4(0.2, 0.2, 0.6, 1.0);
		if (environmentMappingTextureAvailable == 1) {
			vec3 reflectionVector = reflect(normalize(vsPosition.xyz - environmentMappingPosition), normalize(vsNormal * vec3(0.01, 1.0, 0.01)));
			envColor = textureCube(environmentMappingTextureUnit, -reflectionVector);
		}
		gl_FragColor = vsFragColor * 0.4;
		gl_FragColor+= envColor;
		gl_FragColor+= effectColorAdd;
		gl_FragColor = clamp(gl_FragColor, 0.0, 1.0);
		gl_FragColor.a = 0.6;
	#else
		if (diffuseTextureAvailable == 1) {
			gl_FragColor = clamp(effectColorAdd + diffuseTextureColor * vsFragColor, 0.0, 1.0);
		} else {
			gl_FragColor = clamp(effectColorAdd + vsFragColor, 0.0, 1.0);
		}
	#endif

	// reflection
	#if !defined(HAVE_WATER_SHADER)
		if (materialReflection > 0.0 && environmentMappingTextureAvailable == 1) {
			vec3 reflectionVector = reflect(normalize(environmentMappingPosition - vsPosition.xyz), vsNormal);
			gl_FragColor = clamp(gl_FragColor + textureCube(environmentMappingTextureUnit, -reflectionVector) * materialReflection, 0.0, 1.0);
		}
	#endif
}
