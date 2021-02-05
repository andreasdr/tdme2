// @see https://jayconrod.com/posts/34/water-simulation-in-glsl

float wave(int i, float x, float y) {
    float waterFrequency = 2.0 * 3.14159 / waterWavelength[i];
    float phase = waterSpeed[i] * waterFrequency;
    float theta = dot(waterDirection[i], vec2(x, y));
    return waterAmplitude[i] * sin(theta * waterFrequency + time * phase);
}

float waveHeight(float x, float y) {
    float height = 0.0;
    for (int i = 0; i < waterWaves; ++i)
        height += wave(i, x, y);
    return height;
}

float dWavedx(int i, float x, float y) {
    float waterFrequency = 2.0 * 3.14159 / waterWavelength[i];
    float phase = waterSpeed[i] * waterFrequency;
    float theta = dot(waterDirection[i], vec2(x, y));
    float A = waterAmplitude[i] * waterDirection[i].x * waterFrequency;
    return A * cos(theta * waterFrequency + time * phase);
}

float dWavedy(int i, float x, float y) {
    float waterFrequency = 2.0 * 3.14159 / waterWavelength[i];
    float phase = waterSpeed[i] * waterFrequency;
    float theta = dot(waterDirection[i], vec2(x, y));
    float A = waterAmplitude[i] * waterDirection[i].y * waterFrequency;
    return A * cos(theta * waterFrequency + time * phase);
}

vec3 waveNormal(float x, float y) {
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < waterWaves; ++i) {
        dx += dWavedx(i, x, y);
        dy += dWavedy(i, x, y);
    }
    vec3 n = vec3(-dx, 1.0, -dy);
    return normalize(n);
}
