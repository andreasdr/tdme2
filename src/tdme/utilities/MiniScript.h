#pragma once

#include <map>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdme/engine/Transformations.h>
#include <tdme/math/Vector3.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Float.h>
#include <tdme/utilities/Integer.h>
#include <tdme/utilities/StringTools.h>

using std::map;
using std::stack;
using std::string;
using std::to_string;
using std::unordered_map;
using std::vector;

using tdme::engine::Transformations;
using tdme::math::Vector3;
using tdme::utilities::Console;
using tdme::utilities::Float;
using tdme::utilities::Integer;
using tdme::utilities::StringTools;

/**
 * Miniscript
 */
class tdme::utilities::MiniScript {
public:
	enum StateMachineState {
		STATE_NONE = -1,
		STATE_NEXT_STATEMENT,
		STATE_WAIT,
		STATE_WAIT_FOR_CONDITION
	};

	struct ScriptStatement {
		int line;
		int statementIdx;
		string statement;
		int gotoStatementIdx;
	};

	enum ScriptVariableType {
		TYPE_VOID,
		TYPE_BOOLEAN,
		TYPE_INTEGER,
		TYPE_FLOAT,
		TYPE_STRING,
		TYPE_VECTOR3,
		TYPE_TRANSFORMATIONS
	};

	struct ScriptVariable {
		ScriptVariableType type { TYPE_VOID };
		string stringValue;
		Transformations transformationsValue;
		bool booleanValue { false };
		int64_t integerValue { 0 };
		float floatValue { 0.0f };
		Vector3 vector3Value;
	};

	/**
	 * Script State Machine State
	 */
	class ScriptStateMachineState {
	public:
		/**
		 * Constructor
		 */
		ScriptStateMachineState() {}

		/**
		 * Destructor
		 */
		virtual ~ScriptStateMachineState() {}

		/**
		 * @return name
		 */
		virtual const string getName() = 0;

		/**
		 * @return id
		 */
		virtual int getId() = 0;

		/**
		 * Execute script state machine state
		 */
		virtual void execute() = 0;
	};

	/**
	 * Script method
	 */
	class ScriptMethod {
	public:
		struct ArgumentType {
			ScriptVariableType type;
			string name;
			bool optional;
		};

		/**
		 * Constructor
		 * @param argumentTypes argument types
		 * @param returnValueType return value type
		 */
		ScriptMethod(const vector<ArgumentType>& argumentTypes = {}, ScriptVariableType returnValueType = ScriptVariableType::TYPE_VOID): returnValueType(returnValueType), argumentTypes(argumentTypes) {}

		/**
		 * Destructor
		 */
		virtual ~ScriptMethod() {}

		/**
		 * @return script method name
		 */
		virtual const string getMethodName() = 0;

		/**
		 * Execute script method
		 * @param argumentValues argument values
		 * @param returnValue return value
		 * @param statement statement
		 */
		virtual void executeMethod(const vector<ScriptVariable>& argumentValues, ScriptVariable& returnValue, const ScriptStatement& statement) = 0;

		/**
		 * @return arguments
		 */
		const vector<ArgumentType>& getArgumentTypes() {
			return argumentTypes;
		}

		/**
		 * @return return value type
		 */
		const ScriptVariableType& getReturnValueType() {
			return returnValueType;
		}

		/**
		 * @return if variadic method
		 */
		virtual bool isVariadic() {
			return false;
		}

	private:
		vector<ArgumentType> argumentTypes;
		ScriptVariableType returnValueType;
	};

private:
	static constexpr bool VERBOSE { false };

	struct ScriptState {
		enum EndType { ENDTYPE_FOR, ENDTYPE_IF };
		enum ConditionType {
			SCRIPT,
			CONDITIONTYPE_FORTIME
		};
		bool running { false };
		int idx { -1 };
		int statementIdx { -1 };
		int64_t waitStarted { -1LL };
		int64_t waitTime { -1LL };
		string id;
		map<string, ScriptVariable> variables;
		map<int, int64_t> forTimeStarted;
		stack<bool> conditionStack;
		stack<EndType> endTypeStack;
		int state { -1 };
	};

	struct Script {
		int line;
		vector<string> conditions;
		vector<ScriptStatement> statements;
	};

	//
	ScriptState scriptState;

	/**
	 * @return string representation of script variable type
	 */
	inline static const string getScriptVariableTypeAsString(ScriptVariableType type) {
		switch(type) {
			case TYPE_VOID: return "Void";
			case TYPE_BOOLEAN: return "Boolean";
			case TYPE_INTEGER: return "Integer";
			case TYPE_FLOAT: return "Float";
			case TYPE_STRING: return "String";
			case TYPE_VECTOR3: return "Vector3";
			case TYPE_TRANSFORMATIONS: return "Transformations";
		}
		return string();
	}

