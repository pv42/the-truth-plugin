
#include <windows.h>
#include <exception>
#include "MumbleApi.h"
#include "string_format.hpp"
#include "logger.h"

#define SHARED_MEM_NAME L"MumbleLink"

int MumbleApi::initialize() {
	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		SHARED_MEM_NAME);               // name of mapping object

	if (hMapFile == NULL) {
		Logger::e(string_format("[MUMBLE] Could not open file mapping object (%d).",GetLastError()));
		return 1;
	}

	linkedMem = (LinkedMem*)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_READ,  // read permission
		0,
		0,
		sizeof(LinkedMem));

	if (linkedMem == NULL) {
		Logger::e(string_format("[MUMBLE] Could not map view of file (%d).", GetLastError()));
		CloseHandle(hMapFile);
		return 1;
	}
	is_connected = true;
	Logger::i("[MUMBLE] initialized mumble api");
	return 0;
}

void MumbleApi::close() {
	is_connected = false;
	UnmapViewOfFile(linkedMem);
	CloseHandle(hMapFile);
}

uint32_t MumbleApi::getMapId() const {
	if (!is_connected) return (uint32_t)-1;
	return ((MumbleContext*)&(linkedMem->context))->mapId; // less jank pls
}

float* MumbleApi::getPosition() const {
	if (!is_connected) throw std::exception("mumble api is not connected");
	return linkedMem->fAvatarPosition;
}