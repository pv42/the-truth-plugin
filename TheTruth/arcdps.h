#pragma once

#ifndef arcdps_h_
#define arcdps_h_

#include <stdint.h>

/* arcdps export table */
typedef struct arcdps_exports {
	uintptr_t size; /* size of exports table */
	uint32_t sig; /* pick a number between 0 and uint32_t max that isn't used by other modules */
	uint32_t imguivers; /* set this to IMGUI_VERSION_NUM. if you don't use imgui, 18000 (as of 2021-02-02) */
	const char* out_name; /* name string */
	const char* out_build; /* build string */
	void* wnd_nofilter; /* wndproc callback, fn(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) */
	void* combat; /* combat event callback, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
	void* imgui; /* id3dd9::present callback, before imgui::render, fn(uint32_t not_charsel_or_loading) */
	void* options_end; /* id3dd9::present callback, appending to the end of options window in arcdps, fn() */
	void* combat_local;  /* combat event callback like area but from chat log, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
	void* wnd_filter; /* wndproc callback like above, input filered using modifiers */
	void* options_windows; /* called once per 'window' option checkbox, with null at the end, non-zero return disables drawing that option, fn(char* windowname) */
} arcdps_exports;

/* combat event - see evtc docs for details, revision param in combat cb is equivalent of revision byte header */
typedef struct cbtevent {
	uint64_t time;
	uintptr_t src_agent;
	uintptr_t dst_agent;
	int32_t value;
	int32_t buff_dmg;
	uint32_t overstack_value;
	uint32_t skillid;
	uint16_t src_instid;
	uint16_t dst_instid;
	uint16_t src_master_instid;
	uint16_t dst_master_instid;
	uint8_t iff;
	uint8_t buff;
	uint8_t result;
	uint8_t is_activation;
	uint8_t is_buffremove;
	uint8_t is_ninety;
	uint8_t is_fifty;
	uint8_t is_moving;
	uint8_t is_statechange; // cbtstatechange 
	uint8_t is_flanking;
	uint8_t is_shields;
	uint8_t is_offcycle;
	uint8_t pad61;
	uint8_t pad62;
	uint8_t pad63;
	uint8_t pad64;
} cbtevent;

/* is friend/foe*/
enum iff {
	IFF_FRIEND,
	IFF_FOE,
	IFF_UNKNOWN // or uncertain
};

/* combat state change */
enum cbtstatechange {
	CBTS_NONE, // 0 not used - not this kind of event
	CBTS_ENTERCOMBAT, // 1 src_agent entered combat, dst_agent is subgroup
	CBTS_EXITCOMBAT, // 2 src_agent left combat
	CBTS_CHANGEUP, // 3 src_agent is now alive
	CBTS_CHANGEDEAD, // 4 src_agent is now dead
	CBTS_CHANGEDOWN, // 5 src_agent is now downed
	CBTS_SPAWN, // 6 src_agent is now in game tracking range (not in realtime api)
	CBTS_DESPAWN, // 7 src_agent is no longer being tracked (not in realtime api)
	CBTS_HEALTHUPDATE, // 8 src_agent has reached a health marker. dst_agent = percent * 10000 (eg. 99.5% will be 9950) (not in realtime api)
	CBTS_LOGSTART, // 9 log start. value = server unix timestamp **uint32**. buff_dmg = local unix timestamp. src_agent = 0x637261 (arcdps id) if evtc, npc id if realtime
	CBTS_LOGEND, // 10 log end. value = server unix timestamp **uint32**. buff_dmg = local unix timestamp. src_agent = 0x637261 (arcdps id)
	CBTS_WEAPSWAP, // 11 src_agent swapped weapon set. dst_agent = current set id (0/1 water, 4/5 land)
	CBTS_MAXHEALTHUPDATE, // 12 src_agent has had it's maximum health changed. dst_agent = new max health (not in realtime api)
	CBTS_POINTOFVIEW, // 13 src_agent is agent of "recording" player
	CBTS_LANGUAGE, // 14 src_agent is text language
	CBTS_GWBUILD, // 15 src_agent is game build
	CBTS_SHARDID, // 16 src_agent is sever shard id
	CBTS_REWARD, // 17 src_agent is self, dst_agent is reward id, value is reward type. these are the wiggly boxes that you get
	CBTS_BUFFINITIAL, // 18 combat event that will appear once per buff per agent on logging start (statechange==18, buff==18, normal cbtevent otherwise)
	CBTS_POSITION, // 19 src_agent changed, cast float* p = (float*)&dst_agent, access as x/y/z (float[3]) (not in realtime api)
	CBTS_VELOCITY, // 20 src_agent changed, cast float* v = (float*)&dst_agent, access as x/y/z (float[3]) (not in realtime api)
	CBTS_FACING, // 21 src_agent changed, cast float* f = (float*)&dst_agent, access as x/y (float[2]) (not in realtime api)
	CBTS_TEAMCHANGE, // 22 src_agent change, dst_agent new team id
	CBTS_ATTACKTARGET, // 23 src_agent is an attacktarget, dst_agent is the parent agent (gadget type), value is the current targetable state (not in realtime api)
	CBTS_TARGETABLE, // 24 dst_agent is new target-able state (0 = no, 1 = yes. default yes) (not in realtime api)
	CBTS_MAPID, // 25 src_agent is map id
	CBTS_REPLINFO, // 26 internal use, won't see anywhere
	CBTS_STACKACTIVE, // 27 src_agent is agent with buff, dst_agent is the stackid marked active
	CBTS_STACKRESET, // 28 src_agent is agent with buff, value is the duration to reset to (also marks inactive), pad61- is the stackid
	CBTS_GUILD, // 29 src_agent is agent, dst_agent through buff_dmg is 16 byte guid (client form, needs minor rearrange for api form),
	CBTS_BUFFINFO, // 30 is_flanking = probably invuln, is_shields = probably invert, is_offcycle = category, pad61 = stacking type, pad62 = probably resistance, src_master_instid = max stacks (not in realtime)
	CBTS_BUFFFORMULA, // 31 (float*)&time[8]: type attr1 attr2 param1 param2 param3 trait_src trait_self, is_flanking = !npc, is_shields = !player, is_offcycle = break, overstack = value of type determined by pad61 (none/number/skill) (not in realtime, one per formula)
	CBTS_SKILLINFO, // 32 (float*)&time[4]: recharge range0 range1 tooltiptime (not in realtime)
	CBTS_SKILLTIMING, // 33 src_agent = action, dst_agent = at millisecond (not in realtime, one per timing)
	CBTS_BREAKBARSTATE, // 34 src_agent is agent, value is u16 game enum (active, recover, immune, none) (not in realtime api)
	CBTS_BREAKBARPERCENT, // 35 src_agent is agent, value is float with percent (not in realtime api)
	CBTS_ERROR, // 36 (char*)&time[32]: error string (not in realtime api)
	CBTS_TAG // 37 src_agent is agent, value is the id (volatile, game build dependent) of the tag
};

/* agent short */
typedef struct ag {
	char* name; /* agent name. may be null. valid only at time of event. utf8 */
	uintptr_t id; /* agent unique identifier */
	uint32_t prof; /* profession at time of event. refer to evtc notes for identification */
	uint32_t elite; /* elite spec at time of event. refer to evtc notes for identification */
	uint32_t self; /* 1 if self, 0 if not */
	uint16_t team; /* sep21+ */
} ag;
 
#endif // !arcdps_h_
