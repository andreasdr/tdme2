#include <tdme/tools/installer/Installer.h>

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <agui/agui.h>
#include <agui/gui/effects/GUIColorEffect.h>
#include <agui/gui/effects/GUIPositionEffect.h>
#include <agui/gui/events/GUIActionListener.h>
#include <agui/gui/events/GUIChangeListener.h>
#include <agui/gui/nodes/GUIColor.h>
#include <agui/gui/nodes/GUIElementNode.h>
#include <agui/gui/nodes/GUINodeController.h>
#include <agui/gui/nodes/GUIParentNode.h>
#include <agui/gui/nodes/GUIScreenNode.h>
#include <agui/gui/nodes/GUIStyledTextNode.h>
#include <agui/gui/nodes/GUITextNode.h>
#include <agui/gui/GUI.h>
#include <agui/gui/GUIParser.h>
#include <agui/utilities/MutableString.h>

#include <tdme/tdme.h>
#include <tdme/application/Application.h>
#include <tdme/engine/tools/FileSystemTools.h>
#include <tdme/engine/Color4.h>
#include <tdme/engine/Engine.h>
#include <tdme/engine/Version.h>
#include <tdme/os/filesystem/ArchiveFileSystem.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/os/threading/Thread.h>
#include <tdme/tools/editor/controllers/FileDialogScreenController.h>
#include <tdme/tools/editor/controllers/InfoDialogScreenController.h>
#include <tdme/tools/editor/misc/PopUps.h>
#include <tdme/tools/editor/misc/Tools.h>
#include <tdme/utilities/Action.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>
#include <tdme/utilities/ExceptionBase.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/Properties.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>
#include <tdme/utilities/Time.h>

#include <yannet/network/httpclient/HTTPClient.h>
#include <yannet/network/httpclient/HTTPDownloadClient.h>

using tdme::tools::installer::Installer;

using std::distance;
using std::make_unique;
using std::reverse;
using std::sort;
using std::string;
using std::to_string;
using std::unique;
using std::unique_ptr;
using std::unordered_map;
using std::unordered_set;
using std::vector;

using agui::gui::effects::GUIColorEffect;
using agui::gui::effects::GUIPositionEffect;
using agui::gui::events::GUIActionListener;
using agui::gui::events::GUIActionListenerType;
using agui::gui::events::GUIChangeListener;
using agui::gui::nodes::GUIColor;
using agui::gui::nodes::GUIElementNode;
using agui::gui::nodes::GUINodeController;
using agui::gui::nodes::GUIParentNode;
using agui::gui::nodes::GUIScreenNode;
using agui::gui::nodes::GUIStyledTextNode;
using agui::gui::nodes::GUITextNode;
using agui::gui::GUI;
using agui::gui::GUIParser;
using agui::utilities::MutableString;

using tdme::application::Application;
using tdme::engine::tools::FileSystemTools;
using tdme::engine::Color4;
using tdme::engine::Engine;
using tdme::engine::Version;
using tdme::os::filesystem::ArchiveFileSystem;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::os::threading::Thread;
using tdme::tools::editor::controllers::FileDialogScreenController;
using tdme::tools::editor::controllers::InfoDialogScreenController;
using tdme::tools::editor::misc::PopUps;
using tdme::tools::editor::misc::Tools;
using tdme::utilities::Action;
using tdme::utilities::Console;
using tdme::utilities::Exception;
using tdme::utilities::ExceptionBase;
using tdme::utilities::Integer;
using tdme::utilities::Properties;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;
using tdme::utilities::Time;

using yannet::network::httpclient::HTTPClient;
using yannet::network::httpclient::HTTPDownloadClient;

Installer::Installer(): installThreadMutex("install-thread-mutex")
{
	Application::setLimitFPS(true);
	FileSystemTools::loadSettings(this);
	this->engine = Engine::getInstance();
	this->popUps = make_unique<PopUps>();
	installerMode = INSTALLERMODE_NONE;
	screen = SCREEN_WELCOME;
	installed = false;
	remountInstallerArchive = false;
}

void Installer::initializeScreens() {
	try {
		vector<string> installedComponents;
		try {
			FileSystem::getStandardFileSystem()->getContentAsStringArray(".", "install.components.db", installedComponents);
		} catch (Exception& exception) {
		}

		if (timestamp.empty() == true) {
			try {
				timestamp = FileSystem::getStandardFileSystem()->getContentAsString(".", "install.version.db");
			} catch (Exception& exception) {
			}
		}

		string installPath;
		try {
			vector<string> log;
			FileSystem::getStandardFileSystem()->getContentAsStringArray(".", "install.files.db", log);
			if (log.size() > 0) installPath = log[0];
		} catch (Exception& exception) {
		}

		//
		popUps->dispose();
		engine->getGUI()->reset();
		popUps->initialize();

		installerProperties.load("resources/installer", "installer.properties");
		if (installerProperties.get("installer_version", "") != "1.9.176") throw ExceptionBase("Installer is outdated. Please uninstall and update installer");
		unordered_map<string, string> variables = {
			{"name", installerProperties.get("name", "TDME2 based application")},
			{"diskspace", installerProperties.get("diskspace", "Unknown")},
			{"installfolder", installPath.empty() == true?homePath + "/Applications/" + installerProperties.get("install_path", "TDME2-based-application"):installPath}
		};
		engine->getGUI()->addScreen(
			"installer_welcome",
			GUIParser::parse(
				"resources/installer",
				"installer_welcome.xml",
				variables
			)
		);
		engine->getGUI()->addScreen(
			"installer_license",
			GUIParser::parse(
				"resources/installer",
				"installer_license.xml",
				variables
			)
		);
		dynamic_cast<GUIStyledTextNode*>(engine->getGUI()->getScreen("installer_license")->getNodeById("licence_text"))->setText(MutableString(FileSystem::getInstance()->getContentAsString(".", "LICENSE")));
		engine->getGUI()->addScreen(
			"installer_components",
			GUIParser::parse(
				"resources/installer",
				"installer_components.xml",
				variables
			)
		);
		string componentsXML = "<space height=\"10\" />\n";
		for (auto componentIdx = 1; true; componentIdx++) {
			auto componentName = installerProperties.get("component" + to_string(componentIdx), "");
			auto componentRequired = StringTools::trim(StringTools::toLowerCase(installerProperties.get("component" + to_string(componentIdx) + "_required", "false"))) == "true";
			auto componentInstalled = false;
			for (const auto& installedComponentName: installedComponents) {
				if (installedComponentName == componentName) {
					componentInstalled = true;
					break;
				}
			}
			if (componentName.empty() == true) break;
			componentsXML+=
				string("<element id=\"component" + to_string(componentIdx) + "\" width=\"100%\" height=\"25\">\n") +
				string("	<layout width=\"100%\" alignment=\"horizontal\">\n") +
				string("		<space width=\"10\" />\n") +
				string("		<checkbox id=\"checkbox_component" + to_string(componentIdx) + "\" name=\"checkbox_component" + to_string(componentIdx) + "\" value=\"1\" selected=\"" + (componentRequired == true || componentInstalled == true?"true":"false") + "\" disabled=\"" + (componentRequired == true?"true":"false") + "\" />\n") +
				string("		<space width=\"10\" />\n") +
				string("		<text width=\"*\" font=\"{$font.default}\" size=\"16\" text=\"" + GUIParser::escape(componentName) + "\" color=\"{$color.font_normal}\" height=\"100%\" vertical-align=\"center\" />\n") +
				string("	</layout>\n") +
				string("</element>\n");
		}
		dynamic_cast<GUIParentNode*>(engine->getGUI()->getScreen("installer_components")->getNodeById("scrollarea_components_inner"))->replaceSubNodes(
			componentsXML,
			true
		);
		engine->getGUI()->addScreen(
			"installer_folder",
			GUIParser::parse(
				"resources/installer",
				"installer_folder.xml",
				variables
			)
		);
		engine->getGUI()->addScreen(
			"installer_installing",
			GUIParser::parse(
				"resources/installer",
				"installer_installing.xml",
				variables
			)
		);
		engine->getGUI()->addScreen(
			"installer_finished",
			GUIParser::parse(
				"resources/installer",
				"installer_finished.xml",
				variables
			)
		);
		engine->getGUI()->addScreen(
			"installer_welcome2",
			GUIParser::parse(
				"resources/installer",
				"installer_welcome2.xml",
				variables
			)
		);
		engine->getGUI()->addScreen(
			"installer_uninstalling",
			GUIParser::parse(
				"resources/installer",
				"installer_uninstalling.xml",
				variables
			)
		);
		engine->getGUI()->getScreen("installer_welcome")->addActionListener(this);
		engine->getGUI()->getScreen("installer_welcome")->addChangeListener(this);
		engine->getGUI()->getScreen("installer_license")->addActionListener(this);
		engine->getGUI()->getScreen("installer_license")->addChangeListener(this);
		engine->getGUI()->getScreen("installer_components")->addActionListener(this);
		engine->getGUI()->getScreen("installer_components")->addChangeListener(this);
		engine->getGUI()->getScreen("installer_folder")->addActionListener(this);
		engine->getGUI()->getScreen("installer_folder")->addChangeListener(this);
		engine->getGUI()->getScreen("installer_installing")->addActionListener(this);
		engine->getGUI()->getScreen("installer_installing")->addChangeListener(this);
		engine->getGUI()->getScreen("installer_finished")->addActionListener(this);
		engine->getGUI()->getScreen("installer_finished")->addChangeListener(this);
		engine->getGUI()->getScreen("installer_welcome2")->addActionListener(this);
		engine->getGUI()->getScreen("installer_welcome2")->addChangeListener(this);
		engine->getGUI()->getScreen("installer_uninstalling")->addActionListener(this);
		engine->getGUI()->getScreen("installer_uninstalling")->addChangeListener(this);
		engine->getGUI()->addRenderScreen(installed == false?"installer_welcome":"installer_welcome2");
		engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
		engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
	} catch (Exception& exception) {
		engine->getGUI()->resetRenderScreens();
		engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
		engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
		popUps->getInfoDialogScreenController()->show("An error occurred:", exception.what());
	}
}

