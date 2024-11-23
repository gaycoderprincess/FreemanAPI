// HL to game integration
namespace FreemanAPI {
	std::vector<tConfigValue> aCustomBehaviorConfig;
	std::vector<tConfigValue> aCustomCVarConfig;
	std::vector<tConfigValue> aCustomAdvancedConfig;
	std::vector<tConfigValue>* GetCustomConfig(int category) {
		std::vector<tConfigValue>* vec = nullptr;
		switch (category) {
			case 0:
			default:
				vec = &aCustomBehaviorConfig;
				break;
			case 1:
				vec = &aCustomCVarConfig;
				break;
			case 2:
				vec = &aCustomAdvancedConfig;
				break;
		}
		return vec;
	}
	void AddBoolToCustomConfig(std::vector<tConfigValue>* vec, const char* label, const char* configLabel, bool* ptr) {
		if (!vec) return;
		if (!ptr) return;

		for (auto& value : *vec) {
			if (value.bValue == ptr) return;
		}
		tConfigValue value;
		if (label) value.name = label;
		if (configLabel) value.configName = configLabel;
		value.bValue = ptr;
		vec->push_back(value);
	}
	void AddIntToCustomConfig(std::vector<tConfigValue>* vec, const char* label, const char* configLabel, int* ptr) {
		if (!vec) return;
		if (!ptr) return;

		for (auto& value : *vec) {
			if (value.iValue == ptr) return;
		}
		tConfigValue value;
		if (label) value.name = label;
		if (configLabel) value.configName = configLabel;
		value.iValue = ptr;
		vec->push_back(value);
	}
	void AddFloatToCustomConfig(std::vector<tConfigValue>* vec, const char* label, const char* configLabel, float* ptr) {
		if (!vec) return;
		if (!ptr) return;

		for (auto& value : *vec) {
			if (value.fValue == ptr) return;
		}
		tConfigValue value;
		if (label) value.name = label;
		if (configLabel) value.configName = configLabel;
		value.fValue = ptr;
		vec->push_back(value);
	}
	tConfigValue* FindConfigValue(const char* label) {
		if (!label) return nullptr;
		for (auto& config : aBehaviorConfig) {
			if (config.name == label) return &config;
		}
		for (auto& config : aCustomBehaviorConfig) {
			if (config.name == label) return &config;
		}
		for (auto& config : aAdvancedConfig) {
			if (config.name == label) return &config;
		}
		for (auto& config : aCustomAdvancedConfig) {
			if (config.name == label) return &config;
		}
		for (auto& config : aCVarConfig) {
			if (config.name == label) return &config;
		}
		for (auto& config : aCustomCVarConfig) {
			if (config.name == label) return &config;
		}
		return nullptr;
	}

	auto EXT_PlayGameSound = (void(*)(const char*, float))nullptr;
	auto EXT_GetGamePlayerDead = (bool(*)())nullptr;
	auto EXT_GetGamePlayerPosition = (void(*)(double*))nullptr;
	auto EXT_GetGamePlayerVelocity = (void(*)(double*))nullptr;
	auto EXT_GetGamePlayerViewAngle = (void(*)(double*))nullptr;
	auto EXT_SetGamePlayerPosition = (void(*)(const double*, const double*))nullptr;
	auto EXT_SetGamePlayerViewPosition = (void(*)(const double*))nullptr;
	auto EXT_SetGamePlayerViewAngle = (void(*)(const double*))nullptr;
	auto EXT_GetPointContents = (int(*)(const double*))nullptr;
	auto EXT_PointRaytrace = (pmtrace_t*(*)(const double*, const double*))nullptr;
	auto EXT_PM_PlayerTrace = (pmtrace_t*(*)(const double*, const double*))nullptr;
	auto EXT_PM_PlayerTraceDown = (pmtrace_t*(*)(const double*, const double*))nullptr;
	auto EXT_GetGameMoveLeftRight = (float(*)())nullptr;
	auto EXT_GetGameMoveFwdBack = (float(*)())nullptr;
	auto EXT_GetGameMoveUpDown = (float(*)())nullptr;
	auto EXT_GetGameMoveJump = (bool(*)())nullptr;
	auto EXT_GetGameMoveDuck = (bool(*)())nullptr;
	auto EXT_GetGameMoveRun = (bool(*)())nullptr;
	auto EXT_GetGameMoveUse = (bool(*)())nullptr;

