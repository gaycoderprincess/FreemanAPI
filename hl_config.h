// custom config
namespace FreemanAPI {
	void ValueEditorMenu(float& value) {
		ChloeMenuLib::BeginMenu();

		static char inputString[1024] = {};
		ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
		ChloeMenuLib::SetEnterHint("Apply");

		if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
			value = std::stof(inputString);
			memset(inputString,0,sizeof(inputString));
			ChloeMenuLib::BackOut();
		}

		ChloeMenuLib::EndMenu();
	}

	void ValueEditorMenu(int& value) {
		ChloeMenuLib::BeginMenu();

		static char inputString[1024] = {};
		ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
		ChloeMenuLib::SetEnterHint("Apply");

		if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
			value = std::stoi(inputString);
			memset(inputString,0,sizeof(inputString));
			ChloeMenuLib::BackOut();
		}

		ChloeMenuLib::EndMenu();
	}

	void ValueEditorMenu(bool& value, const std::string& name) {
		if (DrawMenuOption(std::format("{} - {}", name, value), "")) {
			value = !value;
		}
	}

	void ValueEditorMenu(float& value, const std::string& name) {
		if (DrawMenuOption(std::format("{} - {}", name, value), "")) {
			ValueEditorMenu(value);
		}
	}

	void ValueEditorMenu(int& value, const std::string& name) {
		if (DrawMenuOption(std::format("{} - {}", name, value), "")) {
			ValueEditorMenu(value);
		}
	}

	struct tConfigValue {
		bool* bValue = nullptr;
		int* iValue = nullptr;
		float* fValue = nullptr;
		std::string name;
		std::string configName;

		void ReadFromConfig(toml::table& config, const char* label) const {
			if (configName.empty()) return;

			if (bValue) *bValue = config[label][configName].value_or(*bValue);
			if (iValue) *iValue = config[label][configName].value_or(*iValue);
			if (fValue) *fValue = config[label][configName].value_or(*fValue);
		}

		void DrawValueEditor() const {
			if (name.empty()) return;

			if (bValue) ValueEditorMenu(*bValue, name);
			if (iValue) ValueEditorMenu(*iValue, name);
			if (fValue) ValueEditorMenu(*fValue, name);
		}
	};
	std::vector<tConfigValue> aBehaviorConfig;
	std::vector<tConfigValue> aCVarConfigHL1;
	std::vector<tConfigValue> aCVarConfigHL2;
	std::vector<tConfigValue> aAdvancedConfig;

	// ingame variable config
	bool bEnabled = true;
	bool bCanLongJump = false;
	bool bAutoHop = true;
	bool bABH = false;
	bool bABHMixed = false;
	bool bBhopCap = false;
	bool bSmartVelocityCap = false;
	bool bNoclipKey = false;
	bool bHL2Mode = true;

	// game integration config
	bool bConvertUnits = true; // do conversions from units to meters when handling game funcs

	// rotation order
	int PITCH = 1;
	int YAW = 0;
	int ROLL = 2;

	// 2 & 1 for Y-up, 1 & 2 for Z-up
	int FORWARD = 2;
	int UP = 1;
}