#include <tdme/utilities/MiniScript.h>

#include <algorithm>
#include <map>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdme/engine/physics/Body.h>
#include <tdme/engine/physics/World.h>
#include <tdme/engine/primitives/BoundingVolume.h>
#include <tdme/engine/prototype/Prototype.h>
#include <tdme/engine/prototype/PrototypeBoundingVolume.h>
#include <tdme/engine/scene/SceneEntity.h>
#include <tdme/engine/Transformations.h>
#include <tdme/math/Math.h>
#include <tdme/math/Vector3.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemException.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/StringTokenizer.h>
#include <tdme/utilities/StringTools.h>
#include <tdme/utilities/Time.h>

using std::map;
using std::sort;
using std::stack;
using std::string;
using std::to_string;
using std::unordered_map;
using std::vector;

using tdme::engine::physics::Body;
using tdme::engine::physics::World;
using tdme::engine::primitives::BoundingVolume;
using tdme::engine::prototype::Prototype;
using tdme::engine::prototype::PrototypeBoundingVolume;
using tdme::engine::scene::SceneEntity;
using tdme::math::Math;
using tdme::math::Vector3;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemException;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::MiniScript;
using tdme::utilities::StringTokenizer;
using tdme::utilities::StringTools;
using tdme::utilities::Time;

MiniScript::MiniScript() {
}

MiniScript::~MiniScript() {
	for (auto& scriptMethodIt: scriptMethods) delete scriptMethodIt.second;
	for (auto& scriptStateMachineStateIt: scriptStateMachineStates) delete scriptStateMachineStateIt.second;
}

void MiniScript::registerStateMachineState(ScriptStateMachineState* state) {
	auto scriptStateMachineStateIt = scriptStateMachineStates.find(state->getId());
	if (scriptStateMachineStateIt != scriptStateMachineStates.end()) {
		Console::println("MiniScript::registerStateMachineState(): state with id + " + to_string(state->getId()) + ", name '" + state->getName() + "' already registered.");
		return;
	}
	scriptStateMachineStates[state->getId()] = state;
}

void MiniScript::registerMethod(ScriptMethod* method) {
	auto scriptMethodsIt = scriptMethods.find(method->getMethodName());
	if (scriptMethodsIt != scriptMethods.end()) {
		Console::println("MiniScript::registerMethod(): method with name '" + method->getMethodName() + "' already registered.");
		return;
	}
	scriptMethods[method->getMethodName()] = method;
}

const MiniScript::ScriptVariable MiniScript::getVariable(const string& name) {
	auto scriptVariableIt = scriptState.variables.find(name);
	if (scriptVariableIt == scriptState.variables.end()) {
		Console::println("MiniScript::getVariable(): variable with name '" + name + "' does not exist.");
		return ScriptVariable();
	}
	return scriptVariableIt->second;
}

void MiniScript::setVariable(const string& name, const ScriptVariable& variable) {
	auto scriptVariableIt = scriptState.variables.find(name);
	if (scriptVariableIt != scriptState.variables.end()) {
		Console::println("MiniScript::getVariable(): variable with name '" + name + "' does exist.");
		return;
	}
	scriptState.variables[name] = variable;
}

void MiniScript::executeScriptLine() {
	if (scriptState.idx == -1 || scriptState.statementIdx == -1 || scriptState.running == false) return;
	auto& script = scripts[scriptState.idx];
	auto& statement = script.statements[scriptState.statementIdx];
	if (VERBOSE == true) Console::println("MiniScript::executeScriptLine(): @" + to_string(statement.line) + ": " + statement.statement);

	scriptState.statementIdx++;
	if (scriptState.statementIdx >= script.statements.size()) {
		scriptState.idx = -1;
		scriptState.statementIdx = -1;
		scriptState.state = STATE_WAIT_FOR_CONDITION;
	}

	string variable;
	string method;
	vector<string> arguments;
	parseScriptStatement(statement.statement, variable, method, arguments);
	auto returnValue = executeScriptStatement(method, arguments, statement);
	if (variable.empty() == false) scriptState.variables[variable] = returnValue;
}

void MiniScript::parseScriptStatement(const string& statement, string& variable, string& method, vector<string>& arguments) {
	if (VERBOSE == true) Console::println("MiniScript::parseScriptStatement(): " + statement);
	auto argumentStartIdx = string::npos;
	auto argumentEndIdx = string::npos;
	auto bracketCount = 0;
	auto quote = false;
	string argument;
	string quotedArgument;
	for (auto i = 0; i < statement.size(); i++) {
		auto c = statement[i];
		if (c == '"') {
			if (bracketCount == 0) {
				Console::println("MiniScript::parseScriptStatement(): quote without argument");
			}
			quote = quote == false?true:false;
			quotedArgument+= c;
		} else
		if (quote == false) {
			if (c == '=') {
				variable = StringTools::trim(method);
				method.clear();
			} else
			if (c == '(') {
				bracketCount++;
				if (bracketCount > 1) {
					argument+= c;
				}
			} else
			if (c == ')') {
				bracketCount--;
				if (bracketCount == 0) {
					if (quotedArgument.empty() == false || argument.empty() == false) {
						arguments.push_back(quotedArgument.empty() == false?quotedArgument:StringTools::trim(argument));
						argument.clear();
						quotedArgument.clear();
					}
				} else {
					argument+= c;
				}
			} else
			if (c == ',') {
				if (bracketCount == 1) {
					if (quotedArgument.empty() == false || argument.empty() == false) {
						arguments.push_back(quotedArgument.empty() == false?quotedArgument:StringTools::trim(argument));
						argument.clear();
						quotedArgument.clear();
					}
				} else {
					argument+= c;
				}
			} else
			if (bracketCount == 0) {
				method+= c;
			} else {
				argument+= c;
			}
		} else {
			quotedArgument+= c;
		}
	}
	method = StringTools::trim(method);
	variable = StringTools::trim(variable);
	if (bracketCount > 0) {
		Console::println("MiniScript::parseScriptStatement(): '" + statement + "': unbalanced bracket count: " + to_string(bracketCount) + " still open");
	}
}

MiniScript::ScriptVariable MiniScript::executeScriptStatement(const string& method, const vector<string>& arguments, const ScriptStatement& statement) {
	string argumentsString;
	for (auto& argument: arguments) argumentsString+= (argumentsString.empty() == false?",":"") + argument;
	if (VERBOSE == true) Console::println("MiniScript::executeScriptStatement(): " + method + "(" + argumentsString + ")");
	vector<ScriptVariable> argumentValues;
	ScriptVariable returnValue;
	// check if argument is a method calls return value
	for (auto& argument: arguments) {
		if (argument.empty() == false &&
			StringTools::startsWith(argument, "\"") == false &&
			StringTools::endsWith(argument, "\"") == false &&
			argument.find('(') != string::npos &&
			argument.find(')') != string::npos) {
			// method call, call method and put its return value into argument value
			string subVariable;
			string subMethod;
			vector<string> subArguments;
			parseScriptStatement(argument, subVariable, subMethod, subArguments);
			auto argumentValue = executeScriptStatement(subMethod, subArguments, statement);
			if (subVariable.empty() == false) scriptState.variables[subVariable] = argumentValue;
			argumentValues.push_back(argumentValue);
		} else
		// variable
		if (StringTools::startsWith(argument, "$") == true) {
			auto variableIt = scriptState.variables.find(argument);
			if (variableIt == scriptState.variables.end()) {
				Console::println("MiniScript::executeScriptStatement(): " + method + "(" + argument + "): variable: '" + argument + "' does not exist");
				argumentValues.push_back(ScriptVariable());
			} else {
				auto argumentValue = variableIt->second;
				argumentValues.push_back(argumentValue);
			}
		} else {
			// literal
			ScriptVariable argumentValue;
			if (StringTools::startsWith(argument, "\"") == true &&
				StringTools::endsWith(argument, "\"") == true) {
				setStringValue(argumentValue, StringTools::substring(argument, 1, argument.size() - 1));
			} else {
				setStringValue(argumentValue, argument);
			}
			argumentValues.push_back(argumentValue);
		}
	}
	auto scriptMethodsIt = scriptMethods.find(method);
	if (scriptMethodsIt != scriptMethods.end()) {
		auto scriptMethod = scriptMethodsIt->second;
		auto argumentIdx = 0;
		if (scriptMethod->isVariadic() == false) {
			for (auto& argumentType: scriptMethod->getArgumentTypes()) {
				auto argumentOk = true;
				switch(argumentType.type) {
					case TYPE_VOID:
						break;
					case TYPE_BOOLEAN:
						{
							bool booleanValue;
							argumentOk = getBooleanValue(argumentValues, argumentIdx, booleanValue, argumentType.optional);
						}
						break;
					case TYPE_INTEGER:
						{
							int64_t integerValue;
							argumentOk = getIntegerValue(argumentValues, argumentIdx, integerValue, argumentType.optional);
						}
						break;
					case TYPE_FLOAT:
						{
							float floatValue;
							argumentOk = getFloatValue(argumentValues, argumentIdx, floatValue, argumentType.optional);
						}
						break;
					case TYPE_STRING:
						{
							string stringValue;
							argumentOk = getStringValue(argumentValues, argumentIdx, stringValue, argumentType.optional);
						}
						break;
					case TYPE_VECTOR3:
						{
							Vector3 vector3Value;
							argumentOk = getVector3Value(argumentValues, argumentIdx, vector3Value, argumentType.optional);
							break;
						}
					case TYPE_TRANSFORMATIONS:
						{
							Transformations transformationsValue;
							argumentOk = getTransformationsValue(argumentValues, argumentIdx, transformationsValue, argumentType.optional);
							break;
						}
				}
				if (argumentOk == false) {
					Console::println(
						string("MiniScript::executeScriptStatement(): ") +
						"@" + to_string(statement.line) +
						": method '" + method + "'" +
						": argument value @ " + to_string(argumentIdx + 1) + ": expected " + getScriptVariableTypeAsString(argumentType.type) + ", but got: " + getScriptVariableAsString(argumentValues[argumentIdx]));
				}
				argumentIdx++;
			}
			if (argumentValues.size() > scriptMethod->getArgumentTypes().size()) {
				Console::println(
					string("MiniScript::executeScriptStatement(): ") +
					"@" + to_string(statement.line) +
					": method '" + method + "'" +
					": too many arguments: expected: " + to_string(scriptMethod->getArgumentTypes().size()) + ", got " + to_string(argumentValues.size()));
			}
		}
		scriptMethod->executeMethod(argumentValues, returnValue, statement);
		if (returnValue.type != scriptMethod->getReturnValueType()) {
			Console::println(
				string("MiniScript::executeScriptStatement(): ") +
				"@" + to_string(statement.line) +
				": method '" + method + "'" +
				": return value: expected " + getScriptVariableTypeAsString(scriptMethod->getReturnValueType()) + ", but got: " + getScriptVariableTypeAsString(returnValue.type));
		}
		return returnValue;
	} else {
		Console::println("MiniScript::executeScriptStatement(): unknown method: " + method + "(" + argumentsString + ")");
		startErrorScript();
	}
	return returnValue;
}

