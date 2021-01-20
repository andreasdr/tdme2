#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController_GenerateConvexMeshes.h>

#include <string>
#include <vector>

#include <ext/v-hacd/src/VHACD_Lib/public/VHACD.h>

#include <tdme/engine/fileio/models/ModelReader.h>
#include <tdme/engine/fileio/models/TMWriter.h>
#include <tdme/engine/model/Color4.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/engine/primitives/Triangle.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeBoundingVolume.h>
#include <tdme/engine/Object3DModel.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/StandardFileSystem.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/controller/ProgressBarScreenController.h>
#include <tdme/tools/shared/controller/PrototypePhysicsSubScreenController.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/tools/shared/views/PrototypePhysicsView.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ExceptionBase.h>
#include <tdme/utilities/ModelTools.h>
#include <tdme/utilities/MutableString.h>

using std::string;
using std::to_string;
using std::vector;

using namespace VHACD;

using tdme::engine::fileio::models::ModelReader;
using tdme::engine::fileio::models::TMWriter;
using tdme::engine::model::Color4;
using tdme::engine::model::Face;
using tdme::engine::model::FacesEntity;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::model::UpVector;
using tdme::engine::primitives::Triangle;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::Object3DModel;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::StandardFileSystem;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::controller::ProgressBarScreenController;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController;
using tdme::tools::shared::controller::PrototypePhysicsSubScreenController_GenerateConvexMeshes;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::PopUps;
using tdme::tools::shared::views::PrototypePhysicsView;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ExceptionBase;
using tdme::utilities::ModelTools;
using tdme::utilities::MutableString;

void PrototypePhysicsSubScreenController_GenerateConvexMeshes::removeConvexMeshes(PrototypePhysicsSubScreenController* prototypePhysicsSubScreenController, Prototype* entityFinal)
{
	string meshPathName = prototypePhysicsSubScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getPathName();
	string meshFileName = prototypePhysicsSubScreenController->view->getPopUpsViews()->getFileDialogScreenController()->getFileName();
	// delete old convex meshes
	for (auto i = 0; i < Prototype::MODEL_BOUNDINGVOLUME_COUNT; i++) {
		auto convexHullFileName = meshFileName + ".cm." + to_string(i) + ".tm";
		if (FileSystem::getInstance()->fileExists(meshPathName + "/" + convexHullFileName) == false) {
			break;
		} else {
			if (FileSystem::getInstance()->getFileName(entityFinal->getBoundingVolume(i)->getModelMeshFile()) == convexHullFileName) {
				prototypePhysicsSubScreenController->onBoundingVolumeNoneApply(entityFinal, i);
			}
			FileSystem::getInstance()->removeFile(
				meshPathName,
				convexHullFileName
			);
		}
	}
}

