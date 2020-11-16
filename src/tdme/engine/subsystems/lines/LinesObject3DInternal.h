#pragma once

#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/fileio/textures/fwd-tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/primitives/fwd-tdme.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/lines/fwd-tdme.h>
#include <tdme/engine/subsystems/manager/VBOManager_VBOManaged.h>
#include <tdme/engine/subsystems/manager/VBOManager.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/Transformations.h>

using std::string;
using std::vector;

using tdme::engine::Transformations;
using tdme::engine::Engine;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::model::Color4;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::subsystems::manager::VBOManager_VBOManaged;
using tdme::engine::subsystems::manager::VBOManager;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;

/**
 * Lines object internal
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::lines::LinesObject3DInternal
	: public Transformations
{
	friend class tdme::engine::subsystems::rendering::EntityRenderer;

protected:
	string id;
	Engine* engine { nullptr };
	Renderer* renderer { nullptr };
	bool enabled;
	float lineWidth;
	Texture* texture { nullptr };
	int32_t textureId { -1 };

	BoundingBox boundingBox;
	BoundingBox boundingBoxTransformed;
	Color4 effectColorMul;
	Color4 effectColorAdd;
	bool pickable;
	bool contributesShadows;
	bool receivesShadows;

	vector<Vector3> points;
	Color4 color;
	vector<Color4> colors;

	vector<int32_t>* vboIds;

	/**
	 * Update bounding volume
	 */
	inline void updateBoundingBox() {
		boundingBoxTransformed.fromBoundingVolumeWithTransformations(&boundingBox, *this);
		boundingBoxTransformed.getMin().sub(0.05f); // scale a bit up to make picking work better
		boundingBoxTransformed.getMax().add(0.05f); // same here
		boundingBoxTransformed.update();
	}

public:
	/**
	 * Public constructor
	 * @param id id
	 * @param lineWidth line width
	 * @param points points
	 * @param color color
	 * @param colors optional colors
	 * @param texture optional texture
	 */
	LinesObject3DInternal(const string& id, float lineWidth, const vector<Vector3>& points, const Color4& color, const vector<Color4>& colors = {}, Texture* texture = nullptr);

	/**
	 * Destructor
	 */
	virtual ~LinesObject3DInternal();

	/**
	 * @return id
	 */
	inline const string& getId() {
		return id;
	}

	/**
	 * Set renderer
	 * @param renderer renderer
	 */
	inline void setRenderer(Renderer* renderer) {
		this->renderer = renderer;
	}

	/**
	 * Set engine
	 * @param engine engine
	 */
	inline void setEngine(Engine* engine) {
		this->engine = engine;
	}

	/**
	 * @return is enabled
	 */
	inline bool isEnabled() {
		return enabled;
	}

	/**
	 * Set enabled
	 * @param enabled enabled
	 */
	inline void setEnabled(bool enabled) {
		this->enabled = enabled;
	}

	/**
	 * @return effect color mul
	 */
	inline const Color4& getEffectColorMul() const {
		return effectColorMul;
	}

	/**
	 * Set effect color mul
	 * @param effectColorMul effect color mul
	 */
	inline void setEffectColorMul(const Color4& effectColorMul) {
		this->effectColorMul = effectColorMul;
	}

	/**
	 * @return effect color mul
	 */
	inline const Color4& getEffectColorAdd() const {
		return effectColorMul;
	}

	/**
	 * Set effect color add
	 * @param effectColorAdd effect color add
	 */
	inline void setEffectColorAdd(const Color4& effectColorAdd) {
		this->effectColorAdd = effectColorAdd;
	}

	/**
	 * @return is pickable
	 */
	inline bool isPickable() const {
		return pickable;
	}

	/**
	 * Set pickable
	 * @param pickable pickable
	 */
	inline void setPickable(bool pickable) {
		this->pickable = pickable;
	}

	/**
	 * @return if entity contributes to shadows
	 */
	inline bool isContributesShadows() {
		return contributesShadows;
	}

	/**
	 * Enable/disable contributes shadows
	 * @param contributesShadows contributes shadows
	 */
	inline void setContributesShadows(bool contributesShadows) {
		this->contributesShadows = contributesShadows;
	}

	/**
	 * @return if entity receives shadows
	 */
	inline bool isReceivesShadows() {
		return receivesShadows;
	}

	/**
	 * Enable/disable receives shadows
	 * @param receivesShadows receives shadows
	 */
	inline void setReceivesShadows(bool receivesShadows) {
		this->receivesShadows = receivesShadows;
	}

	/**
	 * @return bounding box
	 */
	inline BoundingBox* getBoundingBox() {
		return &boundingBox;
	}

	/**
	 * @return bounding box transformed
	 */
	inline BoundingBox* getBoundingBoxTransformed() {
		return &boundingBoxTransformed;
	}

	/**
	 * @return line width
	 */
	inline float getLineWidth() {
		return lineWidth;
	}

	/**
	 * @return texture id
	 */
	inline int32_t getTextureId() {
		return textureId;
	}

	/**
	 * Update transformations
	 */
	void update() override;

	/**
	 * From transformations
	 * @param transformations transformations
	 */
	void fromTransformations(const Transformations& transformations) override;

	/**
	 * Initialize
	 */
	void initialize();

	/**
	 * Dispose
	 */
	void dispose();

};
