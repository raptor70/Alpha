#ifndef __EDITOR_ENTITIES_PROPERTIES_H__
#define __EDITOR_ENTITIES_PROPERTIES_H__
#include "editor_include.h"
#ifdef USE_EDITOR

//..

#include "ui\ui_entities_properties_component.h"

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesComponentFrame, QFrame)
public:
	EditorEntitiesPropertiesComponentFrame(QWidget *_parent);
	void	setComponent(EntityComponent* _component);
	
	Ui_EntitiesPropertiesComponent	m_UiComponent;
public slots:
	void toggleParamsVisibility();
	void onCustomContextMenu(const QPoint &_point);
signals:
	void NeedContextMenu(EntityComponent* _pComp, const QPoint & _point);
protected:
	EntityComponent* m_Component;
END_EDITOR_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesLineEdit, QLineEdit)
public:
	EditorEntitiesPropertiesLineEdit(QWidget *_parent) :SUPER(_parent) {}

	SELECTALL_ONFOCUS
END_EDITOR_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesLabel, QLabel)
public:
	EditorEntitiesPropertiesLabel(QWidget *_parent) :SUPER(_parent) {}

public slots:
	void Changed() { setStyleSheet("font-weight: bold;"); }
END_EDITOR_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesBoolCheckBox, QCheckBox)
public:
	EditorEntitiesPropertiesBoolCheckBox(QWidget *_parent);
	void setParam(const Name& _name, Bool _value);
signals:
	void onBoolChanged(const Name& _name, const Bool _value);
	void onChanged();
public slots:
	void onValueChanged(int _state);
protected:
	Name	m_nParam;
END_EDITOR_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesStrEdit, EditorEntitiesPropertiesLineEdit)
public:
	EditorEntitiesPropertiesStrEdit(QWidget *_parent);
	void setParam(const Name& _name, const Str& _value);
signals:
	void onStrChanged(const Name& _name, const Str& _value);
	void onChanged();
public slots:
	void onValueChanged();
protected:
	Name	m_nParam;
END_EDITOR_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesS32Edit, QSpinBox)
public:
	EditorEntitiesPropertiesS32Edit(QWidget *_parent);
	void setParam(const Name& _name, S32 _value);
	SELECTALL_ONFOCUS
signals:
	void onS32Changed(const Name& _name, S32 _value);
	void onChanged();
public slots:
	void onValueChanged(int _value);
protected:
	Name	m_nParam;
END_EDITOR_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesFloatEdit, EditorEntitiesPropertiesLineEdit)
public:
	EditorEntitiesPropertiesFloatEdit(QWidget *_parent);
	void setParam(const Name& _name, Float _value);
signals:
	void onFloatChanged(const Name& _name, Float _value);
	void onChanged();
public slots:
	void onValueChanged();
protected:
	Name	m_nParam;
END_EDITOR_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesColorButton, QPushButton)
public:
	EditorEntitiesPropertiesColorButton(QWidget *_parent);
	void setParam(const Name& _name, const Color& _color);
	void setColor(const Color& _color);
	Color getColor();
signals:
	void onColorChanged(const Name& _name, const Color& _color);
	void onChanged();
public slots:
	void onClicked();
protected:
	void setColor(const QColor& _color, Bool _emit = FALSE);

	Name	m_nParam;
	QColor	m_CurrentColor;
END_EDITOR_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesEnumCombo, QComboBox)
public:
	EditorEntitiesPropertiesEnumCombo(QWidget *_parent);
	void setParam(const Name& _name, const EntityComponentFactory::Register::SEditorEnum& _enum, S32 _value);
signals:
	void onEnumChanged(const Name& _name, S32 _value);
	void onChanged();
public slots:
	void onValueChanged(int _index);
protected:
	Name	m_nParam;
END_EDITOR_CLASS

//..

#include "ui\ui_entities_properties_component_sound.h"

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesSound, QWidget)
public:
	EditorEntitiesPropertiesSound(QWidget *_parent);
	void setParam(const Name& _name, const Str& _value);
signals:
	void onSoundChanged(const Name& _name, const Str& _value);
	void onChanged();
public slots:
	void onValueChanged();
	void testSound();
protected:
	Name	m_nParam;
	Ui_EntitiesPropertiesComponentSound	m_UISound;
END_EDITOR_CLASS

//..

#include "ui\ui_entities_properties_component_file.h"

BEGIN_EDITOR_CLASS(EditorEntitiesPropertiesFile, QWidget)
public:
	EditorEntitiesPropertiesFile(QWidget *_parent);
	void setParam(const Name& _name, const EntityComponentFactory::Register::SEditorFile& _file, const Str& _value);
signals:
	void onFileChanged(const Name& _name, const Str& _value);
	void onChanged();
public slots:
	void onValueChanged();
	void browseFile();
protected:
	Name	m_nParam;
	QString m_sFilter;
	Ui_EntitiesPropertiesComponentFile m_UIFile;
END_EDITOR_CLASS

//..

#include "ui\ui_entities_properties.h"

BEGIN_EDITOR_CLASS(EditorEntitiesProperties, QDockWidget)
public:
	EditorEntitiesProperties();
	Entity* GetEntity() { return m_pEntity; }

public slots:
void entitiesTreeSelectedEntityChanged() { entitiesTreeSelectedEntityChanged(m_pEntity); }
	void entitiesTreeSelectedEntityChanged(Entity* _entity);
	void onPosChanged();
	void onRotChanged();
	void onScaleChanged();
	void onHideChanged();
	void onColorChanged();
	void onBoolChanged(const Name& _name, Bool _value) { m_pEntity->SetParamBool(_name, _value); m_pEntity->SetParamChangedByEditor(_name); }
	void onFloatChanged(const Name& _name, Float _value) { m_pEntity->SetParamFloat(_name, _value); m_pEntity->SetParamChangedByEditor(_name);}
	void onS32Changed(const Name& _name, S32 _value) { m_pEntity->SetParamS32(_name, _value); m_pEntity->SetParamChangedByEditor(_name);}
	void onStrChanged(const Name& _name, const Str& _value) { m_pEntity->SetParamStr(_name, _value); m_pEntity->SetParamChangedByEditor(_name);}
	void onColorChanged(const Name& _name, const Color& _color) { m_pEntity->SetParamColor(_name, _color); m_pEntity->SetParamChangedByEditor(_name);}
	void onCustomContextMenu(const QPoint &_point);
	void launchContextMenu(EntityComponent* _lastComp, const QPoint& _point);
	void AddComponent();
	void RemoveComponent();
	void CopyComponent();
	void PasteComponent();
	void ReplaceComponent();

protected:
	void AddComponentsContextMenu();
	void AddComponents(Entity* _entity);

	Bool HasComponentInClipboard() const;
	Bool ClipboardComponentIsSameType();

	Ui_EntitiesProperties	m_UiEntitiesProperties;
	Entity*				m_pEntity;
	EntityComponent*		m_pLastComponentOnContextMenu;

	// context menu
	QMenu * m_pContextMenu;
	QAction* m_pMenuDelete;
	QAction* m_pMenuCopy;
	QAction* m_pMenuPasteComponents;
	QAction* m_pMenuPasteValues;
END_EDITOR_CLASS

#endif // USE_EDITOR

#endif // __EDITOR_ENTITIES_PROPERTIES_H__