	/**
	 * @return string representation of script variable type
	 */
	inline static const string getScriptVariableAsString(const ScriptVariable& variable) {
		string result;
		result+= getScriptVariableTypeAsString(variable.type);
		result+= "(";
		result+= getScriptVariableValueString(variable);
		result+= ")";
		return result;
	}

	/**
	 * @return string representation of script variable type
	 */
	inline static const string getScriptVariableValueString(const ScriptVariable& variable) {
		string result;
		switch (variable.type) {
			case TYPE_VOID:
				break;
			case TYPE_BOOLEAN:
				result+= variable.booleanValue == true?"1":"0";
				break;
			case TYPE_INTEGER:
				result+= to_string(variable.integerValue);
				break;
			case TYPE_FLOAT:
				result+= to_string(variable.floatValue);
				break;
			case TYPE_STRING:
				result+= variable.stringValue;
				break;
			case TYPE_VECTOR3:
				result+=
					"Vector3(" +
					to_string(variable.vector3Value.getX()) + ", " +
					to_string(variable.vector3Value.getY()) + ", " +
					to_string(variable.vector3Value.getZ()) + ")";
				break;
			case TYPE_TRANSFORMATIONS:
				result+=
					"Transformations(translation = Vector3(" +
					to_string(variable.transformationsValue.getTranslation().getX()) + ", " +
					to_string(variable.transformationsValue.getTranslation().getY()) + ", " +
					to_string(variable.transformationsValue.getTranslation().getZ()) + "), " +
					"scale = Vector3(" +
					to_string(variable.transformationsValue.getScale().getX()) + ", " +
					to_string(variable.transformationsValue.getScale().getY()) + ", " +
					to_string(variable.transformationsValue.getScale().getZ()) + ")";
				for (auto i = 0; i < variable.transformationsValue.getRotationCount(); i++) {
					result+= ", rotations = (axis = Vector3(" +
							to_string(variable.transformationsValue.getRotationAxis(i).getX()) + ", " +
							to_string(variable.transformationsValue.getRotationAxis(i).getY()) + ", " +
							to_string(variable.transformationsValue.getRotationAxis(i).getZ()) + "), angle = " +
							to_string(variable.transformationsValue.getRotationAngle(i)) + ")";
				}
				result+= ")";
		}
		return result;
	}

	vector<Script> scripts;
	unordered_map<string, ScriptMethod*> scriptMethods;
	unordered_map<int, ScriptStateMachineState*> scriptStateMachineStates;

	/**
	 * Execute a single script line
	 */
	void executeScriptLine();

	/**
	 * Parse a script statement
	 * @param statement statement 
	 * @param variable variable
	 * @param method method
	 * @param arguments arguments
	 */
	void parseScriptStatement(const string& statement, string& variable, string& method, vector<string>& arguments);

	/**
	 * Execute a script statement
	 * @param method method 
	 * @param argument argument
	 * @param statement statement
	 * @return pointer to return value
	 */
	ScriptVariable executeScriptStatement(const string& method, const vector<string>& argument, const ScriptStatement& statement);

	/**
	 * Determine script index to start
	 * @return script index or -1 if no script to start
	 */
	int determineScriptIdxToStart();

protected:
	/**
	 * Start error script
	 */
	inline void startErrorScript() {
		emit("error");
	}

	/**
	 * Set script state machine state
	 * @param state state
	 */
	inline void setScriptState(int state) {
		scriptState.state = state;
	}

	/**
	 * Register state machine states
	 */
	virtual void registerStateMachineStates();

	/**
	 * Register methods
	 */
	virtual void registerMethods();

	/**
	 * Register variables
	 */
	virtual void registerVariables();

public:
	/**
	 * Default constructor
	 */
	MiniScript();

	/**
	 * Destructor
	 */
	virtual ~MiniScript();

	/**
	 * Register script state machine state
	 * @param state state
	 */
	void registerStateMachineState(ScriptStateMachineState* state);

	/**
	 * Register script method
	 * @param method method
	 */
	void registerMethod(ScriptMethod* method);

	/**
	 * Returns variable with given name
	 * @param name name
	 * @return variable
	 */
	const ScriptVariable getVariable(const string& name);

	/**
	 * Set script variable
	 * @param name name
	 * @param variable variable
	 */
	void setVariable(const string& name, const ScriptVariable& variable);

