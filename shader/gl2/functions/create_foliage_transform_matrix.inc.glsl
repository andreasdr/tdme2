mat4 createFoliageTransformMatrix(vec3 origin, vec3 vertex, vec3 translation) {
	float zAxisRotationBase = mod((float(time * speed * 60.0) + (origin.x + translation.x) * 8.0 * (translation.x + origin.x < 0.0?-1.0:1.0)), 160.0);
	float zAxisRotation = (amplitudeDefault * ((vertex.y - origin.y) * 2.0)) + sin((clamp((((zAxisRotationBase < 160.0 / 2.0?zAxisRotationBase:160.0 - zAxisRotationBase) - (160.0 / 4.0)) * 1.0 / 8.0) * ((vertex.y - origin.y) * 2.0), -20.0, +20.0)) / 20.0 * 3.14) * amplitudeMax;
	return
		createTranslationMatrix(vec3(origin.x, origin.y, origin.z)) *
		createRotationMatrix(vec3(0.0, 0.0, 1.0), zAxisRotation) *
		createTranslationMatrix(vec3(origin.x, origin.y, origin.z) * -1.0);
}
