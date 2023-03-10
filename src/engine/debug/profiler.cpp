#include "profiler.h"

#ifdef USE_PROFILER

SINGLETON_Define(Profiler);

void Profiler::BeginValue()
{
	 m_currentLevel++;
}

void Profiler::EndValue(S32 _nameId, const Float _duration)
{
	m_currentLevel--;
	for(U32 i=0;i<m_valuesTable.GetCount();i++)
	{
		ProfilerValue& current_value = m_valuesTable[i];
		if(current_value.m_name == m_Names[_nameId])
		{
			current_value.m_maxduration = Max(current_value.m_maxduration,_duration);
			current_value.m_duration += _duration;
			return;
		}
	}

	ProfilerValue new_value;
	new_value.m_level = m_currentLevel;
	new_value.m_duration = _duration;
	new_value.m_maxduration = _duration;
	new_value.m_name = m_Names[_nameId];
	m_valuesTable.AddLastItem(new_value);
}

void Profiler::DrawDebug()
{
	S32 poxY = 30;
	for(U32 i=0; i<m_valuesTable.GetCount(); i++)
	{
		Str text;

		text = m_valuesTable[i].m_name.GetStr();
		DEBUG_DrawText(text.GetArray(),350 + m_valuesTable[i].m_level * 10,poxY,Color::Black);

		text.SetFrom("%.3f",m_valuesTable[i].m_duration* 1000.f);
		DEBUG_DrawText(text.GetArray(),700,poxY,Color::Black);

		text.SetFrom("%.3f",m_valuesTable[i].m_maxduration* 1000.f);
		DEBUG_DrawText(text.GetArray(),800,poxY,Color::Black);

		poxY += 20;
	}
}

void Profiler::Flush()
{
	for(U32 i=0; i<m_valuesTable.GetCount(); i++)
	{
		m_valuesTable[i].m_duration = 0;
	}
}

#endif