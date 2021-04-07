#include <tdme/utilities/Primitives.h>

#include <array>
#include <string>
#include <vector>

#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/primitives/BoundingBox.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/primitives/Capsule.h>
#include <tdme/engine/primitives/ConvexMesh.h>
#include <tdme/engine/primitives/OrientedBoundingBox.h>
#include <tdme/engine/primitives/Sphere.h>
#include <tdme/math/Math.h>
#include <tdme/math/Quaternion.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/ModelTools.h>

using std::array;
using std::string;
using std::to_string;
using std::vector;

using tdme::engine::model::Color4;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::model::UpVector;
using tdme::engine::primitives::BoundingBox;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::primitives::Capsule;
using tdme::engine::primitives::ConvexMesh;
using tdme::engine::primitives::OrientedBoundingBox;
using tdme::engine::primitives::Sphere;
using tdme::math::Math;
using tdme::math::Quaternion;
using tdme::math::Vector3;
using tdme::utilities::Console;
using tdme::utilities::ModelTools;
using tdme::utilities::Primitives;

constexpr int32_t Primitives::SPHERE_SEGMENTS_X;

constexpr int32_t Primitives::SPHERE_SEGMENTS_Y;

constexpr int32_t Primitives::CAPSULE_SEGMENTS_X;

constexpr int32_t Primitives::CAPSULE_SEGMENTS_Y;

Model* Primitives::createBoundingBoxModel(BoundingBox* boundingBox, const string& id)
{
	// model
	auto model = new Model(id, id, UpVector::Y_UP, RotationOrder::XYZ, nullptr);
	// material
	auto material = new Material("primitive");
	auto specularMaterialProperties = new SpecularMaterialProperties();
	specularMaterialProperties->setAmbientColor(
		Color4(
			245.0f / 255.0f * 0.5f,
			40.0f / 255.0f * 0.5f,
			135.0f / 255.0f * 0.5f,
			1.0f
		)
	);
	specularMaterialProperties->setDiffuseColor(
		Color4(
			245.0f / 255.0f * 0.5f,
			40.0f / 255.0f * 0.5f,
			135.0f / 255.0f * 0.5f,
			0.5f
		)
	);
	specularMaterialProperties->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
	material->setSpecularMaterialProperties(specularMaterialProperties);
	model->getMaterials()[material->getId()] = material;
	// node
	auto node = new Node(model, nullptr, "primitive", "primitive");
	// triangle vertices indexes
	auto fvi = BoundingBox::getFacesVerticesIndexes();
	// vertices
	vector<Vector3> vertices;
	for (auto& vertex : boundingBox->getVertices()) {
		vertices.push_back(vertex);
	}
	// normals
	vector<Vector3> normals;
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(+1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, +1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, +1.0f));
	// faces
	vector<Face> faces;
	//	left
	faces.push_back(Face(node, (*fvi)[0][0], (*fvi)[0][1], (*fvi)[0][2], 0, 0, 0));
	faces.push_back(Face(node, (*fvi)[1][0], (*fvi)[1][1], (*fvi)[1][2], 0, 0, 0));
	//	right
	faces.push_back(Face(node, (*fvi)[2][0], (*fvi)[2][1], (*fvi)[2][2], 1, 1, 1));
	faces.push_back(Face(node, (*fvi)[3][0], (*fvi)[3][1], (*fvi)[3][2], 1, 1, 1));
	//	top
	faces.push_back(Face(node, (*fvi)[4][0], (*fvi)[4][1], (*fvi)[4][2], 2, 2, 2));
	faces.push_back(Face(node, (*fvi)[5][0], (*fvi)[5][1], (*fvi)[5][2], 2, 2, 2));
	//	bottom
	faces.push_back(Face(node, (*fvi)[6][0], (*fvi)[6][1], (*fvi)[6][2], 3, 3, 3));
	faces.push_back(Face(node, (*fvi)[7][0], (*fvi)[7][1], (*fvi)[7][2], 3, 3, 3));
	//	near
	faces.push_back(Face(node, (*fvi)[8][0], (*fvi)[8][1], (*fvi)[8][2], 4, 4, 4));
	faces.push_back(Face(node, (*fvi)[9][0], (*fvi)[9][1], (*fvi)[9][2], 4, 4, 4));
	//	far
	faces.push_back(Face(node, (*fvi)[10][0], (*fvi)[10][1], (*fvi)[10][2], 5, 5, 5));
	faces.push_back(Face(node, (*fvi)[11][0], (*fvi)[11][1], (*fvi)[11][2], 5, 5, 5));
	// faces entity
	FacesEntity nodeFacesEntity(node, "primitive.facesentity");
	nodeFacesEntity.setMaterial(material);
	nodeFacesEntity.setFaces(faces);
	// set up faces entity
	vector<FacesEntity> nodeFacesEntities;
	nodeFacesEntities.push_back(nodeFacesEntity);
	// setup node vertex data
	node->setVertices(vertices);
	node->setNormals(normals);
	node->setFacesEntities(nodeFacesEntities);
	// register node
	model->getNodes()["node"] = node;
	model->getSubNodes()["node"] = node;
	// prepare for indexed rendering
	ModelTools::prepareForIndexedRendering(model);
	//
	return model;
}