void PrototypePhysicsSubScreenController_GenerateConvexMeshes::generateConvexMeshes(PrototypePhysicsSubScreenController* prototypePhysicsSubScreenController, Prototype* entityFinal)
{
	for (auto i = 0; i < Prototype::MODEL_BOUNDINGVOLUME_COUNT; i++) {
		prototypePhysicsSubScreenController->onBoundingVolumeNoneApply(entityFinal, i);
	}
	map<string, MutableString> values;
	prototypePhysicsSubScreenController->getScreenNode()->getValues(values);
	auto convexMeshMode = values["boundingvolume_convexmeshes_mode"].getString();
	vector<string> convexMeshFileNames;
	if (convexMeshMode == "vhacd") {
		class VHACDCallback : public IVHACD::IUserCallback {
			private:
				ProgressBarScreenController* progressBarScreenController;
			public:
				VHACDCallback(ProgressBarScreenController* progressBarScreenController): progressBarScreenController(progressBarScreenController) {}
				~VHACDCallback() {};
				void Update(
					const double overallProgress,
					const double stageProgress,
					const double operationProgress,
					const char* const stage,
					const char* const operation)
				{
					progressBarScreenController->progress((int)(overallProgress + 0.5) / 100.0f);
				};
		};

		class VHACDLogger : public IVHACD::IUserLogger {
			public:
				VHACDLogger() {}
				~VHACDLogger() {};
				void Log(const char* const msg)
				{
					Console::println(msg);
				}
		};

		//
		prototypePhysicsSubScreenController->getView()->getPopUpsViews()->getProgressBarScreenController()->show();
		IVHACD* vhacd = CreateVHACD();
		try {
			IVHACD::Parameters vhacdParams;
			vhacdParams.m_resolution = Tools::convertToInt(prototypePhysicsSubScreenController->convexMeshesResolution->getController()->getValue().getString());
			vhacdParams.m_concavity = Tools::convertToFloat(prototypePhysicsSubScreenController->convexMeshesConcavity->getController()->getValue().getString());
			vhacdParams.m_planeDownsampling = Tools::convertToInt(prototypePhysicsSubScreenController->convexMeshesPlaneDownSampling->getController()->getValue().getString());
			vhacdParams.m_convexhullDownsampling = Tools::convertToInt(prototypePhysicsSubScreenController->convexMeshesConvexHullDownSampling->getController()->getValue().getString());
			vhacdParams.m_alpha = Tools::convertToFloat(prototypePhysicsSubScreenController->convexMeshesAlpha->getController()->getValue().getString());
			vhacdParams.m_beta = Tools::convertToFloat(prototypePhysicsSubScreenController->convexMeshesBeta->getController()->getValue().getString());
			vhacdParams.m_maxNumVerticesPerCH = Tools::convertToInt(prototypePhysicsSubScreenController->convexMeshesMaxVerticesPerConvexHull->getController()->getValue().getString());
			vhacdParams.m_minVolumePerCH = Tools::convertToFloat(prototypePhysicsSubScreenController->convexMeshesMinVolumePerConvexHull->getController()->getValue().getString());
			vhacdParams.m_pca = Tools::convertToInt(prototypePhysicsSubScreenController->convexMeshesPCA->getController()->getValue().getString());
			if (vhacdParams.m_resolution < 10000 || vhacdParams.m_resolution > 64000000) {
				throw ExceptionBase("Resolution must be between 10000 and 64000000");
			}
			if (vhacdParams.m_concavity < 0.0f || vhacdParams.m_concavity > 1.0f) {
				throw ExceptionBase("Concavity must be between 0.0 and 1.0");
			}
			if (vhacdParams.m_planeDownsampling < 1 || vhacdParams.m_planeDownsampling > 16) {
				throw ExceptionBase("Plane down sampling must be between 1 and 16");
			}
			if (vhacdParams.m_convexhullDownsampling < 1 || vhacdParams.m_convexhullDownsampling > 16) {
				throw ExceptionBase("Convex hull down sampling must be between 1 and 16");
			}
			if (vhacdParams.m_alpha < 0.0f || vhacdParams.m_alpha > 1.0f) {
				throw ExceptionBase("Alpha must be between 0.0 and 1.0");
			}
			if (vhacdParams.m_beta < 0.0f || vhacdParams.m_beta > 1.0f) {
				throw ExceptionBase("Beta must be between 0.0 and 1.0");
			}
			if (vhacdParams.m_maxNumVerticesPerCH < 4 || vhacdParams.m_maxNumVerticesPerCH > 1024) {
				throw ExceptionBase("Max number of vertices per convex hull must be between 4 and 1024");
			}
			if (vhacdParams.m_minVolumePerCH < 0.0f || vhacdParams.m_minVolumePerCH > 0.01f) {
				throw ExceptionBase("Min volume per convex hull must be between 0.0 and 0.01");
			}
			if (vhacdParams.m_pca > 1) {
				throw ExceptionBase("PCA must be between 0 and 1");
			}
			VHACDCallback vhacdCallback(prototypePhysicsSubScreenController->getView()->getPopUpsViews()->getProgressBarScreenController());
			VHACDLogger vhacdLogger;
			vhacdParams.m_logger = &vhacdLogger;
			vhacdParams.m_callback = &vhacdCallback;
			vector<float> meshPoints;
			vector<int> meshTriangles;
			string meshPathName = Tools::getPathName(prototypePhysicsSubScreenController->convexMeshesFile->getController()->getValue().getString());
			string meshFileName = Tools::getFileName(prototypePhysicsSubScreenController->convexMeshesFile->getController()->getValue().getString());
			auto meshModel = ModelReader::read(
				meshPathName,
				meshFileName
			);
			{
				Object3DModel meshObject3DModel(meshModel);
				vector<Triangle> meshFaceTriangles;
				meshObject3DModel.getTriangles(meshFaceTriangles);
				for (auto& triangle: meshFaceTriangles) {
					meshTriangles.push_back(meshPoints.size() / 3 + 0);
					meshTriangles.push_back(meshPoints.size() / 3 + 1);
					meshTriangles.push_back(meshPoints.size() / 3 + 2);
					for (auto i = 0; i < triangle.getVertices().size(); i++) {
						meshPoints.push_back(triangle.getVertices()[i].getX());
						meshPoints.push_back(triangle.getVertices()[i].getY());
						meshPoints.push_back(triangle.getVertices()[i].getZ());
					}
				}
			}
			delete meshModel;
			bool vhacdResult =
				vhacd->Compute(
					&meshPoints[0],
					(unsigned int)meshPoints.size() / 3,
					(const unsigned int *)&meshTriangles[0],
					(unsigned int)meshTriangles.size() / 3,
					vhacdParams
				);
			if (vhacdResult == true) {
				auto convexHulls = vhacd->GetNConvexHulls();
				if (convexHulls > Prototype::MODEL_BOUNDINGVOLUME_COUNT) {
					throw ExceptionBase(
						"More than " +
						to_string(Prototype::MODEL_BOUNDINGVOLUME_COUNT) +
						" convex hulls: " + to_string(convexHulls)
					);
				}
				// delete old convex meshes
				for (auto i = 0; i < Prototype::MODEL_BOUNDINGVOLUME_COUNT; i++) {
					auto convexHullFileName = meshFileName + ".cm." + to_string(i) + ".tm";
					if (FileSystem::getInstance()->fileExists(meshPathName + "/" + convexHullFileName) == false) {
						break;
					} else {
						FileSystem::getInstance()->removeFile(
							meshPathName,
							convexHullFileName
						);
					}
				}
				IVHACD::ConvexHull convexHull;
				for (auto i = 0; i < convexHulls; i++) {
					vhacd->GetConvexHull(i, convexHull);
					auto convexHullFileName = meshFileName + ".cm." + to_string(i) + ".tm";
					Console::println(
						"PrototypePhysicsSubScreenController_GenerateConvexMeshes::generateConvexMeshes(): VHACD: Saving convex hull@" +
						to_string(i) +
						": " + convexHullFileName +
						", points = " + to_string(convexHull.m_nPoints) +
						", triangles = " + to_string(convexHull.m_nTriangles)
					);
					auto convexHullModel = createModel(
						meshPathName + "/" + convexHullFileName,
						convexHull.m_points,
						convexHull.m_triangles,
						convexHull.m_nPoints,
						convexHull.m_nTriangles
					);
					TMWriter::write(convexHullModel, meshPathName, convexHullFileName);
					delete convexHullModel;
					convexMeshFileNames.push_back(meshPathName + "/" + convexHullFileName);
				}
			}
		} catch (Exception &exception) {
			convexMeshFileNames.clear();
			prototypePhysicsSubScreenController->view->getPopUpsViews()->getInfoDialogScreenController()->show(
				"Warning: Could not create convex hulls",
				exception.what()
			);
			Console::println(string("Could not create convex hulls: ") + exception.what());
		}
		vhacd->Clean();
		vhacd->Release();
		prototypePhysicsSubScreenController->getView()->getPopUpsViews()->getProgressBarScreenController()->close();
	} else
	if (convexMeshMode == "model") {
		try {
			string meshPathName = Tools::getPathName(prototypePhysicsSubScreenController->convexMeshesFile->getController()->getValue().getString());
			string meshFileName = Tools::getFileName(prototypePhysicsSubScreenController->convexMeshesFile->getController()->getValue().getString());
			auto meshModel = ModelReader::read(
				meshPathName,
				meshFileName
			);
			{
				Object3DModel meshObject3DModel(meshModel);
				for (auto i = 0; i < meshObject3DModel.getNodeCount(); i++) {
					vector<Triangle> nodeTriangles;
					meshObject3DModel.getTriangles(nodeTriangles, i);
					auto convexHullFileName = meshFileName + ".cm." + to_string(i) + ".tm";
					Console::println(
						"PrototypePhysicsSubScreenController_GenerateConvexMeshes::generateConvexMeshes(): Model: Saving convex hull@" +
						to_string(i) +
						": " + convexHullFileName +
						", triangles = " + to_string(nodeTriangles.size())
					);
					auto convexHullModel = createModel(
						meshPathName + "/" + convexHullFileName,
						nodeTriangles
					);
					TMWriter::write(convexHullModel, meshPathName, convexHullFileName);
					delete convexHullModel;
					convexMeshFileNames.push_back(meshPathName + "/" + convexHullFileName);

				}
			}
			delete meshModel;
		} catch (Exception &exception) {
			convexMeshFileNames.clear();
			prototypePhysicsSubScreenController->view->getPopUpsViews()->getInfoDialogScreenController()->show(
				"Warning: Could not create convex hulls",
				exception.what()
			);
			Console::println(string("Could not create convex hulls: ") + exception.what());
		}
	}
	for (auto i = 0; i < Prototype::MODEL_BOUNDINGVOLUME_COUNT && i < convexMeshFileNames.size(); i++) {
		prototypePhysicsSubScreenController->boundingvolumeConvexMeshFile[i]->getController()->setValue(MutableString(convexMeshFileNames[i]));
		prototypePhysicsSubScreenController->onBoundingVolumeConvexMeshApply(entityFinal, i);
		prototypePhysicsSubScreenController->setupModelBoundingVolumeType(entityFinal, i);
	}
}

