#include <tdme/engine/model/TextureCoordinate.h>

#include <array>

using std::array;

using tdme::engine::model::TextureCoordinate;

TextureCoordinate::TextureCoordinate()
{
	data.fill(0.0f);
}

TextureCoordinate::TextureCoordinate(const TextureCoordinate& textureCoordinate)
{
	data = textureCoordinate.data;
}

TextureCoordinate::TextureCoordinate(const array<float, 2>& uv)
{
	data = uv;
}

TextureCoordinate::TextureCoordinate(float u, float v)
{
	data[0] = u;
	data[1] = 1.0f - v;
}

TextureCoordinate& TextureCoordinate::set(const TextureCoordinate& textureCoordinate) {
	data = textureCoordinate.data;
	return *this;
}

TextureCoordinate& TextureCoordinate::set(const array<float, 2>& uv) {
	data = uv;
	return *this;
}

TextureCoordinate& TextureCoordinate::set(float u, float v) {
	data[0] = u;
	data[1] = 1.0f - v;
	return *this;
}

TextureCoordinate TextureCoordinate::clone()
{
	return TextureCoordinate(data[0], 1.0f - data[1]);
}