Model* Primitives::createOrientedBoundingBoxModel(OrientedBoundingBox* orientedBoundingBox, const string& id)
{
	// model
	auto model = new Model(id, id, UpVector::Y_UP, RotationOrder::XYZ, nullptr);
	// material
	auto material = new Material("primitive");
	auto specularMaterialProperties = new SpecularMaterialProperties();
	specularMaterialProperties->setAmbientColor(
		Color4(
			245.0f / 255.0f * 0.5f,
			40.0f / 255.0f * 0.5f,
			135.0f / 255.0f * 0.5f,
			1.0f
		)
	);
	specularMaterialProperties->setDiffuseColor(
		Color4(
			245.0f / 255.0f * 0.5f,
			40.0f / 255.0f * 0.5f,
			135.0f / 255.0f * 0.5f,
			0.5f
		)
	);
	specularMaterialProperties->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
	material->setSpecularMaterialProperties(specularMaterialProperties);
	model->getMaterials()[material->getId()] = material;
	// node
	auto node = new Node(model, nullptr, "primitive", "primitive");
	// triangle vertices indexes
	auto fvi = OrientedBoundingBox::getFacesVerticesIndexes();
	// vertices
	vector<Vector3> vertices;
	for (auto& vertex : orientedBoundingBox->getVertices()) {
		vertices.push_back(transformVector3(orientedBoundingBox, toRP3DVector3(vertex)));
	}
	// normals
	auto axes = orientedBoundingBox->getAxes();
	vector<Vector3> normals;
	normals.push_back(transformVector3Normal(orientedBoundingBox, toRP3DVector3(axes[0].clone().scale(-1.0f))));
	normals.push_back(transformVector3Normal(orientedBoundingBox, toRP3DVector3(axes[0].clone())));
	normals.push_back(transformVector3Normal(orientedBoundingBox, toRP3DVector3(axes[1].clone().scale(-1.0f))));
	normals.push_back(transformVector3Normal(orientedBoundingBox, toRP3DVector3(axes[1].clone())));
	normals.push_back(transformVector3Normal(orientedBoundingBox, toRP3DVector3(axes[2].clone().scale(-1.0f))));
	normals.push_back(transformVector3Normal(orientedBoundingBox, toRP3DVector3(axes[2].clone())));
	// faces
	vector<Face> faces;
	//	left
	faces.push_back(Face(node, fvi[0][0], fvi[0][1], fvi[0][2], 0, 0, 0));
	faces.push_back(Face(node, fvi[1][0], fvi[1][1], fvi[1][2], 0, 0, 0));
	//	right
	faces.push_back(Face(node, fvi[2][0], fvi[2][1], fvi[2][2], 1, 1, 1));
	faces.push_back(Face(node, fvi[3][0], fvi[3][1], fvi[3][2], 1, 1, 1));
	//	top
	faces.push_back(Face(node, fvi[4][0], fvi[4][1], fvi[4][2], 2, 2, 2));
	faces.push_back(Face(node, fvi[5][0], fvi[5][1], fvi[5][2], 2, 2, 2));
	//	bottom
	faces.push_back(Face(node, fvi[6][0], fvi[6][1], fvi[6][2], 3, 3, 3));
	faces.push_back(Face(node, fvi[7][0], fvi[7][1], fvi[7][2], 3, 3, 3));
	//	near
	faces.push_back(Face(node, fvi[8][0], fvi[8][1], fvi[8][2], 4, 4, 4));
	faces.push_back(Face(node, fvi[9][0], fvi[9][1], fvi[9][2], 4, 4, 4));
	//	far
	faces.push_back(Face(node, fvi[10][0], fvi[10][1], fvi[10][2], 5, 5, 5));
	faces.push_back(Face(node, fvi[11][0], fvi[11][1], fvi[11][2], 5, 5, 5));
	// faces entity
	FacesEntity nodeFacesEntity(node, "primitive.facesentity");
	nodeFacesEntity.setMaterial(material);
	nodeFacesEntity.setFaces(faces);
	// set up faces entity
	vector<FacesEntity> nodeFacesEntities;
	nodeFacesEntities.push_back(nodeFacesEntity);
	// setup node vertex data
	node->setVertices(vertices);
	node->setNormals(normals);
	node->setFacesEntities(nodeFacesEntities);
	// register node
	model->getNodes()["node"] = node;
	model->getSubNodes()["node"] = node;
	// prepare for indexed rendering
	ModelTools::prepareForIndexedRendering(model);
	//
	return model;
}

