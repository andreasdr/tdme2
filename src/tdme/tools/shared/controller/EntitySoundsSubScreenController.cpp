#include <tdme/tools/shared/controller/EntitySoundsSubScreenController.h>

#include <string>

#include <tdme/engine/model/Model.h>
#include <tdme/engine/model/AnimationSetup.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/tools/shared/controller/FileDialogPath.h>
#include <tdme/tools/shared/controller/FileDialogScreenController.h>
#include <tdme/tools/shared/controller/InfoDialogScreenController.h>
#include <tdme/tools/shared/model/LevelEditorEntity.h>
#include <tdme/tools/shared/model/LevelEditorEntityAudio.h>
#include <tdme/tools/shared/tools/Tools.h>
#include <tdme/tools/shared/views/EntitySoundsView.h>
#include <tdme/tools/shared/views/SharedModelEditorView.h>
#include <tdme/tools/shared/views/PlayableSoundView.h>
#include <tdme/tools/shared/views/PopUps.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ExceptionBase.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTools.h>

using std::string;
using std::to_string;

using tdme::tools::shared::controller::EntitySoundsSubScreenController;

using tdme::engine::model::Model;
using tdme::engine::model::AnimationSetup;
using tdme::gui::GUIParser;
using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIScreenNode;
using tdme::tools::shared::controller::FileDialogPath;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::tools::shared::controller::InfoDialogScreenController;
using tdme::tools::shared::model::LevelEditorEntity;
using tdme::tools::shared::model::LevelEditorEntityAudio;
using tdme::tools::shared::tools::Tools;
using tdme::tools::shared::views::EntitySoundsView;
using tdme::tools::shared::views::PlayableSoundView;
using tdme::tools::shared::views::PopUps;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ExceptionBase;
using tdme::utilities::Integer;
using tdme::utilities::MutableString;
using tdme::utilities::StringTools;

EntitySoundsSubScreenController::EntitySoundsSubScreenController(PlayableSoundView* playableSoundView, PopUps* popUps, FileDialogPath* audioPath)
{
	this->audioPath = audioPath;
	this->playableSoundView = playableSoundView;
	this->view = new EntitySoundsView(this, popUps);
}

EntitySoundsSubScreenController::~EntitySoundsSubScreenController() {
	delete view;
}

EntitySoundsView* EntitySoundsSubScreenController::getView()
{
	return view;
}

GUIScreenNode* EntitySoundsSubScreenController::getScreenNode() {
	return screenNode;
}

