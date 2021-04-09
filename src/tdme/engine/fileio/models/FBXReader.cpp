#include <tdme/engine/fileio/models/FBXReader.h>

#define FBXSDK_SHARED

#include <fbxsdk.h>

#include <map>
#include <string>
#include <vector>

#include <tdme/engine/model/Animation.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/engine/model/Face.h>
#include <tdme/engine/model/FacesEntity.h>
#include <tdme/engine/model/Joint.h>
#include <tdme/engine/model/JointWeight.h>
#include <tdme/engine/model/Material.h>
#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/Node.h>
#include <tdme/engine/model/RotationOrder.h>
#include <tdme/engine/model/Skinning.h>
#include <tdme/engine/model/SpecularMaterialProperties.h>
#include <tdme/engine/model/TextureCoordinate.h>
#include <tdme/engine/model/UpVector.h>
#include <tdme/math/Math.h>
#include <tdme/math/Matrix2D3x3.h>
#include <tdme/math/Vector2.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/ModelTools.h>

using std::map;
using std::string;
using std::to_string;
using std::vector;

using tdme::engine::fileio::models::FBXReader;
using tdme::engine::model::Animation;
using tdme::engine::model::AnimationSetup;
using tdme::engine::model::Joint;
using tdme::engine::model::JointWeight;
using tdme::engine::model::Material;
using tdme::engine::model::Model;
using tdme::engine::model::Node;
using tdme::engine::model::RotationOrder;
using tdme::engine::model::Skinning;
using tdme::engine::model::SpecularMaterialProperties;
using tdme::engine::model::TextureCoordinate;
using tdme::engine::model::UpVector;
using tdme::math::Math;
using tdme::math::Matrix2D3x3;
using tdme::math::Vector2;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::ModelTools;

const Color4 FBXReader::BLENDER_AMBIENT_NONE(0.0f, 0.0f, 0.0f, 1.0f);

Model* FBXReader::read(const string& pathName, const string& fileName) throw (ModelFileIOException, FileSystemException) {
	// init fbx sdk
	FbxManager* fbxManager = NULL;
	FbxScene* fbxScene = NULL;
	fbxManager = FbxManager::Create();
	if (fbxManager == nullptr) {
		Console::println("FBXReader::read(): Unable to create FBX manager.");
		return nullptr;
	} else {
		Console::println(string("FBXReader::read(): Autodesk FBX SDK version ") + string(fbxManager->GetVersion()));
	}

	Console::println("FBXReader::read(): reading FBX scene");

	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);
	FbxString lPath = FbxGetApplicationDirectory();
	fbxManager->LoadPluginsDirectory(lPath.Buffer());
	fbxScene = FbxScene::Create(fbxManager, "My Scene");
	if (fbxScene == nullptr) {
		throw ModelFileIOException("FBXReader::read(): Error: Unable to create FBX scene");
	}

	// create import and import scene
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool fbxImportStatus = fbxImporter->Initialize((pathName + "/" + fileName).c_str(), -1, fbxManager->GetIOSettings());
	if (fbxImportStatus == false) {
		throw ModelFileIOException("FBXReader::read(): Error: Unable to import FBX scene from '" + pathName + "/" + fileName);
	}
	// import the scene
	fbxImportStatus = fbxImporter->Import(fbxScene);
	if (fbxImportStatus == false) {
		throw ModelFileIOException("FBXReader::read(): Error: Unable to import FBX scene from '" + pathName + "/" + fileName + " into scene");
	}

	Console::println("FBXReader::read(): Authoring program: " + string(fbxScene->GetDocumentInfo()->Original_ApplicationName.Get().Buffer()));

	Console::println("FBXReader::read(): triangulating FBX");
	// triangulate
	FbxGeometryConverter fbxGeometryConverter(fbxManager);
	fbxGeometryConverter.Triangulate(fbxScene, true);

	Console::println("FBXReader::read(): importing FBX");

	// create model
	auto model = new Model(
		FileSystem::getInstance()->getCanonicalPath(pathName, fileName),
		fileName,
		getSceneUpVector(fbxScene),
		getSceneRotationOrder(fbxScene),
		nullptr,
		string(fbxScene->GetDocumentInfo()->Original_ApplicationName.Get().Buffer()).find("Blender") != -1?
			Model::AUTHORINGTOOL_BLENDER:
			Model::AUTHORINGTOOL_UNKNOWN
	);

	// set up model import matrix
	setupModelImportRotationMatrix(model);
	setupModelScaleRotationMatrix(fbxScene, model);

	// process nodes
	processScene(fbxScene, model, pathName);

	//
	Console::println("FBXReader::read(): setting up animations");

	// parse animations stacks
	FbxTime::SetGlobalTimeMode(FbxTime::eCustom, 30.0);
	FbxArray<FbxString*> fbxAnimStackNameArray;
	fbxScene->FillAnimStackNameArray(fbxAnimStackNameArray);
	int framesTotal = 0;
	for(auto i = 0; i < fbxAnimStackNameArray.GetCount(); i++) {
		FbxTime fbxStartTime, fbxEndTime;
		auto fbxCurrentTakeInfo = fbxScene->GetTakeInfo(*(fbxAnimStackNameArray[i]));
		if (fbxCurrentTakeInfo != nullptr) {
			fbxStartTime = fbxCurrentTakeInfo->mLocalTimeSpan.GetStart();
			fbxEndTime = fbxCurrentTakeInfo->mLocalTimeSpan.GetStop();
		} else {
			FbxTimeSpan fbxTimeLineTimeSpan;
			fbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(fbxTimeLineTimeSpan);
			fbxStartTime = fbxTimeLineTimeSpan.GetStart();
			fbxEndTime = fbxTimeLineTimeSpan.GetStop();
		}
		int startFrame = (int)Math::ceil(fbxStartTime.GetMilliSeconds() / (1000.0f * 1.0f / 30.0f));
		int endFrame = (int)Math::ceil(fbxEndTime.GetMilliSeconds() / (1000.0f * 1.0f / 30.0f)) - 1;
		framesTotal+= endFrame - startFrame + 1;
	}
	model->addAnimationSetup(
		Model::ANIMATIONSETUP_DEFAULT,
		0,
		framesTotal,
		true
	);
	int frameOffset = 0;
	for(auto i = 0; i < fbxAnimStackNameArray.GetCount(); i++ ) {
		auto fbxCurrentAnimationStack = fbxScene->FindMember<FbxAnimStack>(fbxAnimStackNameArray[i]->Buffer());
		auto fbxCurrentTakeInfo = fbxScene->GetTakeInfo(*(fbxAnimStackNameArray[i]));
		FbxTime fbxStartTime, fbxEndTime;
		if (fbxCurrentTakeInfo != nullptr) {
			fbxStartTime = fbxCurrentTakeInfo->mLocalTimeSpan.GetStart();
			fbxEndTime = fbxCurrentTakeInfo->mLocalTimeSpan.GetStop();
		} else {
			FbxTimeSpan fbxTimeLineTimeSpan;
			fbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(fbxTimeLineTimeSpan);
			fbxStartTime = fbxTimeLineTimeSpan.GetStart();
			fbxEndTime = fbxTimeLineTimeSpan.GetStop();
		}
		int startFrame = (int)Math::ceil(fbxStartTime.GetMilliSeconds() / (1000.0f * 1.0f / 30.0f));
		int endFrame = (int)Math::ceil(fbxEndTime.GetMilliSeconds() / (1000.0f * 1.0f / 30.0f)) - 1;
		model->addAnimationSetup(
			string(fbxAnimStackNameArray[i]->Buffer()),
			frameOffset + startFrame,
			frameOffset + endFrame,
			false
		);
        fbxScene->SetCurrentAnimationStack(fbxCurrentAnimationStack);
		FbxNode* fbxNode = fbxScene->GetRootNode();
		if (fbxNode == nullptr) continue;
		for(auto i = 0; i < fbxNode->GetChildCount(); i++) {
			processAnimation(fbxNode->GetChild(i), fbxStartTime, fbxEndTime, model, frameOffset);
		}
        frameOffset+= endFrame - startFrame + 1;
	}
	FbxArrayDelete(fbxAnimStackNameArray);

	//
	Console::println("FBXReader::read(): destroying FBX SDK");

	// destroy the importer
	if (fbxImporter != nullptr) fbxImporter->Destroy();

	// destroy fbx manager
	if (fbxManager != nullptr) fbxManager->Destroy();

	Console::println("FBXReader::read(): prepare for indexed rendering");

	//
	ModelTools::setupJoints(model);
	ModelTools::fixAnimationLength(model);
	ModelTools::prepareForIndexedRendering(model);

	Console::println("FBXReader::read(): done");

	//
	return model;
}

