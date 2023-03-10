#ifndef __WINDOW_MENU_H__
#define __WINDOW_MENU_H__

class WindowMenu
{
public :    
	WindowMenu( Str _name );
	virtual ~WindowMenu() { }

	const Str& GetName() const {return m_sName;}

	void AddCommand(const Char* _name, S32 _commandId);

	virtual Bool Create(const PtrTo<Object>& _pParentPlatformData);

	PtrTo<Object>   GetPlatformData() const { return m_pPlatformData; }

private :

	// forbidden
	WindowMenu() {}
	WindowMenu( WindowMenu & ) {}
	WindowMenu & operator=( WindowMenu & ) { return *this; }

	struct MenuCommand
	{
		Str	m_sName;
		S32 m_id;

		MenuCommand(const Char* _name, S32 _commandId) 
		{
			m_sName = _name;
			m_id = _commandId;
		}
	} ;

	Str		m_sName;
	RefTo<Object>	m_pPlatformData;
	ArrayOf< RefTo<MenuCommand> > m_aCommands;
};

typedef RefTo<WindowMenu> WindowMenuRef;
typedef ArrayOf<WindowMenuRef> WindowMenuRefArray;

#endif