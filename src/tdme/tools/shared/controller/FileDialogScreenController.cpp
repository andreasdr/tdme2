#include <tdme/tools/shared/controller/FileDialogScreenController.h>

#include <string>
#include <vector>


#include <tdme/gui/events/Action.h>
#include <tdme/gui/events/GUIActionListener.h>
#include <tdme/gui/events/GUIChangeListener.h>
#include <tdme/gui/nodes/GUIElementNode.h>
#include <tdme/gui/nodes/GUINode.h>
#include <tdme/gui/nodes/GUINodeController.h>
#include <tdme/gui/nodes/GUIParentNode.h>
#include <tdme/gui/nodes/GUIScreenNode.h>
#include <tdme/gui/nodes/GUITextNode.h>
#include <tdme/gui/GUIParser.h>
#include <tdme/os/filesystem/FileNameFilter.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/MutableString.h>
#include <tdme/utilities/StringTools.h>

using std::string;
using std::vector;

using tdme::gui::events::Action;
using tdme::gui::events::GUIActionListenerType;
using tdme::gui::nodes::GUIElementNode;
using tdme::gui::nodes::GUINode;
using tdme::gui::nodes::GUINodeController;
using tdme::gui::nodes::GUIParentNode;
using tdme::gui::nodes::GUIScreenNode;
using tdme::gui::nodes::GUITextNode;
using tdme::gui::GUIParser;
using tdme::os::filesystem::FileNameFilter;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::tools::shared::controller::FileDialogScreenController;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::MutableString;
using tdme::utilities::StringTools;

FileDialogScreenController::FileDialogScreenController()
{
	this->cwd = FileSystem::getStandardFileSystem()->getCurrentWorkingPathName();
	this->applyAction = nullptr;
	this->enableFilter = false;
	this->filtered = false;
}

FileDialogScreenController::~FileDialogScreenController() {
	if (applyAction != nullptr) delete applyAction;
}

GUIScreenNode* FileDialogScreenController::getScreenNode()
{
	return screenNode;
}

const string& FileDialogScreenController::getPathName()
{
	return cwd;
}

const string FileDialogScreenController::getFileName()
{
	return fileName->getController()->getValue().getString();
}

