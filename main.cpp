#include <windows.h>
#include <vector>
#include <cstdint>
#include <string>
#include <format>
#include "toml++/toml.hpp"

#include "nya_commonmath.h"

#include "chloemenulib.h"

#include "hlmov.h"
#include "hl_exports.h"

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {

		} break;
		default:
			break;
	}
	return TRUE;
}