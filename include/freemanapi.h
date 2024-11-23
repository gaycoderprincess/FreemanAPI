#include "hl_consts.h"

namespace FreemanAPI {
	// todo these two structs are currently duplicated!
	struct pmplane_t {
		double normal[3];
		float dist;
	};

	struct pmtrace_t {
		size_t size;
		bool allsolid;			// if true, plane is not valid
		bool startsolid;		// if true, the initial point was in a solid area
		bool inopen, inwater;	// End point is in empty space or in water
		float fraction;			// time completed, 1.0 = didn't hit anything
		double endpos[3];	// final position
		pmplane_t plane;		// surface normal at impact
		int ent;				// entity at impact

		// newly added members
		int surfaceId;

		pmtrace_t() {
			Default();
		}

		void Default() {
			size = sizeof(*this);
			allsolid = false;
			startsolid = false;
			inopen = true;
			inwater = false;
			fraction = 1.0f;
			memset(endpos,0,sizeof(endpos));
			memset(plane.normal,0,sizeof(plane.normal));
			plane.dist = 9999;
			ent = -1;
			surfaceId = 0;
		}
	};

	template<typename T>
	T GetFuncPtr(const char* funcName) {
		if (auto dll = LoadLibraryA("FreemanAPI_gcp.dll")) {
			return (T)GetProcAddress(dll, funcName);
		}
		return nullptr;
	}

	void Register_PlayGameSound(void(*func)(const char*, float)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(void(*func)(const char*, float))>("FreemanAPI_Register_PlayGameSound");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_GetGamePlayerDead(bool(*func)()) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(bool(*func)())>("FreemanAPI_Register_GetGamePlayerDead");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_GetGamePlayerPosition(void(*func)(double*)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(void(*func)(double*))>("FreemanAPI_Register_GetGamePlayerPosition");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_GetGamePlayerVelocity(void(*func)(double*)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(void(*func)(double*))>("FreemanAPI_Register_GetGamePlayerVelocity");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_GetGamePlayerViewAngle(void(*func)(double*)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(void(*func)(double*))>("FreemanAPI_Register_GetGamePlayerViewAngle");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_SetGamePlayerPosition(void(*func)(const double*, const double*)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(void(*func)(const double*, const double*))>("FreemanAPI_Register_SetGamePlayerPosition");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_SetGamePlayerViewPosition(void(*func)(const double*)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(void(*func)(const double*))>("FreemanAPI_Register_SetGamePlayerViewPosition");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_SetGamePlayerViewAngle(void(*func)(const double*)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(void(*func)(const double*))>("FreemanAPI_Register_SetGamePlayerViewAngle");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_GetPointContents(int(*func)(const double*)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(int(*func)(const double*))>("FreemanAPI_Register_GetPointContents");
		if (!funcPtr) return;
		funcPtr(func);
	}

	// point trace check for collisions, used as a fallback if you don't have a suitable AABB trace
	void Register_PointRaytrace(pmtrace_t*(*func)(const double*, const double*)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(pmtrace_t*(*func)(const double*, const double*))>("FreemanAPI_Register_PointRaytrace");
		if (!funcPtr) return;
		funcPtr(func);
	}

	// AABB trace check for collisions, optional
	void Register_PM_PlayerTrace(pmtrace_t*(*func)(const double*, const double*)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(pmtrace_t*(*func)(const double*, const double*))>("FreemanAPI_Register_PM_PlayerTrace");
		if (!funcPtr) return;
		funcPtr(func);
	}

	// AABB trace check for collisions to drop down to nearby floors, optional, defaults to PM_PlayerTrace if not set
	void Register_PM_PlayerTraceDown(pmtrace_t*(*func)(const double*, const double*)) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(pmtrace_t*(*func)(const double*, const double*))>("Register_PM_PlayerTraceDown");
		if (!funcPtr) return;
		funcPtr(func);
	}

	// -1 left, 1, right
	void Register_GetGameMoveLeftRight(float(*func)()) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(float(*func)())>("FreemanAPI_Register_GetGameMoveLeftRight");
		if (!funcPtr) return;
		funcPtr(func);
	}

	// 1 fwd, -1 back
	void Register_GetGameMoveFwdBack(float(*func)()) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(float(*func)())>("FreemanAPI_Register_GetGameMoveFwdBack");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_GetGameMoveUpDown(float(*func)()) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(float(*func)())>("FreemanAPI_Register_GetGameMoveUpDown");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_GetGameMoveJump(bool(*func)()) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(bool(*func)())>("FreemanAPI_Register_GetGameMoveJump");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_GetGameMoveDuck(bool(*func)()) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(bool(*func)())>("FreemanAPI_Register_GetGameMoveDuck");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_GetGameMoveRun(bool(*func)()) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(bool(*func)())>("FreemanAPI_Register_GetGameMoveRun");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void Register_GetGameMoveUse(bool(*func)()) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(bool(*func)())>("FreemanAPI_Register_GetGameMoveUse");
		if (!funcPtr) return;
		funcPtr(func);
	}

	void SetIsZUp(bool value) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(bool)>("FreemanAPI_SetIsZUp");
		if (!funcPtr) return;
		funcPtr(value);
	}

	void SetConvertUnits(bool value) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(bool)>("FreemanAPI_SetConvertUnits");
		if (!funcPtr) return;
		funcPtr(value);
	}

	void GetRotateOrder(int* pitch, int* yaw, int* roll) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(int*, int*, int*)>("FreemanAPI_GetRotateOrder");
		if (!funcPtr) return;
		funcPtr(pitch, yaw, roll);
	}

	void SetRotateOrder(int pitch, int yaw, int roll) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(int, int, int)>("FreemanAPI_SetRotateOrder");
		if (!funcPtr) return;
		funcPtr(pitch, yaw, roll);
	}

