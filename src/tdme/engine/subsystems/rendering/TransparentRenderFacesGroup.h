#pragma once

#include <vector>
#include <string>

#include <tdme/tdme.h>
#include <tdme/engine/model/fwd-tdme.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/subsystems/rendering/fwd-tdme.h>
#include <tdme/engine/subsystems/rendering/BatchRendererTriangles.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer.h>
#include <tdme/engine/subsystems/renderer/fwd-tdme.h>
#include <tdme/math/fwd-tdme.h>
#include <tdme/math/Vector2.h>
#include <tdme/math/Vector3.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/fwd-tdme.h>
#include <tdme/utilities/Console.h>

using std::vector;
using std::string;

using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::subsystems::rendering::BatchRendererTriangles;
using tdme::engine::subsystems::rendering::Object3DGroup;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::math::Vector2;
using tdme::math::Vector3;
using tdme::utilities::Console;

/** 
 * Transparent render faces group
 * @author andreas.drewke
 * @version $Id$
 */
class tdme::engine::subsystems::rendering::TransparentRenderFacesGroup final
{
	friend class Object3DGroupMesh;
	friend class EntityRenderer;
	friend class EntityRenderer_TransparentRenderFacesGroupPool;

private:
	EntityRenderer* object3DRenderer { nullptr };
	vector<BatchRendererTriangles*> batchRenderers;
	Model* model { nullptr };
	Object3DGroup* object3DGroup { nullptr };
	int32_t facesEntityIdx;

	Color4 effectColorAdd;
	Color4 effectColorMul;

	const Material* material { nullptr };
	bool textureCoordinates;

	string shader;

	/** 
	 * Set transparent render faces group
	 * @param object3DRenderer object3D renderer
	 * @param model model
	 * @param object3DGroup object 3D group
	 * @param facesEntityIdx faces entity idx
	 * @param effectColorAdd effect color add
	 * @param effectColorMul effect color mul
	 * @param material material
	 * @param textureCoordinates texture coordinates
	 * @param shader shader
	 */
	void set(EntityRenderer* object3DRenderer, Model* model, Object3DGroup* object3DGroup, int32_t facesEntityIdx, const Color4& effectColorAdd, const Color4& effectColorMul, const Material* material, bool textureCoordinates, const string& shader);

	/** 
	 * Creates a key for given transparent render faces group attributes
	 * @param model model
	 * @param object3DGroup object 3D group
	 * @param facesEntityIdx faces entity idx
	 * @param effectColorAdd effect color add
	 * @param effectColorMul effect color mul
	 * @param material material
	 * @param textureCoordinates texture coordinates
	 * @param shader shader
	 * @return
	 */
	static const string createKey(Model* model, Object3DGroup* object3DGroup, int32_t facesEntityIdx, const Color4& effectColorAdd, const Color4& effectColorMul, const Material* material, bool textureCoordinates, const string& shader);

	/** 
	 * Adds a vertex to this transparent render faces group
	 * @param vertex vertex
	 * @param normal normal
	 * @param textureCoordinate texture coordinate
	 */
	inline void addVertex(const Vector3& vertex, const Vector3& normal, const Vector2& textureCoordinate) {
		// check if we have a batch renderer already?
		if (batchRenderers.size() == 0) {
			// nope, add first one
			auto batchRendererTriangles = object3DRenderer->acquireTrianglesBatchRenderer();
			if (batchRendererTriangles == nullptr) {
				Console::println(string("TransparentRenderFacesGroup::addVertex(): could not acquire triangles batch renderer"));
				return;
			}
			batchRenderers.push_back(batchRendererTriangles);
		}
		// try to add vertex
		auto batchRendererTriangles = batchRenderers[batchRenderers.size() - 1];
		if (batchRendererTriangles->addVertex(vertex, normal, textureCoordinate) == true)
			return;
		// failed, acquire additionally one
		batchRendererTriangles = object3DRenderer->acquireTrianglesBatchRenderer();
		if (batchRendererTriangles == nullptr) {
			Console::println(string("TransparentRenderFacesGroup::addVertex(): could not acquire triangles batch renderer"));
			return;
		}
		// 	add it
		batchRenderers.push_back(batchRendererTriangles);
		// 	add vertex
		batchRendererTriangles->addVertex(vertex, normal, textureCoordinate);
	}

	/**
	 * Render this transparent render faces group
	 * @param engine engine
	 * @param renderer renderer
	 * @param context context
	 */
	void render(Engine* engine, Renderer* renderer, void* context);

	/**
	 * Public constructor
	 */
	TransparentRenderFacesGroup();

};