void Installer::performScreenAction() {
	engine->getGUI()->resetRenderScreens();
	switch (screen) {
		case SCREEN_WELCOME:
			engine->getGUI()->addRenderScreen("installer_welcome");
			engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
			engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
			break;
		case SCREEN_LICENSE:
			engine->getGUI()->addRenderScreen("installer_license");
			engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
			engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
			break;
		case SCREEN_CHECKFORUPDATE:
			{
				engine->getGUI()->addRenderScreen("installer_installing");
				engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
				engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
				dynamic_cast<GUITextNode*>(engine->getGUI()->getScreen("installer_installing")->getNodeById("message"))->setText(MutableString("Checking for updates ..."));
				dynamic_cast<GUITextNode*>(engine->getGUI()->getScreen("installer_installing")->getNodeById("details"))->setText(MutableString("..."));
				dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(0.0f, 2));
				class CheckForUpdateThread: public Thread {
					public:
						CheckForUpdateThread(Installer* installer): Thread("checkforupdate-thread", true), installer(installer) {
						}

					private:
						Installer* installer;

						void run() {
							//
							Console::printLine("CheckForUpdateThread::run(): init");

							//
							auto currentTimestamp = installer->timestamp;

							//
							auto repairHaveLocalFile = false;
							auto completionFileName = Application::getOSName() + "-" + Application::getCPUName() + "-upload-";
							if (installer->installerMode == INSTALLERMODE_REPAIR) completionFileName += installer->timestamp;

							// determine newest component file name
							if (installer->timestamp.empty() == true || installer->installerMode == INSTALLERMODE_REPAIR) {
								vector<string> files;
								FileSystem::getStandardFileSystem()->list("installer", files);
								for (const auto& file: files) {
									if (StringTools::startsWith(file, completionFileName) == true) {
										Console::printLine("CheckForUpdateThread: Have upload completion file: " + file);
										installer->timestamp = StringTools::substring(file, completionFileName.size());
										repairHaveLocalFile = true;
									}
								}
							}
							if (installer->timestamp.empty() == false) {
								Console::printLine("CheckForUpdateThread::run(): filesystem: newest timestamp: " + installer->timestamp);
							}

							//
							auto repository = installer->installerProperties.get("repository", "");
							if (repository.empty() == false && (installer->installerMode != INSTALLERMODE_REPAIR || repairHaveLocalFile == false)) {
								string timestampWeb;

								// check repository via apache index file for now
								if (installer->installerMode == INSTALLERMODE_INSTALL ||
									installer->installerMode == INSTALLERMODE_UPDATE) {
									try {
										HTTPClient httpClient;
										httpClient.setMethod(HTTPClient::HTTP_METHOD_GET);
										httpClient.setURL(repository + "?timestamp=" + to_string(Time::getCurrentMillis()));
										httpClient.setUsername(installer->installerProperties.get("repository_username", ""));
										httpClient.setPassword(installer->installerProperties.get("repository_password", ""));
										httpClient.execute();
										auto response = httpClient.getResponse().str();
										auto pos = 0;
										while ((pos = response.find(completionFileName, pos)) != string::npos) {
											pos+= completionFileName.size();
											auto timestampWebNew = StringTools::substring(response, pos, pos + 14);
											pos+= 14;
											if ((installer->timestamp.empty() == true && (timestampWeb.empty() == true || timestampWebNew > timestampWeb)) ||
												(installer->timestamp.empty() == false && ((timestampWeb.empty() == true || timestampWebNew > timestampWeb) && timestampWebNew > installer->timestamp))) {
												timestampWeb = timestampWebNew;
											}
										}
									} catch (Exception& exception) {
										Console::printLine(string("CheckForUpdateThread::run(): An error occurred: ") + exception.what());
									}
								} else
								if (installer->installerMode == INSTALLERMODE_REPAIR && repairHaveLocalFile == false) {
									timestampWeb = installer->timestamp;
								}

								// download archives if newer
								if (timestampWeb.empty() == false) {
									Console::printLine("CheckForUpdateThread::run(): repository: newest timestamp: " + timestampWeb);

									// we use web installer archives
									installer->timestamp = timestampWeb;

									// download them
									HTTPDownloadClient httpDownloadClient;
									for (auto componentIdx = 0; true; componentIdx++) {
										auto componentId = componentIdx == 0?"installer":"component" + to_string(componentIdx);
										auto componentName = installer->installerProperties.get(componentId, "");
										if (componentName.empty() == true) break;
										Console::printLine("CheckForUpdateThread::run(): Having component: " + to_string(componentIdx) + ": " + componentName);
										auto componentInclude = installer->installerProperties.get(componentId + "_include", "");
										if (componentInclude.empty() == true) {
											Console::printLine("CheckForUpdateThread::run(): component: " + to_string(componentIdx) + ": missing includes. Skipping.");
											continue;
										}
										auto componentFileName = Application::getOSName() + "-" + Application::getCPUName() + "-" + StringTools::replace(StringTools::replace(componentName, " - ", "-"), " ", "-") + "-" + installer->timestamp + ".ta";

										//
										Console::printLine("CheckForUpdateThread::run(): Component: " + to_string(componentIdx) + ": component file name: " + componentFileName + ": Downloading");
										//
										installer->installThreadMutex.lock();
										dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("message"))->setText(MutableString("Downloading ..."));
										dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("details"))->setText(MutableString(componentFileName));
										dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(0.0f, 2));
										installer->installThreadMutex.unlock();

										// sha256
										if (componentIdx > 0) {
											installer->installThreadMutex.lock();
											installer->downloadedFiles.push_back("installer/" + componentFileName + ".sha256.download");
											installer->downloadedFiles.push_back("installer/" + componentFileName + ".sha256");
											installer->installThreadMutex.unlock();
										}
										httpDownloadClient.reset();
										httpDownloadClient.setUsername(installer->installerProperties.get("repository_username", ""));
										httpDownloadClient.setPassword(installer->installerProperties.get("repository_password", ""));
										httpDownloadClient.setFile("installer/" + componentFileName + ".sha256");
										httpDownloadClient.setURL(installer->installerProperties.get("repository", "") + componentFileName + ".sha256");
										httpDownloadClient.start();
										while (httpDownloadClient.isFinished() == false) {
											installer->installThreadMutex.lock();
											dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(httpDownloadClient.getProgress(), 2));
											installer->installThreadMutex.unlock();
											Thread::sleep(250LL);
										}
										httpDownloadClient.join();
										if (httpDownloadClient.getStatusCode() != 200) {
											installer->popUps->getInfoDialogScreenController()->show("An error occurred:", "File not found in repository: " + componentFileName + ".sha256(" + to_string(httpDownloadClient.getStatusCode()) + ")");
											//
											Console::printLine("CheckForUpdateThread::run(): done");
											//
											return;
										}

										// archive
										if (componentIdx > 0) {
											installer->installThreadMutex.lock();
											installer->downloadedFiles.push_back("installer/" + componentFileName + ".download");
											installer->downloadedFiles.push_back("installer/" + componentFileName);
											installer->installThreadMutex.unlock();
										}
										httpDownloadClient.reset();
										httpDownloadClient.setFile("installer/" + componentFileName);
										httpDownloadClient.setURL(installer->installerProperties.get("repository", "") + componentFileName);
										httpDownloadClient.start();
										while (httpDownloadClient.isFinished() == false) {
											installer->installThreadMutex.lock();
											dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(httpDownloadClient.getProgress(), 2));
											installer->installThreadMutex.unlock();
											Thread::sleep(250LL);
										}
										httpDownloadClient.join();
										if (httpDownloadClient.getStatusCode() != 200) {
											installer->popUps->getInfoDialogScreenController()->show("An error occurred:", "File not found in repository: " + componentFileName + "(" + to_string(httpDownloadClient.getStatusCode()) + ")");
											//
											Console::printLine("CheckForUpdateThread::run(): done");
											//
											return;
										}
									}
								}
							}

							// no update available, but update was requested
							if (installer->installerMode == INSTALLERMODE_UPDATE && installer->timestamp <= currentTimestamp) {
								installer->popUps->getInfoDialogScreenController()->show("Information", "No update available");
								//
								Console::printLine("CheckForUpdateThread::run(): done");
								//
								return;
							}

							//
							installer->installThreadMutex.lock();
							installer->screen = SCREEN_COMPONENTS;
							installer->remountInstallerArchive = true;
							installer->installThreadMutex.unlock();

							//
							Console::printLine("CheckForUpdateThread::run(): done");
						}
				};
				//
				auto checkForUpdateThread = new CheckForUpdateThread(this);
				checkForUpdateThread->start();
				//
				break;
			}
		case SCREEN_COMPONENTS:
			engine->getGUI()->addRenderScreen("installer_components");
			engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
			engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
			break;
		case SCREEN_PATH:
			engine->getGUI()->addRenderScreen("installer_folder");
			engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
			engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
			break;
		case SCREEN_INSTALLING:
			{
				engine->getGUI()->addRenderScreen("installer_installing");
				engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
				engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
				class InstallThread: public Thread {
					public:
						InstallThread(Installer* installer): Thread("install-thread", true), installer(installer) {
						}
						void run() {
							//
							Console::printLine("InstallThread::run(): init");

							// we can not just overwrite executables on windows specially the installer exe or the libraries of it
							// we need to install them with .update suffix and remove the suffix after installer is closed
							#if defined(_WIN32)
								vector<string> windowsUpdateRenameFiles;
							#endif

							//
							installer->installThreadMutex.lock();
							dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("message"))->setText(MutableString("Initializing ..."));
							dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("details"))->setText(MutableString("..."));
							dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(0.0f, 2));
							installer->installThreadMutex.unlock();

							//
							auto hadException = false;
							vector<string> log;
							vector<string> components;
							auto installPath = dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_folder")->getNodeById("install_folder"))->getController()->getValue().getString();
							try {
								Installer::createPathRecursively(installPath);
							} catch (Exception& exception) {
								installer->popUps->getInfoDialogScreenController()->show("An error occurred:", exception.what());
								hadException = true;
							}

							//
							log.push_back(installPath);
							if (hadException == false) {
								// copy installer archive and sha256
								if (installer->installerMode == INSTALLERMODE_INSTALL) {
									auto file = dynamic_cast<ArchiveFileSystem*>(FileSystem::getInstance())->getArchiveFileName();
									{
										vector<uint8_t> content;
										Console::printLine("InstallThread::run(): Installer: Copy: " + file);
										FileSystem::getStandardFileSystem()->getContent(
											FileSystem::getStandardFileSystem()->getPathName(file),
											FileSystem::getStandardFileSystem()->getFileName(file),
											content
										);
										auto generatedFileName = installPath + "/" + file;
										Installer::createPathRecursively(
											FileSystem::getStandardFileSystem()->getPathName(generatedFileName)
										);
										FileSystem::getStandardFileSystem()->setContent(
											FileSystem::getStandardFileSystem()->getPathName(generatedFileName),
											FileSystem::getStandardFileSystem()->getFileName(generatedFileName),
											content
										);
									}
									file+= ".sha256";
									{
										vector<uint8_t> content;
										Console::printLine("InstallThread::run(): Installer: Copy: " + file);
										FileSystem::getStandardFileSystem()->getContent(
											FileSystem::getStandardFileSystem()->getPathName(file),
											FileSystem::getStandardFileSystem()->getFileName(file),
											content
										);
										auto generatedFileName = installPath + "/" + file;
										Installer::createPathRecursively(
											FileSystem::getStandardFileSystem()->getPathName(generatedFileName)
										);
										FileSystem::getStandardFileSystem()->setContent(
											FileSystem::getStandardFileSystem()->getPathName(generatedFileName),
											FileSystem::getStandardFileSystem()->getFileName(generatedFileName),
											content
										);
									}
								}
								// copy components
								for (auto componentIdx = 1; true; componentIdx++) {
									//
									auto componentName = installer->installerProperties.get("component" + to_string(componentIdx), "");
									if (componentName.empty() == true) break;

									// component file name
									auto componentFileName = Application::getOSName() + "-" + Application::getCPUName() + "-" + StringTools::replace(StringTools::replace(componentName, " - ", "-"), " ", "-") + "-" + installer->timestamp + ".ta";

									// check if marked
									installer->installThreadMutex.lock();
									if (dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_components")->getNodeById("checkbox_component" + to_string(componentIdx)))->getController()->getValue().equals("1") == false) {
										installer->installThreadMutex.unlock();
										// delete installer component archive archive
										if (installer->installerMode == Installer::INSTALLERMODE_UPDATE || installer->installerMode == Installer::INSTALLERMODE_REPAIR) {
											installer->installThreadMutex.lock();
											dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(0.0f, 2));
											dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("message"))->setText(MutableString("Deleting " + componentName));
											dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("details"))->setText(MutableString());
											installer->installThreadMutex.unlock();
											FileSystem::getStandardFileSystem()->removeFile("installer", componentFileName);
											installer->installThreadMutex.lock();
											dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(50.0f, 2));
											installer->installThreadMutex.unlock();
											FileSystem::getStandardFileSystem()->removeFile("installer", componentFileName + ".sha256");
											installer->installThreadMutex.lock();
											dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(100.0f, 2));
											installer->installThreadMutex.unlock();
										}
										continue;
									}
									installer->installThreadMutex.unlock();

									//
									components.push_back(componentName);

									//
									Console::printLine("InstallThread::run(): Having component: " + to_string(componentIdx) + ": " + componentName);
									auto componentInclude = installer->installerProperties.get("component" + to_string(componentIdx) + "_include", "");
									if (componentInclude.empty() == true) {
										Console::printLine("InstallThread::run(): component: " + to_string(componentIdx) + ": missing includes. Skipping.");
										continue;
									}
									//
									Console::printLine("InstallThread::run(): Component: " + to_string(componentIdx) + ": component file name: " + componentFileName);
									//
									installer->installThreadMutex.lock();
									dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(0.0f, 2));
									dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("details"))->setText(MutableString());
									installer->installThreadMutex.unlock();

									unique_ptr<ArchiveFileSystem> archiveFileSystem;
									try {
										installer->installThreadMutex.lock();
										dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("message"))->setText(MutableString("Verifying " + componentName));
										installer->installThreadMutex.unlock();
										archiveFileSystem = make_unique<ArchiveFileSystem>("installer/" + componentFileName);
										if (archiveFileSystem->computeSHA256Hash() != FileSystem::getStandardFileSystem()->getContentAsString("installer", componentFileName + ".sha256")) {
											throw ExceptionBase("Failed to verify: " + componentFileName + ", remove files in ./installer/ and try again");
										}
										installer->installThreadMutex.lock();
										dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("message"))->setText(MutableString("Installing " + componentName));
										installer->installThreadMutex.unlock();
										vector<string> files;
										Installer::scanArchive(archiveFileSystem.get(), files);
										uint64_t totalSize = 0LL;
										uint64_t doneSize = 0LL;
										for (const auto& file: files) {
											totalSize+= archiveFileSystem->getFileSize(
												archiveFileSystem->getPathName(file),
												archiveFileSystem->getFileName(file)
											);
										}
										for (const auto& file: files) {
											vector<uint8_t> content;
											Console::printLine("InstallThread::run(): Component: " + to_string(componentIdx) + ": " + file);
											archiveFileSystem->getContent(
												archiveFileSystem->getPathName(file),
												archiveFileSystem->getFileName(file),
												content
											);
											auto generatedFileName = installPath + "/" + file;
											Installer::createPathRecursively(
												FileSystem::getStandardFileSystem()->getPathName(generatedFileName)
											);
											#if defined(_WIN32)
												auto windowsGeneratedFile = generatedFileName;
												if (
													(installer->installerMode == INSTALLERMODE_REPAIR ||
													installer->installerMode == INSTALLERMODE_UPDATE) &&
													(StringTools::endsWith(StringTools::toLowerCase(generatedFileName), ".exe") == true ||
													StringTools::endsWith(StringTools::toLowerCase(generatedFileName), ".dll") == true)) {
													windowsUpdateRenameFiles.push_back(FileSystem::getStandardFileSystem()->getFileName(generatedFileName));
													windowsGeneratedFile+= ".update";
												}
											#endif

											if (archiveFileSystem->isExecutable(archiveFileSystem->getPathName(file), archiveFileSystem->getFileName(file)) == true) {
												#if defined(__FreeBSD__) || defined(__linux__) || defined(__NetBSD__)
													FileSystem::getStandardFileSystem()->setContent(
														FileSystem::getStandardFileSystem()->getPathName(generatedFileName),
														FileSystem::getStandardFileSystem()->getFileName(generatedFileName),
														content
													);
													log.push_back(generatedFileName);
													FileSystem::getStandardFileSystem()->setExecutable(
														FileSystem::getStandardFileSystem()->getPathName(generatedFileName),
														FileSystem::getStandardFileSystem()->getFileName(generatedFileName)
													);
													auto launcherName = installer->installerProperties.get("launcher_" + StringTools::toLowerCase(FileSystem::getStandardFileSystem()->getFileName(generatedFileName)), "");
													if (launcherName.empty() == false) {
														Installer::createPathRecursively(installer->homePath + "/" + ".local/share/applications/");
														FileSystem::getStandardFileSystem()->setContentFromString(
															FileSystem::getStandardFileSystem()->getPathName(generatedFileName),
															FileSystem::getStandardFileSystem()->getFileName(generatedFileName) + ".sh",
															string() +
															"#!/bin/sh\n" +
															"cd " + installPath + "\n" +
															"nohup " +
															FileSystem::getStandardFileSystem()->getPathName(generatedFileName) + "/" + FileSystem::getStandardFileSystem()->getFileName(generatedFileName) + " " +
															"</dev/null &>/dev/null &\n"
														);
														FileSystem::getStandardFileSystem()->setExecutable(
															FileSystem::getStandardFileSystem()->getPathName(generatedFileName),
															FileSystem::getStandardFileSystem()->getFileName(generatedFileName) + ".sh"
														);
														auto executablePathName = FileSystem::getInstance()->getPathName(generatedFileName);
														auto executableFileName = FileSystem::getInstance()->getFileName(generatedFileName);
														auto iconFileName = StringTools::toLowerCase(executableFileName) + "-icon.png";
														if (archiveFileSystem->exists("resources/platforms/icons/" + iconFileName) == false &&
															FileSystem::getInstance()->exists(executablePathName + "/resources/platforms/icons/" + iconFileName) == false) iconFileName = "default-icon.png";
														FileSystem::getStandardFileSystem()->setContentFromString(
															installer->homePath + "/" + ".local/share/applications",
															launcherName + ".desktop",
															string() +
															"[Desktop Entry]\n" +
															"Name="  + launcherName + "\n" +
															"Exec=" + FileSystem::getStandardFileSystem()->getPathName(generatedFileName) + "/" + FileSystem::getStandardFileSystem()->getFileName(generatedFileName) + ".sh\n" +
															"Terminal=false\n" +
															"Type=Application\n" +
															"Icon=" + installPath + "/resources/platforms/icons/" + iconFileName + "\n"
														);
														log.push_back(generatedFileName + ".sh");
														log.push_back(installer->homePath + "/" + ".local/share/applications/" + launcherName + ".desktop");
													}
												#elif defined(_WIN32)
													FileSystem::getStandardFileSystem()->setContent(
														FileSystem::getStandardFileSystem()->getPathName(generatedFileName),
														FileSystem::getStandardFileSystem()->getFileName(windowsGeneratedFile),
														content
													);
													log.push_back(generatedFileName);
													auto executable = FileSystem::getStandardFileSystem()->getFileName(generatedFileName);
													auto launcherName = installer->installerProperties.get(
														"launcher_" +
														StringTools::substring(
															StringTools::toLowerCase(executable),
															0,
															executable.rfind('.')
														),
														""
													);
													if (launcherName.empty() == false) {
														FileSystem::getStandardFileSystem()->setContentFromString(
															installPath,
															"windows-create-shortcut.bat",
															FileSystem::getInstance()->getContentAsString(".", "windows-create-shortcut.bat")
														);
														auto startMenuPath = string(installer->homePath) + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/" + installer->installerProperties.get("name", "TDME2 based application");
														Installer::createPathRecursively(startMenuPath);
														auto linkFile = startMenuPath + "/" + launcherName + ".lnk";
														auto executablePathName = FileSystem::getInstance()->getPathName(generatedFileName);
														auto executableFileName = FileSystem::getInstance()->getFileName(generatedFileName);
														auto iconFileName = StringTools::replace(StringTools::toLowerCase(executableFileName), ".exe", "") + "-icon.ico";
														if (FileSystem::getInstance()->exists("resources/platforms/win32/" + iconFileName) == false &&
															FileSystem::getInstance()->exists(executablePathName + "/resources/platforms/win32/" + iconFileName) == false) iconFileName = "default-icon.ico";
														Console::printLine(
															StringTools::replace(StringTools::replace(installPath, "/", "\\"), " ", "^ ") + "\\windows-create-shortcut.bat " +
															"\"" + StringTools::replace(generatedFileName, "/", "\\") + "\" " +
															"\"" + StringTools::replace(linkFile, "/", "\\") + "\" " +
															"\"resources\\platforms\\win32\\" + iconFileName + "\" "
														);
														Console::printLine(
															Application::execute(
																StringTools::replace(StringTools::replace(installPath, "/", "\\"), " ", "^ ") + "\\windows-create-shortcut.bat " +
																"\"" + StringTools::replace(generatedFileName, "/", "\\") + "\" " +
																"\"" + StringTools::replace(linkFile, "/", "\\") + "\" " +
																"\"resources\\platforms\\win32\\" + iconFileName + "\" "
															)
														);
														log.push_back(linkFile);
														FileSystem::getStandardFileSystem()->removeFile(installPath, "windows-create-shortcut.bat");
													}
												#else
													FileSystem::getStandardFileSystem()->setContent(
														FileSystem::getStandardFileSystem()->getPathName(generatedFileName),
														FileSystem::getStandardFileSystem()->getFileName(generatedFileName),
														content
													);
													log.push_back(generatedFileName);
													FileSystem::getStandardFileSystem()->setExecutable(
														FileSystem::getStandardFileSystem()->getPathName(generatedFileName),
														FileSystem::getStandardFileSystem()->getFileName(generatedFileName)
													);

												#endif
											} else {
												FileSystem::getStandardFileSystem()->setContent(
													FileSystem::getStandardFileSystem()->getPathName(generatedFileName),
													#if defined(_WIN32)
														FileSystem::getStandardFileSystem()->getFileName(windowsGeneratedFile),
													#else
														FileSystem::getStandardFileSystem()->getFileName(generatedFileName),
													#endif
													content
												);
												log.push_back(generatedFileName);
											}
											doneSize+= content.size();
											installer->installThreadMutex.lock();
											dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("details"))->setText(MutableString(file));
											dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(static_cast<float>(doneSize) / static_cast<float>(totalSize), 2));
											installer->installThreadMutex.unlock();
										}

										// delete installer component archive archive
										if (installer->installerMode == Installer::INSTALLERMODE_UPDATE || installer->installerMode == Installer::INSTALLERMODE_REPAIR) {
											installer->installThreadMutex.lock();
											dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(0.0f, 2));
											dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("message"))->setText(MutableString("Deleting " + componentName));
											dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("details"))->setText(MutableString());
											installer->installThreadMutex.unlock();
											FileSystem::getStandardFileSystem()->removeFile("installer", componentFileName);
											installer->installThreadMutex.lock();
											dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(50.0f, 2));
											installer->installThreadMutex.unlock();
											FileSystem::getStandardFileSystem()->removeFile("installer", componentFileName + ".sha256");
											installer->installThreadMutex.lock();
											dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_installing")->getNodeById("progressbar"))->getController()->setValue(MutableString(100.0f, 2));
											installer->installThreadMutex.unlock();
										}
									} catch (Exception& exception) {
										installer->popUps->getInfoDialogScreenController()->show("An error occurred:", exception.what());
										hadException = true;
										break;
									}
								}
							}

							#if defined(_WIN32)
								if (installer->installerMode == INSTALLERMODE_REPAIR ||
									installer->installerMode == INSTALLERMODE_UPDATE) {
									string updateFinishBatch;
									updateFinishBatch+= "@ECHO OFF\r\n";
									updateFinishBatch+= "ECHO FINISHING UPDATE. PLEASE DO NOT CLOSE.\r\n";
									updateFinishBatch+= "setlocal EnableDelayedExpansion\r\n";
									auto loopIdx = 0;
									for (const auto& file: windowsUpdateRenameFiles) {
										auto updateFile = file + ".update";
										updateFinishBatch+=
											":loop" + to_string(loopIdx) + "\r\n" +
											"if exist \"" + updateFile + "\" (\r\n" +
											"	if exist \"" + file + "\" (\r\n" +
											"		del \"" + file + "\" > nul 2>&1\r\n" +
											"		if exist \"" + file + "\" goto loop" + to_string(loopIdx) + "\r\n" +
											"		rename \"" + updateFile + "\" \"" + file + "\" > nul 2>&1\r\n" +
											"	) else (\r\n" +
											"		rename \"" + updateFile + "\" \"" + file + "\" > nul 2>&1\r\n" +
											"	)\r\n" +
											")\r\n";
										loopIdx++;
									}
									try {
										FileSystem::getStandardFileSystem()->setContentFromString(installPath, "update-finish.bat", updateFinishBatch);
									} catch (Exception& exception) {
										installer->popUps->getInfoDialogScreenController()->show("An error occurred:", exception.what());
										hadException = true;
									}
								}
							#endif

							try {
								FileSystem::getStandardFileSystem()->setContentFromStringArray(installPath, "install.files.db", log);
								FileSystem::getStandardFileSystem()->setContentFromStringArray(installPath, "install.components.db", components);
								FileSystem::getStandardFileSystem()->setContentFromString(installPath, "install.version.db", installer->timestamp);
							} catch (Exception& exception) {
								installer->popUps->getInfoDialogScreenController()->show("An error occurred:", exception.what());
								hadException = true;
							}

							//
							if (hadException == false) {
								installer->installThreadMutex.lock();
								installer->screen = SCREEN_FINISHED;
								installer->engine->getGUI()->resetRenderScreens();
								installer->engine->getGUI()->addRenderScreen("installer_finished");
								installer->engine->getGUI()->addRenderScreen(installer->popUps->getFileDialogScreenController()->getScreenNode()->getId());
								installer->engine->getGUI()->addRenderScreen(installer->popUps->getInfoDialogScreenController()->getScreenNode()->getId());
								installer->installThreadMutex.unlock();
							}
							// determine set names
							Console::printLine("InstallThread::run(): done");
						}
					private:
						Installer* installer;
				};
				//
				auto installThread = new InstallThread(this);
				installThread->start();
				//
				break;
			}
		case SCREEN_FINISHED:
			engine->getGUI()->addRenderScreen("installer_finished");
			engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
			engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
			break;
		case SCREEN_WELCOME2:
			engine->getGUI()->addRenderScreen("installer_welcome2");
			engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
			engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
			break;
		case SCREEN_UNINSTALLING:
		{
			engine->getGUI()->addRenderScreen("installer_uninstalling");
			engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
			engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
			class UninstallThread: public Thread {
				public:
					UninstallThread(Installer* installer): Thread("install-thread", true), installer(installer) {
					}
					void run() {
						//
						Console::printLine("UninstallThread::run(): init");

						//
						installer->installThreadMutex.lock();
						dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_uninstalling")->getNodeById("message"))->setText(MutableString("Uninstalling ..."));
						dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_uninstalling")->getNodeById("details"))->setText(MutableString("..."));
						dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_uninstalling")->getNodeById("progressbar"))->getController()->setValue(MutableString(0.0f, 2));
						installer->installThreadMutex.unlock();

						// remove files that we installed
						auto hadException = false;
						vector<string> log;
						try {
							FileSystem::getStandardFileSystem()->getContentAsStringArray(".", "install.files.db", log);
						} catch (Exception& exception) {
							installer->popUps->getInfoDialogScreenController()->show("An error occurred:", exception.what());
							hadException = true;
						}

						//
						auto installPath = log.size() > 0?log[0] + "/":"";

						if (hadException == false) {
							#if defined(_WIN32)
								auto uninstallFinishBatchLoopIdx = 0;
								string uninstallFinishBatch;
								uninstallFinishBatch+= "@ECHO OFF\r\n";
								uninstallFinishBatch+= "ECHO FINISHING UNINSTALL. PLEASE DO NOT CLOSE.\r\n";
								uninstallFinishBatch+= "setlocal EnableDelayedExpansion\r\n";
							#endif
							for (auto i = 1; i < log.size(); i++) {
								try {
									installer->installThreadMutex.lock();
									dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_uninstalling")->getNodeById("message"))->setText(MutableString("Uninstalling ..."));
									dynamic_cast<GUITextNode*>(installer->engine->getGUI()->getScreen("installer_uninstalling")->getNodeById("details"))->setText(MutableString(log[i]));
									dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_uninstalling")->getNodeById("progressbar"))->getController()->setValue(MutableString(static_cast<float>(i) / static_cast<float>(log.size()), 2));
									installer->installThreadMutex.unlock();
									if (FileSystem::getStandardFileSystem()->exists(log[i]) == true) {
										FileSystem::getStandardFileSystem()->removeFile(
											FileSystem::getStandardFileSystem()->getPathName(log[i]),
											FileSystem::getStandardFileSystem()->getFileName(log[i])
										);
									}
								} catch (Exception& innerException) {
									Console::printLine(string("UninstallThread::run(): An error occurred: ") + innerException.what());
									#if defined(_WIN32)
										if (installer->installerMode == INSTALLERMODE_UNINSTALL &&
											(StringTools::endsWith(log[i], ".dll") == true ||
											StringTools::endsWith(log[i], ".exe") == true)) {
											auto file = FileSystem::getStandardFileSystem()->getFileName(log[i]);
											uninstallFinishBatch+=
												":loop" + to_string(uninstallFinishBatchLoopIdx) + "\r\n" +
												"if exist \"" + file + "\" (\r\n" +
												"	del \"" + file + "\" > nul 2>&1\r\n" +
												"	if exist \"" + file + "\" goto loop" + to_string(uninstallFinishBatchLoopIdx) + "\r\n" +
												")\r\n";
											uninstallFinishBatchLoopIdx++;
										}
									#endif
								}
							}
							#if defined(_WIN32)
								if (installer->installerMode == INSTALLERMODE_UNINSTALL) {
									{
										auto file = "console.log";
										uninstallFinishBatch+=
											":loop" + to_string(uninstallFinishBatchLoopIdx) + "\r\n" +
											"if exist \"" + file + "\" (\r\n" +
											"	del \"" + file + "\" > nul 2>&1\r\n" +
											"	if exist \"" + file + "\" goto loop" + to_string(uninstallFinishBatchLoopIdx) + "\r\n" +
											")\r\n";
										uninstallFinishBatchLoopIdx++;
									}
									try {
										FileSystem::getStandardFileSystem()->setContentFromString(installPath, "uninstall-finish.bat", uninstallFinishBatch);
									} catch (Exception& exception) {
										installer->popUps->getInfoDialogScreenController()->show("An error occurred:", exception.what());
										hadException = true;
									}
								}
							#endif
						}

						// remove paths that we created non recursive
						if (hadException == false) {
							vector<string> paths;
							for (auto i = 1; i < log.size(); i++) {
								auto pathCandidate = FileSystem::getStandardFileSystem()->getPathName(log[i]);
								if (FileSystem::getStandardFileSystem()->isPath(pathCandidate) == true) {
									while (StringTools::startsWith(pathCandidate, installPath) == true) {
										paths.push_back(pathCandidate);
										pathCandidate = FileSystem::getStandardFileSystem()->getPathName(pathCandidate);
									}
								}
							}
							sort(paths.begin(), paths.end());
							reverse(paths.begin(), paths.end());
							auto newEnd = unique(paths.begin(), paths.end());
							paths.resize(distance(paths.begin(), newEnd));
							for (const auto& path: paths) {
								try {
									FileSystem::getStandardFileSystem()->removePath(
										path,
										false
									);
								} catch (Exception& exception) {
									Console::printLine(string("UninstallThread::run(): An error occurred: ") + exception.what());
								}
							}

							// remove installer path if not updating
							if (installer->installerMode == INSTALLERMODE_UNINSTALL) {
								FileSystem::unsetFileSystem();
								vector<string> installerFiles;
								try {
									FileSystem::getStandardFileSystem()->list(
										installPath + "installer",
										installerFiles
									);
								} catch (Exception& exception) {
									Console::printLine(string("UninstallThread::run(): An error occurred: ") + exception.what());
								}
								for (const auto& installerFile: installerFiles) {
									if (StringTools::endsWith(installerFile, ".ta") == true ||
										StringTools::endsWith(installerFile, ".ta.sha256") == true) {
										try {
											FileSystem::getStandardFileSystem()->removeFile(
												installPath + "installer",
												installerFile
											);
										} catch (Exception& exception) {
											Console::printLine(string("UninstallThread::run(): An error occurred: ") + exception.what());
										}
									}
								}
								try {
									FileSystem::getStandardFileSystem()->removePath(installPath + "installer", false);
								} catch (Exception& exception) {
									Console::printLine(string("UninstallThread::run(): An error occurred: ") + exception.what());
								}
							}
						}

						// remove install databases and console.log
						if (hadException == false) {
							try {
								FileSystem::getStandardFileSystem()->removeFile(".", "install.files.db");
								FileSystem::getStandardFileSystem()->removeFile(".", "install.components.db");
								FileSystem::getStandardFileSystem()->removeFile(".", "install.version.db");
								FileSystem::getStandardFileSystem()->removeFile(".", "console.log");
							} catch (Exception& exception) {
								installer->popUps->getInfoDialogScreenController()->show("An error occurred:", exception.what());
								hadException = true;
							}
						}

						// remove install path
						if (hadException == false) {
							try {
								FileSystem::getStandardFileSystem()->removePath(installPath, false);
							} catch (Exception& exception) {
								Console::printLine(string("UninstallThread::run(): An error occurred: ") + exception.what());
							}
						}

						//
						if (hadException == false) {
							if (installer->installerMode == INSTALLERMODE_UPDATE || installer->installerMode == INSTALLERMODE_REPAIR) {
								installer->installThreadMutex.lock();
								installer->screen = SCREEN_INSTALLING;
								installer->performScreenAction();
								installer->installThreadMutex.unlock();
							} else {
								// TODO: Maybe show a finishing screen
								#if defined(_WIN32)
									string drive;
									if (installPath[1] == ':') drive = StringTools::substring(installPath, 0, 2) + " && ";
									system(
										(
											string() +
											drive +
											"cd " +
											"\"" + installPath + "/" + "\"" +
											" && start cmd /c \"uninstall-finish.bat && del uninstall-finish.bat && cd .. && rmdir \"\"\"" + StringTools::replace(installPath, "/", "\\") + "\"\"\" > nul 2>&1\"").c_str()
									);
								#endif
								//
								Application::exit(0);
							}
						}
						// determine set names
						Console::printLine("UninstallThread::run(): done");
					}
				private:
					Installer* installer;
			};
			//
			UninstallThread* uninstallThread = new UninstallThread(this);
			uninstallThread->start();
			//
			break;
		}
		default:
			Console::printLine("Installer::performScreenAction(): Unhandled screen: " + to_string(screen));
			break;
	}
	engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
	engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
}