Model* Primitives::createSphereModel(Sphere* sphere, const string& id, int32_t segmentsX, int32_t segmentsY)
{
	// sphere properties
	auto radius = sphere->getRadius();
	// model
	auto model = new Model(id, id, UpVector::Y_UP, RotationOrder::XYZ, nullptr);
	// material
	auto material = new Material("primitive");
	auto specularMaterialProperties = new SpecularMaterialProperties();
	specularMaterialProperties->setAmbientColor(
		Color4(
			245.0f / 255.0f * 0.5f,
			40.0f / 255.0f * 0.5f,
			135.0f / 255.0f * 0.5f,
			1.0f
		)
	);
	specularMaterialProperties->setDiffuseColor(
		Color4(
			245.0f / 255.0f * 0.5f,
			40.0f / 255.0f * 0.5f,
			135.0f / 255.0f * 0.5f,
			0.5f
		)
	);
	specularMaterialProperties->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
	material->setSpecularMaterialProperties(specularMaterialProperties);
	model->getMaterials()[material->getId()] = material;
	// node
	auto node = new Node(model, nullptr, "primitive", "primitive");
	// vertices
	vector<Vector3> vertices;
	for (auto ySegment = 0; ySegment < segmentsY + 1; ySegment++)
		for (auto xSegment = 0; xSegment < segmentsX; xSegment++) {
			auto vertex = (
				Vector3(
					((Math::sin(Math::PI * ySegment / segmentsY) * Math::cos(Math::PI * 2 * xSegment / segmentsX))),
					((Math::cos(Math::PI * ySegment / segmentsY))),
					((Math::sin(Math::PI * ySegment / segmentsY) * Math::sin(Math::PI * 2 * xSegment / segmentsX))))
				).scale(radius);
			vertices.push_back(transformVector3(sphere, toRP3DVector3(vertex)));
		}
	// normals
	vector<Vector3> normals;
	// faces
	vector<Face> faces;
	int32_t vi0, vi1, vi2;
	int32_t ni;
	for (auto y = 0; y < segmentsY + 1; y++) {
		for (auto x = 0; x < segmentsX; x++) {
			vi0 = ((y + 0) % (segmentsY + 1)) * segmentsX + ((x + 0) % (segmentsX));
			vi1 = ((y + 1) % (segmentsY + 1)) * segmentsX + ((x + 1) % (segmentsX));
			vi2 = ((y + 1) % (segmentsY + 1)) * segmentsX + ((x + 0) % (segmentsX));
			ni = normals.size();
			{
				array<Vector3, 3> faceVertices = {
					vertices.at(vi0),
					vertices.at(vi1),
					vertices.at(vi2)
				};
				for (auto& normal: ModelTools::computeNormals(faceVertices)) {
					normals.push_back(normal);
				}
			}
			faces.push_back(Face(node, vi0, vi1, vi2, ni + 0, ni + 1, ni + 2));
			vi0 = ((y + 0) % (segmentsY + 1)) * segmentsX + ((x + 0) % (segmentsX));
			vi1 = ((y + 0) % (segmentsY + 1)) * segmentsX + ((x + 1) % (segmentsX));
			vi2 = ((y + 1) % (segmentsY + 1)) * segmentsX + ((x + 1) % (segmentsX));
			ni = normals.size();
			{
				array<Vector3, 3> faceVertices = {
					vertices.at(vi0),
					vertices.at(vi1),
					vertices.at(vi2)
				};
				for (auto& normal: ModelTools::computeNormals(faceVertices)) {
					normals.push_back(normal);
				}
			}
			faces.push_back(Face(node, vi0, vi1, vi2, ni + 0, ni + 1, ni + 2));
		}
	}
	// set up faces entity
	FacesEntity nodeFacesEntity(node, "primitive.facesentity");
	nodeFacesEntity.setMaterial(material);
	nodeFacesEntity.setFaces(faces);
	// node faces entities
	vector<FacesEntity> nodeFacesEntities;
	nodeFacesEntities.push_back(nodeFacesEntity);
	// setup node vertex data
	node->setVertices(vertices);
	node->setNormals(normals);
	node->setFacesEntities(nodeFacesEntities);
	// register node
	model->getNodes()["node"] = node;
	model->getSubNodes()["node"] = node;
	// prepare for indexed rendering
	ModelTools::computeNormals(model);
	ModelTools::prepareForIndexedRendering(model);
	//
	return model;
}

