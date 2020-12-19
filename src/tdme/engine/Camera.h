#pragma once

#include <tdme/tdme.h>
#include <tdme/engine/fwd-tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/Frustum.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::Frustum;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;

/**
 * Camera
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::Camera final
{
public:
	enum CameraMode { CAMERAMODE_LOOKAT, CAMERAMODE_NONE };

private:
	static Vector3 defaultUp;
	Renderer* renderer { nullptr };
	int32_t width;
	int32_t height;
	float aspect;
	float fovY;
	float zNear;
	float zFar;
	CameraMode cameraMode;
	Vector3 lookFrom;
	Vector3 lookAt;
	Vector3 upVector;
	Vector3 forwardVector;
	Vector3 sideVector;
	Matrix4x4 projectionMatrix;
	Matrix4x4 modelViewMatrix;
	Matrix4x4 mvpMatrix;
	Matrix4x4 mvpInvertedMatrix;
	Frustum* frustum { nullptr };
	float lastZNear;
	float lastZFar;
	Vector3 lastForwardVector;
	Vector3 lastSideVector;
	Vector3 lastLookFrom;
	bool frustumChanged;
	bool viewPortEnabled;
	int viewPortLeft;
	int viewPortTop;
	int viewPortWidth;
	int viewPortHeight;

public:
	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	Camera(Renderer* renderer);

	/**
	 * Destructor
	 */
	~Camera();

	/**
	 * @return width
	 */
	inline int getWidth() {
		return width;
	}

	/**
	 * @return height
	 */
	inline int getHeight() {
		return height;
	}

	/**
	 * @return camera mode
	 */
	inline CameraMode getCameraMode() const {
		return cameraMode;
	}

	/**
	 * Set camera mode
	 * @param camera mode
	 */
	inline void setCameraMode(CameraMode cameraMode) {
		this->cameraMode = cameraMode;
	}

	/**
	 * @return field of view Y
	 */
	inline float getFovY() const {
		return fovY;
	}

	/**
	 * Set field of view Y
	 * @param fovY field of view y
	 */
	inline void setFovY(float fovY) {
		this->fovY = fovY;
	}

	/**
	 * @return float
	 */
	inline float getZNear() const {
		return zNear;
	}

	/**
	 * Set z near
	 * @param zNear zNear
	 */
	inline void setZNear(float zNear) {
		this->zNear = zNear;
	}

	/**
	 * @return float
	 */
	inline float getZFar() const {
		return zFar;
	}

	/**
	 * Set z far
	 * @param zFar zFar
	 */
	inline void setZFar(float zFar) {
		this->zFar = zFar;
	}

	/**
	 * @return up vector
	 */
	inline const Vector3& getUpVector() const {
		return upVector;
	}

	/**
	 * Set up vector
	 * @param upVector up vector
	 */
	inline void setUpVector(const Vector3& upVector) {
		this->upVector = upVector;
	}

	/**
	 * @return forward vector
	 */
	inline const Vector3& getForwardVector() const {
		return forwardVector;
	}

	/**
	 * Set forward vector
	 * @param forwardVector forward vector
	 */
	inline void setForwardVector(const Vector3& forwardVector) {
		this->forwardVector = forwardVector;
	}

	/**
	 * @return side vector
	 */
	inline const Vector3& getSideVector() const {
		return sideVector;
	}

	/**
	 * Set side vector
	 * @param sideVector side vector
	 */
	inline void setSideVector(const Vector3& sideVector) {
		this->sideVector = sideVector;
	}

	/**
	 * @return look from vector
	 */
	inline const Vector3& getLookFrom() const {
		return lookFrom;
	}

	/**
	 * Set look from
	 * @param lookFrom look from
	 */
	inline void setLookFrom(const Vector3& lookFrom) {
		this->lookFrom = lookFrom;
	}

	/**
	 * @return look at vector
	 */
	inline const Vector3& getLookAt() const {
		return lookAt;
	}

	/**
	 * Set look at
	 * @param lookAt look at
	 */
	inline void setLookAt(const Vector3& lookAt) {
		this->lookAt = lookAt;
	}

	/**
	 * @return model view matrix or camera matrix
	 */
	inline const Matrix4x4& getModelViewMatrix() const {
		return modelViewMatrix;
	}

	/**
	 * @return projection matrix
	 */
	inline const Matrix4x4& getProjectionMatrix() const {
		return projectionMatrix;
	}

	/**
	 * @return model view projection matrix
	 */
	inline const Matrix4x4& getModelViewProjectionMatrix() const {
		return mvpMatrix;
	}

	/**
	 * @return inverted model view porjection matrix
	 */
	inline const Matrix4x4& getModelViewProjectionInvertedMatrix() const {
		return mvpInvertedMatrix;
	}

	/**
	 * @return frustum
	 */
	inline Frustum* getFrustum() {
		return frustum;
	}

	/**
	 * Computes the up vector for given look from and look at vectors
	 * @param lookFrom look from
	 * @param lookAt look at
	 * @return up vector
	 */
	static Vector3 computeUpVector(const Vector3& lookFrom, const Vector3& lookAt);

	/**
	 * Enable custom view port
	 * @param viewPortLeft view port left
	 * @param viewPortTop view port right
	 * @param viewPortWidth view port width
	 * @param viewPortHeight view port height
	 */
	inline void enableViewPort(int viewPortLeft, int viewPortTop, int viewPortWidth, int viewPortHeight) {
		this->viewPortEnabled = true;
		this->viewPortLeft = viewPortLeft;
		this->viewPortTop = viewPortTop;
		this->viewPortWidth = viewPortWidth;
		this->viewPortHeight = viewPortHeight;
	}

	/**
	 * Disable custom view port
	 */
	inline void disableViewPort() {
		this->viewPortEnabled = false;
		this->viewPortLeft = 0;
		this->viewPortTop = 0;
		this->viewPortWidth = width;
		this->viewPortHeight = height;
	}

	/**
	 * @return viewport left
	 */
	inline int getViewPortLeft() {
		return viewPortLeft;
	}

	/**
	 * @return viewport top
	 */
	inline int getViewPortTop() {
		return viewPortTop;
	}

	/**
	 * @return viewport width
	 */
	inline int getViewPortWidth() {
		return viewPortWidth;
	}

	/**
	 * @return viewport height
	 */
	inline int getViewPortHeight() {
		return viewPortHeight;
	}

	/**
	 * @return if frustum has changed
	 */
	inline bool hasFrustumChanged() {
		return frustumChanged;
	}

	/**
	 * Sets up camera while resizing the view port
	 * @param context context
	 * @param width width
	 * @param height height
	 */
	void update(void* context, int32_t width, int32_t height);

private:

	/**
	 * Computes the projection matrix
	 * @return projection matrix
	 */
	Matrix4x4& computeProjectionMatrix();

	/**
	 * Computes frustum matrix
	 * @param leftPlane left plane
	 * @param rightPlane right plane
	 * @param bottomPlane top plane
	 * @param topPlane bottom plane
	 * @param nearPlane near plane
	 * @param farPlane far plane
	 * @return frustum matrix
	 */
	Matrix4x4& computeFrustumMatrix(float leftPlane, float rightPlane, float bottomPlane, float topPlane, float nearPlane, float farPlane);

	/**
	 * Computes projection matrix for given look from, look at and up vector
	 * @return model view matrix
	 */
	Matrix4x4& computeModelViewMatrix();

};
