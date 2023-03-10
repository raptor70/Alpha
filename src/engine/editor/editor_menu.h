#ifndef __EDITOR_MENU_H__
#define __EDITOR_MENU_H__
#include "editor_include.h"
#ifdef USE_EDITOR

class EditorMenu;

BEGIN_EDITOR_CLASS(EditorMenuBar, QMenuBar)
public:
	EditorMenuBar(QWidget * _parent);

protected:
	EditorMenu* AddNewMenu(const Str& _name);
END_EDITOR_CLASS

// ..

BEGIN_EDITOR_CLASS(EditorMenu, QMenu)
public:
	EditorMenu(const Str& _name) :SUPER(_name.GetArray()) {}
	void AddDebugFlagAction(const Str& _name, const DebugFlag& _flag);
	void AddCommandAction(const Str& _name, const Name& _command);
END_EDITOR_CLASS

// ..

BEGIN_EDITOR_CLASS(EditorMenuAction, QAction)
public:
EditorMenuAction(const Char* _name, EditorMenu* _menu):SUPER(_name, _menu) { }
END_EDITOR_CLASS

// ..

BEGIN_EDITOR_CLASS(EditorDebugFlagAction, EditorMenuAction)
public:
	EditorDebugFlagAction(EditorMenu* _menu, const Char* _name, const DebugFlag& _flag);

	~EditorDebugFlagAction()
	{
	}
	DebugFlag		m_Flag;

	public slots:
	void OnAction();
END_EDITOR_CLASS

// ..

BEGIN_EDITOR_CLASS(EditorCommandAction, EditorMenuAction)
public:
	EditorCommandAction(EditorMenu* _menu, const Char* _name, const Name& _command);

	~EditorCommandAction()
	{
	}
	Name		m_nCommand;

	public slots:
	void OnAction();
	END_EDITOR_CLASS

// ..

#endif // USE_EDITOR+

#ifdef USE_EDITOR_V2

//--------

class EditorMenuBase
{
public:
	EditorMenuBase(const Char* _title) : m_Title(_title) {}

	virtual void Update(Float _dTime) {}

protected:
	Str	m_Title;
};

typedef RefTo<EditorMenuBase> EditorMenuBaseRef;
typedef ArrayOf<EditorMenuBaseRef> EditorMenuBaseRefArray;

//--------

BEGIN_SUPER_CLASS(EditorMenu, EditorMenuBase)
public:
	EditorMenu(const Char* _title) : SUPER(_title) {}

	virtual void Update(Float _dTime) OVERRIDE;

	template<typename Type, class ... Args>
	Type*	AddItem(Args&& ... _args)
	{
		return m_Items.AddLastItem(NEW Type(_args...)).GetCastPtr<Type>();
	}
	
protected:
	EditorMenuBaseRefArray	m_Items;

END_SUPER_CLASS

//--------

BEGIN_SUPER_CLASS(EditorMenuCheck, EditorMenuBase)
public:
	EditorMenuCheck(const Char* _title, Bool* _pValue) : SUPER(_title), m_pValue(_pValue) {}

	virtual void Update(Float _dTime) OVERRIDE;

protected:
	Bool*	m_pValue = NULL;

END_SUPER_CLASS

//--------

template<class Action>
BEGIN_SUPER_CLASS(EditorMenuAction, EditorMenuBase)
public:
	EditorMenuAction(const Char* _title, Action& _action) : SUPER(_title), m_Action(FWD(_action)) {}

	void Update(Float _dTime)
	{
		if(ImGui::MenuItem(m_Title.GetArray(), ""))
		{
			m_Action();
		}
	}

protected:
	Action	m_Action; 
END_SUPER_CLASS

//--------

BEGIN_SUPER_CLASS(EditorMenuStats, EditorMenuBase)
public:
	EditorMenuStats() : SUPER("Stats") {}

	void Update(Float _dTime)
	{
		constexpr Float nbSample = 30.f;
		m_fAverageDTime *= nbSample;
		m_fAverageDTime += _dTime;
		m_fAverageDTime /= nbSample + 1.f;
		Float fFPS = 1.f / m_fAverageDTime;
		Str label("%.0f FPS",fFPS);
		ImGui::SameLine(ImGui::GetWindowWidth() - 60.f);
		ImGui::MenuItem(label.GetArray(),NULL,FALSE,FALSE); // TODO : clickable to display stats ?
	}
protected:
	Float	m_fAverageDTime = 1.f / 30.f;
END_SUPER_CLASS

//--------

class EditorMenuBar
{
public:
	void Update(Float _dTime);

	EditorMenu*	AddMenu(const Char* _title)
	{
		return m_Menus.AddLastItem(NEW EditorMenu(_title)).GetCastPtr<EditorMenu>();
	}

	void	AddStats()
	{
		m_Menus.AddLastItem(NEW EditorMenuStats());
	}

protected:
	EditorMenuBaseRefArray	m_Menus;
};

#endif // USE_EDITOR_V2

#endif // __EDITOR_MENU_H__