Model* Primitives::createCapsuleModel(Capsule* capsule, const string& id, int32_t segmentsX, int32_t segmentsY)
{
	// capsule properties
	auto radius = capsule->getRadius();
	auto& a = capsule->getA();
	auto& b = capsule->getB();
	auto& center = capsule->getCenter();
	// rotation quaternion
	Quaternion rotationQuaternion;
	rotationQuaternion.identity();
	// angle between a and b
	Vector3 yAxis(0.0f, -1.0f, 0.0f);
	Vector3 abNormalized = a.clone().sub(b).normalize();
	auto& abNormalizedVectorXYZ = abNormalized.getArray();
	Vector3 rotationAxis;
	if (Math::abs(abNormalizedVectorXYZ[0]) < Math::EPSILON && Math::abs(abNormalizedVectorXYZ[2]) < Math::EPSILON) {
		rotationAxis.set(abNormalizedVectorXYZ[1], 0.0f, 0.0f);
	} else {
		rotationAxis = Vector3::computeCrossProduct(yAxis, abNormalized).normalize();
	}
	auto angle = Vector3::computeAngle(yAxis, abNormalized, yAxis);
	rotationQuaternion.rotate(rotationAxis, angle);
	auto rotationQuaternionMatrixInverted = rotationQuaternion.computeMatrix();
	rotationQuaternionMatrixInverted.invert();
	auto aInverted = a.clone().sub(center);
	auto bInverted = b.clone().sub(center);
	aInverted = rotationQuaternionMatrixInverted.multiply(aInverted);
	bInverted = rotationQuaternionMatrixInverted.multiply(bInverted);
	// model
	auto model = new Model(id, id, UpVector::Y_UP, RotationOrder::XYZ, nullptr);
	// material
	auto material = new Material("primitive");
	auto specularMaterialProperties = new SpecularMaterialProperties();
	specularMaterialProperties->setAmbientColor(
		Color4(
			245.0f / 255.0f * 0.5f,
			40.0f / 255.0f * 0.5f,
			135.0f / 255.0f * 0.5f,
			1.0f
		)
	);
	specularMaterialProperties->setDiffuseColor(
		Color4(
			245.0f / 255.0f * 0.5f,
			40.0f / 255.0f * 0.5f,
			135.0f / 255.0f * 0.5f,
			0.5f
		)
	);
	specularMaterialProperties->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
	material->setSpecularMaterialProperties(specularMaterialProperties);
	model->getMaterials()[material->getId()] = material;
	// node
	auto node = new Node(model, nullptr, "primitive", "primitive");
	// vertices
	vector<Vector3> vertices;
	//	bottom half sphere
	for (auto ySegment = 0; ySegment < segmentsY / 2; ySegment++)
	for (auto xSegment = 0; xSegment < segmentsX; xSegment++) {
		auto vertex =
			rotationQuaternion.multiply(
				Vector3(
					((Math::sin(Math::PI * ySegment / segmentsY) * Math::cos(Math::PI * 2 * xSegment / segmentsX))),
					((Math::cos(Math::PI * ySegment / segmentsY))),
					((Math::sin(Math::PI * ySegment / segmentsY) * Math::sin(Math::PI * 2 * xSegment / segmentsX)))
				)
			);
		vertex.scale(radius);
		vertex = rotationQuaternionMatrixInverted.multiply(vertex);
		vertex.add(bInverted);
		vertices.push_back(transformVector3(capsule, toRP3DVector3(vertex)));
	}
	//	top half sphere
	for (auto ySegment = segmentsY / 2; ySegment < segmentsY + 1; ySegment++)
	for (auto xSegment = 0; xSegment < segmentsX; xSegment++) {
		auto vertex =
			rotationQuaternion.multiply(
				Vector3(
					((Math::sin(Math::PI * ySegment / segmentsY) * Math::cos(Math::PI * 2 * xSegment / segmentsX))),
					((Math::cos(Math::PI * ySegment / segmentsY))),
					((Math::sin(Math::PI * ySegment / segmentsY) * Math::sin(Math::PI * 2 * xSegment / segmentsX)))
				)
			);
		vertex.scale(radius);
		vertex = rotationQuaternionMatrixInverted.multiply(vertex);
		vertex.add(aInverted);
		vertices.push_back(transformVector3(capsule, toRP3DVector3(vertex)));
	}
	// normals
	vector<Vector3> normals;
	// faces
	vector<Face> faces;
	int32_t vi0, vi1, vi2;
	int32_t ni;
	for (auto y = 0; y < segmentsY + 1; y++) {
		for (auto x = 0; x < segmentsX; x++) {
			vi0 = ((y + 0) % (segmentsY + 1)) * segmentsX + ((x + 0) % (segmentsX));
			vi1 = ((y + 1) % (segmentsY + 1)) * segmentsX + ((x + 1) % (segmentsX));
			vi2 = ((y + 1) % (segmentsY + 1)) * segmentsX + ((x + 0) % (segmentsX));
			ni = normals.size();
			{
				array<Vector3, 3> faceVertices = {
					vertices.at(vi0),
					vertices.at(vi1),
					vertices.at(vi2)
				};
				for (auto& normal: ModelTools::computeNormals(faceVertices)) {
					normals.push_back(normal);
				}
			}
			faces.push_back(Face(node, vi0, vi1, vi2, ni + 0, ni + 1, ni + 2));
			vi0 = ((y + 0) % (segmentsY + 1)) * segmentsX + ((x + 0) % (segmentsX));
			vi1 = ((y + 0) % (segmentsY + 1)) * segmentsX + ((x + 1) % (segmentsX));
			vi2 = ((y + 1) % (segmentsY + 1)) * segmentsX + ((x + 1) % (segmentsX));
			{
				array<Vector3, 3> faceVertices = {
					vertices.at(vi0),
					vertices.at(vi1),
					vertices.at(vi2)
				};
				for (auto& normal: ModelTools::computeNormals(faceVertices)) {
					normals.push_back(normal);
				}
			}
			faces.push_back(Face(node, vi0, vi1, vi2, ni + 0, ni + 1, ni + 2));
		}
	}
	// node faces entities
	FacesEntity nodeFacesEntity(node, "primitive.facesentity");
	nodeFacesEntity.setMaterial(material);
	nodeFacesEntity.setFaces(faces);
	// set up faces entity
	vector<FacesEntity> nodeFacesEntities;
	nodeFacesEntities.push_back(nodeFacesEntity);
	// setup node vertex data
	node->setVertices(vertices);
	node->setNormals(normals);
	node->setFacesEntities(nodeFacesEntities);
	// register node
	model->getNodes()["node"] = node;
	model->getSubNodes()["node"] = node;
	// prepare for indexed rendering
	ModelTools::computeNormals(model);
	ModelTools::prepareForIndexedRendering(model);
	//
	return model;
}

