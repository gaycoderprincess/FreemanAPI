extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_PlayGameSound(void(*func)(const char*, float)) {
	FreemanAPI::EXT_PlayGameSound = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGamePlayerDead(bool(*func)()) {
	FreemanAPI::EXT_GetGamePlayerDead = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGamePlayerPosition(void(*func)(double*)) {
	FreemanAPI::EXT_GetGamePlayerPosition = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGamePlayerVelocity(void(*func)(double*)) {
	FreemanAPI::EXT_GetGamePlayerVelocity = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGamePlayerViewAngle(void(*func)(double*)) {
	FreemanAPI::EXT_GetGamePlayerViewAngle = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_SetGamePlayerPosition(void(*func)(const double*, const double*)) {
	FreemanAPI::EXT_SetGamePlayerPosition = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_SetGamePlayerViewPosition(void(*func)(const double*)) {
	FreemanAPI::EXT_SetGamePlayerViewPosition = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_SetGamePlayerViewAngle(void(*func)(const double*)) {
	FreemanAPI::EXT_SetGamePlayerViewAngle = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetPointContents(int(*func)(const double*)) {
	FreemanAPI::EXT_GetPointContents = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_PointRaytrace(FreemanAPI::pmtrace_t*(*func)(const double*, const double*)) {
	FreemanAPI::EXT_PointRaytrace = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_PM_PlayerTrace(FreemanAPI::pmtrace_t*(*func)(const double*, const double*)) {
	FreemanAPI::EXT_PM_PlayerTrace = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_PM_PlayerTraceDown(FreemanAPI::pmtrace_t*(*func)(const double*, const double*)) {
	FreemanAPI::EXT_PM_PlayerTraceDown = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveLeftRight(float(*func)()) {
	FreemanAPI::EXT_GetGameMoveLeftRight = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveFwdBack(float(*func)()) {
	FreemanAPI::EXT_GetGameMoveFwdBack = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveUpDown(float(*func)()) {
	FreemanAPI::EXT_GetGameMoveUpDown = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveJump(bool(*func)()) {
	FreemanAPI::EXT_GetGameMoveJump = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveDuck(bool(*func)()) {
	FreemanAPI::EXT_GetGameMoveDuck = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveRun(bool(*func)()) {
	FreemanAPI::EXT_GetGameMoveRun = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveUse(bool(*func)()) {
	FreemanAPI::EXT_GetGameMoveUse = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetIsZUp(bool on) {
	if (on) {
		NyaMat4x4::bZUp = true;
		FreemanAPI::FORWARD = 1;
		FreemanAPI::UP = 2;
	}
	else {
		NyaMat4x4::bZUp = false;
		FreemanAPI::FORWARD = 2;
		FreemanAPI::UP = 1;
	}
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetConvertUnits(bool on) {
	FreemanAPI::bConvertUnits = on;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetRotateOrder(int pitch, int yaw, int roll) {
	if (pitch >= 0 && pitch <= 2) FreemanAPI::PITCH = pitch;
	if (yaw >= 0 && yaw <= 2) FreemanAPI::YAW = yaw;
	if (roll >= 0 && roll <= 2) FreemanAPI::ROLL = roll;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Process(double delta) {
	static bool bOnce = true;
	if (bOnce) {
		FreemanAPI::Reset();
		bOnce = false;
	}

	FreemanAPI::Process(delta);
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_ProcessChloeMenu() {
	FreemanAPI::ProcessMenu();
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_ResetPhysics() {
	FreemanAPI::Reset();
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_ToggleNoclip() {
	FreemanAPI::ToggleNoclip();
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetMoveType(int type) {
	FreemanAPI::pmove->movetype = type;
}
extern "C" __declspec(dllexport) bool __cdecl FreemanAPI_GetIsEnabled() {
	return FreemanAPI::bEnabled;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetIsEnabled(bool on) {
	FreemanAPI::bEnabled = on;
}
extern "C" __declspec(dllexport) double* __cdecl FreemanAPI_GetPlayerBBoxMin() {
	return &FreemanAPI::pmove->player_mins[FreemanAPI::pmove->usehull].x;
}
extern "C" __declspec(dllexport) double* __cdecl FreemanAPI_GetPlayerBBoxMax() {
	return &FreemanAPI::pmove->player_maxs[FreemanAPI::pmove->usehull].x;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_RegisterCustomBoolean(const char* label, const char* configLabel, bool* ptr, int category) {
	FreemanAPI::AddBoolToCustomConfig(FreemanAPI::GetCustomConfig(category), label, configLabel, ptr);
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_RegisterCustomInt(const char* label, const char* configLabel, int* ptr, int category) {
	FreemanAPI::AddIntToCustomConfig(FreemanAPI::GetCustomConfig(category), label, configLabel, ptr);
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_RegisterCustomFloat(const char* label, const char* configLabel, float* ptr, int category) {
	FreemanAPI::AddFloatToCustomConfig(FreemanAPI::GetCustomConfig(category), label, configLabel, ptr);
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetConfigName(const char* name) {
	if (!name) return;
	FreemanAPI::sConfigName = name;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_LoadConfig() {
	FreemanAPI::LoadConfig();
}
extern "C" __declspec(dllexport) float __cdecl FreemanAPI_GetPlayerVelocity() {
	return FreemanAPI::pmove->velocity.length();
}
extern "C" __declspec(dllexport) float __cdecl FreemanAPI_GetPlayerVelocity2D() {
	auto vel = FreemanAPI::pmove->velocity;
	vel[FreemanAPI::UP] = 0;
	return vel.length();
}
extern "C" __declspec(dllexport) bool* __cdecl FreemanAPI_GetConfigBoolean(const char* label) {
	auto config = FreemanAPI::FindConfigValue(label);
	if (!config) return nullptr;
	return config->bValue;
}
extern "C" __declspec(dllexport) int* __cdecl FreemanAPI_GetConfigInt(const char* label) {
	auto config = FreemanAPI::FindConfigValue(label);
	if (!config) return nullptr;
	return config->iValue;
}
extern "C" __declspec(dllexport) float* __cdecl FreemanAPI_GetConfigFloat(const char* label) {
	auto config = FreemanAPI::FindConfigValue(label);
	if (!config) return nullptr;
	return config->fValue;
}