void Installer::initialize()
{
	try {
		installed =
			FileSystem::getStandardFileSystem()->exists("install.files.db") == true &&
			FileSystem::getStandardFileSystem()->exists("install.components.db") == true &&
			FileSystem::getStandardFileSystem()->exists("install.version.db") == true;
		if (installed == true) screen = SCREEN_WELCOME2;
		engine->initialize();
		engine->setSceneColor(Color4(125.0f / 255.0f, 125.0f / 255.0f, 125.0f / 255.0f, 1.0f));
		setEventHandler(engine->getGUI());
		popUps->initialize();
		#if defined(_WIN32)
			homePath = StringTools::replace(string(getenv("USERPROFILE")), '\\', '/');
		#else
			homePath = string(getenv("HOME"));
		#endif
	} catch (Exception& exception) {
		engine->getGUI()->resetRenderScreens();
		engine->getGUI()->addRenderScreen(popUps->getFileDialogScreenController()->getScreenNode()->getId());
		engine->getGUI()->addRenderScreen(popUps->getInfoDialogScreenController()->getScreenNode()->getId());
		popUps->getInfoDialogScreenController()->show("An error occurred:", exception.what());
	}
	initializeScreens();
}

void Installer::dispose()
{
	engine->dispose();
}

void Installer::reshape(int width, int height)
{
	engine->reshape(width, height);
}