Model* Primitives::createConvexMeshModel(ConvexMesh* mesh, const string& id) {
	Console::println("Primitives::createConvexMeshModel(): This is not supported. Rather load the model and use Primitives::setupConvexMeshModel().");
	return nullptr;
}

void Primitives::setupConvexMeshModel(Model* model)
{
	// TODO: take bounding volume scale into account
	//	Note: there is no hurry as LE and ME do not do scaling of bounding volumes
	model->setImportTransformationsMatrix(model->getImportTransformationsMatrix().clone().scale(1.01f));
	auto material = new Material("primitive");
	auto specularMaterialProperties = new SpecularMaterialProperties();
	specularMaterialProperties->setAmbientColor(
		Color4(
			245.0f / 255.0f * 0.5f,
			40.0f / 255.0f * 0.5f,
			135.0f / 255.0f * 0.5f,
			1.0f
		)
	);
	specularMaterialProperties->setDiffuseColor(
		Color4(
			245.0f / 255.0f * 0.5f,
			40.0f / 255.0f * 0.5f,
			135.0f / 255.0f * 0.5f,
			0.5f
		)
	);
	specularMaterialProperties->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
	material->setSpecularMaterialProperties(specularMaterialProperties);
	model->getMaterials()[material->getId()] = material;
	setupConvexMeshMaterial(model->getSubNodes(), material);
}

