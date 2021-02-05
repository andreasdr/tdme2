#version 330

// standard layouts
layout (location = 0) in vec3 inVertex;
layout (location = 2) in vec2 inTextureUV;

// instanced rendering
layout (location = 6) in mat4 inModelMatrix;

// render groups
layout (location = 12) in vec3 inOrigin;

// uniforms
uniform mat3 textureMatrix;
uniform int normalTextureAvailable;
uniform float time;

{$DEFINITIONS}

#if defined(HAVE_FOLIAGE)
	uniform float speed;
	uniform float amplitudeDefault;
	uniform float amplitudeMax;
	uniform float xxx;
#elif defined(HAVE_TREE)
	uniform float speed;
#endif

uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;

// will be passed to fragment shader
out vec2 vsFragTextureUV;
out vec3 vsPosition;

{$FUNCTIONS}

void main(void) {
	#if defined(HAVE_TREE)
		mat4 shaderTransformMatrix = createTreeTransformMatrix(inOrigin, inVertex, vec3(inModelMatrix[3][0], inModelMatrix[3][1], inModelMatrix[3][2]));
	#elif defined(HAVE_FOLIAGE)
		mat4 shaderTransformMatrix = createFoliageTransformMatrix(inOrigin, inVertex, vec3(inModelMatrix[3][0], inModelMatrix[3][1], inModelMatrix[3][2]));
	#else
		mat4 shaderTransformMatrix = mat4(1.0);
	#endif

	// transformations matrices
	mat4 mvpMatrix = projectionMatrix * cameraMatrix * inModelMatrix * shaderTransformMatrix;

	// texure UV
	vsFragTextureUV = vec2(textureMatrix * vec3(inTextureUV, 1.0));

	// gl position
	gl_Position = mvpMatrix * vec4(inVertex, 1.0);

	// world position
	vec4 vsPosition4 = inModelMatrix * shaderTransformMatrix * vec4(inVertex, 1.0);
	vsPosition = vsPosition4.xyz / vsPosition4.w;

	// eye direction
	vsPosition4 = cameraMatrix * vsPosition4;
}