	/**
	 * Load script
	 * @param pathName path name
	 * @param fileName file name
	 */
	void loadScript(const string& pathName, const string& fileName);

	/**
	 * Start script
	 * @param delay delay
	 */
	void startScript(int64_t delay = 0LL);

	/**
	 * Emit
	 * @param condition condition
	 */
	void emit(const string& condition);

	/**
	 * Execute state machine
	 * @return quit requested
	 */
	inline void executeStateMachine() {
		if (scriptState.state == STATE_NONE) return;
		// execute state machine
		auto scriptStateMachineStateIt = scriptStateMachineStates.find(scriptState.state);
		if (scriptStateMachineStateIt != scriptStateMachineStates.end()) {
			scriptStateMachineStateIt->second->execute();
		} else {
			Console::println("MiniScript::executeStateMachine(): unknown state with id: " + to_string(scriptState.state));
		}
	}

	/**
	 * @return is running
	 */
	inline bool isRunning() {
		return scriptState.running;
	}

	/**
	 * @return script state machine state
	 */
	inline int getScriptState() {
		return scriptState.state;
	}

	/**
	 * Get boolean value from given variable
	 * @param arguments arguments
	 * @param idx argument index
	 * @param value value
	 * @param optional optionalfalse
	 * @return success
	 */
	inline static bool getBooleanValue(const vector<ScriptVariable>& arguments, int idx, bool& value, bool optional = false) {
		if (idx >= arguments.size()) return optional;
		auto& argument = arguments[idx];
		switch(argument.type) {
			case TYPE_VOID:
				return optional;
			case TYPE_BOOLEAN:
				value = argument.booleanValue;
				return true;
				break;
			case TYPE_INTEGER:
				value = argument.integerValue != 0;
				return true;
			case TYPE_FLOAT:
				value = argument.floatValue != 0.0f;
				return true;
			case TYPE_STRING:
				{
					auto lowerCaseString = StringTools::toLowerCase(argument.stringValue);
					if (lowerCaseString != "false" && lowerCaseString != "true" && lowerCaseString != "1" && lowerCaseString != "0") return optional;
					value = lowerCaseString == "true" || lowerCaseString == "1";
					return true;
				}
			case TYPE_VECTOR3:
				return false;
			case TYPE_TRANSFORMATIONS:
				return false;
				break;
		}
		return false;
	}

	/**
	 * Get integer value from given variable
	 * @param arguments arguments
	 * @param idx argument index
	 * @param value value
	 * @param optional optional
	 * @return success
	 */
	inline static bool getIntegerValue(const vector<ScriptVariable>& arguments, int idx, int64_t& value, bool optional = false) {
		if (idx >= arguments.size()) return optional;
		auto& argument = arguments[idx];
		switch(argument.type) {
			case TYPE_VOID:
				return optional;
			case TYPE_BOOLEAN:
				value = argument.booleanValue == true?1:0;
				return true;
				break;
			case TYPE_INTEGER:
				value = argument.integerValue;
				return true;
			case TYPE_FLOAT:
				Console::println("MiniScript::getIntegerValue(): converting float to integer: precision loss");
				value = argument.floatValue;
				return true;
			case TYPE_STRING:
				if (Integer::isInt(argument.stringValue) == true) {
					value = Integer::parseInt(argument.stringValue);
					return true;
				} else
				if (Float::isFloat(argument.stringValue) == true) {
					Console::println("MiniScript::getIntegerValue(): converting float to integer: precision loss");
					value = static_cast<int64_t>(Float::parseFloat(argument.stringValue));
					return true;
				} else {
					return optional;
				}
			case TYPE_VECTOR3:
				return optional;
			case TYPE_TRANSFORMATIONS:
				return optional;
		}
		return false;
	}

	/**
	 * Get float value from given variable
	 * @param arguments arguments
	 * @param idx argument index
	 * @param value value
	 * @param optional optional
	 * @return success
	 */
	inline static bool getFloatValue(const vector<ScriptVariable>& arguments, int idx, float& value, bool optional = false) {
		if (idx >= arguments.size()) return optional;
		auto& argument = arguments[idx];
		switch(argument.type) {
			case TYPE_VOID:
				return optional;
			case TYPE_BOOLEAN:
				value = argument.booleanValue == true?1.0f:0.0f;
				return true;
				break;
			case TYPE_INTEGER:
				value = argument.integerValue;
				return true;
			case TYPE_FLOAT:
				value = argument.floatValue;
				return true;
			case TYPE_STRING:
				if (Float::isFloat(argument.stringValue) == false) return optional;
				value = Float::parseFloat(argument.stringValue);
				return true;
			case TYPE_VECTOR3:
				return optional;
			case TYPE_TRANSFORMATIONS:
				return optional;
		}
		return false;
	}