void FileDialogScreenController::initialize()
{
	try {
		screenNode = GUIParser::parse("resources/engine/tools/shared/gui", "filedialog.xml");
		screenNode->setVisible(false);
		screenNode->addActionListener(this);
		screenNode->addChangeListener(this);
		screenNode->addFocusListener(this);
		caption = dynamic_cast< GUITextNode* >(screenNode->getNodeById("filedialog_caption"));
		files = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("filedialog_files"));
		fileName = dynamic_cast< GUIElementNode* >(screenNode->getNodeById("filedialog_filename"));
	} catch (Exception& exception) {
		Console::print(string("FileDialogScreenController::initialize(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void FileDialogScreenController::dispose()
{
	screenNode = nullptr;
}

bool FileDialogScreenController::setupFileDialogListBox()
{
	class ExtensionFilter: public virtual FileNameFilter
	{
	public:
		bool accept(const string& pathName, const string& fileName) override {
			if (FileSystem::getStandardFileSystem()->isPath(pathName + "/" + fileName) == true) return true;
			for (auto& extension : fileDialogScreenController->extensions) {
				if (StringTools::endsWith(StringTools::toLowerCase(fileName), "." + extension) == true) return true;

			}
			return false;
		}

		/**
		 * Public constructor
		 * @param fileDialogScreenController file dialog screen controller
		 */
		ExtensionFilter(FileDialogScreenController* fileDialogScreenController): fileDialogScreenController(fileDialogScreenController) {
		}

	private:
		FileDialogScreenController* fileDialogScreenController;
	};

	auto success = true;
	auto directory = cwd;
	if (directory.length() > 50) {
		directory = "..." + StringTools::substring(directory, directory.length() - 50 + 3);
	}

	caption->setText(MutableString(captionText).append(directory));

	fileList.clear();
	try {
		auto directory = cwd;
		ExtensionFilter extensionsFilter(this);
		FileSystem::getStandardFileSystem()->list(directory, fileList, &extensionsFilter, FileSystem::getStandardFileSystem()->isDrive(directory));
	} catch (Exception& exception) {
		Console::print(string("FileDialogScreenController::setupFileDialogListBox(): An error occurred: "));
		Console::println(string(exception.what()));
		success = false;
	}
	setupFileDialogListBoxFiles(fileList);
	if (enableFilter == true) fileName->getController()->setValue(MutableString("Type a string to filter the list..."));

	//
	filtered = false;

	//
	return success;
}

void FileDialogScreenController::setupFileDialogListBoxFiles(const vector<string>& fileList, const string& selectedFile) {
	auto filesInnerNode = dynamic_cast< GUIParentNode* >(files->getScreenNode()->getNodeById(files->getId() + "_inner"));
	auto idx = 1;
	string filesInnerNodeSubNodesXML = "";
	filesInnerNodeSubNodesXML =
		filesInnerNodeSubNodesXML +
		"<scrollarea width=\"100%\" height=\"100%\">\n";
	for (auto& file : fileList) {
		filesInnerNodeSubNodesXML =
			filesInnerNodeSubNodesXML +
			"<selectbox-option text=\"" +
			GUIParser::escapeQuotes(file) +
			"\" value=\"" +
			GUIParser::escapeQuotes(file) +
			"\"" +
			(selectedFile == file?" selected=\"true\"":"") +
			"/>\n";
	}
	filesInnerNodeSubNodesXML =
		filesInnerNodeSubNodesXML + "</scrollarea>\n";
	try {
		filesInnerNode->replaceSubNodes(filesInnerNodeSubNodesXML, false);
	} catch (Exception& exception) {
		Console::print(string("FileDialogScreenController::setupFileDialogListBox(): An error occurred: "));
		Console::println(string(exception.what()));
	}
}

void FileDialogScreenController::show(const string& cwd, const string& captionText, const vector<string>& extensions, const string& fileName, bool enableFilter, Action* applyAction)
{
	try {
		this->cwd = FileSystem::getStandardFileSystem()->getCanonicalPath(cwd, "");
		if (FileSystem::getStandardFileSystem()->isPath(this->cwd) == false) {
			this->cwd = FileSystem::getStandardFileSystem()->getCurrentWorkingPathName();
		}
	} catch (Exception& exception) {
		Console::print(string("FileDialogScreenController::show(): An error occurred: "));
		Console::print(string(exception.what()));
		Console::println(": using cwd!");
		this->cwd = FileSystem::getStandardFileSystem()->getCurrentWorkingPathName();
	}
	this->captionText = captionText;
	this->extensions = extensions;
	this->fileName->getController()->setValue(fileName);
	this->enableFilter = enableFilter;
	setupFileDialogListBox();
	screenNode->setVisible(true);
	if (this->applyAction != nullptr) delete this->applyAction;
	this->applyAction = applyAction;
}

void FileDialogScreenController::close()
{
	screenNode->setVisible(false);
}

void FileDialogScreenController::onValueChanged(GUIElementNode* node)
{
	if (node->getId() == "filedialog_files") {
		try {
			auto selectedFile = node->getController()->getValue().getString();
			if (FileSystem::getStandardFileSystem()->isDrive(selectedFile) == true) {
				auto lastCwd = cwd;
				cwd = selectedFile;
				if (setupFileDialogListBox() == false) {
					cwd = lastCwd;
					setupFileDialogListBox();
				}
			} else
			if (FileSystem::getStandardFileSystem()->isPath(cwd + "/" + selectedFile) == true) {
				auto lastCwd = cwd;
				try {
					cwd = FileSystem::getStandardFileSystem()->getCanonicalPath(cwd, selectedFile);
				} catch (Exception& exception) {
					Console::print(string("FileDialogScreenController::onValueChanged(): An error occurred: "));
					Console::println(string(exception.what()));
				}
				if (setupFileDialogListBox() == false) {
					cwd = lastCwd;
					setupFileDialogListBox();
				}
			} else {
				if (filtered == true) {
					setupFileDialogListBoxFiles(fileList, selectedFile);
					filtered = false;
				}
				fileName->getController()->setValue(selectedFile);
			}
		} catch (Exception& exception) {
			Console::print(string("FileDialogScreenController::onValueChanged(): An error occurred: "));
			Console::println(string(exception.what()));
			fileName->getController()->setValue(MutableString());
		}
	} else
	if (node->getId() == "filedialog_filename") {
		try {
			if (enableFilter == true) {
				auto filterString = StringTools::toLowerCase(node->getController()->getValue().getString());
				if (FileSystem::getStandardFileSystem()->fileExists(cwd + "/" + filterString) == true) {
					auto selectedFile = node->getController()->getValue().getString();
					setupFileDialogListBoxFiles(fileList, selectedFile);
				} else {
					vector<string> fileListFiltered;
					for (auto file: fileList) {
						if (StringTools::toLowerCase(file).find(filterString) != -1) fileListFiltered.push_back(file);
					}
					setupFileDialogListBoxFiles(fileListFiltered);
					filtered = true;
				}
			}
		} catch (Exception& exception) {
			Console::print(string("FileDialogScreenController::onValueChanged(): An error occurred: "));
			Console::println(string(exception.what()));
			fileName->getController()->setValue(MutableString());
		}
	}
}

void FileDialogScreenController::onActionPerformed(GUIActionListenerType type, GUIElementNode* node)
{
	if (type == GUIActionListenerType::PERFORMED) {
		if (node->getId() == "filedialog_apply") {
			if (applyAction != nullptr) applyAction->performAction();
		} else
		if (node->getId() == "filedialog_abort") {
			close();
		}
	}
}

void FileDialogScreenController::onFocus(GUIElementNode* node) {
	if (node->getId() == "filedialog_filename") {
		if (enableFilter == true) node->getController()->setValue(MutableString(""));
	}
}

void FileDialogScreenController::onUnfocus(GUIElementNode* node) {
}

