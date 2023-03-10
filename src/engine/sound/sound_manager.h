#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

namespace FMOD
{
	namespace Studio
	{
		class EventInstance;
		class Bus;
		class System;
		class Bank;
	}
}

class Sound
{
public:
	Sound(const Str& _eventName);
	~Sound();

	Bool Play();
	Bool IsPlaying() const;
	void Stop(Bool _immediate = FALSE);

	void SetParam(const Char* _name, Float _value);

	void Pause();
	void Resume();

protected:
	Str		m_sEventName;
	FMOD::Studio::EventInstance*	m_pInstance;
};

typedef RefTo<Sound>		SoundRef;
typedef ArrayOf<SoundRef>	SoundRefArray;

class SoundManager
{
	DECLARE_SCRIPT_CLASS(SoundManager);
	SINGLETON_Declare(SoundManager);

public:
	SoundManager();
	virtual ~SoundManager();

	Bool Initialize();
	void Update();
	void Finalize();

	void		Pause();
	void		Resume();

	SoundRef	PlaySound(const Str& _eventName);
	void		ToggleMuteGroup(const Str& _group);
	Bool		IsGroupMuted(const Str& _group) const;
	Bool		IsPaused() const { return m_bPaused; }

	FMOD::Studio::System*			GetSystem() const { return m_pSystem; }

	void		Dump();

protected:
	FMOD::Studio::Bus*				GetBus(const Str& _bus) const;

	FMOD::Studio::System*			m_pSystem;
	FMOD::Studio::Bank*				m_pBank;
	FMOD::Studio::Bank*				m_pStringBank;

	SoundRefArray					m_Sounds;
	Bool							m_bPaused;
};

#endif