void MiniScript::emit(const string& condition) {
	if (VERBOSE == true) Console::println("MiniScript::emit(): " + condition);
	auto scriptIdx = 0;
	for (auto& script: scripts) {
		auto conditionMet = true;
		if (script.conditions.size() == 1 && script.conditions[0] == condition) {
			break;
		} else {
			scriptIdx++;
		}
	}
	if (scriptIdx == scripts.size()) {
		scriptIdx = -1;
		startErrorScript();
		return;
	}
	if (scriptState.idx == scriptIdx) {
		Console::println("MiniScript::emit(): script already running: " + condition);
		return;
	}
	//
	scriptState.forTimeStarted.clear();
	while (scriptState.conditionStack.empty() == false) scriptState.conditionStack.pop();
	while (scriptState.endTypeStack.empty() == false) scriptState.endTypeStack.pop();
	scriptState.id.clear();
	scriptState.idx = scriptIdx;
	scriptState.statementIdx = 0;
	scriptState.waitStarted = Time::getCurrentMillis();
	scriptState.waitTime = 0LL;
	scriptState.state = STATE_NEXT_STATEMENT;
}

void MiniScript::loadScript(const string& pathName, const string& fileName) {
	// shutdown methods and machine states
	for (auto& scriptMethodIt: scriptMethods) delete scriptMethodIt.second;
	for (auto& scriptStateMachineStateIt: scriptStateMachineStates) delete scriptStateMachineStateIt.second;
	scriptMethods.clear();
	scriptStateMachineStates.clear();

	// shutdown script state
	scriptState.variables.clear();
	scriptState.forTimeStarted.clear();
	while (scriptState.conditionStack.empty() == false) scriptState.conditionStack.pop();
	while (scriptState.endTypeStack.empty() == false) scriptState.endTypeStack.pop();
	scriptState.id.clear();
	scriptState.statementIdx = 0;
	scriptState.waitStarted = Time::getCurrentMillis();
	scriptState.waitTime = 0LL;
	scriptState.state = STATE_WAIT_FOR_CONDITION;
	scriptState.running = false;

	//
	registerStateMachineStates();
	registerMethods();
	registerVariables();

	//
	vector<string> scriptLines;
	try {
		FileSystem::getInstance()->getContentAsStringArray(pathName, fileName, scriptLines);
	} catch (FileSystemException& fse)	{
		Console::println("MiniScript::loadScript(): " + pathName + "/" + fileName + ": an error occurred: " + fse.what());
	}
	auto haveCondition = false;
	auto line = 1;
	auto statementIdx = 1;
	enum GotoStatementType { GOTOSTATEMENTTYPE_FOR, GOTOSTATEMENTTYPE_IF, GOTOSTATEMENTTYPE_ELSE, GOTOSTATEMENTTYPE_ELSEIF };
	struct GotoStatementStruct {
		GotoStatementType type;
		int statementIdx;
	};
	stack<GotoStatementStruct> gotoStatementStack;
	for (auto scriptLine: scriptLines) {
		scriptLine = StringTools::trim(scriptLine);
		if (StringTools::startsWith(scriptLine, "#") == true) {
			line++;
			continue;
		}
		if (haveCondition == false) {
			if (StringTools::startsWith(scriptLine, "on:") == true) {
				haveCondition = true;
				scriptLine = StringTools::trim(StringTools::substring(scriptLine, string("on:").size()));
				vector<string> conditions;
				StringTokenizer t;
				t.tokenize(scriptLine, ";");
				while (t.hasMoreTokens()) conditions.push_back(StringTools::trim(t.nextToken()));
				scripts.push_back({ .line = line, .conditions = conditions, .statements = {} });
				statementIdx = 0;
			} else {
				Console::println("MiniScript::MiniScript(): " + pathName + "/" + fileName + ": expecting 'on:' script condition");
			}
		} else {
			if (scriptLine == "end") {
				if (gotoStatementStack.empty() == false) {
					auto gotoStatementStackElement = gotoStatementStack.top();
					gotoStatementStack.pop();
					switch(gotoStatementStackElement.type) {
						case GOTOSTATEMENTTYPE_FOR:
							{
								scripts[scripts.size() - 1].statements.push_back({ .line = line, .statementIdx = statementIdx, .statement = scriptLine, .gotoStatementIdx = gotoStatementStackElement.statementIdx });
								scripts[scripts.size() - 1].statements[gotoStatementStackElement.statementIdx].gotoStatementIdx = scripts[scripts.size() - 1].statements.size();
							}
							break;
						case GOTOSTATEMENTTYPE_IF:
							{
								scripts[scripts.size() - 1].statements[gotoStatementStackElement.statementIdx].gotoStatementIdx = scripts[scripts.size() - 1].statements.size();
								scripts[scripts.size() - 1].statements.push_back({ .line = line, .statementIdx = statementIdx, .statement = scriptLine, .gotoStatementIdx = -1 });
							}
							break;
						case GOTOSTATEMENTTYPE_ELSE:
							{
								scripts[scripts.size() - 1].statements[gotoStatementStackElement.statementIdx].gotoStatementIdx = scripts[scripts.size() - 1].statements.size();
								scripts[scripts.size() - 1].statements.push_back({ .line = line, .statementIdx = statementIdx, .statement = scriptLine, .gotoStatementIdx = -1 });
							}
							break;
						case GOTOSTATEMENTTYPE_ELSEIF:
							{
								scripts[scripts.size() - 1].statements[gotoStatementStackElement.statementIdx].gotoStatementIdx = scripts[scripts.size() - 1].statements.size();
								scripts[scripts.size() - 1].statements.push_back({ .line = line, .statementIdx = statementIdx, .statement = scriptLine, .gotoStatementIdx = -1 });
							}
							break;
					}
				} else{
					haveCondition = false;
				}
			} else
			if (scriptLine == "else") {
				if (gotoStatementStack.empty() == false) {
					auto gotoStatementStackElement = gotoStatementStack.top();
					gotoStatementStack.pop();
					switch(gotoStatementStackElement.type) {
						case GOTOSTATEMENTTYPE_IF:
							{
								scripts[scripts.size() - 1].statements[gotoStatementStackElement.statementIdx].gotoStatementIdx = scripts[scripts.size() - 1].statements.size();
								scripts[scripts.size() - 1].statements.push_back({ .line = line, .statementIdx = statementIdx, .statement = scriptLine, .gotoStatementIdx = -1 });
							}
							break;
						case GOTOSTATEMENTTYPE_ELSEIF:
							{
								scripts[scripts.size() - 1].statements[gotoStatementStackElement.statementIdx].gotoStatementIdx = scripts[scripts.size() - 1].statements.size();
								scripts[scripts.size() - 1].statements.push_back({ .line = line, .statementIdx = statementIdx, .statement = scriptLine, .gotoStatementIdx = -1 });
							}
							break;
						default:
							Console::println("MiniScript::MiniScript(): " + pathName + "/" + fileName + ": else without if/elseif");
							break;
					}
					gotoStatementStack.push(
						{
							.type = GOTOSTATEMENTTYPE_ELSE,
							.statementIdx = statementIdx
						}
					);
				} else {
					Console::println("MiniScript::MiniScript(): " + pathName + "/" + fileName + ": else without if");
				}
			} else
			if (StringTools::regexMatch(scriptLine, "^elseif[\\s]*\\(.*\\)$") == true) {
				if (gotoStatementStack.empty() == false) {
					auto gotoStatementStackElement = gotoStatementStack.top();
					gotoStatementStack.pop();
					switch(gotoStatementStackElement.type) {
						case GOTOSTATEMENTTYPE_IF:
							{
								scripts[scripts.size() - 1].statements[gotoStatementStackElement.statementIdx].gotoStatementIdx = scripts[scripts.size() - 1].statements.size();
								scripts[scripts.size() - 1].statements.push_back({ .line = line, .statementIdx = statementIdx, .statement = scriptLine, .gotoStatementIdx = -1 });
							}
							break;
						case GOTOSTATEMENTTYPE_ELSEIF:
							{
								scripts[scripts.size() - 1].statements[gotoStatementStackElement.statementIdx].gotoStatementIdx = scripts[scripts.size() - 1].statements.size();
								scripts[scripts.size() - 1].statements.push_back({ .line = line, .statementIdx = statementIdx, .statement = scriptLine, .gotoStatementIdx = -1 });
							}
							break;
						default:
							Console::println("MiniScript::MiniScript(): " + pathName + "/" + fileName + ": elseif without if");
							break;
					}
					gotoStatementStack.push(
						{
							.type = GOTOSTATEMENTTYPE_ELSEIF,
							.statementIdx = statementIdx
						}
					);
				} else {
					Console::println("MiniScript::MiniScript(): " + pathName + "/" + fileName + ": elseif without if");
				}
			} else {
				if (StringTools::regexMatch(scriptLine, "^forTime[\\s]*\\(.*\\)$") == true ||
					StringTools::regexMatch(scriptLine, "^forCondition[\\s]*\\(.*\\)$") == true) {
					gotoStatementStack.push(
						{
							.type = GOTOSTATEMENTTYPE_FOR,
							.statementIdx = statementIdx
						}
					);
				} else
				if (StringTools::regexMatch(scriptLine, "^if[\\s]*\\(.*\\)$") == true) {
					gotoStatementStack.push(
						{
							.type = GOTOSTATEMENTTYPE_IF,
							.statementIdx = statementIdx
						}
					);
				}
				scripts.at(scripts.size() - 1).statements.push_back({ .line = line, .statementIdx = statementIdx, .statement = scriptLine, .gotoStatementIdx = -1 });
			}
			statementIdx++;
		}
		line++;
	}
	Console::println("MiniScript::loadScript(): " + pathName + "/" + fileName);
	for (auto& script: scripts) {
		for (auto& condition: script.conditions)
			Console::print(condition + "; ");
		Console::println();
		for (auto& scriptStatement: script.statements) {
			Console::println("\t" + to_string(scriptStatement.statementIdx) + ": " + scriptStatement.statement + (scriptStatement.gotoStatementIdx != -1?" (gotoStatement " + to_string(scriptStatement.gotoStatementIdx) + ")":""));
		}
	}

	//
	if (gotoStatementStack.empty() == false) {
		Console::println("MiniScript::loadScript(): " + pathName + "/" + fileName + ": unbalanced forXXX/if/elseif/else/end");
	}

	//
	Console::println("DUMPINFO:\n\n" + dumpInfo() + "\n\n");

	//
	startScript();
}

