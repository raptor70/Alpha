#include <debug/debug_flag_list.h>

SINGLETON_Define(DebugFlagList);

DebugFlagList::DebugFlagList()
{
#define DECLARE_DEBUGFLAG(_name,_deFaultvalue)	m_Flags[DEBUGFLAG_##_name]=_deFaultvalue;
#include <debug/debug_flag.h>
#include <game_debug_flag.h>
#undef DECLARE_DEBUGFLAG
}