void Installer::onAction(GUIActionListenerType type, GUIElementNode* node) {
	if (type == GUIActionListenerType::PERFORMED) {
		if (node->getId() == "button_next") {
			if (screen == SCREEN_COMPONENTS && (installerMode == INSTALLERMODE_UPDATE || installerMode == INSTALLERMODE_REPAIR)) {
				screen = SCREEN_UNINSTALLING;
			} else {
				screen = static_cast<Screen>(static_cast<int>(screen) + 1 % static_cast<int>(SCREEN_MAX));
			}
			performScreenAction();
		} else
		if (node->getId() == "button_back") {
			if (screen == SCREEN_COMPONENTS && (installerMode == INSTALLERMODE_UPDATE || installerMode == INSTALLERMODE_REPAIR)) {
				try {
					timestamp = FileSystem::getStandardFileSystem()->getContentAsString(".", "install.version.db");
				} catch (Exception& exception) {
					Console::printLine(string("Installer::onAction(): An error occurred: ") + exception.what());
				}
				screen = SCREEN_WELCOME2;
			} else
			if (screen == SCREEN_COMPONENTS && (installerMode == INSTALLERMODE_INSTALL)) {
				screen = SCREEN_LICENSE;
			} else {
				screen = static_cast<Screen>(static_cast<int>(screen) - 1 % static_cast<int>(SCREEN_MAX));
			}
			performScreenAction();
		} else
		if (node->getId() == "button_agree") {
			installerMode = INSTALLERMODE_INSTALL;
			screen = SCREEN_CHECKFORUPDATE;
			performScreenAction();
		} else
		if (node->getId() == "button_install") {
			dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("installer_folder")->getNodeById("install_folder"))->getController()->setValue(MutableString(StringTools::replace(dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("installer_folder")->getNodeById("install_folder"))->getController()->getValue().getString(), "\\", "/")));
			screen = SCREEN_INSTALLING;
			performScreenAction();
		} else
		if (node->getId() == "button_cancel") {
			FileSystem::unsetFileSystem();
			// delete downloaded files
			for (const auto& downloadedFile: downloadedFiles) {
				try {
					FileSystem::getStandardFileSystem()->removeFile(".", downloadedFile);
				} catch (Exception& exception) {
					Console::printLine("Installer::onAction(): Removing downloaded file failed: " + downloadedFile);
				}
			}
			Application::exit(0);
		} else
		if (node->getId() == "button_finish") {
			if (installerProperties.get("launch", "").empty() == false &&
				dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("installer_finished")->getNodeById("checkbox_launch"))->getController()->getValue().equals("1") == true) {
				#if defined(_WIN32)
					auto installPath = dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("installer_folder")->getNodeById("install_folder"))->getController()->getValue().getString();
					string drive;
					if (installPath[1] == ':') drive = StringTools::substring(installPath, 0, 2) + " && ";
					string finishCommand;
					if (installerMode == INSTALLERMODE_REPAIR ||
						installerMode == INSTALLERMODE_UPDATE) {
						finishCommand+= " && start cmd /c \"update-finish.bat && del update-finish.bat && " + installerProperties.get("launch", "") + ".exe" + "\"";
					} else {
						finishCommand+= " && start cmd /c \"" + installerProperties.get("launch", "") + ".exe" + "\"";
					}
					system(
						(
							string() +
							drive +
							"cd " +
							"\"" + installPath + "/" + "\"" +
							finishCommand
						).c_str()
					);
				#elif defined(__APPLE__)
					Application::executeBackground(
						"open " +
						dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("installer_folder")->getNodeById("install_folder"))->getController()->getValue().getString() + "/" +
						installerProperties.get("launch", "") + ".app"
					);
				#else
					Application::executeBackground(
						dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("installer_folder")->getNodeById("install_folder"))->getController()->getValue().getString() + "/" +
						installerProperties.get("launch", "") + ".sh"
					);
				#endif
			} else {
				#if defined(_WIN32)
					auto installPath = dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("installer_folder")->getNodeById("install_folder"))->getController()->getValue().getString();
					string drive;
					if (installPath[1] == ':') drive = StringTools::substring(installPath, 0, 2) + " && ";
					if (installerMode == INSTALLERMODE_REPAIR ||
						installerMode == INSTALLERMODE_UPDATE) {
						system(
							(
								string() +
								drive +
								"cd " +
								"\"" + installPath + "/" + "\"" +
								" && start cmd /c \"update-finish.bat && del update-finish.bat\"").c_str()
						);
					}
				#endif
			}
			FileSystem::unsetFileSystem();
			Application::exit(0);
		} else
		if (node->getId() == "button_browse") {
			class OnBrowseAction: public Action
			{
			public:
				void performAction() override {
					installer->popUps->getFileDialogScreenController()->close();
					auto pathToShow = installer->popUps->getFileDialogScreenController()->getPathName() + "/" + installer->installerProperties.get("name", "TDME2 based application");
					dynamic_cast<GUIElementNode*>(installer->engine->getGUI()->getScreen("installer_folder")->getNodeById("install_folder"))->getController()->setValue(MutableString(pathToShow));
				}

				/**
				 * Public constructor
				 * @param installer installer
				 */
				OnBrowseAction(Installer* installer): installer(installer) {
				}

			private:
				Installer* installer;
			};

			vector<string> extensions;
			auto pathToShow = dynamic_cast<GUIElementNode*>(engine->getGUI()->getScreen("installer_folder")->getNodeById("install_folder"))->getController()->getValue().getString();
			pathToShow = StringTools::replace(pathToShow, "\\", "/");
			while (FileSystem::getStandardFileSystem()->exists(pathToShow) == false || FileSystem::getStandardFileSystem()->isPath(pathToShow) == false) {
				pathToShow = FileSystem::getStandardFileSystem()->getPathName(pathToShow);
			}
			if (StringTools::startsWith(pathToShow, "/") == false) pathToShow = homePath;
			popUps->getFileDialogScreenController()->show(
				pathToShow,
				"Install in: ",
				extensions,
				"",
				true,
				new OnBrowseAction(this)
			);
		} else
		if (node->getId() == "button_uninstall") {
			installerMode = INSTALLERMODE_UNINSTALL;
			screen = SCREEN_UNINSTALLING;
			performScreenAction();
		} else
		if (node->getId() == "button_update") {
			installerMode = INSTALLERMODE_UPDATE;
			screen = SCREEN_CHECKFORUPDATE;
			performScreenAction();
		} else
		if (node->getId() == "button_repair") {
			installerMode = INSTALLERMODE_REPAIR;
			screen = SCREEN_CHECKFORUPDATE;
			performScreenAction();
		} else
		if (StringTools::startsWith(node->getId(), "component") == true) {
			auto componentIdx = Integer::parse(StringTools::substring(node->getId(), string("component").size()));
			dynamic_cast<GUIStyledTextNode*>(engine->getGUI()->getScreen("installer_components")->getNodeById("component_description"))->setText(MutableString(installerProperties.get("component" + to_string(componentIdx) + "_description", "No detail description.")));
		}
	}
}

