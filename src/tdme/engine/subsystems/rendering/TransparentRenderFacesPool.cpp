#include <tdme/engine/subsystems/rendering/TransparentRenderFacesPool.h>

#include <vector>
#include <string>

#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Group.h>
#include <tdme/engine/subsystems/rendering/Object3DGroup.h>
#include <tdme/engine/subsystems/rendering/Object3DGroupMesh.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFace.h>
#include <tdme/engine/subsystems/rendering/TransparentRenderFacesPool_TransparentRenderFacesPool.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Pool.h>
#include <tdme/utilities/Console.h>

using std::vector;
using std::string;

using tdme::engine::subsystems::rendering::TransparentRenderFacesPool;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Group;
using tdme::engine::subsystems::rendering::Object3DGroup;
using tdme::engine::subsystems::rendering::Object3DGroupMesh;
using tdme::engine::subsystems::rendering::TransparentRenderFace;
using tdme::engine::subsystems::rendering::TransparentRenderFacesPool_TransparentRenderFacesPool;
using tdme::math::Matrix4x4;
using tdme::math::Vector3;
using tdme::utilities::Pool;
using tdme::utilities::Console;

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