Model* PrototypePhysicsSubScreenController_GenerateConvexMeshes::createModel(const string& id, double* points, unsigned int* triangles, unsigned int pointCount, unsigned int triangleCount) {
	auto model = new Model(id, id, UpVector::Y_UP, RotationOrder::XYZ, nullptr);
	auto material = new Material("primitive");
	material->setSpecularMaterialProperties(new SpecularMaterialProperties());
	material->getSpecularMaterialProperties()->setAmbientColor(Color4(0.5f, 0.5f, 0.5f, 1.0f));
	material->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 0.5f, 0.5f, 0.5f));
	material->getSpecularMaterialProperties()->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
	model->getMaterials()[material->getId()] = material;
	auto node = new Node(model, nullptr, "node", "node");
	vector<Vector3> vertices;
	vector<Vector3> normals;
	vector<Face> faces;
	int normalIndex = -1;
	for (auto i = 0; i < pointCount; i++) {
		vertices.push_back(
			Vector3(
				static_cast<float>(points[i * 3 + 0]),
				static_cast<float>(points[i * 3 + 1]),
				static_cast<float>(points[i * 3 + 2])
			)
		);
	}
	for (auto i = 0; i < triangleCount; i++) {
		normalIndex = normals.size();
		{
			array<Vector3, 3> faceVertices = {
				vertices[triangles[i * 3 + 0]],
				vertices[triangles[i * 3 + 1]],
				vertices[triangles[i * 3 + 2]]
			};
			array<Vector3, 3> faceNormals;
			ModelTools::computeNormals(faceVertices, faceNormals);
			for (auto& normal : faceNormals) {
				normals.push_back(normal);
			}
		}
		faces.push_back(
			Face(
				node,
				triangles[i * 3 + 0],
				triangles[i * 3 + 1],
				triangles[i * 3 + 2],
				normalIndex + 0,
				normalIndex + 1,
				normalIndex + 2
			)
		);
	}
	FacesEntity nodeFacesEntity(node, "faces entity");
	nodeFacesEntity.setMaterial(material);
	nodeFacesEntity.setFaces(faces);
	vector<FacesEntity> nodeFacesEntities;
	nodeFacesEntities.push_back(nodeFacesEntity);
	node->setVertices(vertices);
	node->setNormals(normals);
	node->setFacesEntities(nodeFacesEntities);
	model->getNodes()["node"] = node;
	model->getSubNodes()["node"] = node;
	ModelTools::prepareForIndexedRendering(model);
	return model;
}