RotationOrder* FBXReader::getSceneRotationOrder(FbxScene* fbxScene) {
	auto upVector = getSceneUpVector(fbxScene);

	// take rotation order from root node now
	FbxNode* fbxNode = fbxScene->GetRootNode();
	EFbxRotationOrder fbxRotationOrder;
	fbxNode->GetRotationOrder(FbxNode::eSourcePivot, fbxRotationOrder);
	if (fbxRotationOrder == eEulerXYZ) {
		if (upVector == UpVector::Y_UP) {
			return RotationOrder::ZYX;
		} else
		if (upVector == UpVector::Z_UP) {
			return RotationOrder::YZX;
		} else {
			throw ModelFileIOException("Unknown Up vector");
		}
	} else {
		throw ModelFileIOException("Not supported rotation order(" + to_string(fbxRotationOrder) + ")");
	}
}

UpVector* FBXReader::getSceneUpVector(FbxScene* fbxScene) {
	int fbxUpVectorSign;
	auto fbxUpVector = fbxScene->GetGlobalSettings().GetAxisSystem().GetUpVector(fbxUpVectorSign);
	switch (fbxUpVector) {
		case FbxAxisSystem::eXAxis:
			throw ModelFileIOException("X-Up is not supported");
		case FbxAxisSystem::eYAxis:
			return UpVector::Y_UP;
		case FbxAxisSystem::eZAxis:
			return UpVector::Z_UP;
		default:
			throw ModelFileIOException("Unknown Up vector");
	}
	return UpVector::Y_UP;
}

void FBXReader::setupModelImportRotationMatrix(Model* model) {
	if (model->getUpVector() == UpVector::Y_UP) {
		// no op
	} else
	if (model->getUpVector() == UpVector::Z_UP) {
		model->setImportTransformationsMatrix(model->getImportTransformationsMatrix().clone().rotate(Vector3(1.0f, 0.0f, 0.0f), -90.0));
	}
}

void FBXReader::setupModelScaleRotationMatrix(FbxScene* fbxScene, Model* model) {
	FbxSystemUnit fbxSceneSystemUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
	model->setImportTransformationsMatrix(model->getImportTransformationsMatrix().clone().scale(static_cast<float>(fbxSceneSystemUnit.GetConversionFactorTo(FbxSystemUnit::m))));
}

void FBXReader::processScene(FbxScene* fbxScene, Model* model, const string& pathName) {
	FbxNode* fbxNode = fbxScene->GetRootNode();
	if (fbxNode == nullptr) return;
	for(auto i = 0; i < fbxNode->GetChildCount(); i++) {
		processNode(fbxNode->GetChild(i), model, nullptr, pathName);
	}
}

