#pragma once

#include <map>
#include <string>
#include <vector>

#include <tdme/tdme.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/skinning/fwd-tdme.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/os/threading/Mutex.h>

using std::map;
using std::string;
using std::vector;

using tdme::engine::subsystems::renderer::Renderer;
using tdme::engine::subsystems::rendering::Object3DBase;
using tdme::engine::subsystems::rendering::Object3DNodeMesh;
using tdme::math::Matrix4x4;
using tdme::os::threading::Mutex;

/**
 * Interface to compute shader skinning shader program
 * @author Andreas Drewke
 * @version $Id$
 */
class tdme::engine::subsystems::skinning::SkinningShader final
{
private:
	struct SkinningContext {
		bool running { false };
	};

	struct ModelSkinningCache {
		string id;
		vector<int32_t>* vboIds;
		vector<vector<int32_t>*> matricesVboIds;
	};

	int32_t programId { -1 };
	int32_t shaderId { -1 };
	int32_t uniformVertexCount { -1 };
	int32_t uniformMatrixCount { -1 };
	int32_t uniformInstanceCount { -1 };
	bool isRunning;
	bool initialized;
	Renderer* renderer { nullptr };
	vector<SkinningContext> contexts;
	map<string, ModelSkinningCache> cache;
	Mutex mutex;

public:
	/**
	 * Public constructor
	 * @param renderer renderer
	 */
	SkinningShader(Renderer* renderer);

	/**
	 * @return initialized and ready to be used
	 */
	bool isInitialized();

	/**
	 * Initialize renderer
	 */
	void initialize();

	/**
	 * Use skinning program
	 */
	void useProgram();

	/**
	 * Unuse skinning program
	 */
	void unUseProgram();

	/**
	 * Compute skinning
	 * @param context context
	 * @param object3DBase object 3D base
	 * @param object3DNodeMesh object 3d node mesh
	 */
	void computeSkinning(void* context, Object3DBase* object3DBase, Object3DNodeMesh* object3DNodeMesh);

	/**
	 * Reset
	 */
	void reset();

};
