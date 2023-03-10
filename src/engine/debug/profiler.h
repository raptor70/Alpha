#ifndef __PROFILER_H__
#define __PROFILER_H__

#include "core/timer.h"
#include "profiler_pix.h"
#include "profiler_optick.h"

#ifdef USE_PROFILER

struct ProfilerValue
{
	Name	m_name;
	U32		m_level;
	Float	m_duration;
	Float	m_maxduration;
};

class Profiler
{
	SINGLETON_Declare(Profiler);
public:
	Profiler(){ m_currentLevel = 0; };
	~Profiler(){};

	void BeginValue();
	void EndValue(S32 _nameId, const Float _duration);
	void DrawDebug();
	void Flush();

	S32 AddName(const Name& _name)	{ m_Names.AddLastItem(_name); return m_Names.GetCount() - 1; }

private:
	ArrayOf<ProfilerValue> m_valuesTable;
	ArrayOf<Name>			m_Names;
	U32 m_currentLevel;
};


#define PROFILER_Begin(__NAME__) \
	{\
		PROFILER_PIX_Begin(Color::Red,__NAME__); \
		PROFILER_OPTICK_START(__NAME__); \
		Profiler::GetInstance().BeginValue(); \
		static S32 profilerId = Profiler::GetInstance().AddName(__NAME__); \
		Timer profilerTimer; \
		profilerTimer.Start();

#define PROFILER_End() \
		Profiler::GetInstance().EndValue(profilerId,profilerTimer.GetSecondCount()); \
		PROFILER_OPTICK_STOP(); \
		PROFILER_PIX_End(); \
	}

#define PROFILER_Flush() Profiler::GetInstance().Flush();
#else
	#define PROFILER_Begin(__NAME__)	{}
	#define PROFILER_End(__NAME__)		{}
	#define PROFILER_Flush()			{}
#endif //USE_PROFILER

#endif