void FBXReader::processNode(FbxNode* fbxNode, Model* model, Node* parentNode, const string& pathName) {
	Node* node = nullptr;
	if (fbxNode->GetNodeAttribute() != nullptr) {
		auto fbxAttributeType = fbxNode->GetNodeAttribute()->GetAttributeType();
		switch (fbxAttributeType) {
			case FbxNodeAttribute::eMesh:
				{
					node = processMeshNode(fbxNode, model, parentNode, pathName);
					break;
				}
			case FbxNodeAttribute::eSkeleton:
				{
					node = processSkeletonNode(fbxNode, model, parentNode, pathName);
					break;
				}
			default:
				{
					break;
				}
		}
	}
	if (node == nullptr) {
		auto fbxNodeName = fbxNode->GetName();
		node = new Node(model, parentNode, fbxNodeName, fbxNodeName);
	}
	FbxAMatrix& fbxNodeLocalTransform = fbxNode->EvaluateLocalTransform();
	node->setTransformationsMatrix(
		Matrix4x4(
			fbxNodeLocalTransform.Get(0,0),
			fbxNodeLocalTransform.Get(0,1),
			fbxNodeLocalTransform.Get(0,2),
			fbxNodeLocalTransform.Get(0,3),
			fbxNodeLocalTransform.Get(1,0),
			fbxNodeLocalTransform.Get(1,1),
			fbxNodeLocalTransform.Get(1,2),
			fbxNodeLocalTransform.Get(1,3),
			fbxNodeLocalTransform.Get(2,0),
			fbxNodeLocalTransform.Get(2,1),
			fbxNodeLocalTransform.Get(2,2),
			fbxNodeLocalTransform.Get(2,3),
			fbxNodeLocalTransform.Get(3,0),
			fbxNodeLocalTransform.Get(3,1),
			fbxNodeLocalTransform.Get(3,2),
			fbxNodeLocalTransform.Get(3,3)
		)
	);
	if (parentNode == nullptr) {
		model->getSubNodes()[node->getId()] = node;
	} else {
		parentNode->getSubNodes()[node->getId()] = node;
	}
	model->getNodes()[node->getId()] = node;
	parentNode = node;
	for(auto i = 0; i < fbxNode->GetChildCount(); i++) {
		processNode(fbxNode->GetChild(i), model, parentNode, pathName);
	}
}

