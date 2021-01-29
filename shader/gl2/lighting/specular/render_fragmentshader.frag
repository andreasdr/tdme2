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

varying vec2 vsFragTextureUV;
varying vec4 vsFragColor;

{$DEFINITIONS}

#if defined(HAVE_TERRAIN_SHADER)
	#define TERRAIN_UV_SCALE			0.2
	#define TERRAIN_LEVEL_0				-4.0
	#define TERRAIN_LEVEL_1				10.0
	#define TERRAIN_HEIGHT_BLEND			4.0
	#define TERRAIN_SLOPE_BLEND			5.0

	uniform sampler2D grasTextureUnit;
	uniform sampler2D dirtTextureUnit;
	uniform sampler2D stoneTextureUnit;
	uniform sampler2D snowTextureUnit;

	varying vec3 vertex;
	varying vec3 normal;
	varying float height;
	varying float slope;

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
#elif defined(HAVE_WATER_SHADER)
	varying vec3 vsPosition;
	varying vec3 vsNormal;
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
		vec3 uvMappingBlending = abs(normal);
		uvMappingBlending = normalize(max(uvMappingBlending, 0.00001)); // Force weights to sum to 1.0
		float b = (uvMappingBlending.x + uvMappingBlending.y + uvMappingBlending.z);
		uvMappingBlending /= vec3(b, b, b);

		vec4 terrainBlending = vec4(0.0, 0.0, 0.0, 0.0); // gras, dirt, stone, snow

		// height
		if (height > TERRAIN_LEVEL_1) {
			float blendFactorHeight = clamp((height - TERRAIN_LEVEL_1) / TERRAIN_HEIGHT_BLEND, 0.0, 1.0);
			if (slope >= 45.0) {
				terrainBlending[2]+= blendFactorHeight; // stone
			} else
			if (slope >= 45.0 - TERRAIN_SLOPE_BLEND) {
				terrainBlending[2]+= blendFactorHeight * ((slope - (45.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // stone
				terrainBlending[3]+= blendFactorHeight * (1.0 - (slope - (45.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // snow
			} else {
				terrainBlending[3]+= blendFactorHeight; // snow
			}
		}
		if (height >= TERRAIN_LEVEL_0 && height < TERRAIN_LEVEL_1 + TERRAIN_HEIGHT_BLEND) {
			float blendFactorHeight = 1.0;
			if (height > TERRAIN_LEVEL_1) {
				blendFactorHeight = 1.0 - clamp((height - TERRAIN_LEVEL_1) / TERRAIN_HEIGHT_BLEND, 0.0, 1.0);
			} else
			if (height < TERRAIN_LEVEL_0 + TERRAIN_HEIGHT_BLEND) {
				blendFactorHeight = clamp((height - TERRAIN_LEVEL_0) / TERRAIN_HEIGHT_BLEND, 0.0, 1.0);
			}

			if (slope >= 45.0) {
				terrainBlending[2]+= blendFactorHeight; // stone
			} else
			if (slope >= 45.0 - TERRAIN_SLOPE_BLEND) {
				terrainBlending[2]+= blendFactorHeight * ((slope - (45.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // stone
				terrainBlending[1]+= blendFactorHeight * (1.0 - (slope - (45.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // dirt
			} else
			if (slope >= 26.0) {
				terrainBlending[1]+= blendFactorHeight; // dirt
			} else
			if (slope >= 26.0 - TERRAIN_SLOPE_BLEND) {
				terrainBlending[1]+= blendFactorHeight * ((slope - (26.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // dirt
				terrainBlending[0]+= blendFactorHeight * (1.0 - (slope - (26.0 - TERRAIN_SLOPE_BLEND)) / TERRAIN_SLOPE_BLEND); // gras
			} else {
				terrainBlending[0]+= blendFactorHeight; // gras
			}
		}
		if (height < TERRAIN_LEVEL_0 + TERRAIN_HEIGHT_BLEND) {
			float blendFactorHeight = 1.0;
			if (height > TERRAIN_LEVEL_0) {
				blendFactorHeight = 1.0 - clamp((height - TERRAIN_LEVEL_0) / TERRAIN_HEIGHT_BLEND, 0.0, 1.0);
			}
			// 0- meter
			terrainBlending[1]+= blendFactorHeight; // dirt
		}

		//
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
		if (terrainBlending[0] > 0.001) gl_FragColor+= readTerrainTextureGras(vertex, uvMappingBlending, TERRAIN_UV_SCALE) * terrainBlending[0];
		if (terrainBlending[1] > 0.001) gl_FragColor+= readTerrainTextureDirt(vertex, uvMappingBlending, TERRAIN_UV_SCALE) * terrainBlending[1];
		if (terrainBlending[2] > 0.001) gl_FragColor+= readTerrainTextureStone(vertex, uvMappingBlending, TERRAIN_UV_SCALE) * terrainBlending[2];
		if (terrainBlending[3] > 0.001) gl_FragColor+= readTerrainTextureSnow(vertex, uvMappingBlending, TERRAIN_UV_SCALE) * terrainBlending[3];
		gl_FragColor*= vsFragColor;
		gl_FragColor = clamp(effectColorAdd + gl_FragColor, 0.0, 1.0);
	#elif defined(HAVE_WATER_SHADER)
		vec4 envColor = vec4(0.0f, 0.0f, 0.4f, 1.0f);
		if (environmentMappingTextureAvailable == 1) {
			vec3 reflectionVector = reflect(normalize(vsPosition.xyz - environmentMappingPosition), vec3(0.0, 1.0, 0.0) * normalize(vsNormal * vec3(0.1, 1.0, 0.1)));
			envColor = textureCube(environmentMappingTextureUnit, -reflectionVector) * 0.6;
		}
		gl_FragColor = vsFragColor * 0.4;
		gl_FragColor+= vec4(envColor.rgb, 0.0);
		gl_FragColor+= effectColorAdd;
		gl_FragColor = clamp(gl_FragColor, 0.0, 1.0);
		gl_FragColor.a = 1.0;
	#else
		if (diffuseTextureAvailable == 1) {
			gl_FragColor = clamp(effectColorAdd + diffuseTextureColor * vsFragColor, 0.0, 1.0);
		} else {
			gl_FragColor = clamp(effectColorAdd + vsFragColor, 0.0, 1.0);
		}
	#endif
}
