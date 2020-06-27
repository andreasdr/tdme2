#pragma once

#include <string>

#include <tdme/tdme.h>
#include <tdme/utils/fwd-tdme.h>
#include <tdme/utils/ReferenceCounter.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>

using std::string;

using tdme::utils::ByteBuffer;
using tdme::utils::ReferenceCounter;

/** 
 * Texture entity
 * @version $Id$
 * @author Andreas Drewke
 */
class tdme::engine::fileio::textures::Texture final: public ReferenceCounter
{
public:

	/**
	 * Public constructor
	 * @param id id
	 * @param depth depth
	 * @param width width
	 * @param height height
	 * @param textureWidth texture width
	 * @param textureHeight texture height
	 * @param textureData texture data
	 */
	inline Texture(
		const string& id,
		int32_t depth,
		int32_t width, int32_t height,
		int32_t textureWidth, int32_t textureHeight,
		ByteBuffer* textureData):
		ReferenceCounter(),
		id(id),
		depth(depth),
		width(width),
		height(height),
		textureWidth(textureWidth),
		textureHeight(textureHeight),
		textureData(textureData),
		useMipMap(true),
		repeat(true) {
		//
	}

	/** 
	 * @return id
	 */
	inline const string& getId() const  {
		return id;
	}

	/** 
	 * @return depth in bits per pixel
	 */
	inline int32_t getDepth() const {
		return depth;
	}

	/** 
	 * @return image width
	 */
	inline int32_t getWidth() const {
		return width;
	}

	/** 
	 * @return image height
	 */
	inline int32_t getHeight() const {
		return height;
	}

	/** 
	 * @return texture height
	 */
	inline int32_t getTextureHeight() const {
		return textureHeight;
	}

	/** 
	 * @return texture width
	 */
	inline int32_t getTextureWidth() const {
		return textureWidth;
	}

	/** 
	 * @return texture data wrapped in a byte buffer
	 */
	inline ByteBuffer* getTextureData() {
		return textureData;
	}

	/**
	 * @return use mip map
	 */
	inline bool isUseMipMap() const {
		return useMipMap;
	}

	/**
	 * Set if to use mip map
	 * @param useMipMap mip map enabled
	 */
	inline void setUseMipMap(bool useMipMap) {
		this->useMipMap = useMipMap;
	}

	/**
	 * @return is repeat
	 */
	inline bool isRepeat() const {
		return repeat;
	}

	/**
	 * Set repeat
	 * @param repeat repeat
	 */
	inline void setRepeat(bool repeat) {
		this->repeat = repeat;
	}

	// overriden methods
	inline virtual void onDelete() override;

private:
	string id;
	int32_t depth;
	int32_t width;
	int32_t height;
	int32_t textureHeight;
	int32_t textureWidth;
	ByteBuffer* textureData;
	bool useMipMap;
	bool repeat;

	/**
	 * Destructor
	 */
	~Texture();
};
