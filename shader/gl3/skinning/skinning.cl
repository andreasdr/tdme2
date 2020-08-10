static float3 mat4MULvec3(float16 mat4, float3 vec3) {
	float3 result;
	result.x = vec3.x * mat4[0] + vec3.y * mat4[4] + vec3.z * mat4[8] + mat4[12];
	result.y = vec3.x * mat4[1] + vec3.y * mat4[5] + vec3.z * mat4[9] + mat4[13];
	result.z = vec3.x * mat4[2] + vec3.y * mat4[6] + vec3.z * mat4[10] + mat4[14];
	return result;
}

static float3 mat4MULvec3NoTranslation(float16 mat4, float3 vec3) {
	float3 result;
	result.x = vec3.x * mat4[0] + vec3.y * mat4[4] + vec3.z * mat4[8];
	result.y = vec3.x * mat4[1] + vec3.y * mat4[5] + vec3.z * mat4[9];
	result.z = vec3.x * mat4[2] + vec3.y * mat4[6] + vec3.z * mat4[10];
	return result;
}

kernel void computeSkinning(
	global float* inVertexFloatArray,
	global float* inNormalFloatArray,
	global int* inSkinningVertexJoints,
	global int* inSkinningVertexJointIdxs,
	global float* inSkinningVertexJointWeights,
	global float* outVertexFloatArray,
	global float* outNormalFloatArray,
	global float16* inSkinningJointsTransformationsMatrices,
	const int vertexCount,
	const int matrixCount,
	const int instanceCount
	) {
	// TODO: tangents and bitangents
	int vertexOffset = get_global_id(0);
	int instance = get_global_id(1);
	if (instance < instanceCount && vertexOffset < vertexCount) {
		float3 skinnedVertex = (float3)(0.0, 0.0, 0.0);
		float3 skinnedNormal = (float3)(0.0, 0.0, 0.0);
		float totalWeights = 0.0;
		float3 inVertex = (float3)(inVertexFloatArray[vertexOffset * 3 + 0], inVertexFloatArray[vertexOffset * 3 + 1], inVertexFloatArray[vertexOffset * 3 + 2]);
		float3 inNormal = (float3)(inNormalFloatArray[vertexOffset * 3 + 0], inNormalFloatArray[vertexOffset * 3 + 1], inNormalFloatArray[vertexOffset * 3 + 2]);
		for (int i = 0; i < inSkinningVertexJoints[vertexOffset]; i++) {
			float16 transformationsMatrix = inSkinningJointsTransformationsMatrices[instance * matrixCount + inSkinningVertexJointIdxs[vertexOffset * 4 + i]];
			skinnedVertex+= mat4MULvec3(transformationsMatrix, inVertex) * inSkinningVertexJointWeights[vertexOffset * 4 + i];
			skinnedNormal+= mat4MULvec3NoTranslation(transformationsMatrix, inNormal) * inSkinningVertexJointWeights[vertexOffset * 4 + i];
			totalWeights+= inSkinningVertexJointWeights[vertexOffset * 4 + i];
		}
		if (totalWeights != 1.0) {
			float weightNormalized = totalWeights != 0.0?1.0 / totalWeights:0.0;
			skinnedVertex*= weightNormalized;
			skinnedNormal*= weightNormalized;
		}	
		outVertexFloatArray[instance * vertexCount * 3 + vertexOffset * 3 + 0] = skinnedVertex.x;
		outVertexFloatArray[instance * vertexCount * 3 + vertexOffset * 3 + 1] = skinnedVertex.y;
		outVertexFloatArray[instance * vertexCount * 3 + vertexOffset * 3 + 2] = skinnedVertex.z;
		outNormalFloatArray[instance * vertexCount * 3 + vertexOffset * 3 + 0] = skinnedNormal.x;
		outNormalFloatArray[instance * vertexCount * 3 + vertexOffset * 3 + 1] = skinnedNormal.y;
		outNormalFloatArray[instance * vertexCount * 3 + vertexOffset * 3 + 2] = skinnedNormal.z;
	}
}
