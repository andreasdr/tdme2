#pragma once

#include <tdme/tdme.h>
#include <tdme/application/InputDefinitions.h>
#include <tdme/gui/events/fwd-tdme.h>

/**
 * GUI mouse event
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::gui::events::GUIMouseEvent final
{
public:
	enum GUIMouseEventType {
		MOUSEEVENT_NONE,
		MOUSEEVENT_WHEEL_MOVED,
		MOUSEEVENT_PRESSED,
		MOUSEEVENT_RELEASED,
		MOUSEEVENT_MOVED,
		MOUSEEVENT_DRAGGED
	};

private:
	int64_t time;
	GUIMouseEventType type { MOUSEEVENT_NONE };
	int xUnscaled;
	int yUnscaled;
	int x;
	int y;
	int button;
	float wheelX;
	float wheelY;
	float wheelZ;
	bool metaDown;
	bool controlDown;
	bool altDown;
	bool shiftDown;
	bool processed;

public:
	/**
	 * Destructor
	 */
	~GUIMouseEvent();

	/**
	 * @return time in milliseconds
	 */
	inline int64_t getTime() {
		return time;
	}

	/**
	 * Set time
	 * @param time time
	 */
	inline void setTime(int64_t time) {
		this->time = time;
	}

	/**
	 * @return type
	 */
	inline GUIMouseEventType getType() {
		return type;
	}

	/**
	 * Set type
	 * @param type type
	 */
	inline void setType(GUIMouseEventType type) {
		this->type = type;
	}

	/**
	 * @return x unscaled
	 */
	inline int getXUnscaled() {
		return xUnscaled;
	}

	/**
	 * Set x unscaled
	 * @param x x unscaled
	 */
	inline void setXUnscaled(int xUnscaled) {
		this->xUnscaled = xUnscaled;
	}

	/**
	 * @return y unscaled
	 */
	inline int getYUnscaled() {
		return yUnscaled;
	}

	/**
	 * Set y unscaled
	 * @param y y  unscaled
	 */
	inline void setYUnscaled(int yUnscaled) {
		this->yUnscaled = yUnscaled;
	}

	/**
	 * @return x
	 */
	inline int getX() {
		return x;
	}

	/**
	 * Set x
	 * @param x x
	 */
	inline void setX(int x) {
		this->x = x;
	}

	/**
	 * @return y
	 */
	inline int getY() {
		return y;
	}

	/**
	 * Set y
	 * @param y y
	 */
	inline void setY(int y) {
		this->y = y;
	}

	/**
	 * @return button
	 */
	inline int getButton() {
		return button;
	}

	/**
	 * Set button
	 * @param button button
	 */
	inline void setButton(int button) {
		this->button = button;
	}

	/**
	 * @return wheel x
	 */
	inline float getWheelX() {
		return wheelX;
	}

	/**
	 * Set up wheel x
	 * @param wheelX wheel x
	 */
	inline void setWheelX(float wheelX) {
		this->wheelX = wheelX;
	}

	/**
	 * @return wheel y
	 */
	inline float getWheelY() {
		return wheelY;
	}

	/**
	 * Set up wheel y
	 * @param wheelY wheel y
	 */
	inline void setWheelY(float wheelY) {
		this->wheelY = wheelY;
	}

	/**
	 * @return wheel z
	 */
	inline float getWheelZ() {
		return wheelZ;
	}

	/**
	 * Set up wheel z
	 * @param wheelZ wheel z
	 */
	inline void setWheelZ(float wheelZ) {
		this->wheelZ = wheelZ;
	}

	/**
	 * @return is meta down
	 */
	inline bool isMetaDown() {
		return metaDown;
	}

	/**
	 * Set meta down
	 * @param metaDown meta down
	 */
	inline void setMetaDown(bool metaDown) {
		this->metaDown = metaDown;
	}

	/**
	 * @return control down
	 */
	inline bool isControlDown() {
		return controlDown;
	}

	/**
	 * Set control down
	 * @param controlDown control down
	 */
	inline void setControlDown(bool controlDown) {
		this->controlDown = controlDown;
	}

	/**
	 * @return is alt down
	 */
	inline bool isAltDown() {
		return altDown;
	}

	/**
	 * Set alt down
	 * @param altDown alt down
	 */
	inline void setAltDown(bool altDown) {
		this->altDown = altDown;
	}

	/**
	 * @return is shift down
	 */
	inline bool isShiftDown() {
		return shiftDown;
	}

	/**
	 * Set shift down
	 * @param shiftDown shiftDown
	 */
	inline void setShiftDown(bool shiftDown) {
		this->shiftDown = shiftDown;
	}

	/**
	 * @return processed
	 */
	inline bool isProcessed() {
		return processed;
	}

	/**
	 * Set processed
	 * @param processed processed
	 */
	inline void setProcessed(bool processed) {
		this->processed = processed;
	}

	/**
	 * Public constructor
	 */
	GUIMouseEvent();

private:
	friend class GUIMouseEvent_Type;
};