Model* PrototypePhysicsSubScreenController_GenerateConvexMeshes::createModel(const string& id, vector<Triangle>& triangles) {
	auto model = new Model(id, id, UpVector::Y_UP, RotationOrder::XYZ, nullptr);
	auto material = new Material("primitive");
	material->setSpecularMaterialProperties(new SpecularMaterialProperties());
	material->getSpecularMaterialProperties()->setAmbientColor(Color4(0.5f, 0.5f, 0.5f, 1.0f));
	material->getSpecularMaterialProperties()->setDiffuseColor(Color4(1.0f, 0.5f, 0.5f, 0.5f));
	material->getSpecularMaterialProperties()->setSpecularColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
	model->getMaterials()[material->getId()] = material;
	auto node = new Node(model, nullptr, "node", "node");
	vector<Vector3> vertices;
	vector<Vector3> normals;
	vector<Face> faces;
	auto index = 0;
	for (auto& triangle: triangles) {
		for (auto& vertex: triangle.getVertices()) {
			vertices.push_back(vertex);
		}
		{
			array<Vector3, 3> faceVertices = {
				triangle.getVertices()[0],
				triangle.getVertices()[1],
				triangle.getVertices()[2],
			};
			array<Vector3, 3> faceNormals;
			ModelTools::computeNormals(faceVertices, faceNormals);
			for (auto& normal : faceNormals) {
				normals.push_back(normal);
			}
		}
		faces.push_back(
			Face(
				node,
				index + 0,
				index + 1,
				index + 2,
				index + 0,
				index + 1,
				index + 2
			)
		);
		index+= 3;
	}
	FacesEntity nodeFacesEntity(node, "faces entity");
	nodeFacesEntity.setMaterial(material);
	nodeFacesEntity.setFaces(faces);
	vector<FacesEntity> nodeFacesEntities;
	nodeFacesEntities.push_back(nodeFacesEntity);
	node->setVertices(vertices);
	node->setNormals(normals);
	node->setFacesEntities(nodeFacesEntities);
	model->getNodes()["node"] = node;
	model->getSubNodes()["node"] = node;
	ModelTools::prepareForIndexedRendering(model);
	return model;
}