Node* FBXReader::processMeshNode(FbxNode* fbxNode, Model* model, Node* parentNode, const string& pathName) {
	auto fbxNodeName = fbxNode->GetName();
	FbxMesh* fbxMesh = (FbxMesh*)fbxNode->GetNodeAttribute();

	auto node = new Node(model, parentNode, fbxNodeName, fbxNodeName);
	vector<Vector3> vertices;
	vector<Vector3> normals;
	vector<TextureCoordinate> textureCoordinates;
	vector<Vector3> tangents;
	vector<Vector3> bitangents;
	vector<FacesEntity> facesEntities;
	vector<Face> faces;
	FacesEntity* facesEntity = nullptr;

	int fbxVertexId = 0;
	int fbxPolygonCount = fbxMesh->GetPolygonCount();

	FbxVector4* fbxControlPoints = fbxMesh->GetControlPoints();
	for (auto i = 0; i < fbxMesh->GetControlPointsCount(); i++) {
		auto fbxControlPoint = fbxControlPoints[i];
		vertices.push_back(Vector3(fbxControlPoint[0], fbxControlPoint[1], fbxControlPoint[2]));
	}
	for (auto l = 0; l < fbxMesh->GetElementUVCount() && l < 1; ++l) {
		auto fbxUV = fbxMesh->GetElementUV(l);
		for (int i = 0; i < fbxUV->GetDirectArray().GetCount(); i++) {
			auto fbxUVArray = fbxUV->GetDirectArray().GetAt(i);
			textureCoordinates.push_back(TextureCoordinate(fbxUVArray[0], fbxUVArray[1]));
		}
	}
	for (auto l = 0; l < fbxMesh->GetElementNormalCount() && l < 1; ++l) {
		auto fbxNormal = fbxMesh->GetElementNormal(l);
		for (int i = 0; i < fbxNormal->GetDirectArray().GetCount(); i++) {
			auto fbxNormalArray = fbxNormal->GetDirectArray().GetAt(i);
			normals.push_back(Vector3(fbxNormalArray[0], fbxNormalArray[1], fbxNormalArray[2]));
		}
	}
	for (auto l = 0; l < fbxMesh->GetElementTangentCount() && l < 1; ++l) {
		auto fbxTangent = fbxMesh->GetElementTangent(l);
		for (int i = 0; i < fbxTangent->GetDirectArray().GetCount(); i++) {
			auto fbxTangentArray = fbxTangent->GetDirectArray().GetAt(i);
			tangents.push_back(Vector3(fbxTangentArray[0], fbxTangentArray[1], fbxTangentArray[2]));
		}
	}
	for (auto l = 0; l < fbxMesh->GetElementBinormalCount() && l < 1; ++l) {
		auto fbxBinormal = fbxMesh->GetElementBinormal(l);
		for (int i = 0; i < fbxBinormal->GetDirectArray().GetCount(); i++) {
			auto fbxBinormalArray = fbxBinormal->GetDirectArray().GetAt(i);
			bitangents.push_back(Vector3(fbxBinormalArray[0], fbxBinormalArray[1], fbxBinormalArray[2]));
		}
	}

	for (auto i = 0; i < fbxPolygonCount; i++) {
		FbxSurfaceMaterial* fbxMaterial = nullptr;
		for (auto l = 0; l < fbxMesh->GetElementMaterialCount() & l < 1; l++) {
			FbxGeometryElementMaterial* fbxMaterialElement = fbxMesh->GetElementMaterial(l);
			if (fbxMaterialElement->GetMappingMode() == FbxGeometryElement::eAllSame) {
				fbxMaterial = fbxMesh->GetNode()->GetMaterial(fbxMaterialElement->GetIndexArray().GetAt(0));
			} else {
				fbxMaterial = fbxMesh->GetNode()->GetMaterial(fbxMaterialElement->GetIndexArray().GetAt(i));
			}
		}
		Material* material = nullptr;
		if (fbxMaterial == nullptr) {
			material = model->getMaterials()["tdme.nomaterial"];
			if (material == nullptr) {
				material = new Material("tdme.nomaterial");
				model->getMaterials()[material->getId()] = material;
			}
		} else {
			string fbxMaterialName = fbxMaterial->GetName();
			material = model->getMaterials()[fbxMaterialName];
			if (material == nullptr) {
				material = new Material(fbxMaterialName);
				auto specularMaterialProperties = new SpecularMaterialProperties();
				if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) {
					FbxPropertyT<FbxDouble3> fbxColor3;
					FbxPropertyT<FbxDouble> fbxTransparency;
					FbxPropertyT<FbxDouble> fbxFactor;
					FbxPropertyT<FbxDouble> fbxShininess;
					FbxPropertyT<FbxDouble> fbxShininessFactor;
					FbxPropertyT<FbxDouble> fbxReflection;
					fbxColor3 = ((FbxSurfacePhong*)fbxMaterial)->Ambient;
					fbxFactor = ((FbxSurfacePhong*)fbxMaterial)->AmbientFactor;
					if (fbxColor3.IsValid() == true && fbxFactor.IsValid() == true) {
						specularMaterialProperties->setAmbientColor(
							Color4(
								static_cast<float>(fbxColor3.Get()[0] * fbxFactor.Get()),
								static_cast<float>(fbxColor3.Get()[1] * fbxFactor.Get()),
								static_cast<float>(fbxColor3.Get()[2] * fbxFactor.Get()),
								1.0f
							)
						);
					}
					fbxColor3 = ((FbxSurfacePhong*)fbxMaterial)->Diffuse;
					fbxFactor = ((FbxSurfacePhong*)fbxMaterial)->DiffuseFactor;
					fbxTransparency = ((FbxSurfacePhong*)fbxMaterial)->TransparencyFactor;
					if (fbxColor3.IsValid() == true && fbxFactor.IsValid() == true && fbxTransparency.IsValid() == true)
						specularMaterialProperties->setDiffuseColor(
						Color4(
							static_cast<float>(fbxColor3.Get()[0] * fbxFactor.Get()),
							static_cast<float>(fbxColor3.Get()[1] * fbxFactor.Get()),
							static_cast<float>(fbxColor3.Get()[2] * fbxFactor.Get()),
							// TODO: I am not sure about this here, but it seem to work
							(
								1.0f - static_cast<float>(fbxTransparency) < Math::EPSILON?
									1.0f:
									1.0f - static_cast<float>(fbxTransparency)
							)
						)
					);
					fbxColor3 = ((FbxSurfacePhong*)fbxMaterial)->Emissive;
					fbxFactor = ((FbxSurfacePhong*)fbxMaterial)->EmissiveFactor;
					if (fbxColor3.IsValid() == true && fbxFactor.IsValid() == true) {
						specularMaterialProperties->setEmissionColor(
							Color4(
								static_cast<float>(fbxColor3.Get()[0] * fbxFactor.Get()),
								static_cast<float>(fbxColor3.Get()[1] * fbxFactor.Get()),
								static_cast<float>(fbxColor3.Get()[2] * fbxFactor.Get()),
								1.0f
							)
						);
					}
					fbxColor3 = ((FbxSurfacePhong*)fbxMaterial)->Specular;
					fbxFactor = ((FbxSurfacePhong*)fbxMaterial)->SpecularFactor;
					if (fbxColor3.IsValid() == true && fbxFactor.IsValid() == true) {
						specularMaterialProperties->setSpecularColor(
							Color4(
								static_cast<float>(fbxColor3.Get()[0] * fbxFactor.Get()),
								static_cast<float>(fbxColor3.Get()[1] * fbxFactor.Get()),
								static_cast<float>(fbxColor3.Get()[2] * fbxFactor.Get()),
								1.0f
							)
						);
					}
					fbxShininess = ((FbxSurfacePhong*)fbxMaterial)->Reflection;
					fbxShininessFactor = ((FbxSurfacePhong*)fbxMaterial)->ReflectionFactor;
					if (fbxShininess.IsValid() == true && fbxShininessFactor.IsValid() == true) {
						specularMaterialProperties->setShininess(static_cast<float>(fbxShininess.Get() * fbxShininessFactor.Get()));
					}
					fbxReflection = ((FbxSurfacePhong*)fbxMaterial)->Reflection;
					if (fbxReflection.IsValid() == true) {
						specularMaterialProperties->setReflection(static_cast<float>(fbxReflection.Get()));
					}
				} else
				if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
					FbxPropertyT<FbxDouble3> fbxColor3;
					FbxPropertyT<FbxDouble> fbxTransparency;
					FbxPropertyT<FbxDouble> fbxFactor;
					fbxColor3 = ((FbxSurfaceLambert*)fbxMaterial)->Ambient;
					fbxFactor = ((FbxSurfaceLambert*)fbxMaterial)->AmbientFactor;
					if (fbxColor3.IsValid() == true && fbxFactor.IsValid() == true) {
						specularMaterialProperties->setAmbientColor(
							Color4(
								static_cast<float>(fbxColor3.Get()[0] * fbxFactor.Get()),
								static_cast<float>(fbxColor3.Get()[1] * fbxFactor.Get()),
								static_cast<float>(fbxColor3.Get()[2] * fbxFactor.Get()),
								1.0f
							)
						);
					}
					fbxColor3 = ((FbxSurfaceLambert*)fbxMaterial)->Diffuse;
					fbxFactor = ((FbxSurfaceLambert*)fbxMaterial)->DiffuseFactor;
					fbxTransparency = ((FbxSurfaceLambert*)fbxMaterial)->TransparencyFactor;
					if (fbxColor3.IsValid() == true && fbxFactor.IsValid() == true && fbxTransparency.IsValid() == true)
						specularMaterialProperties->setDiffuseColor(
						Color4(
							static_cast<float>(fbxColor3.Get()[0] * fbxFactor.Get()),
							static_cast<float>(fbxColor3.Get()[1] * fbxFactor.Get()),
							static_cast<float>(fbxColor3.Get()[2] * fbxFactor.Get()),
							// TODO: I am not sure about this here, but it seem to work
							(
								1.0f - static_cast<float>(fbxTransparency) < Math::EPSILON?
									1.0f:
									1.0f - static_cast<float>(fbxTransparency)
							)
						)
					);
					fbxColor3 = ((FbxSurfaceLambert*)fbxMaterial)->Emissive;
					fbxFactor = ((FbxSurfaceLambert*)fbxMaterial)->EmissiveFactor;
					if (fbxColor3.IsValid() == true && fbxFactor.IsValid() == true) {
						specularMaterialProperties->setEmissionColor(
							Color4(
								static_cast<float>(fbxColor3.Get()[0] * fbxFactor.Get()),
								static_cast<float>(fbxColor3.Get()[1] * fbxFactor.Get()),
								static_cast<float>(fbxColor3.Get()[2] * fbxFactor.Get()),
								1.0f
							)
						);
					}
				} else {
					Console::println("FBXReader::processMeshNode(): unsupported material shading class: " + fbxMaterialName);
				}
				FbxProperty fbxProperty;
				fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
				string diffuseTextureFileName;
				{
					if (fbxProperty.GetSrcObjectCount<FbxLayeredTexture>() > 0) {
						auto texture = FbxCast<FbxFileTexture>(fbxProperty.GetSrcObject<FbxLayeredTexture>(0));
						diffuseTextureFileName = texture->GetFileName();
						Matrix2D3x3 textureMatrix;
						textureMatrix.identity();
						textureMatrix.multiply(Matrix2D3x3().identity().scale(Vector2(texture->GetScaleU(), texture->GetScaleV())));
						// TODO: not sure about texture rotation with 2D textures here and I have no test model for now
						textureMatrix.multiply(Matrix2D3x3::rotateAroundTextureCenter(texture->GetRotationU()));
						textureMatrix.multiply(Matrix2D3x3().identity().translate(Vector2(texture->GetTranslationU(), texture->GetTranslationV())));
						material->setTextureMatrix(textureMatrix);
					} else
					if (fbxProperty.GetSrcObjectCount<FbxTexture>() > 0) {
						auto texture = FbxCast<FbxFileTexture>(fbxProperty.GetSrcObject<FbxTexture>(0));
						diffuseTextureFileName = texture->GetFileName();
						Matrix2D3x3 textureMatrix;
						textureMatrix.identity();
						textureMatrix.multiply(Matrix2D3x3().identity().scale(Vector2(texture->GetScaleU(), texture->GetScaleV())));
						// TODO: not sure about texture rotation with 2D textures here and I have no test model for now
						textureMatrix.multiply(Matrix2D3x3::rotateAroundTextureCenter(texture->GetRotationU()));
						textureMatrix.multiply(Matrix2D3x3().identity().translate(Vector2(texture->GetTranslationU(), texture->GetTranslationV())));
						material->setTextureMatrix(textureMatrix);
					}
				}
				fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor);
				string diffuseTransparencyTextureFileName =
					fbxProperty.GetSrcObjectCount<FbxLayeredTexture>() == 0?
						(fbxProperty.GetSrcObjectCount<FbxTexture>() > 0?
							FbxCast<FbxFileTexture>(fbxProperty.GetSrcObject<FbxTexture>(0))->GetFileName():
							""
						):
						FbxCast<FbxFileTexture>(fbxProperty.GetSrcObject<FbxLayeredTexture>(0))->GetFileName();
				if (diffuseTransparencyTextureFileName.length() == 0) {
					fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
					diffuseTransparencyTextureFileName =
						fbxProperty.GetSrcObjectCount<FbxLayeredTexture>() == 0?
							(fbxProperty.GetSrcObjectCount<FbxTexture>() > 0?
								FbxCast<FbxFileTexture>(fbxProperty.GetSrcObject<FbxTexture>(0))->GetFileName():
								""
							):
						FbxCast<FbxFileTexture>(fbxProperty.GetSrcObject<FbxLayeredTexture>(0))->GetFileName();
				}
				if (diffuseTextureFileName.length() > 0) {
					specularMaterialProperties->setDiffuseTexture(
						FileSystem::getInstance()->fileExists(
							FileSystem::getInstance()->getCanonicalPath(pathName, FileSystem::getInstance()->getFileName(diffuseTextureFileName))
						)?pathName:FileSystem::getInstance()->getPathName(diffuseTextureFileName),
						FileSystem::getInstance()->getFileName(diffuseTextureFileName),
						FileSystem::getInstance()->fileExists(
							FileSystem::getInstance()->getCanonicalPath(pathName, FileSystem::getInstance()->getFileName(diffuseTransparencyTextureFileName))
						)?pathName:FileSystem::getInstance()->getPathName(diffuseTransparencyTextureFileName),
						FileSystem::getInstance()->getFileName(diffuseTransparencyTextureFileName)
					);
				}
				fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
				string normalTextureFileName =
					fbxProperty.GetSrcObjectCount<FbxLayeredTexture>() == 0?
						(fbxProperty.GetSrcObjectCount<FbxTexture>() > 0?
							FbxCast<FbxFileTexture>(fbxProperty.GetSrcObject<FbxTexture>(0))->GetFileName():
							""
						):
						FbxCast<FbxFileTexture>(fbxProperty.GetSrcObject<FbxLayeredTexture>(0))->GetFileName();
				if (normalTextureFileName.length() > 0) {
					specularMaterialProperties->setNormalTexture(
						FileSystem::getInstance()->fileExists(
							FileSystem::getInstance()->getCanonicalPath(pathName, FileSystem::getInstance()->getFileName(normalTextureFileName))
						)?pathName:FileSystem::getInstance()->getPathName(normalTextureFileName),
						FileSystem::getInstance()->getFileName(normalTextureFileName)
					);
				}
				fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
				string specularTextureFileName =
					fbxProperty.GetSrcObjectCount<FbxLayeredTexture>() == 0?
						(fbxProperty.GetSrcObjectCount<FbxTexture>() > 0?
							FbxCast<FbxFileTexture>(fbxProperty.GetSrcObject<FbxTexture>(0))->GetFileName():
							""
						):
						FbxCast<FbxFileTexture>(fbxProperty.GetSrcObject<FbxLayeredTexture>(0))->GetFileName();
				if (specularTextureFileName.length() > 0) {
					specularMaterialProperties->setSpecularTexture(
						FileSystem::getInstance()->fileExists(
							FileSystem::getInstance()->getCanonicalPath(pathName, FileSystem::getInstance()->getFileName(specularTextureFileName))
						)?pathName:FileSystem::getInstance()->getPathName(specularTextureFileName),
						FileSystem::getInstance()->getFileName(specularTextureFileName)
					);
				}
				// adjust ambient light with blender
				if (model->getAuthoringTool() == Model::AUTHORINGTOOL_BLENDER && specularMaterialProperties->getAmbientColor().equals(BLENDER_AMBIENT_NONE)) {
					specularMaterialProperties->setAmbientColor(
						Color4(
							specularMaterialProperties->getDiffuseColor().getRed() * BLENDER_AMBIENT_FROM_DIFFUSE_SCALE,
							specularMaterialProperties->getDiffuseColor().getGreen() * BLENDER_AMBIENT_FROM_DIFFUSE_SCALE,
							specularMaterialProperties->getDiffuseColor().getBlue() * BLENDER_AMBIENT_FROM_DIFFUSE_SCALE,
							1.0f
						)
					);
					specularMaterialProperties->setDiffuseColor(
						Color4(
							specularMaterialProperties->getDiffuseColor().getRed() * BLENDER_DIFFUSE_SCALE,
							specularMaterialProperties->getDiffuseColor().getGreen() * BLENDER_DIFFUSE_SCALE,
							specularMaterialProperties->getDiffuseColor().getBlue() * BLENDER_DIFFUSE_SCALE,
							specularMaterialProperties->getDiffuseColor().getAlpha()
						)
					);
				}
				material->setSpecularMaterialProperties(specularMaterialProperties);
				model->getMaterials()[material->getId()] = material;
			}
		}
		auto foundFacesEntity = false;
		string facesEntityName = "facesentity-" + material->getId();
		for (auto& facesEntityLookUp: facesEntities) {
			if (facesEntityLookUp.getId() == facesEntityName) {
				if (&facesEntityLookUp != facesEntity) {
					if (facesEntity != nullptr) {
						facesEntity->setFaces(faces);
					}
					faces = facesEntityLookUp.getFaces();
					facesEntity = &facesEntityLookUp;
				}
				foundFacesEntity = true;
				break;
			}
		}
		if (foundFacesEntity == false) {
			if (facesEntity != nullptr) {
				facesEntity->setFaces(faces);
				faces.clear();
			}
			facesEntities.push_back(FacesEntity(node, facesEntityName));
			facesEntity = &facesEntities[facesEntities.size() - 1];
			facesEntity->setMaterial(material);
		}
		auto fbxPolygonSize = fbxMesh->GetPolygonSize(i);
		if (fbxPolygonSize != 3) throw ModelFileIOException("we only support triangles in '" + node->getName() + "'");
		int controlPointIndicesIdx = 0;
		array<int, 3> controlPointIndices;
		int textureCoordinateIndicesIdx = 0;
		array<int, 3> textureCoordinateIndices;
		int normalIndicesIdx = 0;
		array<int, 3> normalIndices;
		int tangentIndicesIdx = 0;
		array<int, 3> tangentIndices;
		int binormalIndicesIdx = 0;
		array<int, 3> binormalIndices;
		for (auto j = 0; j < fbxPolygonSize; j++) {
			int fbxControlPointIndex = fbxMesh->GetPolygonVertex(i, j);
			controlPointIndices[controlPointIndicesIdx++] = fbxControlPointIndex;
			for (auto l = 0; l < fbxMesh->GetElementUVCount() && l < 1; ++l) {
				FbxGeometryElementUV* fbxUV = fbxMesh->GetElementUV(l);
				switch (fbxUV->GetMappingMode()) {
					case FbxGeometryElement::eByControlPoint:
						switch (fbxUV->GetReferenceMode()) {
							case FbxGeometryElement::eDirect:
								{
									textureCoordinateIndices[textureCoordinateIndicesIdx++] = fbxControlPointIndex;
									break;
								}
							case FbxGeometryElement::eIndexToDirect:
								{
									textureCoordinateIndices[textureCoordinateIndicesIdx++] = fbxUV->GetIndexArray().GetAt(fbxControlPointIndex);
									break;
								}
							default:
								break;
						}
						break;
					case FbxGeometryElement::eByPolygonVertex:
						{
							switch (fbxUV->GetReferenceMode()) {
								case FbxGeometryElement::eDirect:
								case FbxGeometryElement::eIndexToDirect:
									{
										textureCoordinateIndices[textureCoordinateIndicesIdx++] = fbxMesh->GetTextureUVIndex(i, j);
										break;
									}
								default:
									break;
							}
							break;
						}
						default:
							break;
				}
			}
			for (auto l = 0; l < fbxMesh->GetElementNormalCount() && l < 1; ++l) {
				FbxGeometryElementNormal* fbxNormal = fbxMesh->GetElementNormal(l);
				if (fbxNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
					switch (fbxNormal->GetReferenceMode()) {
						case FbxGeometryElement::eDirect:
							{
								normalIndices[normalIndicesIdx++] = fbxVertexId;
								break;
							}
						case FbxGeometryElement::eIndexToDirect:
							{
								normalIndices[normalIndicesIdx++] = fbxNormal->GetIndexArray().GetAt(fbxVertexId);
								break;
							}
						default:
							break;
					}
				} else {
				}
			}
			for (auto l = 0; l < fbxMesh->GetElementTangentCount() && l < 1; ++l) {
				FbxGeometryElementTangent* fbxTangent = fbxMesh->GetElementTangent(l);
				if (fbxTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
					switch (fbxTangent->GetReferenceMode()) {
						case FbxGeometryElement::eDirect:
							{
								tangentIndices[tangentIndicesIdx++] = fbxVertexId;
								break;
							}
						case FbxGeometryElement::eIndexToDirect:
							{
								tangentIndices[tangentIndicesIdx++] = fbxTangent->GetIndexArray().GetAt(fbxVertexId);
								break;
							}
						default:
							break;
					}
				}
			}
			for (auto l = 0; l < fbxMesh->GetElementBinormalCount() && l < 1; ++l) {
				FbxGeometryElementBinormal* fbxBinormal = fbxMesh->GetElementBinormal(l);
				if (fbxBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
					switch (fbxBinormal->GetReferenceMode()) {
						case FbxGeometryElement::eDirect:
							{
								binormalIndices[binormalIndicesIdx++] = fbxVertexId;
								break;
							}
						case FbxGeometryElement::eIndexToDirect:
							{
								binormalIndices[binormalIndicesIdx++] = fbxBinormal->GetIndexArray().GetAt(fbxVertexId);
								break;
							}
						default:
							break;
					}
				}
			}
			fbxVertexId++;
		}
		Face f(
			node,
			controlPointIndices[0],
			controlPointIndices[1],
			controlPointIndices[2],
			normalIndices[0],
			normalIndices[1],
			normalIndices[2]
		);
		if (textureCoordinateIndicesIdx == 3) {
			f.setTextureCoordinateIndices(
				textureCoordinateIndices[0],
				textureCoordinateIndices[1],
				textureCoordinateIndices[2]
			);
		}
		if (tangentIndicesIdx == 3 && binormalIndicesIdx == 3) {
			f.setTangentIndices(
				tangentIndices[0],
				tangentIndices[1],
				tangentIndices[2]
			);
			f.setBitangentIndices(
				binormalIndices[0],
				binormalIndices[1],
				binormalIndices[2]
			);
		}
		faces.push_back(f);
	}
	if (facesEntity != nullptr) {
		facesEntity->setFaces(faces);
	}

	node->setVertices(vertices);
	node->setNormals(normals);
	if (tangents.size() > 0 && bitangents.size() > 0) {
		node->setTangents(tangents);
		node->setBitangents(bitangents);
	}
	if (textureCoordinates.size() > 0) node->setTextureCoordinates(textureCoordinates);
	node->setFacesEntities(facesEntities);

	int fbxSkinCount = fbxNode->GetMesh()->GetDeformerCount(FbxDeformer::eSkin);
	if (fbxSkinCount == 0) {
		// no op
	} else
	if (fbxSkinCount == 1) {
		FbxSkin* fbxSkinDeformer = (FbxSkin*)fbxNode->GetMesh()->GetDeformer(0, FbxDeformer::eSkin);
		int fbxClusterCount = fbxSkinDeformer->GetClusterCount();
		auto skinning = new Skinning();
		vector<Joint> joints;
		vector<float> weights;
		map<int, vector<JointWeight>> jointWeightsByVertices;
		for (auto fbxClusterIndex = 0; fbxClusterIndex < fbxClusterCount; fbxClusterIndex++) {
			FbxCluster* fbxCluster = fbxSkinDeformer->GetCluster(fbxClusterIndex);
			if (fbxCluster->GetLink() == nullptr) continue;
			auto fbxJointName = fbxCluster->GetLink()->GetName();
			auto jointIndex = joints.size();
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			fbxCluster->GetTransformMatrix(transformMatrix);
			fbxCluster->GetTransformLinkMatrix(transformLinkMatrix);
			Matrix4x4 bindShapeMatrixArray;
			Joint joint(fbxJointName);
			Matrix4x4 bindMatrix;
			bindMatrix.set(
				Matrix4x4(
					transformMatrix.Get(0,0),
					transformMatrix.Get(0,1),
					transformMatrix.Get(0,2),
					transformMatrix.Get(0,3),
					transformMatrix.Get(1,0),
					transformMatrix.Get(1,1),
					transformMatrix.Get(1,2),
					transformMatrix.Get(1,3),
					transformMatrix.Get(2,0),
					transformMatrix.Get(2,1),
					transformMatrix.Get(2,2),
					transformMatrix.Get(2,3),
					transformMatrix.Get(3,0),
					transformMatrix.Get(3,1),
					transformMatrix.Get(3,2),
					transformMatrix.Get(3,3)
				)
			);
			bindMatrix.multiply(
				Matrix4x4(
					transformLinkMatrix.Get(0,0),
					transformLinkMatrix.Get(0,1),
					transformLinkMatrix.Get(0,2),
					transformLinkMatrix.Get(0,3),
					transformLinkMatrix.Get(1,0),
					transformLinkMatrix.Get(1,1),
					transformLinkMatrix.Get(1,2),
					transformLinkMatrix.Get(1,3),
					transformLinkMatrix.Get(2,0),
					transformLinkMatrix.Get(2,1),
					transformLinkMatrix.Get(2,2),
					transformLinkMatrix.Get(2,3),
					transformLinkMatrix.Get(3,0),
					transformLinkMatrix.Get(3,1),
					transformLinkMatrix.Get(3,2),
					transformLinkMatrix.Get(3,3)
				).invert()
			);
			joint.setBindMatrix(bindMatrix);
			joints.push_back(joint);
			auto fbxClusterControlPointIndexCount = fbxCluster->GetControlPointIndicesCount();
			auto fbxClusterControlPointIndices = fbxCluster->GetControlPointIndices();
			for (auto fbxClusterControlPointIndex = 0; fbxClusterControlPointIndex < fbxClusterControlPointIndexCount; fbxClusterControlPointIndex++) {
				int fbxControlPointIndex = fbxClusterControlPointIndices[fbxClusterControlPointIndex];
				auto weightIndex = weights.size();
				weights.push_back(fbxCluster->GetControlPointWeights()[fbxClusterControlPointIndex]);
				jointWeightsByVertices[fbxControlPointIndex].push_back(JointWeight(jointIndex, weightIndex));
			}
		}
		skinning->setJoints(joints);
		skinning->setWeights(weights);
		vector<vector<JointWeight>> verticesJointsWeights;
		for (auto vertexIndex = 0; vertexIndex < vertices.size(); vertexIndex++) {
			verticesJointsWeights.push_back(vector<JointWeight>());
			auto jointWeightsByVerticesIt = jointWeightsByVertices.find(vertexIndex);
			if (jointWeightsByVerticesIt != jointWeightsByVertices.end()) {
				for (auto& jointWeight: jointWeightsByVerticesIt->second) {
					verticesJointsWeights[verticesJointsWeights.size() - 1].push_back(jointWeight);
				}
			}
		}
		skinning->setVerticesJointsWeights(verticesJointsWeights);
		node->setSkinning(skinning);
	} else {
		Console::println("FBXReader::processMeshNode(): " + to_string(fbxSkinCount) + " skins per mesh: Not supported");
	}

	return node;
}

