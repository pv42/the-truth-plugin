#pragma once
#include <stdint.h>
#include <windows.h>

struct LinkedMem {
	uint32_t uiVersion;
	uint32_t uiTick;
	float fAvatarPosition[3];
	float fAvatarFront[3];
	float fAvatarTop[3];
	wchar_t name[256];
	float fCameraPosition[3];
	float fCameraFront[3];
	float fCameraTop[3];
	wchar_t identity[256];
	uint32_t context_len; // Despite the actual context containing more data, this value is always 48. See "context" section below.
	unsigned char context[256];
	wchar_t description[2048];
};

typedef struct {
	unsigned char serverAddress[28]; // contains sockaddr_in or sockaddr_in6
	uint32_t mapId;
	uint32_t mapType;
	uint32_t shardId;
	uint32_t instance;
	uint32_t buildId;
	// Additional data beyond the 48 bytes Mumble uses for identification
	uint32_t uiState; // Bitmask: Bit 1 = IsMapOpen, Bit 2 = IsCompassTopRight, Bit 3 = DoesCompassHaveRotationEnabled, Bit 4 = Game has focus, Bit 5 = Is in Competitive game mode, Bit 6 = Textbox has focus, Bit 7 = Is in Combat
	uint16_t compassWidth; // pixels
	uint16_t compassHeight; // pixels
	float compassRotation; // radians
	float playerX; // continentCoords
	float playerY; // continentCoords
	float mapCenterX; // continentCoords
	float mapCenterY; // continentCoords
	float mapScale;
	uint32_t processId;
	uint8_t mountIndex;
} MumbleContext;


enum MountIndex {
	None = 0,
	Jackal = 1,
	Griffon = 2,
	Springer = 3,
	Skimmer = 4,
	Raptor = 5,
	RollerBeetle = 6,
	Warclaw = 7,
	Skyscale = 8
};

enum Profession {
	Guardian = 1,
	Warrior = 2,
	Engineer = 3,
	Ranger = 4,
	Thief = 5,
	Elementalist = 6,
	Mesmer = 7,
	Necromancer = 8,
	Revenant = 9
};

class MumbleApi {
	public: 	
		int initialize();
		uint32_t getMapId() const;
		float* getPosition() const;
		void close();
	private:
		HANDLE hMapFile;
		LinkedMem* linkedMem;
		bool is_connected;
};
