#include <tdme/engine/subsystems/rendering/TransparentRenderPointsPool.h>

#include <algorithm>
#include <vector>
#include <string>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Color4Base.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderPoint.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>

using std::vector;
using std::string;

using tdme::engine::subsystems::rendering::TransparentRenderPointsPool;
using tdme::engine::model::Color4;
using tdme::engine::model::Color4Base;
using tdme::engine::subsystems::rendering::TransparentRenderPoint;
using tdme::math::Vector3;
using tdme::utilities::Console;

TransparentRenderPointsPool::TransparentRenderPointsPool(int32_t pointsMax) 
{
	poolIdx = 0;
	transparentRenderPoints.resize(pointsMax);
	for (auto i = 0; i < transparentRenderPoints.size(); i++) {
		transparentRenderPoints[i] = new TransparentRenderPoint();
		transparentRenderPoints[i]->acquired = false;
	}
}

TransparentRenderPointsPool::~TransparentRenderPointsPool() {
	for (auto i = 0; i < transparentRenderPoints.size(); i++) delete transparentRenderPoints[i];
}

void TransparentRenderPointsPool::reset()
{
	poolIdx = 0;
	for (auto point: transparentRenderPoints) point->acquired = false;
}

void TransparentRenderPointsPool::sort()
{
	std::sort(transparentRenderPoints.begin(), transparentRenderPoints.begin() + poolIdx, TransparentRenderPoint::compare);
}