	/**
	 * Get string value from given variable
	 * @param arguments arguments
	 * @param idx argument index
	 * @param value value
	 * @param optional optional
	 * @return success
	 */
	inline static bool getStringValue(const vector<ScriptVariable>& arguments, int idx, string& value, bool optional = false) {
		if (idx >= arguments.size()) return optional;
		auto& argument = arguments[idx];
		switch(argument.type) {
			case TYPE_VOID:
				return optional;
			case TYPE_BOOLEAN:
				value = argument.booleanValue == true?"true":"false";
				return true;
			case TYPE_INTEGER:
				value = to_string(argument.integerValue);
				return true;
			case TYPE_FLOAT:
				value = to_string(argument.floatValue);
				return true;
			case TYPE_STRING:
				value = argument.stringValue;
				return true;
			case TYPE_VECTOR3:
				return false;
			case TYPE_TRANSFORMATIONS:
				return false;
		}
		return false;
	}

	/**
	 * Get vector3 value from given variable
	 * @param arguments arguments
	 * @param idx argument index
	 * @param value value
	 * @param optional optional
	 * @return success
	 */
	inline static bool getVector3Value(const vector<ScriptVariable>& arguments, int idx, Vector3& value, bool optional = false) {
		if (idx >= arguments.size()) return optional;
		auto& argument = arguments[idx];
		switch(argument.type) {
			case TYPE_VOID:
				return optional;
			case TYPE_BOOLEAN:
				return optional;
			case TYPE_INTEGER:
				return optional;
			case TYPE_FLOAT:
				return optional;
			case TYPE_STRING:
				return optional;
			case TYPE_VECTOR3:
				value = argument.vector3Value;
				return true;
			case TYPE_TRANSFORMATIONS:
				return optional;
		}
		return false;
	}
	/**
	 * Get transformations value from given variable
	 * @param arguments arguments
	 * @param idx argument index
	 * @param value value
	 * @param optional optional
	 * @return success
	 */
	inline static bool getTransformationsValue(const vector<ScriptVariable>& arguments, int idx, Transformations& value, bool optional = false) {
		if (idx >= arguments.size()) return optional;
		auto& argument = arguments[idx];
		switch(argument.type) {
			case TYPE_VOID:
				return optional;
			case TYPE_BOOLEAN:
				return optional;
			case TYPE_INTEGER:
				return optional;
			case TYPE_FLOAT:
				return optional;
			case TYPE_STRING:
				return optional;
			case TYPE_VECTOR3:
				return optional;
			case TYPE_TRANSFORMATIONS:
				value = argument.transformationsValue;
				return true;
		}
		return false;
	}

	/**
	 * Set boolean value from given value into variable
	 * @param argument argument
	 * @param value value
	 */
	inline static void setBooleanValue(ScriptVariable& variable, bool value) {
		variable.type = TYPE_BOOLEAN;
		variable.booleanValue = value;
	}

	/**
	 * Set integer value from given value into variable
	 * @param argument argument
	 * @param value value
	 */
	inline static void setIntegerValue(ScriptVariable& variable, int64_t value) {
		variable.type = TYPE_INTEGER;
		variable.integerValue = value;
	}

	/**
	 * Set float value from given value into variable
	 * @param argument argument
	 * @param value value
	 */
	inline static void setFloatValue(ScriptVariable& variable, float value) {
		variable.type = TYPE_FLOAT;
		variable.floatValue = value;
	}

	/**
	 * Set string value from given value into variable
	 * @param argument argument
	 * @param value value
	 */
	inline static void setStringValue(ScriptVariable& variable, const string& value) {
		variable.type = TYPE_STRING;
		variable.stringValue = value;
	}

	/**
	 * Set vector3 value from given value into variable
	 * @param argument argument
	 * @param value value
	 */
	inline static void setVector3Value(ScriptVariable& variable, const Vector3& value) {
		variable.type = TYPE_VECTOR3;
		variable.vector3Value = value;
	}

	/**
	 * Set transformations value from given value into variable
	 * @param argument argument
	 * @param value value
	 */
	inline static void setTransformationsValue(ScriptVariable& variable, const Transformations& value) {
		variable.type = TYPE_TRANSFORMATIONS;
		variable.transformationsValue = value;
	}

	/**
	 * Dump info
	 */
	const string dumpInfo();

};