void Installer::onChange(GUIElementNode* node) {
	Console::printLine(node->getName() + ": onChange(): " + node->getController()->getValue().getString());
}

void Installer::display()
{
	installThreadMutex.lock();
	if (remountInstallerArchive == true) {
		mountInstallerFileSystem(timestamp, true);
		initializeScreens();
		performScreenAction();
		remountInstallerArchive = false;
	}
	engine->display();
	engine->getGUI()->render();
	engine->getGUI()->handleEvents();
	installThreadMutex.unlock();
}

void Installer::mountInstallerFileSystem(const string& timestamp, bool remountInstallerArchive) {
	Console::printLine("Installer::mountInstallerFileSystem(): timestamp: " + (timestamp.empty() == false?timestamp:"no timestamp"));
	// determine installer tdme archive
	try {
		auto installerArchiveFileNameStart = Application::getOSName() + "-" + Application::getCPUName() + "-Installer-" + (timestamp.empty() == false?timestamp + ".ta":"");
		string installerArchiveFileName;
		// determine newest component file name
		vector<string> files;
		FileSystem::getStandardFileSystem()->list("installer", files);
		for (const auto& file: files) {
			if (StringTools::startsWith(file, installerArchiveFileNameStart) == true &&
				StringTools::endsWith(file, ".ta") == true) {
				Console::printLine("Installer::main(): Have installer tdme archive file: " + file);
				installerArchiveFileName = file;
			}
		}
		if (installerArchiveFileName.empty() == true) {
			Console::printLine("Installer::main(): No installer TDME archive found. Exiting.");
			Application::exit(1);
		}
		// file system
		auto installerFileSystem = make_unique<ArchiveFileSystem>("installer/" + installerArchiveFileName);
		if (installerFileSystem->computeSHA256Hash() != FileSystem::getStandardFileSystem()->getContentAsString("installer", installerArchiveFileName + ".sha256")) {
			throw ExceptionBase("Installer::main(): Failed to verify: " + installerArchiveFileName + ", get new installer and try again");
		}
		Console::printLine("Installer::mountInstallerFileSystem(): unmounting");
		// check if to remove old installer file system
		auto lastInstallerArchiveFileName = remountInstallerArchive == true?static_cast<ArchiveFileSystem*>(FileSystem::getInstance())->getArchiveFileName():string();
		FileSystem::unsetFileSystem();
		// so? Also check if new installer archive file name is same as currently used installer archive file name
		if (lastInstallerArchiveFileName.empty() == false && lastInstallerArchiveFileName != "installer/" + installerArchiveFileName) {
			// yep
			Console::printLine("Installer::mountInstallerFileSystem(): deleting installer tdme archive file: " + lastInstallerArchiveFileName);
			try {
				FileSystem::getStandardFileSystem()->removeFile(
					FileSystem::getStandardFileSystem()->getPathName(lastInstallerArchiveFileName),
					FileSystem::getStandardFileSystem()->getFileName(lastInstallerArchiveFileName)
				);
				FileSystem::getStandardFileSystem()->removeFile(
					FileSystem::getStandardFileSystem()->getPathName(lastInstallerArchiveFileName),
					FileSystem::getStandardFileSystem()->getFileName(lastInstallerArchiveFileName) + ".sha256"
				);
			} catch (Exception& exception) {
				Console::printLine(string("Installer::mountInstallerFileSystem(): An error occurred: ") + exception.what());
			}
		}
		Console::printLine("Installer::mountInstallerFileSystem(): mounting: " + installerArchiveFileName);
		FileSystem::setupFileSystem(installerFileSystem.release());
	} catch (Exception& exception) {
		Console::printLine(string("Installer::mountInstallerFileSystem(): ") + exception.what());
		Application::exit(1);
	}
}

