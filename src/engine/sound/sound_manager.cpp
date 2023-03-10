#include "sound_manager.h"

#include "file/base_file.h"

#include INCLUDE_LIB(LIB_FMOD, windows/studio/inc/fmod_studio.hpp)
#ifdef _DEBUG
	#pragma LINK_LIB(LIB_FMOD, windows/lowlevel/lib/fmodL64_vc.lib)
	#pragma LINK_LIB(LIB_FMOD, windows/studio/lib/fmodstudioL64_vc.lib)
#else
	#pragma LINK_LIB(LIB_FMOD, windows/lowlevel/lib/fmod64_vc.lib)
	#pragma LINK_LIB(LIB_FMOD, windows/studio/lib/fmodstudio64_vc.lib)
#endif

#if defined(ANDROID) || defined(IOS)
	#define PLATFORM_FOLDER	"Mobile"
#else
	#define PLATFORM_FOLDER	"Desktop"
#endif

SINGLETON_Define(SoundManager);

static S32 soundcount = 0;
static S32 fmodcount = 0;

BEGIN_SCRIPT_CLASS(SoundManager)
	SCRIPT_STATIC_RESULT_METHOD(GetInstance,SoundManager)
	SCRIPT_VOID_METHOD1(PlaySound,Str)
	SCRIPT_VOID_METHOD1(ToggleMuteGroup,Str)
	SCRIPT_RESULT_METHOD1(IsGroupMuted,Bool,Str)
END_SCRIPT_CLASS

//------------------------------------------

Sound::Sound(const Str& _eventName)
{
	m_sEventName = _eventName;
	m_pInstance = NULL;
	soundcount++;
}

//------------------------------------------

Sound::~Sound()
{
	Stop(TRUE);
	soundcount--;
}

//------------------------------------------

Bool Sound::Play()
{
	if(SoundManager::GetInstance().IsPaused())
		return FALSE;

	FMOD::Studio::EventDescription* soundEvent = NULL;
	if( SoundManager::GetInstance().GetSystem()->getEvent(m_sEventName.GetArray(), &soundEvent) != FMOD_OK )
	{
		LOGGER_LogError("Unable to get FMOD event %s !!!\n",m_sEventName.GetArray());
		return FALSE;
	}

	if( soundEvent->createInstance(&m_pInstance) != FMOD_OK )
	{
		LOGGER_LogError("Unable to create FMOD instance !!!\n");
		return FALSE;
	}

	{
		FMOD_RESULT res = m_pInstance->start();
	}

	fmodcount++;

	return TRUE;
}

//------------------------------------------

Bool Sound::IsPlaying() const
{
	if(SoundManager::GetInstance().IsPaused())
			return FALSE;

	FMOD_STUDIO_PLAYBACK_STATE state;
	if( m_pInstance && m_pInstance->getPlaybackState(&state) == FMOD_OK)
		return state != FMOD_STUDIO_PLAYBACK_STOPPED;

	return FALSE;
}

//------------------------------------------

void Sound::Stop(Bool _immediate)
{
	if(SoundManager::GetInstance().IsPaused())
				return;

	if( m_pInstance )
	{
		m_pInstance->stop(_immediate?FMOD_STUDIO_STOP_IMMEDIATE:FMOD_STUDIO_STOP_ALLOWFADEOUT);
		m_pInstance->release();
		m_pInstance = NULL;
		fmodcount--;
	}
}

//------------------------------------------

void Sound::Pause()
{
	if(SoundManager::GetInstance().IsPaused())
		return;

	if( m_pInstance )
		m_pInstance->setPaused(TRUE);
}

//------------------------------------------

void Sound::Resume()
{
	if(SoundManager::GetInstance().IsPaused())
			return;

	if( m_pInstance )
		m_pInstance->setPaused(FALSE);
}

//------------------------------------------

void Sound::SetParam(const Char* _name, Float _value)
{
	if(SoundManager::GetInstance().IsPaused())
		return;

	if( m_pInstance )
	{
		if( m_pInstance->setParameterValue(_name,_value) != FMOD_OK  )
		{
			LOGGER_LogError("Unable to set parameter %s on event %s !!!\n",_name,m_sEventName.GetArray());
		}

		LOGGER_Log("SetParam %s = %f\n",_name,_value);
	}
}

//------------------------------------------

SoundManager::SoundManager()
{
	m_pSystem = NULL;
	m_pBank = NULL;
	m_pStringBank = NULL;
	m_bPaused = FALSE;
}

//------------------------------------------

SoundManager::~SoundManager()
{
	Finalize();
}

//------------------------------------------

Bool SoundManager::Initialize()
{
#ifdef WIN32
	FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_WARNING);
#endif

	if( FMOD::Studio::System::create(&m_pSystem) != FMOD_OK )
	{
		LOGGER_LogError("Unable to create FMOD system !!!\n");
		return FALSE;
	}

	FMOD_STUDIO_INITFLAGS flag = FMOD_STUDIO_INIT_NORMAL;