	void PlayGameSound(const std::string& path, float volume) {
		if (!EXT_PlayGameSound) return;
		EXT_PlayGameSound(path.c_str(), volume);
	}

	bool GetGamePlayerDead() {
		if (EXT_GetGamePlayerDead) return EXT_GetGamePlayerDead();
		return false;
	}

	void GetGamePlayerPosition(NyaVec3Double* out) {
		*out = {0,0,0};
		if (EXT_GetGamePlayerPosition) {
			EXT_GetGamePlayerPosition(&out->x);
		}
	}

	void GetGamePlayerVelocity(NyaVec3Double* out) {
		*out = {0,0,0};
		if (EXT_GetGamePlayerVelocity) {
			EXT_GetGamePlayerVelocity(&out->x);
		}
	}

	void GetGamePlayerViewAngle(NyaVec3Double* out) {
		*out = {0,0,0};
		if (EXT_GetGamePlayerViewAngle) {
			EXT_GetGamePlayerViewAngle(&out->x);
		}
	}

	void SetGamePlayerPosition(const NyaVec3Double* in, const NyaVec3Double* inVelocity) {
		if (!EXT_SetGamePlayerPosition) return;
		EXT_SetGamePlayerPosition(&in->x, &inVelocity->x);
	}

	void SetGamePlayerViewPosition(const NyaVec3Double* in) {
		if (!EXT_SetGamePlayerViewPosition) return;
		EXT_SetGamePlayerViewPosition(&in->x);
	}

	void SetGamePlayerViewAngle(const NyaVec3Double* in) {
		if (!EXT_SetGamePlayerViewAngle) return;
		EXT_SetGamePlayerViewAngle(&in->x);
	}

	int GetPointContentsGame(const NyaVec3Double* point) {
		if (!EXT_GetPointContents) return CONTENTS_EMPTY;
		return EXT_GetPointContents(&point->x);
	}

	pmtrace_t* PointRaytraceGame(const NyaVec3Double* _origin, const NyaVec3Double* _end) {
		static pmtrace_t trace;
		trace.allsolid = false;
		trace.startsolid = false;
		trace.inopen = true;
		trace.inwater = false;
		trace.fraction = 1.0f;
		trace.endpos = *_end;
		trace.plane.normal = {0,0,0};
		trace.plane.dist = 9999;
		trace.ent = -1;
		trace.surfaceId = 0;
		// fallback to regular PlayerTrace if there's no specific point trace func
		auto func = EXT_PointRaytrace;
		if (!func) func = EXT_PM_PlayerTrace;
		if (func) {
			trace = *func(&_origin->x, &_end->x);
		}
		return &trace;
	}

	pmtrace_t* PM_PlayerTraceGame(const NyaVec3Double* _origin, const NyaVec3Double* _end) {
		static pmtrace_t trace;
		trace.allsolid = false;
		trace.startsolid = false;
		trace.inopen = true;
		trace.inwater = false;
		trace.fraction = 1.0f;
		trace.endpos = *_end;
		trace.plane.normal = {0,0,0};
		trace.plane.dist = 9999;
		trace.ent = -1;
		trace.surfaceId = 0;
		if (EXT_PM_PlayerTrace) {
			trace = *EXT_PM_PlayerTrace(&_origin->x, &_end->x);
		}
		return &trace;
	}

	pmtrace_t* PM_PlayerTraceDownGame(const NyaVec3Double* _origin, const NyaVec3Double* _end) {
		static pmtrace_t trace;
		trace.allsolid = false;
		trace.startsolid = false;
		trace.inopen = true;
		trace.inwater = false;
		trace.fraction = 1.0f;
		trace.endpos = *_end;
		trace.plane.normal = {0,0,0};
		trace.plane.dist = 9999;
		trace.ent = -1;
		trace.surfaceId = 0;
		// fallback to regular PlayerTrace if there's no specific down trace func
		auto func = EXT_PM_PlayerTraceDown;
		if (!func) func = EXT_PM_PlayerTrace;
		if (func) {
			trace = *func(&_origin->x, &_end->x);
		}
		return &trace;
	}

	bool IsUsingPlayerTraceFallback() {
		return !EXT_PM_PlayerTrace;
	}
}