void EntitySoundsSubScreenController::initialize(GUIScreenNode* screenNode)
{
	this->screenNode = screenNode;
	try {
		for (auto i = 0; i < LevelEditorEntity::MODEL_SOUNDS_COUNT; i++) {
			soundsSoundAnimationDropDown[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_animation_" + to_string(i)));
			soundsSoundKey[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_key_" + to_string(i)));
			soundsSoundFile[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_file_" + to_string(i)));
			soundsSoundLoad[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_load_" + to_string(i)));
			soundsSoundClear[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_clear_" + to_string(i)));
			soundsSoundGain[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_gain_" + to_string(i)));
			soundsSoundPitch[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_pitch_" + to_string(i)));
			soundsSoundOffset[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_offset_" + to_string(i)));
			soundsSoundLooping[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_looping_" + to_string(i)));
			soundsSoundFixed[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_fixed_" + to_string(i)));
			soundsSoundApply[i] = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("sounds_sound_apply_" + to_string(i)));
		}
	} catch (Exception& exception) {
		Console::print(string("EntitySoundsSubScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void EntitySoundsSubScreenController::setSoundAnimationDropDown(int soundIdx, Model* model) {
	auto animationsDropDownInnerNode = dynamic_cast< GUIParentNode* >((soundsSoundAnimationDropDown[soundIdx]->getScreenNode()->getNodeById(soundsSoundAnimationDropDown[soundIdx]->getId() + "_inner")));
	auto idx = 0;
	string animationsDropDownInnerNodeSubNodesXML = "";
	animationsDropDownInnerNodeSubNodesXML =
		animationsDropDownInnerNodeSubNodesXML +
		"<scrollarea-vertical id=\"" +
		soundsSoundAnimationDropDown[soundIdx]->getId() +
		"_inner_scrollarea\" width=\"100%\" height=\"100\">\n";
	animationsDropDownInnerNodeSubNodesXML = animationsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"None\" value=\"\" selected=\"true\" />";
	if (model != nullptr) {
		for (auto it: model->getAnimationSetups()) {
			auto animationSetupId = it.second->getId();
			animationsDropDownInnerNodeSubNodesXML =
				animationsDropDownInnerNodeSubNodesXML + "<dropdown-option text=\"" +
				GUIParser::escapeQuotes(animationSetupId) +
				"\" value=\"" +
				GUIParser::escapeQuotes(animationSetupId) +
				"\" />\n";
			idx++;
		}
	}
	animationsDropDownInnerNodeSubNodesXML = animationsDropDownInnerNodeSubNodesXML + "</scrollarea-vertical>";
	try {
		animationsDropDownInnerNode->replaceSubNodes(animationsDropDownInnerNodeSubNodesXML, true);
	} catch (Exception& exception) {
		Console::print(string("void EntitySoundsSubScreenController::setSounds(): An error occurred: "));
		Console::println(string(exception.what()));
	}
	// TODO: this usually works most of the time out of the box, so custom layouting is not required, but in this case not, need to find out whats going wrong there
	// soundsSoundAnimation[i]->getScreenNode()->layout(soundsSoundAnimation[i]);

}

void EntitySoundsSubScreenController::unsetSound(int soundIdx) {
	setSoundAnimationDropDown(soundIdx, nullptr);
	soundsSoundKey[soundIdx]->getController()->setValue(MutableString(""));
	soundsSoundKey[soundIdx]->getController()->setDisabled(true);
	soundsSoundAnimationDropDown[soundIdx]->getController()->setValue(MutableString(""));
	soundsSoundAnimationDropDown[soundIdx]->getController()->setDisabled(true);
	soundsSoundFile[soundIdx]->getController()->setValue(MutableString(""));
	soundsSoundFile[soundIdx]->getController()->setDisabled(true);
	soundsSoundLoad[soundIdx]->getController()->setDisabled(true);
	soundsSoundClear[soundIdx]->getController()->setDisabled(true);
	soundsSoundGain[soundIdx]->getController()->setValue(MutableString(""));
	soundsSoundGain[soundIdx]->getController()->setDisabled(true);
	soundsSoundPitch[soundIdx]->getController()->setValue(MutableString(""));
	soundsSoundPitch[soundIdx]->getController()->setDisabled(true);
	soundsSoundOffset[soundIdx]->getController()->setValue(MutableString(""));
	soundsSoundOffset[soundIdx]->getController()->setDisabled(true);
	soundsSoundLooping[soundIdx]->getController()->setValue(MutableString(""));
	soundsSoundLooping[soundIdx]->getController()->setDisabled(true);
	soundsSoundFixed[soundIdx]->getController()->setValue(MutableString(""));
	soundsSoundFixed[soundIdx]->getController()->setDisabled(true);
	soundsSoundApply[soundIdx]->getController()->setDisabled(true);
}

void EntitySoundsSubScreenController::unsetSounds() {
	for (auto i = 0; i < LevelEditorEntity::MODEL_SOUNDS_COUNT; i++) {
		unsetSound(i);
	}
}

void EntitySoundsSubScreenController::setSounds(LevelEditorEntity* entity) {
	auto i = 0;
	auto& sounds = entity->getSounds();
	for (auto sound: sounds) {
		setSoundAnimationDropDown(i, entity->getModel());
		soundsSoundKey[i]->getController()->setValue(MutableString(sound->getId()));
		soundsSoundKey[i]->getController()->setDisabled(false);
		soundsSoundAnimationDropDown[i]->getController()->setValue(MutableString(sound->getAnimation()));
		soundsSoundAnimationDropDown[i]->getController()->setDisabled(false);
		soundsSoundFile[i]->getController()->setValue(MutableString(sound->getFileName()));
		soundsSoundFile[i]->getController()->setDisabled(false);
		soundsSoundLoad[i]->getController()->setDisabled(false);
		soundsSoundClear[i]->getController()->setDisabled(false);
		soundsSoundGain[i]->getController()->setValue(MutableString(sound->getGain()));
		soundsSoundGain[i]->getController()->setDisabled(false);
		soundsSoundPitch[i]->getController()->setValue(MutableString(sound->getPitch(), 4));
		soundsSoundPitch[i]->getController()->setDisabled(false);
		soundsSoundOffset[i]->getController()->setValue(MutableString(sound->getOffset()));
		soundsSoundOffset[i]->getController()->setDisabled(false);
		soundsSoundLooping[i]->getController()->setValue(sound->isLooping() == true?MutableString("1"):MutableString());
		soundsSoundLooping[i]->getController()->setDisabled(false);
		soundsSoundFixed[i]->getController()->setValue(sound->isFixed() == true?MutableString("1"):MutableString());
		soundsSoundFixed[i]->getController()->setDisabled(false);
		soundsSoundApply[i]->getController()->setDisabled(false);
		i++;
	}
	for (; i < sounds.size() + 1 && i < LevelEditorEntity::MODEL_SOUNDS_COUNT; i++) {
		setSoundAnimationDropDown(i, entity->getModel());
		soundsSoundKey[i]->getController()->setValue(MutableString(""));
		soundsSoundKey[i]->getController()->setDisabled(false);
		soundsSoundAnimationDropDown[i]->getController()->setValue(MutableString(""));
		soundsSoundAnimationDropDown[i]->getController()->setDisabled(false);
		soundsSoundFile[i]->getController()->setValue(MutableString(""));
		soundsSoundFile[i]->getController()->setDisabled(false);
		soundsSoundLoad[i]->getController()->setDisabled(false);
		soundsSoundClear[i]->getController()->setDisabled(false);
		soundsSoundGain[i]->getController()->setValue(MutableString("1.0"));
		soundsSoundGain[i]->getController()->setDisabled(false);
		soundsSoundPitch[i]->getController()->setValue(MutableString("1.0"));
		soundsSoundPitch[i]->getController()->setDisabled(false);
		soundsSoundOffset[i]->getController()->setValue(MutableString("0"));
		soundsSoundOffset[i]->getController()->setDisabled(false);
		soundsSoundLooping[i]->getController()->setValue(MutableString(""));
		soundsSoundLooping[i]->getController()->setDisabled(false);
		soundsSoundFixed[i]->getController()->setValue(MutableString(""));
		soundsSoundFixed[i]->getController()->setDisabled(false);
		soundsSoundApply[i]->getController()->setDisabled(false);
	}
	for (; i < LevelEditorEntity::MODEL_SOUNDS_COUNT; i++) {
		unsetSound(i);
	}

}

void EntitySoundsSubScreenController::onSoundClear(int soundIdx) {
	soundsSoundFile[soundIdx]->getController()->setValue(MutableString(""));
}

void EntitySoundsSubScreenController::onSoundLoad(int soundIdx) {
	class LoadSoundAction: public virtual Action
	{
	public:
		LoadSoundAction(EntitySoundsSubScreenController* entitySoundsSubScreenController, int soundIdx): entitySoundsSubScreenController(entitySoundsSubScreenController), soundIdx(soundIdx) {
		}
		void performAction() override {
			entitySoundsSubScreenController->soundsSoundFile[soundIdx]->getController()->setValue(
				entitySoundsSubScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName() +
				"/" +
				entitySoundsSubScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getFileName()
			);
			entitySoundsSubScreenController->audioPath->setPath(entitySoundsSubScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->getPathName());
			entitySoundsSubScreenController->getView()->getPopUpsViews()->getFileDialogScreenController()->close();
		}
	private:
		EntitySoundsSubScreenController* entitySoundsSubScreenController;
		int soundIdx;
	};

	vector<string> extensions = {{"ogg"}};
	view->getPopUpsViews()->getFileDialogScreenController()->show(
		soundsSoundFile[soundIdx]->getController()->getValue().getString().length() > 0?Tools::getPath(soundsSoundFile[soundIdx]->getController()->getValue().getString()):audioPath->getPath(),
		"Load from: ",
		extensions,
		soundsSoundFile[soundIdx]->getController()->getValue().getString().length() > 0?Tools::getFileName(soundsSoundFile[soundIdx]->getController()->getValue().getString()):"",
		true,
		new LoadSoundAction(this, soundIdx)
	);
}


void EntitySoundsSubScreenController::onSoundApply(int soundIdx, LevelEditorEntity* entity) {
	try {
		LevelEditorEntityAudio* sound = nullptr;
		if (soundIdx == entity->getSounds().size()) {
			sound = entity->addSound(soundsSoundKey[soundIdx]->getController()->getValue().getString());
		} else {
			sound = entity->getSound(entity->getSounds()[soundIdx]->getId());
		}
		if (sound == nullptr) throw ExceptionBase("Sound could not be loaded or created");
		if (sound->getId() != soundsSoundKey[soundIdx]->getController()->getValue().getString()) {
			if (entity->renameSound(sound->getId(), soundsSoundKey[soundIdx]->getController()->getValue().getString()) == false) {
				throw ExceptionBase("Key could not be renamed");
			}
		}
		sound->setId(soundsSoundKey[soundIdx]->getController()->getValue().getString());
		sound->setAnimation(soundsSoundAnimationDropDown[soundIdx]->getController()->getValue().getString());
		sound->setFileName(soundsSoundFile[soundIdx]->getController()->getValue().getString());
		sound->setGain(Float::parseFloat(soundsSoundGain[soundIdx]->getController()->getValue().getString()));
		sound->setPitch(Float::parseFloat(soundsSoundPitch[soundIdx]->getController()->getValue().getString()));
		sound->setOffset(Integer::parseInt(soundsSoundOffset[soundIdx]->getController()->getValue().getString()));
		sound->setLooping(soundsSoundLooping[soundIdx]->getController()->getValue().getString() == "1");
		sound->setFixed(soundsSoundFixed[soundIdx]->getController()->getValue().getString() == "1");
		playableSoundView->playSound(sound->getId());
	} catch (Exception& exception) {
		showErrorPopUp("Warning", (string(exception.what())));
	}
}

void EntitySoundsSubScreenController::showErrorPopUp(const string& caption, const string& message)
{
	view->getPopUpsViews()->getInfoDialogScreenController()->show(caption, message);
}

void EntitySoundsSubScreenController::onValueChanged(GUIElementNode* node, LevelEditorEntity* entity) {
}

void EntitySoundsSubScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node, LevelEditorEntity* entity)
{
	if (type != GUIActionListenerType::PERFORMED) return;
	if (StringTools::startsWith(node->getId(), "sounds_sound_apply_") == true) {
		auto soundIdx = Integer::parseInt(StringTools::substring(node->getId(), string("sounds_sound_apply_").size()));
		onSoundApply(soundIdx, entity);
		setSounds(entity);
	} else
	if (StringTools::startsWith(node->getId(), "sounds_sound_clear_") == true) {
		auto soundIdx = Integer::parseInt(StringTools::substring(node->getId(), string("sounds_sound_clear_").size()));
		onSoundClear(soundIdx);
	} else
	if (StringTools::startsWith(node->getId(), "sounds_sound_load_") == true) {
		auto soundIdx = Integer::parseInt(StringTools::substring(node->getId(), string("sounds_sound_load_").size()));
		onSoundLoad(soundIdx);
	}
}
