// based on http://fabiensanglard.net/shadowmapping/index.php, modified by me

#version 100

precision highp float;

attribute vec3 inVertex;
attribute vec3 inNormal;
attribute vec2 inTextureUV;
attribute vec3 inOrigin;

uniform mat4 depthBiasMVPMatrix;
uniform mat4 mvpMatrix;
uniform mat4 normalMatrix;
uniform vec3 modelTranslation;
uniform mat3 textureMatrix;
uniform vec3 lightDirection;

uniform float time;

// will be passed to fragment shader
varying vec2 vsFragTextureUV;
varying vec4 vsShadowCoord;
varying float vsShadowIntensity;
varying vec3 vsPosition;

{$DEFINITIONS}

{$FUNCTIONS}

void main() {
	#if defined(HAVE_TREE)
		// TODO: apply scale, rotation from model view matrix
		mat4 shaderTransformMatrix = createTreeTransformMatrix(inOrigin, inVertex, modelTranslation);
	#elif defined(HAVE_FOLIAGE)
		// TODO: apply scale, rotation from model view matrix
		mat4 shaderTransformMatrix = createFoliageTransformMatrix(inOrigin, inVertex, modelTranslation);
	#else
		mat4 shaderTransformMatrix = mat4(1.0);
	#endif

	// pass texture uv to fragment shader
	vsFragTextureUV = vec2(textureMatrix * vec3(inTextureUV, 1.0));

	// shadow coord
	vsShadowCoord = depthBiasMVPMatrix * vec4(inVertex, 1.0);
	vsShadowCoord = vsShadowCoord / vsShadowCoord.w;

	// shadow intensity 
	vec3 normal = normalize(vec3(normalMatrix * shaderTransformMatrix * vec4(inNormal, 0.0)));
	vsShadowIntensity = clamp(abs(dot(normalize(lightDirection.xyz), normal)), 0.0, 1.0);

	// compute gl position
	gl_Position = mvpMatrix * shaderTransformMatrix * vec4(inVertex, 1.0);
}
