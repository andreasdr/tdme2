#include <tdme/engine/subsystems/rendering/TransparentRenderFacesGroup.h>

#include <string>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/subsystems/rendering/BatchRendererTriangles.h>
#include <tdme/engine/subsystems/rendering/Object3DNode.h>
#include <tdme/engine/subsystems/rendering/EntityRenderer.h>
#include <tdme/engine/subsystems/renderer/Renderer.h>
#include <tdme/math/Matrix4x4.h>
#include <tdme/utilities/Console.h>

using std::string;
using std::to_string;

using tdme::engine::subsystems::rendering::TransparentRenderFacesGroup;
using tdme::engine::model::Color4;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::subsystems::rendering::BatchRendererTriangles;
using tdme::engine::subsystems::rendering::Object3DNode;
using tdme::engine::subsystems::rendering::EntityRenderer;
using tdme::engine::subsystems::renderer::Renderer;
using tdme::math::Matrix4x4;
using tdme::utilities::Key;
using tdme::utilities::Console;

TransparentRenderFacesGroup::TransparentRenderFacesGroup()
{
	this->object3DRenderer = nullptr;
	this->model = nullptr;
	this->object3DNode = nullptr;
	this->facesEntityIdx = -1;
	this->material = nullptr;
	this->textureCoordinates = false;
}

void TransparentRenderFacesGroup::set(EntityRenderer* object3DRenderer, Model* model, Object3DNode* object3DNode, int32_t facesEntityIdx, const Color4& effectColorAdd, const Color4& effectColorMul, const Material* material, bool textureCoordinates, const string& shader)
{
	this->object3DRenderer = object3DRenderer;
	this->batchRenderers.clear();
	this->model = model;
	this->object3DNode = object3DNode;
	this->facesEntityIdx = facesEntityIdx;
	this->effectColorAdd.set(effectColorAdd);
	this->effectColorMul.set(effectColorMul);
	this->material = material;
	this->textureCoordinates = textureCoordinates;
	this->shader = shader;
}

const string TransparentRenderFacesGroup::createKey(Model* model, Object3DNode* object3DNode, int32_t facesEntityIdx, const Color4& effectColorAdd, const Color4& effectColorMul, const Material* material, bool textureCoordinates, const string& shader)
{
	auto& efcmData = effectColorMul.getArray();
	auto& efcaData = effectColorAdd.getArray();
	string key =
		model->getId() +
		"," +
		object3DNode->id +
		"," +
		(textureCoordinates == true ? "TCT" : "TCF");
		"," +
		shader +
		",";
		(material == nullptr ? "tdme.material.none" : material->getId()) + // TODO: material id could contain this "," delimiter
		",";
	key.append((const char*)&object3DNode->specularMaterialDynamicDiffuseTextureIdsByEntities[facesEntityIdx], sizeof(object3DNode->specularMaterialDynamicDiffuseTextureIdsByEntities[facesEntityIdx]));
	key.append(",");
	key.append((const char*)&facesEntityIdx, sizeof(facesEntityIdx));
	key.append(",");
	key.append((const char*)&efcmData[0], sizeof(efcmData[0]));
	key.append(",");
	key.append((const char*)&efcmData[1], sizeof(efcmData[1]));
	key.append(",");
	key.append((const char*)&efcmData[2], sizeof(efcmData[2]));
	key.append(",");
	key.append((const char*)&efcmData[3], sizeof(efcmData[3]));
	key.append(",");
	key.append((const char*)&efcaData[0], sizeof(efcaData[0]));
	key.append(",");
	key.append((const char*)&efcaData[1], sizeof(efcaData[1]));
	key.append(",");
	key.append((const char*)&efcaData[2], sizeof(efcaData[2]));
	key.append(",");
	key.append((const char*)&efcaData[3], sizeof(efcaData[3]));
	return key;
}

void TransparentRenderFacesGroup::render(Engine* engine, Renderer* renderer, void* context)
{
	//
	if (renderer->getShader(context) != shader) {
		// update sahder
		renderer->setShader(context, shader);
		renderer->onUpdateShader(context);
		// update lights
		for (auto j = 0; j < engine->lights.size(); j++) {
			engine->lights[j].update(context);
		}
		// have identity texture matrix
		renderer->getTextureMatrix(context).identity();
		renderer->onUpdateTextureMatrix(context);
	}
	// store model view matrix
	Matrix4x4 modelViewMatrix;
	modelViewMatrix.set(renderer->getModelViewMatrix());
	// effect
	renderer->getEffectColorMul(context) = effectColorMul.getArray();
	renderer->getEffectColorAdd(context) = effectColorAdd.getArray();
	renderer->onUpdateEffect(context);
	// material
	string materialKey;
	object3DRenderer->setupMaterial(context, object3DNode, facesEntityIdx, EntityRenderer::RENDERTYPE_ALL, false, materialKey);
	// model view matrix
	renderer->getModelViewMatrix().identity();
	renderer->onUpdateModelViewMatrix(context);
	// render, reset
	for (auto batchRendererTriangles: batchRenderers) {
		batchRendererTriangles->render();
		batchRendererTriangles->clear();
		batchRendererTriangles->release();
	}
	batchRenderers.clear();
	// restore GL state, model view matrix
	renderer->unbindBufferObjects(context);
	renderer->getModelViewMatrix().set(modelViewMatrix);
	renderer->onUpdateModelViewMatrix(context);
}
