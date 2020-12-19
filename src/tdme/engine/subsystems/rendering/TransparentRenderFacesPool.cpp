#include <tdme/engine/subsystems/rendering/TransparentRenderFacesPool.h>

#include <string>
#include <vector>

#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/subsystems/rendering/Object3DNode.h>
#include <tdme/engine/subsystems/rendering/Object3DNodeMesh.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFace.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesPool_TransparentRenderFacesPool.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Pool.h>

using std::string;
using std::vector;

using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Node;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::engine::subsystems::rendering::Object3DNodeMesh;
using tdme::engine::subsystems::rendering::TransparentRenderFace;
using tdme::engine::subsystems::rendering::TransparentRenderFacesPool;
using tdme::engine::subsystems::rendering::TransparentRenderFacesPool_TransparentRenderFacesPool;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::utilities::Console;
using tdme::utilities::Pool;

constexpr int32_t TransparentRenderFacesPool::FACES_MAX;

TransparentRenderFacesPool::TransparentRenderFacesPool()
{
}

int32_t TransparentRenderFacesPool::size()
{
	return transparentRenderFacesPool.size();
}

void TransparentRenderFacesPool::reset()
{
	transparentRenderFacesPool.reset();
	transparentRenderFaces.clear();
}

vector<TransparentRenderFace*>& TransparentRenderFacesPool::getTransparentRenderFaces()
{
	return transparentRenderFaces;
}