void Primitives::setupConvexMeshMaterial(const map<string, Node*>& nodes, Material* material)
{
	for (auto it: nodes) {
		Node* node = it.second;
		auto facesEntities = node->getFacesEntities();
		for (auto& faceEntity : facesEntities) {
			faceEntity.setMaterial(material);
		}
		node->setFacesEntities(node->getFacesEntities());
		setupConvexMeshMaterial(node->getSubNodes(), material);
	}
}

Model* Primitives::createModel(BoundingBox* boundingVolume, const string& id)
{
	return Primitives::createBoundingBoxModel(boundingVolume, id);
}

Model* Primitives::createModel(BoundingVolume* boundingVolume, const string& id)
{
	if (dynamic_cast<OrientedBoundingBox*>(boundingVolume) != nullptr) {
		return Primitives::createOrientedBoundingBoxModel(dynamic_cast< OrientedBoundingBox* >(boundingVolume), id);
	} else
	if (dynamic_cast<Sphere*>(boundingVolume) != nullptr) {
		return Primitives::createSphereModel(dynamic_cast< Sphere* >(boundingVolume), id, SPHERE_SEGMENTS_X, SPHERE_SEGMENTS_Y);
	} else
	if (dynamic_cast<Capsule*>(boundingVolume) != nullptr) {
		return Primitives::createCapsuleModel(dynamic_cast< Capsule* >(boundingVolume), id, CAPSULE_SEGMENTS_X, CAPSULE_SEGMENTS_Y);
	} else
	if (dynamic_cast<ConvexMesh*>(boundingVolume) != nullptr) {
		return Primitives::createConvexMeshModel(dynamic_cast< ConvexMesh* >(boundingVolume), id);
	} else {
		Console::println(string("Primitives::createModel(): unsupported bounding volume"));
		return nullptr;
	}
}