void MiniScript::startScript(int64_t delay) {
	if (VERBOSE == true) Console::println("MiniScript::startScript(): waiting " + to_string(delay) + "ms; starting script.");
	scriptState.variables.clear();
	scriptState.forTimeStarted.clear();
	while (scriptState.conditionStack.empty() == false) scriptState.conditionStack.pop();
	while (scriptState.endTypeStack.empty() == false) scriptState.endTypeStack.pop();
	scriptState.id.clear();
	registerVariables();
	scriptState.statementIdx = 0;
	scriptState.waitStarted = Time::getCurrentMillis();
	scriptState.waitTime = delay;
	scriptState.state = delay > 0LL?STATE_WAIT:STATE_WAIT_FOR_CONDITION;
	scriptState.running = true;
}

int MiniScript::determineScriptIdxToStart() {
	if (VERBOSE == true) Console::println("MiniScript::determineScriptIdxToStart()");
	auto currentScriptStateScript = scriptState.state;
	auto nothingScriptIdx = -1;
	auto scriptIdx = 0;
	for (auto& script: scripts) {
		auto conditionMet = true;
		if (script.conditions.size() == 1 && script.conditions[0] == "nothing") {
			nothingScriptIdx = scriptIdx;
		} else
		if (script.conditions.size() == 1 &&
			script.conditions[0].find('(') == string::npos &&
			script.conditions[0].rfind(')') == string::npos) {
			// user emit condition
		} else {
			for (auto condition: script.conditions) {
				string variable;
				string method;
				vector<string> arguments;
				parseScriptStatement(condition, variable, method, arguments);
				auto returnValue = executeScriptStatement(
					method,
					arguments,
					{
						.line = 0,
						.statementIdx = 0,
						.statement = condition,
						.gotoStatementIdx = -0
					}
				);
				if (returnValue.type != ScriptVariableType::TYPE_BOOLEAN) {
					Console::println("MiniScript::determineScriptIdxToStart(): " + condition + ": expecting boolean return value");
					conditionMet = false;
					break;
				} else
				if (returnValue.booleanValue == false) {
					conditionMet = false;
					break;
				}
			}
			if (conditionMet == false) {
				if (VERBOSE == true) {
					Console::print("MiniScript::determineScriptIdxToStart(): ");
					for (auto condition: script.conditions) Console::print(condition + "; ");
					Console::println(": FAILED");
				}	
				scriptIdx++;
				continue;
			}
			if (VERBOSE == true) {
				Console::print("MiniScript::determineScriptIdxToStart(): ");
				for (auto condition: script.conditions) Console::print(condition + "; ");
				Console::println(": OK");
			}
			scriptState.state = currentScriptStateScript;
			return scriptIdx;
		}	
		scriptIdx++;
	}
	scriptState.state = currentScriptStateScript;
	return nothingScriptIdx;
}

const string MiniScript::dumpInfo() {
	string result;
	result+="State Machine States:\n";
	{
		vector<string> states;
		for (auto& scriptStateMachineStateIt: scriptStateMachineStates) {
			string state;
			state = scriptStateMachineStateIt.second->getName() + "(" + to_string(scriptStateMachineStateIt.second->getId()) + ")";
			states.push_back(state);
		}
		sort(states.begin(), states.end());
		for (auto& state: states) result+= state+ "\n";
	}
	result+= "\n";
	result+= "Methods:\n";
	{
		vector<string> methods;
		for (auto& scriptMethodIt: scriptMethods) {
			auto scriptMethod = scriptMethodIt.second;
			string method;
			method+= scriptMethod->getMethodName();
			method+= "(";
			auto argumentIdx = 0;
			if (scriptMethod->isVariadic() == true) {
				method+="...";
			} else {
				for (auto& argumentType: scriptMethod->getArgumentTypes()) {
					if (argumentIdx > 0) method+= ", ";
					method+= argumentType.name + ": " + getScriptVariableTypeAsString(argumentType.type);
					if (argumentType.optional == true) {
						method+= "(OPTIONAL)";
					}
					argumentIdx++;
				}
			}
			method+= "): ";
			method+= getScriptVariableTypeAsString(scriptMethod->getReturnValueType());
			methods.push_back(method);
		}
		sort(methods.begin(), methods.end());
		for (auto& method: methods) result+= method + "\n";
	}
	result+= "\n";
	result+= "Variables:\n";
	{
		vector<string> variables;
		for (auto& scriptVariableIt: scriptState.variables) {
			string variable;
			auto& scriptVariable = scriptVariableIt.second;
			variable+= scriptVariableIt.first + " = " + getScriptVariableAsString(scriptVariable);
			variables.push_back(variable);
		}
		sort(variables.begin(), variables.end());
		for (auto& variable: variables) result+= variable + "\n";
	}
	return result;
}