Node* FBXReader::processSkeletonNode(FbxNode* fbxNode, Model* model, Node* parentNode, const string& pathName) {
	auto fbxNodeName = fbxNode->GetName();
	return new Node(model, parentNode, fbxNodeName, fbxNodeName);
}

void FBXReader::processAnimation(FbxNode* fbxNode, const FbxTime& fbxStartFrame, const FbxTime& fbxEndFrame, Model* model, int frameOffset) {
	auto fbxNodeName = fbxNode->GetName();
	auto node = model->getNodeById(fbxNodeName);
	auto animation = node->getAnimation();
	if (node->getAnimation() == nullptr) {
		animation = new Animation();
		node->setAnimation(animation);
	}
	auto transformationsMatrices = node->getAnimation()->getTransformationsMatrices();
	transformationsMatrices.resize(model->getAnimationSetup(Model::ANIMATIONSETUP_DEFAULT)->getFrames());
	FbxTime fbxFrameTime;
	fbxFrameTime.SetMilliSeconds(1000.0f * 1.0f / 30.0f);
	for(auto i = fbxStartFrame; i < fbxEndFrame; i+= fbxFrameTime) {
		FbxAMatrix& fbxTransformationMatrix = fbxNode->EvaluateLocalTransform(i);
		int frameIdx = frameOffset + (int)Math::ceil((i.GetMilliSeconds() - fbxStartFrame.GetMilliSeconds()) / (1000.0f * 1.0f / 30.0f));
		transformationsMatrices[frameIdx].set(
			fbxTransformationMatrix.Get(0,0),
			fbxTransformationMatrix.Get(0,1),
			fbxTransformationMatrix.Get(0,2),
			fbxTransformationMatrix.Get(0,3),
			fbxTransformationMatrix.Get(1,0),
			fbxTransformationMatrix.Get(1,1),
			fbxTransformationMatrix.Get(1,2),
			fbxTransformationMatrix.Get(1,3),
			fbxTransformationMatrix.Get(2,0),
			fbxTransformationMatrix.Get(2,1),
			fbxTransformationMatrix.Get(2,2),
			fbxTransformationMatrix.Get(2,3),
			fbxTransformationMatrix.Get(3,0),
			fbxTransformationMatrix.Get(3,1),
			fbxTransformationMatrix.Get(3,2),
			fbxTransformationMatrix.Get(3,3)
		);
	}
	node->getAnimation()->setTransformationsMatrices(transformationsMatrices);
	for(auto i = 0; i < fbxNode->GetChildCount(); i++) {
		processAnimation(fbxNode->GetChild(i), fbxStartFrame, fbxEndFrame, model, frameOffset);
	}
}
