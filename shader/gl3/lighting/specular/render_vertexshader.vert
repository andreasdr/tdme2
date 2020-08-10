#version 330

// standard layouts
layout (location = 0) in vec3 inVertex;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureUV;

// normal mapping
layout (location = 4) in vec3 inTangent;
layout (location = 5) in vec3 inBitangent;

// instanced rendering
layout (location = 6) in mat4 inModelMatrix;
layout (location = 10) in vec4 inEffectColorMul;
layout (location = 11) in vec4 inEffectColorAdd;

// render groups
layout (location = 12) in vec3 inOrigin;

// uniforms
uniform mat3 textureMatrix;
uniform int normalTextureAvailable;
uniform float time;

{$DEFINITIONS}

uniform mat4 projectionMatrix;
uniform mat4 cameraMatrix;

// will be passed to fragment shader
out vec2 vsFragTextureUV;
out vec3 vsNormal;
out vec3 vsPosition;
out vec3 vsTangent;
out vec3 vsBitangent;
out vec4 vsEffectColorMul;
out vec4 vsEffectColorAdd;
out vec3 vsEyeDirection;

#if defined(HAVE_TERRAIN_SHADER)
	out vec3 vertex;
	out vec3 normal;
	out float height;
	out float slope;
#elif defined(HAVE_WATER_SHADER)
	// uniforms
	uniform float waterHeight;
	uniform int numWaves;
	uniform float amplitude[4];
	uniform float wavelength[4];
	uniform float speed[4];
	uniform vec2 direction[4];
	out float height;
#endif

#if defined(HAVE_DEPTH_FOG)
	out float fragDepth;
#endif

{$FUNCTIONS}

void main(void) {
	#if defined(HAVE_TREE)
		mat4 shaderTransformMatrix = createTreeTransformMatrix(
			inOrigin,
			inVertex * mat3(inModelMatrix),
			vec3(inModelMatrix[3][0], inModelMatrix[3][1], inModelMatrix[3][2])
		);
	#elif defined(HAVE_FOLIAGE)
		mat4 shaderTransformMatrix = createFoliageTransformMatrix(
			inOrigin,
			inVertex * mat3(inModelMatrix),
			vec3(inModelMatrix[3][0], inModelMatrix[3][1], inModelMatrix[3][2]));
	#else
		mat4 shaderTransformMatrix = mat4(1.0);
	#endif

	// normal matrix
	mat4 normalMatrix = mat4(transpose(inverse(mat3(inModelMatrix * shaderTransformMatrix))));

	#if defined(HAVE_TERRAIN_SHADER)
		vec4 heightVector4 = inModelMatrix * vec4(inVertex, 1.0);
		vec3 heightVector3 = heightVector4.xyz / heightVector4.w;
		vertex = heightVector3;
		height = heightVector3.y;
		vec4 normalVector4 = mat4(transpose(inverse(mat3(inModelMatrix)))) * vec4(inNormal, 0.0);
		normal = normalize(normalVector4.xyz);
		slope = abs(180.0 / 3.14 * acos(clamp(dot(normal, vec3(0.0, 1.0, 0.0)), -1.0, 1.0)));
		vsNormal = normalize(vec3(normalMatrix * vec4(inNormal, 0.0)));
	#elif defined(HAVE_WATER_SHADER)
		// transformations matrices
		vec4 worldPosition4 = inModelMatrix * vec4(inVertex, 1.0);
		vec3 worldPosition = (worldPosition4.xyz / worldPosition4.w).xyz * 10.0;
		float height = waterHeight * waveHeight(worldPosition.x, worldPosition.z);
		shaderTransformMatrix =
			mat4(
				1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, height, 0.0, 1.0
			);
		vsNormal = normalize(vec3(normalMatrix * shaderTransformMatrix * vec4(waveNormal(worldPosition.x, worldPosition.z), 0.0)));
	#else
		// compute the normal
		vsNormal = normalize(vec3(normalMatrix * vec4(inNormal, 0.0)));
	#endif

	// transformations matrices
	mat4 mvpMatrix = projectionMatrix * cameraMatrix * inModelMatrix * shaderTransformMatrix;

	// texure UV
	vsFragTextureUV = vec2(textureMatrix * vec3(inTextureUV, 1.0));

	// normal texture
	if (normalTextureAvailable == 1) {
		vsTangent = normalize(vec3(normalMatrix * vec4(inTangent, 0.0)));
		vsBitangent = normalize(vec3(normalMatrix * vec4(inBitangent, 0.0)));
	} else {
		vsTangent = vec3(0.0, 0.0, 0.0);
		vsBitangent = vec3(0.0, 0.0, 0.0);
	}

	// effect colors
	vsEffectColorMul = inEffectColorMul;
	vsEffectColorAdd = inEffectColorAdd;

	// gl position
	gl_Position = mvpMatrix * vec4(inVertex, 1.0);

	// world position
	vec4 vsPosition4 = inModelMatrix * shaderTransformMatrix * vec4(inVertex, 1.0);
	vsPosition = vsPosition4.xyz / vsPosition4.w;

	// eye direction
	vsPosition4 = cameraMatrix * vsPosition4;
	vsEyeDirection = normalize(-vsPosition4.xyz / vsPosition4.w);

	#if defined(HAVE_DEPTH_FOG)
		fragDepth = gl_Position.z;
	#endif
}
