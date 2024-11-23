extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_PlayGameSound(void(*func)(const char*, float)) {
	HLMovement::EXT_PlayGameSound = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGamePlayerDead(bool(*func)()) {
	HLMovement::EXT_GetGamePlayerDead = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGamePlayerPosition(void(*func)(double*)) {
	HLMovement::EXT_GetGamePlayerPosition = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGamePlayerVelocity(void(*func)(double*)) {
	HLMovement::EXT_GetGamePlayerVelocity = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGamePlayerViewAngle(void(*func)(double*)) {
	HLMovement::EXT_GetGamePlayerViewAngle = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_SetGamePlayerPosition(void(*func)(const double*, const double*)) {
	HLMovement::EXT_SetGamePlayerPosition = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_SetGamePlayerViewPosition(void(*func)(const double*)) {
	HLMovement::EXT_SetGamePlayerViewPosition = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_SetGamePlayerViewAngle(void(*func)(const double*)) {
	HLMovement::EXT_SetGamePlayerViewAngle = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetPointContents(int(*func)(const double*)) {
	HLMovement::EXT_GetPointContents = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_PointRaytrace(HLMovement::pmtrace_t*(*func)(const double*, const double*)) {
	HLMovement::EXT_PointRaytrace = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveLeftRight(float(*func)()) {
	HLMovement::EXT_GetGameMoveLeftRight = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveFwdBack(float(*func)()) {
	HLMovement::EXT_GetGameMoveFwdBack = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveUpDown(float(*func)()) {
	HLMovement::EXT_GetGameMoveUpDown = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveJump(bool(*func)()) {
	HLMovement::EXT_GetGameMoveJump = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveDuck(bool(*func)()) {
	HLMovement::EXT_GetGameMoveDuck = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveRun(bool(*func)()) {
	HLMovement::EXT_GetGameMoveRun = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_GetGameMoveUse(bool(*func)()) {
	HLMovement::EXT_GetGameMoveUse = func;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_FOV(float* value) {
	HLMovement::EXT_fov = value;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_Sensitivity(float* value) {
	HLMovement::EXT_sensitivity = value;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Register_Volume(float* value) {
	HLMovement::EXT_volume = value;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetIsZUp(bool on) {
	if (on) {
		NyaMat4x4::bZUp = true;
		HLMovement::FORWARD = 1;
		HLMovement::UP = 2;
	}
	else {
		NyaMat4x4::bZUp = false;
		HLMovement::FORWARD = 2;
		HLMovement::UP = 1;
	}
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetConvertUnits(bool on) {
	HLMovement::bConvertUnits = on;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetRotateOrder(int pitch, int yaw, int roll) {
	if (pitch >= 0 && pitch <= 2) HLMovement::PITCH = pitch;
	if (yaw >= 0 && yaw <= 2) HLMovement::YAW = yaw;
	if (roll >= 0 && roll <= 2) HLMovement::ROLL = roll;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_Process(double delta) {
	static bool bOnce = true;
	if (bOnce) {
		HLMovement::Reset();
		bOnce = false;
	}

	HLMovement::Process(delta);
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_ProcessChloeMenu() {
	HLMovement::ProcessMenu();
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_ResetPhysics() {
	HLMovement::Reset();
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_ToggleNoclip() {
	HLMovement::ToggleNoclip();
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetMoveType(int type) {
	HLMovement::pmove->movetype = type;
}
extern "C" __declspec(dllexport) bool __cdecl FreemanAPI_GetIsEnabled() {
	return HLMovement::bEnabled;
}
extern "C" __declspec(dllexport) void __cdecl FreemanAPI_SetIsEnabled(bool on) {
	HLMovement::bEnabled = on;
}