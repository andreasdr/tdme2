#version 330

// based on http://fabiensanglard.net/shadowmapping/index.php, modified by me

// standard layouts
layout (location = 0) in vec3 inVertex;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureUV;

// indexed rendering
layout (location = 6) in mat4 inModelMatrix;

// render groups
layout (location = 12) in vec3 inOrigin;

// uniforms
uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;
uniform mat4 depthBiasMVPMatrix;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform mat3 textureMatrix;
uniform float time;

{$DEFINITIONS}

#if defined(HAVE_FOLIAGE)
	uniform float speed;
	uniform float amplitudeDefault;
	uniform float amplitudeMax;
#elif defined(HAVE_TREE)
	uniform float speed;
#endif

// will be passed to fragment shader
out vec2 vsFragTextureUV;
out vec4 vsShadowCoord;
out float vsShadowIntensity;
out vec3 vsPosition;

{$FUNCTIONS}

void main() {
	#if defined(HAVE_TREE)
		mat4 shaderTransformMatrix =
			createTreeTransformMatrix(
				inOrigin,
				inVertex * mat3(inModelMatrix),
				vec3(inModelMatrix[3][0], inModelMatrix[3][1], inModelMatrix[3][2])
			);
	#elif defined(HAVE_FOLIAGE)
		mat4 shaderTransformMatrix =
			createFoliageTransformMatrix(
				inOrigin,
				inVertex * mat3(inModelMatrix),
				vec3(inModelMatrix[3][0], inModelMatrix[3][1], inModelMatrix[3][2])
			);
	#else
		mat4 shaderTransformMatrix = mat4(1.0);
	#endif

	// compute vertex and pass to fragment shader
	mat4 mvpMatrix = projectionMatrix * cameraMatrix * inModelMatrix * shaderTransformMatrix;
	mat4 normalMatrix = mat4(transpose(inverse(mat3(inModelMatrix * shaderTransformMatrix))));

	// texure UV
	vsFragTextureUV = vec2(textureMatrix * vec3(inTextureUV, 1.0));

	// shadow coord
	vsShadowCoord = depthBiasMVPMatrix * inModelMatrix * shaderTransformMatrix * vec4(inVertex, 1.0);
	vsShadowCoord = vsShadowCoord / vsShadowCoord.w;

	// shadow intensity
	vec3 normalTransformed = normalize(vec3(normalMatrix * vec4(inNormal, 0.0)));
	vsShadowIntensity = clamp(abs(dot(normalize(lightDirection.xyz), normalTransformed)), 0.0, 1.0);

	// eye coordinate position of vertex, needed in various calculations
	vec4 vsPosition4 = inModelMatrix * shaderTransformMatrix * vec4(inVertex, 1.0);
	vsPosition = vsPosition4.xyz / vsPosition4.w;

	// compute gl position
	gl_Position = mvpMatrix * vec4(inVertex, 1.0);
	gl_Position.z-= 0.0001;
}
