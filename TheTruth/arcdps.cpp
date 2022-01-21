#include <Windows.h>
#include "arcdps.h"


typedef void(*e3_func_ptr)(const char* str);
typedef uint64_t(*e6_func_ptr)();
typedef uint64_t(*e7_func_ptr)();
typedef void(*e8_func_ptr)(const char* str);

e3_func_ptr e3;
e6_func_ptr e6;
e7_func_ptr e7;
e8_func_ptr e8;

void WriteArcLogFile(const char* str) {
	if (e3 != NULL) e3(str);
}

void WriteArcLog(const char* str) {
	if (e8 != NULL) e8(str);
}


void initArcExports(HMODULE arcdpsdll) {
	e3 = (e3_func_ptr)GetProcAddress(arcdpsdll, "e3");
	e6 = (e6_func_ptr)GetProcAddress(arcdpsdll, "e6");
	e7 = (e7_func_ptr)GetProcAddress(arcdpsdll, "e7");
	e8 = (e8_func_ptr)GetProcAddress(arcdpsdll, "e8");
}

arc_config readArcConfig() {
	uint64_t e6_result = e6();
	uint64_t e7_result = e7();
	arc_config cf;
	cf.hideAll = (e6_result & 0x01);
	cf.drawAlways = (e6_result & 0x02);
	cf.uiMoveLock = (e6_result & 0x04);
	cf.uiClickLock = (e6_result & 0x08);
	cf.escClose = (e6_result & 0x10);
	uint16_t* ra = (uint16_t*)&e7_result;
	if (ra) {
		cf.mod1Key = ra[0];
		cf.mod2Key = ra[1];
		cf.modMultiKey = ra[2];
	}
	return cf;
}