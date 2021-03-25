float tdme2fmod(float x, float y) {
	return x-y*floor(x/y);
}

mat4 createTreeTransformMatrix(vec3 origin, vec3 vertex, vec3 translation) {
	if (abs(vertex.x - origin.x) < 0.25 || abs(vertex.z - origin.z) < 0.25) return mat4(1.0);
	float zAxisRotation = sin(tdme2fmod(((translation.x * 50.0 + translation.z * 50.0 + vertex.y * 50.0 + int(time * speed * 60.0f)) / 2.0), 160.0) / 160.0 * 3.14) * 5.0;
	if (vertex.x > 0.0) zAxisRotation*= -1.0;
	return
		createTranslationMatrix(vec3(origin.x, origin.y, origin.z)) *
		createRotationMatrix(vec3(0.0, 0.0, 1.0), zAxisRotation) *
		createTranslationMatrix(vec3(origin.x, origin.y, origin.z) * -1.0);
}