#ifdef WIN32
	flag |= FMOD_STUDIO_INIT_LIVEUPDATE;
#endif

	if( m_pSystem->initialize(32, flag, FMOD_INIT_NORMAL, NULL) != FMOD_OK )
	{
		LOGGER_LogError("Unable to initialize FMOD system !!!\n");
		return FALSE;
	}

	{
		OldFile file("sound/" PLATFORM_FOLDER "/Master Bank.bank",OldFile::USE_TYPE_ReadBuffer);
		file.Open();
		FMOD_RESULT res = m_pSystem->loadBankMemory(file.GetBuffer(),file.GetBufferSize(),FMOD_STUDIO_LOAD_MEMORY,FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pBank);
		if( res != FMOD_OK )
		{
			LOGGER_LogError("Unable to load FMOD Master bank !!!\n");
			file.Close();
			return FALSE;
		}
		file.Close();
	}

	{
		OldFile file("sound/" PLATFORM_FOLDER "/Master Bank.strings.bank",OldFile::USE_TYPE_ReadBuffer);
		file.Open();
		FMOD_RESULT res = m_pSystem->loadBankMemory(file.GetBuffer(),file.GetBufferSize(),FMOD_STUDIO_LOAD_MEMORY,FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pStringBank);
		if( res != FMOD_OK )
		{
			LOGGER_LogError("Unable to load FMOD Master string bank !!!\n");
			file.Close();
			return FALSE;
		}
		file.Close();
	}
	
	return TRUE;
}

//------------------------------------------

void SoundManager::Update()
{
	if( m_bPaused )
		return;

	if( m_pSystem )
		m_pSystem->update();

	for(S32 i=m_Sounds.GetCount()-1;i>=0;i--)
	{
		if( !m_Sounds[i]->IsPlaying() )
		{
			m_Sounds.RemoveItems(i,1);
		}
	}
}

//------------------------------------------

void SoundManager::Finalize()
{
	if(m_bPaused)
		Resume();

	m_Sounds.SetEmpty(); 

	LOGGER_Log("SoundManager::Finalize fmodcount %d soundcount %d\n",fmodcount,soundcount);

	if( m_pStringBank )
		m_pStringBank->unload();

	if( m_pBank )
		m_pBank->unload();

	if( m_pSystem )
		m_pSystem->release();
}

//------------------------------------------

void	SoundManager::Pause()
{
	LOGGER_Log("SoundManager::Pause\n");
	if( m_pSystem )
	{
		FMOD::System* system = NULL;
		m_pSystem->getLowLevelSystem(&system);
		if( system )
			system->mixerSuspend();
	}

	m_bPaused = TRUE;
}

//------------------------------------------

void	SoundManager::Resume()
{
	LOGGER_Log("SoundManager::Resume\n");
	if( m_pSystem )
	{
		FMOD::System* system = NULL;
		m_pSystem->getLowLevelSystem(&system);
		if( system )
			system->mixerResume();
	}

	m_bPaused = FALSE;
}

//------------------------------------------

SoundRef SoundManager::PlaySound(const Str& _eventName)
{
	if(m_bPaused)
		return SoundRef();

	if(_eventName.IsEmpty() )
		return SoundRef();

	SoundRef newSound = NEW Sound(_eventName);
	if( newSound->Play() )
	{
		m_Sounds.AddLastItem(newSound);
		return newSound;
	}

	return SoundRef();
}

//------------------------------------------

void		SoundManager::ToggleMuteGroup(const Str& _group)
{
	if(m_bPaused)
		return;

	FMOD::Studio::Bus* bus = GetBus(_group);
	if( bus )
	{
		Bool isMute = FALSE;
		bus->getMute(&isMute);
		bus->setMute(!isMute);
	}
}
		
//------------------------------------------

Bool		SoundManager::IsGroupMuted(const Str& _group) const
{
	if(m_bPaused)
		return TRUE;

	FMOD::Studio::Bus* bus = GetBus(_group);
	if( bus )
	{
		Bool isMute = FALSE;
		bus->getMute(&isMute);
		return isMute;
	}

	return FALSE;
}
		
//------------------------------------------

FMOD::Studio::Bus*		SoundManager::GetBus(const Str& _bus) const
{
	FMOD::Studio::Bus* bus = NULL;
	FMOD_RESULT res = m_pSystem->getBus(_bus.GetArray(),&bus);
	if( res != FMOD_OK )
	{
		LOGGER_LogError("Unable to get FMOD bus %s !!!\n",_bus.GetArray());
		return NULL;
	}

	return bus;
}

//------------------------------------------

void		SoundManager::Dump()
{
	int current, maxi;
	if (FMOD::Memory_GetStats(&current, &maxi, TRUE) == FMOD_OK)
	{
		LOGGER_Log("Sound memory - Current %.2f Mo - Maxi %.2f Mo\n", Float(current) / (1024.f * 1024.f), Float(maxi) / (1024.f * 1024.f));
	}
}