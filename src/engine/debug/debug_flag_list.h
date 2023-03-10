#ifndef __DEBUG_FLAG_LIST_H__
#define __DEBUG_FLAG_LIST_H__

enum DebugFlag
{
	DEBUGFLAG_None = -1,

#define DECLARE_DEBUGFLAG(_name,_deFaultvalue)	DEBUGFLAG_##_name,
#include <debug/debug_flag.h>
#include <game_debug_flag.h>
#undef DECLARE_DEBUGFLAG

	DEBUGFLAG_Count
};


class DebugFlagList
{
	SINGLETON_Declare(DebugFlagList);
public:
	DebugFlagList();

	Bool	IsFlagEnable(const DebugFlag& _flag) { return m_Flags[_flag]; }
	void	SetFlag(const DebugFlag& _flag, Bool _value) { m_Flags[_flag] = _value; }
	Bool*	GetFlagPtr(const DebugFlag& _flag) { return &(m_Flags[_flag]); }

protected:
	Bool m_Flags[DEBUGFLAG_Count];
};

#define DEBUGFLAG(_name)	DebugFlagList::GetInstance().IsFlagEnable(DEBUGFLAG_##_name)

#endif