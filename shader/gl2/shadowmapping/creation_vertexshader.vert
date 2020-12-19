#version 100

precision highp float;

attribute vec3 inVertex;
attribute vec3 inNormal;
attribute vec2 inTextureUV;
attribute vec3 inOrigin;

uniform mat4 mvpMatrix;
uniform vec3 modelTranslation;
uniform mat3 textureMatrix;

uniform float time;

// will be passed to fragment shader
varying vec2 vsFragTextureUV;

{$DEFINITIONS}

{$FUNCTIONS}

void main(){
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

	// compute gl position
	gl_Position = mvpMatrix * shaderTransformMatrix * vec4(inVertex, 1.0);
}