	void Process(double delta) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(double)>("FreemanAPI_Process");
		if (!funcPtr) return;
		funcPtr(delta);
	}

	void ProcessChloeMenu() {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)()>("FreemanAPI_ProcessChloeMenu");
		if (!funcPtr) return;
		funcPtr();
	}

	void ResetPhysics() {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)()>("FreemanAPI_ResetPhysics");
		if (!funcPtr) return;
		funcPtr();
	}

	void ToggleNoclip() {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)()>("FreemanAPI_ToggleNoclip");
		if (!funcPtr) return;
		funcPtr();
	}

	void SetMoveType(int type) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(int)>("FreemanAPI_SetMoveType");
		if (!funcPtr) return;
		funcPtr(type);
	}

	void SetDefaultMoveType(int type) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(int)>("FreemanAPI_SetDefaultMoveType");
		if (!funcPtr) return;
		funcPtr(type);
	}

	bool GetIsEnabled() {
		static auto funcPtr = GetFuncPtr<bool(__cdecl*)()>("FreemanAPI_GetIsEnabled");
		if (!funcPtr) return false;
		return funcPtr();
	}

	void SetIsEnabled(bool on) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(bool)>("FreemanAPI_SetIsEnabled");
		if (!funcPtr) return;
		funcPtr(on);
	}

	// get player mins for use in PM_PlayerTrace
	double* GetPlayerBBoxMin() {
		static auto funcPtr = GetFuncPtr<double*(__cdecl*)()>("FreemanAPI_GetPlayerBBoxMin");
		if (!funcPtr) return nullptr;
		return funcPtr();
	}

	// get player maxs for use in PM_PlayerTrace
	double* GetPlayerBBoxMax() {
		static auto funcPtr = GetFuncPtr<double*(__cdecl*)()>("FreemanAPI_GetPlayerBBoxMax");
		if (!funcPtr) return nullptr;
		return funcPtr();
	}

	// 0 - behavior, 1 - cvars, 2 - advanced
	// add something custom to the chloe menu and the config reader
	void RegisterCustomBoolean(const char* label, const char* configLabel, bool* ptr, int category) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(const char*, const char*, bool*, int)>("FreemanAPI_RegisterCustomBoolean");
		if (!funcPtr) return;
		return funcPtr(label, configLabel, ptr, category);
	}

	void RegisterCustomInt(const char* label, const char* configLabel, int* ptr, int category) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(const char*, const char*, int*, int)>("FreemanAPI_RegisterCustomInt");
		if (!funcPtr) return;
		return funcPtr(label, configLabel, ptr, category);
	}

	void RegisterCustomFloat(const char* label, const char* configLabel, float* ptr, int category) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(const char*, const char*, float*, int)>("FreemanAPI_RegisterCustomFloat");
		if (!funcPtr) return;
		return funcPtr(label, configLabel, ptr, category);
	}

	void SetConfigName(const char* name) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(const char*)>("FreemanAPI_SetConfigName");
		if (!funcPtr) return;
		return funcPtr(name);
	}

	void LoadConfig() {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)()>("FreemanAPI_LoadConfig");
		if (!funcPtr) return;
		return funcPtr();
	}

	float GetPlayerVelocity() {
		static auto funcPtr = GetFuncPtr<float(__cdecl*)()>("FreemanAPI_GetPlayerVelocity");
		if (!funcPtr) return 0;
		return funcPtr();
	}

	float GetPlayerVelocity2D() {
		static auto funcPtr = GetFuncPtr<float(__cdecl*)()>("FreemanAPI_GetPlayerVelocity2D");
		if (!funcPtr) return 0;
		return funcPtr();
	}

	bool* GetConfigBoolean(const char* label) {
		static auto funcPtr = GetFuncPtr<bool*(__cdecl*)(const char*)>("FreemanAPI_GetConfigBoolean");
		if (!funcPtr) return nullptr;
		return funcPtr(label);
	}

	int* GetConfigInt(const char* label) {
		static auto funcPtr = GetFuncPtr<int*(__cdecl*)(const char*)>("FreemanAPI_GetConfigInt");
		if (!funcPtr) return nullptr;
		return funcPtr(label);
	}

	float* GetConfigFloat(const char* label) {
		static auto funcPtr = GetFuncPtr<float*(__cdecl*)(const char*)>("FreemanAPI_GetConfigFloat");
		if (!funcPtr) return nullptr;
		return funcPtr(label);
	}
}