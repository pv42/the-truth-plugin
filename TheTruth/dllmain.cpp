// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <memory>

#include "arcdps.h"
#include "imgui/imgui.h"
#include "TheTruth.h"
#include "logger.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)

#define CURL_STATICLIB 

/* dll attach -- from winapi */
void dll_init(HANDLE hModule) {
	try {
		Logger::init("addons\\thetruth-log.txt");
	} catch (std::exception e) {
		// yeah nothing
	}
	return;
}

/* dll detach -- from winapi */
void dll_exit() {
	return;
}

/* dll main from winapi */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		dll_init(hModule);
		break;
	case DLL_PROCESS_DETACH:
		dll_exit();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}


DLL_EXPORT void* get_init_addr(char* arcversionstr, ImGuiContext* imguicontext, void* id3dptr, HANDLE arcdll, void* mallocfn, void* freefn, uint32_t d3dversion) {
	ImGui::SetCurrentContext((ImGuiContext*)imguicontext);
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))mallocfn, (void (*)(void*, void*))freefn); // on imgui 1.80+
	TheTruth::theTruth = std::make_unique<TheTruth>(); // todo fix this
	return TheTruth::s_init;
}


DLL_EXPORT void* get_release_addr() {
	return TheTruth::s_release;
}
