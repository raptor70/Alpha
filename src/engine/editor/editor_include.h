#ifndef __EDITOR_INCLUDE_H__
#define __EDITOR_INCLUDE_H__
#ifdef USE_EDITOR

#define BEGIN_EDITOR_CLASS(_name,_parent) class _name : public _parent { \
typedef _parent SUPER; \
Q_OBJECT

#define END_EDITOR_CLASS	};

//..

#define SELECTALL_ONFOCUS \
	virtual void focusInEvent(QFocusEvent *e) OVERRIDE		  \
	{														  \
		SUPER::focusInEvent(e);								  \
		QTimer::singleShot(0, this, SLOT(selectAll()));		  \
	}

#endif //USE_EDITOR
#endif // __EDITOR_INCLUDE_H__