void MiniScript::registerStateMachineStates() {
	// base
	{
		//
		class ScriptStateNextStatement: public ScriptStateMachineState {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptStateNextStatement(MiniScript* miniScript): ScriptStateMachineState(), miniScript(miniScript) {}
			virtual const string getName() override {
				return "STATE_NEXT_STATEMENT";
			}
			virtual int getId() override {
				return STATE_NEXT_STATEMENT;
			}
			virtual void execute() override {
				if (miniScript->scriptState.statementIdx == -1) {
					miniScript->scriptState.state = STATE_WAIT_FOR_CONDITION;
					return;
				}
				miniScript->executeScriptLine();
			}
		};
		registerStateMachineState(new ScriptStateNextStatement(this));
	}
	{
		//
		class ScriptStateWait: public ScriptStateMachineState {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptStateWait(MiniScript* miniScript): ScriptStateMachineState(), miniScript(miniScript) {}
			virtual const string getName() override {
				return "STATE_WAIT";
			}
			virtual int getId() override {
				return STATE_WAIT;
			}
			virtual void execute() override {
				auto now = Time::getCurrentMillis();
				if (now > miniScript->scriptState.waitStarted + miniScript->scriptState.waitTime) {
					miniScript->scriptState.state = STATE_NEXT_STATEMENT;
				}
			}
		};
		registerStateMachineState(new ScriptStateWait(this));
	}
	{
		//
		class ScriptStateWaitForCondition: public ScriptStateMachineState {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptStateWaitForCondition(MiniScript* miniScript): ScriptStateMachineState(), miniScript(miniScript) {}
			virtual const string getName() override {
				return "STATE_WAIT_FOR_CONDITION";
			}
			virtual int getId() override {
				return STATE_WAIT_FOR_CONDITION;
			}
			virtual void execute() override {
				auto now = Time::getCurrentMillis();
				if (now < miniScript->scriptState.waitStarted + miniScript->scriptState.waitTime) {
					return;
				}
				auto scriptIdxToStart = miniScript->determineScriptIdxToStart();
				if (scriptIdxToStart == -1) {
					miniScript->scriptState.waitStarted = now;
					miniScript->scriptState.waitTime = 1000LL;
					return;
				}
				miniScript->scriptState.forTimeStarted.clear();
				miniScript->scriptState.id.clear();
				miniScript->scriptState.idx = scriptIdxToStart;
				miniScript->scriptState.statementIdx = 0;
				miniScript->scriptState.waitStarted = Time::getCurrentMillis();
				miniScript->scriptState.waitTime = 0LL;
				miniScript->scriptState.state = STATE_NEXT_STATEMENT;

			}
		};
		registerStateMachineState(new ScriptStateWaitForCondition(this));
	}
}