int Installer::main(int argc, char** argv)
{
	Console::printLine(string("Installer ") + Version::getVersion());
	Console::printLine(Version::getCopyright());
	Console::printLine();
	if (argc > 1) {
		Console::printLine("Usage: Installer");
		Application::exit(1);
	}
	#if defined(__APPLE__)
		// TODO: We have this duplicated in Application::run() also, but we need it twice for installer special case: move me into Application as static method or something
		// change working directory on MacOSX if started from app bundle
		auto executablePathName = string(argv[0]);
		if (executablePathName.find(".app/Contents/MacOS/") != string::npos) {
			auto appBundleName = StringTools::substring(executablePathName, 0, executablePathName.rfind(".app") + string(".app").size());
			auto workingPathName = StringTools::substring(appBundleName, 0, appBundleName.rfind('/'));
			FileSystem::getStandardFileSystem()->changePath(workingPathName);
		}
	#endif
	mountInstallerFileSystem();
	auto installer = new Installer();
	return installer->run(argc, argv, "Installer", nullptr, Application::WINDOW_HINT_NOTRESIZEABLE);
}

void Installer::scanArchive(ArchiveFileSystem* archiveFileSystem, vector<string>& totalFiles, const string& pathName) {
	vector<string> files;
	archiveFileSystem->list(pathName, files);
	for (const auto& fileName: files) {
		if (archiveFileSystem->isPath(pathName + "/" + fileName) == false) {
			totalFiles.push_back((pathName.empty() == true?"":pathName + "/") + fileName);
		} else {
			scanArchive(archiveFileSystem, totalFiles, pathName + "/" + fileName);
		}
	}

}

void Installer::createPathRecursively(const string& pathName) {
	StringTokenizer t;
	t.tokenize(pathName, "/");
	string pathCreating;
	while (t.hasMoreTokens() == true) {
		string pathComponent = t.nextToken();
		#if defined(_WIN32)
			pathCreating+= pathCreating.empty() == true?pathComponent:"/" + pathComponent;
		#else
			pathCreating+= "/" + pathComponent;
		#endif
		if (FileSystem::getStandardFileSystem()->isDrive(pathCreating) == false && FileSystem::getStandardFileSystem()->exists(pathCreating) == false) {
			Console::printLine("Creating: " + pathCreating);
			FileSystem::getStandardFileSystem()->createPath(pathCreating);
		}
	}
}

void Installer::onClose() {
	Application::cancelExit();
}