void MiniScript::registerMethods() {
	// script base methods
	{
		//
		class ScriptMethodEnd: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodEnd(MiniScript* miniScript): ScriptMethod(), miniScript(miniScript) {}
			const string getMethodName() override {
				return "end";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				if (miniScript->scriptState.endTypeStack.empty() == true) {
					Console::println("ScriptMethodEnd::executeMethod(): end without forXXX/if");
					miniScript->startErrorScript();
					return;
				}
				auto endType = miniScript->scriptState.endTypeStack.top();
				miniScript->scriptState.endTypeStack.pop();
				switch(endType) {
					case ScriptState::ENDTYPE_FOR:
						// no op
						break;
					case ScriptState::ENDTYPE_IF:
						miniScript->scriptState.conditionStack.pop();
						break;
				}
				if (statement.gotoStatementIdx != STATE_NONE) {
					miniScript->scriptState.statementIdx = statement.gotoStatementIdx;
					miniScript->scriptState.state = STATE_NEXT_STATEMENT;
				}
			}
		};
		registerMethod(new ScriptMethodEnd(this));
	}
	{
		//
		class ScriptMethodForTime: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodForTime(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_INTEGER, .name = "time", .optional = false }
					}
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "forTime";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t time;
				if (miniScript->getIntegerValue(argumentValues, 0, time) == false) {
					Console::println("ScriptMethodForTime::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: integer expected");
					miniScript->startErrorScript();
					return;
				}
				//
				auto now = Time::getCurrentMillis();
				auto timeWaitStarted = now;
				auto forTimeStartedIt = miniScript->scriptState.forTimeStarted.find(statement.line);
				if (forTimeStartedIt == miniScript->scriptState.forTimeStarted.end()) {
					miniScript->scriptState.forTimeStarted[statement.line] = timeWaitStarted;
				} else {
					timeWaitStarted = forTimeStartedIt->second;
				}
				//
				if (Time::getCurrentMillis() > timeWaitStarted + time) {
					miniScript->scriptState.forTimeStarted.erase(statement.line);
					miniScript->scriptState.state = STATE_NEXT_STATEMENT;
					miniScript->scriptState.statementIdx = statement.gotoStatementIdx;
				} else {
					miniScript->scriptState.endTypeStack.push(ScriptState::ENDTYPE_FOR);
				}
			}
		};
		registerMethod(new ScriptMethodForTime(this));
	}
	{
		//
		class ScriptMethodForCondition: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodForCondition(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_BOOLEAN, .name = "condition", .optional = false }
					}
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "forCondition";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				bool booleanValue;
				if (miniScript->getBooleanValue(argumentValues, 0, booleanValue, false) == false) {
					Console::println("ScriptMethodForCondition::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: boolean expected");
					miniScript->startErrorScript();
					return;
				}
				//
				auto now = Time::getCurrentMillis();
				if (booleanValue == false) {
					miniScript->scriptState.state = STATE_NEXT_STATEMENT;
					miniScript->scriptState.statementIdx = statement.gotoStatementIdx;
				} else {
					miniScript->scriptState.endTypeStack.push(ScriptState::ENDTYPE_FOR);
				}
			}
		};
		registerMethod(new ScriptMethodForCondition(this));
	}
	{
		//
		class ScriptMethodIfCondition: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodIfCondition(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_BOOLEAN, .name = "condition", .optional = false }
					}
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "if";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				bool booleanValue;
				if (miniScript->getBooleanValue(argumentValues, 0, booleanValue, false) == false) {
					Console::println("ScriptMethodIfCondition::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: boolean expected");
					miniScript->startErrorScript();
					return;
				}
				//
				miniScript->scriptState.endTypeStack.push(ScriptState::ENDTYPE_IF);
				//
				miniScript->scriptState.conditionStack.push(booleanValue);
				if (booleanValue == false) {
					miniScript->scriptState.state = STATE_NEXT_STATEMENT;
					miniScript->scriptState.statementIdx = statement.gotoStatementIdx;
				}
			}
		};
		registerMethod(new ScriptMethodIfCondition(this));
	}
	{
		//
		class ScriptMethodElseIfCondition: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodElseIfCondition(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_BOOLEAN, .name = "condition", .optional = false }
					}
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "elseif";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				bool booleanValue;
				if (miniScript->getBooleanValue(argumentValues, 0, booleanValue, false) == false) {
					Console::println("ScriptMethodElseIfCondition::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: boolean expected");
					miniScript->startErrorScript();
					return;
				}
				if (miniScript->scriptState.conditionStack.empty() == true) {
					Console::println("ScriptMethodElseIfCondition::executeMethod(): elseif without if");
					miniScript->startErrorScript();
					return;
				}
				//
				auto conditionStackElement = miniScript->scriptState.conditionStack.top();
				if (conditionStackElement == false) {
					miniScript->scriptState.conditionStack.pop();
					miniScript->scriptState.conditionStack.push(booleanValue);
				}
				if (conditionStackElement == true || booleanValue == false) {
					miniScript->scriptState.state = STATE_NEXT_STATEMENT;
					miniScript->scriptState.statementIdx = statement.gotoStatementIdx;
				}
			}
		};
		registerMethod(new ScriptMethodElseIfCondition(this));
	}
	{
		//
		class ScriptMethodElse: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodElse(MiniScript* miniScript): ScriptMethod(), miniScript(miniScript) {}
			const string getMethodName() override {
				return "else";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				if (miniScript->scriptState.conditionStack.empty() == true) {
					Console::println("ScriptMethodElse::executeMethod(): else without if");
					miniScript->startErrorScript();
					return;
				}
				auto conditionStackElement = miniScript->scriptState.conditionStack.top();
				if (conditionStackElement == true) {
					miniScript->scriptState.statementIdx = statement.gotoStatementIdx;
					miniScript->scriptState.state = STATE_NEXT_STATEMENT;
				}
			}
		};
		registerMethod(new ScriptMethodElse(this));
	}
	{
		//
		class ScriptMethodScriptWaitForCondition: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodScriptWaitForCondition(MiniScript* miniScript): miniScript(miniScript) {}
			const string getMethodName() override {
				return "script.waitForCondition";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				// script bindings
				miniScript->scriptState.state = STATE_WAIT_FOR_CONDITION;
				miniScript->scriptState.waitStarted = Time::getCurrentMillis();
				miniScript->scriptState.waitTime = 1000LL;
			}
		};
		registerMethod(new ScriptMethodScriptWaitForCondition(this));
		//
		class ScriptMethodScriptWait: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodScriptWait(MiniScript* miniScript):
				ScriptMethod({
					{.type = ScriptVariableType::TYPE_INTEGER, .name = "time", .optional = false }
				}),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "script.wait";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t time;
				if (miniScript->getIntegerValue(argumentValues, 0, time) == true) {
					miniScript->scriptState.state = STATE_WAIT;
					miniScript->scriptState.waitStarted = Time::getCurrentMillis();
					miniScript->scriptState.waitTime = time;
				} else {
					Console::println("ScriptMethodScriptWait::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: integer expected");
				}
			}
		};
		registerMethod(new ScriptMethodScriptWait(this));
	}
	{
		//
		class ScriptMethodEmit: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodEmit(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_STRING, .name = "condition", .optional = false }
					}
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "emit";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				string condition;
				if (MiniScript::getStringValue(argumentValues, 0, condition, false) == true) {
					miniScript->emit(condition);
				} else {
					Console::println("ScriptMethodScriptWait::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: string expected");
					miniScript->startErrorScript();
					return;
				}
			}
		};
		registerMethod(new ScriptMethodEmit(this));
	}
	{
		//
		class ScriptMethodConsoleLog: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodConsoleLog(MiniScript* miniScript): ScriptMethod(), miniScript(miniScript) {}
			const string getMethodName() override {
				return "console.log";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				for (auto& argumentValue: argumentValues) {
					Console::print(MiniScript::getScriptVariableValueString(argumentValue));
				}
				Console::println();
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodConsoleLog(this));
	}
	{
		//
		class ScriptMethodStop: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodStop(MiniScript* miniScript): ScriptMethod(), miniScript(miniScript) {}
			const string getMethodName() override {
				return "stop";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				miniScript->scriptState.running = false;
			}
		};
		registerMethod(new ScriptMethodStop(this));
	}
	// equals
	{
		//
		class ScriptMethodEquals: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodEquals(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_BOOLEAN), miniScript(miniScript) {}
			const string getMethodName() override {
				return "equals";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				for (auto i = 1; i < argumentValues.size(); i++) {
					if (MiniScript::getScriptVariableValueString(argumentValues[0]) != MiniScript::getScriptVariableValueString(argumentValues[i])) {
						MiniScript::setBooleanValue(returnValue, false);
						return;
					}
				}
				MiniScript::setBooleanValue(returnValue, true);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodEquals(this));
	}
	// int methods
	{
		//
		class ScriptMethodInt: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodInt(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_INTEGER, .name = "int", .optional = false }
					},
					ScriptVariableType::TYPE_INTEGER
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "int";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t integerValue;
				if (MiniScript::getIntegerValue(argumentValues, 0, integerValue, false) == true) {
					MiniScript::setIntegerValue(returnValue, integerValue);
				} else {
					Console::println("ScriptMethodInt::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: integer expected");
					miniScript->startErrorScript();
					return;
				}
			}
		};
		registerMethod(new ScriptMethodInt(this));
	}
	{
		//
		class ScriptMethodGreater: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodGreater(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_INTEGER, .name = "a", .optional = false },
						{.type = ScriptVariableType::TYPE_INTEGER, .name = "b", .optional = false }
					},
					ScriptVariableType::TYPE_BOOLEAN),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "greater";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t integerValueA;
				int64_t integerValueB;
				if (MiniScript::getIntegerValue(argumentValues, 0, integerValueA, false) == true &&
					MiniScript::getIntegerValue(argumentValues, 1, integerValueB, false) == true) {
					MiniScript::setBooleanValue(returnValue, integerValueA > integerValueB);
				} else {
					Console::println("ScriptMethodGreater::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: integer expected, @ argument 1: integer expected");
					miniScript->startErrorScript();
					return;
				}
			}
		};
		registerMethod(new ScriptMethodGreater(this));
	}
	{
		//
		class ScriptMethodLesser: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodLesser(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_INTEGER, .name = "a", .optional = false },
						{.type = ScriptVariableType::TYPE_INTEGER, .name = "b", .optional = false }
					},
					ScriptVariableType::TYPE_BOOLEAN),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "lesser";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t integerValueA;
				int64_t integerValueB;
				if (MiniScript::getIntegerValue(argumentValues, 0, integerValueA, false) == true &&
					MiniScript::getIntegerValue(argumentValues, 1, integerValueB, false) == true) {
					MiniScript::setBooleanValue(returnValue, integerValueA < integerValueB);
				} else {
					Console::println("ScriptMethodLesser::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: integer expected, @ argument 1: integer expected");
					miniScript->startErrorScript();
					return;
				}
			}
		};
		registerMethod(new ScriptMethodLesser(this));
	}
	{
		//
		class ScriptMethodAdd: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodAdd(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_INTEGER), miniScript(miniScript) {}
			const string getMethodName() override {
				return "add";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t result = 0LL;
				for (auto i = 0; i < argumentValues.size(); i++) {
					int64_t integerValue;
					if (MiniScript::getIntegerValue(argumentValues, i, integerValue, false) == true) {
						result+= integerValue;
					} else {
						Console::println("ScriptMethodAdd::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": integer expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setIntegerValue(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodAdd(this));
	}
	{
		//
		class ScriptMethodSub: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodSub(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_INTEGER), miniScript(miniScript) {}
			const string getMethodName() override {
				return "sub";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t result = 0LL;
				{
					int64_t integerValue;
					if (MiniScript::getIntegerValue(argumentValues, 0, integerValue, false) == true) {
						result = integerValue;
					} else {
						Console::println("ScriptMethodSub::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(0) + ": integer expected");
						miniScript->startErrorScript();
						return;
					}
				}
				for (auto i = 1; i < argumentValues.size(); i++) {
					int64_t integerValue;
					if (MiniScript::getIntegerValue(argumentValues, i, integerValue, false) == true) {
						result-= integerValue;
					} else {
						Console::println("ScriptMethodSub::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": integer expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setIntegerValue(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodSub(this));
	}
	{
		//
		class ScriptMethodMul: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodMul(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_INTEGER), miniScript(miniScript) {}
			const string getMethodName() override {
				return "mul";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t result = 0LL;
				{
					int64_t integerValue;
					if (MiniScript::getIntegerValue(argumentValues, 0, integerValue, false) == true) {
						result = integerValue;
					} else {
						Console::println("ScriptMethodMul::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(0) + ": integer expected");
						miniScript->startErrorScript();
						return;
					}
				}
				for (auto i = 1; i < argumentValues.size(); i++) {
					int64_t integerValue;
					if (MiniScript::getIntegerValue(argumentValues, i, integerValue, false) == true) {
						result*= integerValue;
					} else {
						Console::println("ScriptMethodMul::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": integer expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setIntegerValue(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodMul(this));
	}
	{
		//
		class ScriptMethodDiv: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodDiv(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_INTEGER), miniScript(miniScript) {}
			const string getMethodName() override {
				return "div";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t result = 0LL;
				{
					int64_t integerValue;
					if (MiniScript::getIntegerValue(argumentValues, 0, integerValue, false) == true) {
						result = integerValue;
					} else {
						Console::println("ScriptMethodDiv::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(0) + ": integer expected");
						miniScript->startErrorScript();
						return;
					}
				}
				for (auto i = 1; i < argumentValues.size(); i++) {
					int64_t integerValue;
					if (MiniScript::getIntegerValue(argumentValues, i, integerValue, false) == true) {
						result/= integerValue;
					} else {
						Console::println("ScriptMethodDiv::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": integer expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setIntegerValue(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodDiv(this));
	}
	// float methods
	{
		//
		class ScriptMethodFloat: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodFloat(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_FLOAT, .name = "float", .optional = false }
					},
					ScriptVariableType::TYPE_FLOAT
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "float";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				float floatValue;
				if (MiniScript::getFloatValue(argumentValues, 0, floatValue, false) == true) {
					MiniScript::setFloatValue(returnValue, floatValue);
				} else {
					Console::println("ScriptMethodFloat::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: float expected");
					miniScript->startErrorScript();
					return;
				}
			}
		};
		registerMethod(new ScriptMethodFloat(this));
	}
	{
		//
		class ScriptMethodFGreater: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodFGreater(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_FLOAT, .name = "a", .optional = false },
						{.type = ScriptVariableType::TYPE_FLOAT, .name = "b", .optional = false }
					},
					ScriptVariableType::TYPE_BOOLEAN),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "fgreater";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				float floatValueA;
				float floatValueB;
				if (MiniScript::getFloatValue(argumentValues, 0, floatValueA, false) == true &&
					MiniScript::getFloatValue(argumentValues, 1, floatValueB, false) == true) {
					MiniScript::setBooleanValue(returnValue, floatValueA > floatValueB);
				} else {
					Console::println("ScriptMethodFGreater::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: float expected, @ argument 1: float expected");
					miniScript->startErrorScript();
					return;
				}
			}
		};
		registerMethod(new ScriptMethodFGreater(this));
	}
	{
		//
		class ScriptMethodFLesser: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodFLesser(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_FLOAT, .name = "a", .optional = false },
						{.type = ScriptVariableType::TYPE_FLOAT, .name = "b", .optional = false }
					},
					ScriptVariableType::TYPE_BOOLEAN),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "flesser";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				float floatValueA;
				float floatValueB;
				if (MiniScript::getFloatValue(argumentValues, 0, floatValueA, false) == true &&
					MiniScript::getFloatValue(argumentValues, 1, floatValueB, false) == true) {
					MiniScript::setBooleanValue(returnValue, floatValueA < floatValueB);
				} else {
					Console::println("ScriptMethodFLesser::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: float expected, @ argument 1: float expected");
					miniScript->startErrorScript();
					return;
				}
			}
		};
		registerMethod(new ScriptMethodFLesser(this));
	}
	{
		//
		class ScriptMethodFAdd: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodFAdd(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_FLOAT), miniScript(miniScript) {}
			const string getMethodName() override {
				return "fadd";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				float result = 0.0f;
				for (auto i = 0; i < argumentValues.size(); i++) {
					float floatValue;
					if (MiniScript::getFloatValue(argumentValues, i, floatValue, false) == true) {
						result+= floatValue;
					} else {
						Console::println("ScriptMethodFAdd::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": float expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setFloatValue(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodFAdd(this));
	}
	{
		//
		class ScriptMethodFSub: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodFSub(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_FLOAT), miniScript(miniScript) {}
			const string getMethodName() override {
				return "fsub";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				float result = 0.0f;
				{
					float floatValue;
					if (MiniScript::getFloatValue(argumentValues, 0, floatValue, false) == true) {
						result = floatValue;
					} else {
						Console::println("ScriptMethodFSub::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(0) + ": float expected");
						miniScript->startErrorScript();
						return;
					}
				}
				for (auto i = 1; i < argumentValues.size(); i++) {
					float floatValue;
					if (MiniScript::getFloatValue(argumentValues, i, floatValue, false) == true) {
						result-= floatValue;
					} else {
						Console::println("ScriptMethodFSub::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": float expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setFloatValue(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodFSub(this));
	}
	{
		//
		class ScriptMethodFMul: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodFMul(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_FLOAT), miniScript(miniScript) {}
			const string getMethodName() override {
				return "fmul";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				float result = 0.0f;
				{
					float floatValue;
					if (MiniScript::getFloatValue(argumentValues, 0, floatValue, false) == true) {
						result = floatValue;
					} else {
						Console::println("ScriptMethodFMul::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(0) + ": float expected");
						miniScript->startErrorScript();
						return;
					}
				}
				for (auto i = 1; i < argumentValues.size(); i++) {
					float floatValue;
					if (MiniScript::getFloatValue(argumentValues, i, floatValue, false) == true) {
						result*= floatValue;
					} else {
						Console::println("ScriptMethodFMul::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": float expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setFloatValue(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodFMul(this));
	}
	{
		//
		class ScriptMethodFDiv: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodFDiv(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_FLOAT), miniScript(miniScript) {}
			const string getMethodName() override {
				return "fdiv";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				float result = 0.0f;
				{
					float floatValue;
					if (MiniScript::getFloatValue(argumentValues, 0, floatValue, false) == true) {
						result = floatValue;
					} else {
						Console::println("ScriptMethodFDiv::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(0) + ": float expected");
						miniScript->startErrorScript();
						return;
					}
				}
				for (auto i = 1; i < argumentValues.size(); i++) {
					float floatValue;
					if (MiniScript::getFloatValue(argumentValues, i, floatValue, false) == true) {
						result/= floatValue;
					} else {
						Console::println("ScriptMethodFDiv::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": float expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setFloatValue(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodFDiv(this));
	}
	// vector3 methods
	{
		//
		class ScriptMethodVec3: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_FLOAT, .name = "x", .optional = false },
						{.type = ScriptVariableType::TYPE_FLOAT, .name = "y", .optional = false },
						{.type = ScriptVariableType::TYPE_FLOAT, .name = "z", .optional = false }
					},
					ScriptVariableType::TYPE_VECTOR3
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 result;
				float xValue;
				float yValue;
				float zValue;
				if (MiniScript::getFloatValue(argumentValues, 0, xValue, false) == true &&
					MiniScript::getFloatValue(argumentValues, 1, yValue, false) == true &&
					MiniScript::getFloatValue(argumentValues, 2, zValue, false) == true) {
					MiniScript::setVector3Value(returnValue, Vector3(xValue, yValue, zValue));
				} else {
					Console::println("ScriptMethodVec3::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: float expected, @ argument 1: float expected, @ argument 2: float expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodVec3(this));
	}
	{
		//
		class ScriptMethodVec3ComputeLength: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3ComputeLength(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "vec3", .optional = false }
					},
					ScriptVariableType::TYPE_FLOAT),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.computeLength";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 vec3;
				if (MiniScript::getVector3Value(argumentValues, 0, vec3, false) == true) {
					MiniScript::setFloatValue(returnValue, vec3.computeLength());
				} else {
					Console::println("ScriptMethodVec3ComputeLength::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodVec3ComputeLength(this));
	}
	{
		//
		class ScriptMethodVec3ComputeLengthSquared: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3ComputeLengthSquared(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "vec3", .optional = false }
					},
					ScriptVariableType::TYPE_FLOAT),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.computeLengthSquared";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 vec3;
				if (MiniScript::getVector3Value(argumentValues, 0, vec3, false) == true) {
					MiniScript::setFloatValue(returnValue, vec3.computeLengthSquared());
				} else {
					Console::println("ScriptMethodVec3ComputeLengthSquared::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodVec3ComputeLengthSquared(this));
	}
	{
		//
		class ScriptMethodVec3Add: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3Add(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_VECTOR3), miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.add";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 result;
				for (auto i = 0; i < argumentValues.size(); i++) {
					Vector3 vector3Value;
					if (MiniScript::getVector3Value(argumentValues, i, vector3Value, false) == true) {
						result+= vector3Value;
					} else {
						Console::println("ScriptMethodVec3Add::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": vector3 expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setVector3Value(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodVec3Add(this));
	}
	{
		//
		class ScriptMethodVec3Sub: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3Sub(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_VECTOR3), miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.sub";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 result;
				{
					Vector3 vector3Value;
					if (MiniScript::getVector3Value(argumentValues, 0, vector3Value, false) == true) {
						result = vector3Value;
					} else {
						Console::println("ScriptMethodVec3Sub::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(0) + ": vector3 expected");
						miniScript->startErrorScript();
						return;
					}
				}
				for (auto i = 1; i < argumentValues.size(); i++) {
					Vector3 vector3Value;
					if (MiniScript::getVector3Value(argumentValues, i, vector3Value, false) == true) {
						result-= vector3Value;
					} else {
						Console::println("ScriptMethodVec3Sub::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": vector3 expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setVector3Value(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodVec3Sub(this));
	}
	{
		//
		class ScriptMethodVec3Mul: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3Mul(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_VECTOR3), miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.mul";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 result;
				{
					Vector3 vector3Value;
					if (MiniScript::getVector3Value(argumentValues, 0, vector3Value, false) == true) {
						result = vector3Value;
					} else {
						Console::println("ScriptMethodVec3Mul::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(0) + ": vector3 expected");
						miniScript->startErrorScript();
						return;
					}
				}
				for (auto i = 1; i < argumentValues.size(); i++) {
					float floatValue;
					Vector3 vector3Value;
					if (MiniScript::getVector3Value(argumentValues, i, vector3Value, false) == true) {
						result*= vector3Value;
					} else
					if (MiniScript::getFloatValue(argumentValues, i, floatValue, false) == true) {
						result*= floatValue;
					} else {
						Console::println("ScriptMethodVec3Mul::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": vector3 or float expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setVector3Value(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodVec3Mul(this));
	}
	{
		//
		class ScriptMethodVec3Div: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3Div(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_VECTOR3), miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.div";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 result;
				{
					Vector3 vector3Value;
					if (MiniScript::getVector3Value(argumentValues, 0, vector3Value, false) == true) {
						result = vector3Value;
					} else {
						Console::println("ScriptMethodVec3Div::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(0) + ": vector3 expected");
						miniScript->startErrorScript();
						return;
					}
				}
				for (auto i = 1; i < argumentValues.size(); i++) {
					float floatValue;
					Vector3 vector3Value;
					if (MiniScript::getVector3Value(argumentValues, i, vector3Value, false) == true) {
						result/= vector3Value;
					} else
					if (MiniScript::getFloatValue(argumentValues, i, floatValue, false) == true) {
						result/= floatValue;
					} else {
						Console::println("ScriptMethodVec3Div::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": vector3 or float expected");
						miniScript->startErrorScript();
						return;
					}
				}
				MiniScript::setVector3Value(returnValue, result);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodVec3Div(this));
	}
	{
		//
		class ScriptMethodVec3ComputeDotProduct: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3ComputeDotProduct(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "a", .optional = false },
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "b", .optional = false }
					},
					ScriptVariableType::TYPE_FLOAT),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.computeDotProduct";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 a;
				Vector3 b;
				if (MiniScript::getVector3Value(argumentValues, 0, a, false) == true &&
					MiniScript::getVector3Value(argumentValues, 1, b, false) == true) {
					MiniScript::setFloatValue(returnValue, Vector3::computeDotProduct(a, b));
				} else {
					Console::println("ScriptMethodVec3ComputeDotProduct::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: vector3 expected, @ argument 1: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodVec3ComputeDotProduct(this));
	}
	{
		//
		class ScriptMethodVec3ComputeCrossProduct: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3ComputeCrossProduct(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "a", .optional = false },
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "b", .optional = false }
					},
					ScriptVariableType::TYPE_VECTOR3),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.computeCrossProduct";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 a;
				Vector3 b;
				if (MiniScript::getVector3Value(argumentValues, 0, a, false) == true &&
					MiniScript::getVector3Value(argumentValues, 1, b, false) == true) {
					MiniScript::setVector3Value(returnValue, Vector3::computeCrossProduct(a, b));
				} else {
					Console::println("ScriptMethodVec3ComputeCrossProduct::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: vector3 expected, @ argument 1: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodVec3ComputeCrossProduct(this));
	}
	{
		//
		class ScriptMethodVec3Normalize: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3Normalize(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "vec3", .optional = false },
					},
					ScriptVariableType::TYPE_VECTOR3),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.normalize";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 vec3;
				if (MiniScript::getVector3Value(argumentValues, 0, vec3, false) == true) {
					MiniScript::setVector3Value(returnValue, vec3.normalize());
				} else {
					Console::println("ScriptMethodVec3Normalize::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodVec3Normalize(this));
	}
	{
		//
		class ScriptMethodVec3ComputeAngle: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3ComputeAngle(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "a", .optional = false },
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "b", .optional = false },
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "n", .optional = false },
					},
					ScriptVariableType::TYPE_FLOAT),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.computeAngle";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 a;
				Vector3 b;
				Vector3 n;
				if (MiniScript::getVector3Value(argumentValues, 0, a, false) == true &&
					MiniScript::getVector3Value(argumentValues, 1, b, false) == true &&
					MiniScript::getVector3Value(argumentValues, 2, n, false) == true) {
					MiniScript::setFloatValue(returnValue, Vector3::computeAngle(a, b, n));
				} else {
					Console::println("ScriptMethodVec3ComputeAngle::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: vector3 expected, @ argument 1: vector3 expected @ argument 2: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodVec3ComputeAngle(this));
	}
	{
		//
		class ScriptMethodVec3GetX: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3GetX(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "vec3", .optional = false },
					},
					ScriptVariableType::TYPE_FLOAT),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.getX";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 vec3;
				if (MiniScript::getVector3Value(argumentValues, 0, vec3, false) == true) {
					MiniScript::setFloatValue(returnValue, vec3.getX());
				} else {
					Console::println("ScriptMethodVec3GetX::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodVec3GetX(this));
	}
	{
		//
		class ScriptMethodVec3GetY: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3GetY(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "vec3", .optional = false },
					},
					ScriptVariableType::TYPE_FLOAT),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.getY";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 vec3;
				if (MiniScript::getVector3Value(argumentValues, 0, vec3, false) == true) {
					MiniScript::setFloatValue(returnValue, vec3.getY());
				} else {
					Console::println("ScriptMethodVec3GetY::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodVec3GetY(this));
	}
	{
		//
		class ScriptMethodVec3GetZ: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodVec3GetZ(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "vec3", .optional = false },
					},
					ScriptVariableType::TYPE_FLOAT),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "vec3.getZ";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Vector3 vec3;
				if (MiniScript::getVector3Value(argumentValues, 0, vec3, false) == true) {
					MiniScript::setFloatValue(returnValue, vec3.getZ());
				} else {
					Console::println("ScriptMethodVec3GetZ::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodVec3GetZ(this));
	}
	// transformations
	{
		//
		class ScriptMethodTransformations: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodTransformations(MiniScript* miniScript):
				ScriptMethod(
					{
					},
					ScriptVariableType::TYPE_TRANSFORMATIONS),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "transformations";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Transformations transformations;
				Vector3 vec3Value;
				if (argumentValues.size() >= 1) {
					if (MiniScript::getVector3Value(argumentValues, 0, vec3Value, true) == true) {
						transformations.setTranslation(vec3Value);
					} else {
						Console::println("ScriptMethodTransformations::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: vector3 expected");
						miniScript->startErrorScript();
					}
				}
				if (argumentValues.size() >= 2) {
					if (MiniScript::getVector3Value(argumentValues, 1, vec3Value, true) == true) {
						transformations.setScale(vec3Value);
					} else {
						Console::println("ScriptMethodTransformations::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 1: vector3 expected");
						miniScript->startErrorScript();
					}
				}
				for (auto i = 2; i < argumentValues.size(); i++) {
					if (MiniScript::getVector3Value(argumentValues, i, vec3Value, true) == true) {
						transformations.addRotation(vec3Value, 0.0f);
					} else {
						Console::println("ScriptMethodTransformations::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": vector3 expected");
						miniScript->startErrorScript();
					}
				}
				transformations.update();
				MiniScript::setTransformationsValue(returnValue, transformations);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodTransformations(this));
	}
	{
		//
		class ScriptMethodTransformationsGetTranslation: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodTransformationsGetTranslation(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_TRANSFORMATIONS, .name = "transformations", .optional = false },
					},
					ScriptVariableType::TYPE_VECTOR3),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "transformations.getTranslation";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Transformations transformations;
				if (MiniScript::getTransformationsValue(argumentValues, 0, transformations, false) == true) {
					MiniScript::setVector3Value(returnValue, transformations.getTranslation());
				} else {
					Console::println("ScriptMethodTransformationsGetTranslation::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: transformations expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodTransformationsGetTranslation(this));
	}
	{
		//
		class ScriptMethodTransformationsSetTranslation: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodTransformationsSetTranslation(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_TRANSFORMATIONS, .name = "transformations", .optional = false },
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "translation", .optional = false },
					},
					ScriptVariableType::TYPE_TRANSFORMATIONS),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "transformations.setTranslation";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Transformations transformations;
				Vector3 translation;
				if (MiniScript::getTransformationsValue(argumentValues, 0, transformations, false) == true &&
					MiniScript::getVector3Value(argumentValues, 1, translation, false) == true) {
					transformations.setTranslation(translation);
					transformations.update();
					MiniScript::setTransformationsValue(returnValue, transformations);
				} else {
					Console::println("ScriptMethodTransformationsSetTranslation::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: transformations expected, @ argument 1: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodTransformationsSetTranslation(this));
	}
	{
		//
		class ScriptMethodTransformationsGetScale: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodTransformationsGetScale(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_TRANSFORMATIONS, .name = "transformations", .optional = false },
					},
					ScriptVariableType::TYPE_VECTOR3),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "transformations.getScale";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Transformations transformations;
				if (MiniScript::getTransformationsValue(argumentValues, 0, transformations, false) == true) {
					MiniScript::setVector3Value(returnValue, transformations.getScale());
				} else {
					Console::println("ScriptMethodTransformationsGetScale::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: transformations expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodTransformationsGetScale(this));
	}
	{
		//
		class ScriptMethodTransformationsSetScale: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodTransformationsSetScale(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_TRANSFORMATIONS, .name = "transformations", .optional = false },
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "scale", .optional = false },
					},
					ScriptVariableType::TYPE_TRANSFORMATIONS),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "transformations.setScale";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Transformations transformations;
				Vector3 scale;
				if (MiniScript::getTransformationsValue(argumentValues, 0, transformations, false) == true &&
					MiniScript::getVector3Value(argumentValues, 1, scale, false) == true) {
					transformations.setScale(scale);
					transformations.update();
					MiniScript::setTransformationsValue(returnValue, transformations);
				} else {
					Console::println("ScriptMethodTransformationsSetScale::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: transformations expected, @ argument 1: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodTransformationsSetScale(this));
	}
	{
		//
		class ScriptMethodTransformationsGetRotationAxis: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodTransformationsGetRotationAxis(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_TRANSFORMATIONS, .name = "transformations", .optional = false },
						{.type = ScriptVariableType::TYPE_INTEGER, .name = "idx", .optional = false },
					},
					ScriptVariableType::TYPE_VECTOR3),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "transformations.getRotationAxis";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t idx;
				Transformations transformations;
				if (MiniScript::getTransformationsValue(argumentValues, 0, transformations, false) == true &&
					MiniScript::getIntegerValue(argumentValues, 1, idx, false) == true) {
					if (idx < transformations.getRotationCount()) {
						MiniScript::setVector3Value(returnValue, transformations.getRotationAxis(idx));
					} else {
						Console::println("ScriptMethodTransformationsGetRotationAxis::executeMethod(): " + getMethodName() + "(): rotation index invalid: " + to_string(idx) + " / " + to_string(transformations.getRotationCount()));
						miniScript->startErrorScript();
					}
				} else {
					Console::println("ScriptMethodTransformationsGetRotationAxis::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: transformations expected, @ argument 1: integer expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodTransformationsGetRotationAxis(this));
	}
	{
		//
		class ScriptMethodTransformationsGetRotationAngle: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodTransformationsGetRotationAngle(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_TRANSFORMATIONS, .name = "transformations", .optional = false },
						{.type = ScriptVariableType::TYPE_INTEGER, .name = "idx", .optional = false },
					},
					ScriptVariableType::TYPE_FLOAT),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "transformations.getRotationAngle";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t idx;
				Transformations transformations;
				if (MiniScript::getTransformationsValue(argumentValues, 0, transformations, false) == true &&
					MiniScript::getIntegerValue(argumentValues, 1, idx, false) == true) {
					if (idx < transformations.getRotationCount()) {
						MiniScript::setFloatValue(returnValue, transformations.getRotationAngle(idx));
					} else {
						Console::println("ScriptMethodTransformationsGetRotationAngle::executeMethod(): " + getMethodName() + "(): rotation index invalid: " + to_string(idx) + " / " + to_string(transformations.getRotationCount()));
						miniScript->startErrorScript();
					}
				} else {
					Console::println("ScriptMethodTransformationsGetRotationAngle::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: transformations expected, @ argument 1: integer expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodTransformationsGetRotationAngle(this));
	}
	{
		//
		class ScriptMethodTransformationsSetRotationAngle: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodTransformationsSetRotationAngle(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_TRANSFORMATIONS, .name = "transformations", .optional = false },
						{.type = ScriptVariableType::TYPE_INTEGER, .name = "idx", .optional = false },
						{.type = ScriptVariableType::TYPE_FLOAT, .name = "angle", .optional = false },
					},
					ScriptVariableType::TYPE_TRANSFORMATIONS),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "transformations.setRotationAngle";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t idx;
				Transformations transformations;
				float angle;
				if (MiniScript::getTransformationsValue(argumentValues, 0, transformations, false) == true &&
					MiniScript::getIntegerValue(argumentValues, 1, idx, false) == true &&
					MiniScript::getFloatValue(argumentValues, 2, angle, false) == true) {
					if (idx < transformations.getRotationCount()) {
						transformations.setRotationAngle(idx, angle);
						transformations.update();
						MiniScript::setTransformationsValue(returnValue, transformations);
					} else {
						Console::println("ScriptMethodTransformationsSetRotationAngle::executeMethod(): " + getMethodName() + "(): rotation index invalid: " + to_string(idx) + " / " + to_string(transformations.getRotationCount()));
						miniScript->startErrorScript();
					}
				} else {
					Console::println("ScriptMethodTransformationsSetRotationAngle::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: transformations expected, @ argument 1: integer expected, @ argument 2: float expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodTransformationsSetRotationAngle(this));
	}
	{
		//
		class ScriptMethodTransformationsMultiply: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodTransformationsMultiply(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_TRANSFORMATIONS, .name = "transformations", .optional = false },
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "vec3", .optional = false },
					},
					ScriptVariableType::TYPE_VECTOR3),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "transformations.multiply";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Transformations transformations;
				Vector3 vec3;
				if (MiniScript::getTransformationsValue(argumentValues, 0, transformations, false) == true &&
					MiniScript::getVector3Value(argumentValues, 1, vec3, false) == true) {
					MiniScript::setVector3Value(returnValue, transformations.getTransformationsMatrix() * vec3);
				} else {
					Console::println("ScriptMethodTransformationsSetScale::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: transformations expected, @ argument 1: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodTransformationsMultiply(this));
	}
	{
		//
		class ScriptMethodTransformationsRotate: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodTransformationsRotate(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_TRANSFORMATIONS, .name = "transformations", .optional = false },
						{.type = ScriptVariableType::TYPE_VECTOR3, .name = "vec3", .optional = false },
					},
					ScriptVariableType::TYPE_VECTOR3),
					miniScript(miniScript) {}
			const string getMethodName() override {
				return "transformations.rotate";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				Transformations transformations;
				Vector3 vec3;
				if (MiniScript::getTransformationsValue(argumentValues, 0, transformations, false) == true &&
					MiniScript::getVector3Value(argumentValues, 1, vec3, false) == true) {
					MiniScript::setVector3Value(returnValue, transformations.getRotationsQuaternion() * vec3);
				} else {
					Console::println("ScriptMethodTransformationsSetScale::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: transformations expected, @ argument 1: vector3 expected");
					miniScript->startErrorScript();
				}
			}
		};
		registerMethod(new ScriptMethodTransformationsRotate(this));
	}
	// bool methods
	{
		//
		class ScriptMethodBool: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodBool(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_BOOLEAN, .name = "bool", .optional = false }
					},
					ScriptVariableType::TYPE_BOOLEAN
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "bool";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				bool boolValue;
				if (MiniScript::getBooleanValue(argumentValues, 0, boolValue, false) == true) {
					MiniScript::setBooleanValue(returnValue, boolValue);
				} else {
					Console::println("ScriptMethodBool::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: boolean expected");
					miniScript->startErrorScript();
					return;
				}
			}
		};
		registerMethod(new ScriptMethodBool(this));
	}
	{
		//
		class ScriptMethodNot: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodNot(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_BOOLEAN, .name = "bool", .optional = false }
					},
					ScriptVariableType::TYPE_BOOLEAN), miniScript(miniScript) {}
			const string getMethodName() override {
				return "not";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				bool booleanValue = false;
				if (MiniScript::getBooleanValue(argumentValues, 0, booleanValue, false) == true) {
					MiniScript::setBooleanValue(returnValue, !booleanValue);
				} else {
					Console::println("ScriptMethodNot::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: boolean expected");
					miniScript->startErrorScript();
					return;
				}
			}
		};
		registerMethod(new ScriptMethodNot(this));
	}
	{
		//
		class ScriptMethodAnd: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodAnd(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_BOOLEAN), miniScript(miniScript) {}
			const string getMethodName() override {
				return "and";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				for (auto i = 0; i < argumentValues.size(); i++) {
					bool booleanValue;
					if (MiniScript::getBooleanValue(argumentValues, i, booleanValue, false) == false) {
						Console::println("ScriptMethodAnd::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": boolean expected");
						miniScript->startErrorScript();
						return;
					} else
					if (booleanValue == false) {
						MiniScript::setBooleanValue(returnValue, false);
						return;
					}
				}
				MiniScript::setBooleanValue(returnValue, true);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodAnd(this));
	}
	{
		//
		class ScriptMethodAnd: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodAnd(MiniScript* miniScript): ScriptMethod({}, ScriptVariableType::TYPE_BOOLEAN), miniScript(miniScript) {}
			const string getMethodName() override {
				return "or";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				for (auto i = 0; i < argumentValues.size(); i++) {
					bool booleanValue;
					if (MiniScript::getBooleanValue(argumentValues, i, booleanValue, false) == false) {
						Console::println("ScriptMethodOr::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument " + to_string(i) + ": boolean expected");
						miniScript->startErrorScript();
						return;
					} else
					if (booleanValue == true) {
						MiniScript::setBooleanValue(returnValue, true);
						return;
					}
				}
				MiniScript::setBooleanValue(returnValue, false);
			}
			bool isVariadic() override {
				return true;
			}
		};
		registerMethod(new ScriptMethodAnd(this));
	}
	// string functions
	{
		//
		class ScriptMethodString: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodString(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_STRING, .name = "string", .optional = false }
					},
					ScriptVariableType::TYPE_STRING
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "string";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				string stringValue;
				if (MiniScript::getStringValue(argumentValues, 0, stringValue, false) == true) {
					MiniScript::setStringValue(returnValue, stringValue);
				} else {
					Console::println("ScriptMethodString::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: string expected");
					miniScript->startErrorScript();
					return;
				}
			}
		};
		registerMethod(new ScriptMethodString(this));
	}
	{
		//
		class ScriptMethodSpace: public ScriptMethod {
		private:
			MiniScript* miniScript { nullptr };
		public:
			ScriptMethodSpace(MiniScript* miniScript):
				ScriptMethod(
					{
						{.type = ScriptVariableType::TYPE_INTEGER, .name = "spaces", .optional = true }
					},
					ScriptVariableType::TYPE_STRING
				),
				miniScript(miniScript) {}
			const string getMethodName() override {
				return "space";
			}
			void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) override {
				int64_t spaces = 1;
				if (MiniScript::getIntegerValue(argumentValues, 0, spaces, true) == false) {
					Console::println("ScriptMethodSpace::executeMethod(): " + getMethodName() + "(): parameter type mismatch @ argument 0: integer expected");
					miniScript->startErrorScript();
					return;
				} else {
					string spacesString;
					for (auto i = 0; i < spaces; i++) spacesString+= " ";
					MiniScript::setStringValue(returnValue, spacesString);
				}
			}
		};
		registerMethod(new ScriptMethodSpace(this));
	}
}

void MiniScript::registerVariables() {
}
