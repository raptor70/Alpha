#ifndef __EDITOR_ENTITIES_TREE_H__
#define __EDITOR_ENTITIES_TREE_H__
#include "editor_include.h"
#ifdef USE_EDITOR

BEGIN_EDITOR_CLASS(EditorEntitiesTreeModel, QStandardItemModel)
public:
	EditorEntitiesTreeModel();

	void	CreateTree();

public slots:
	void onItemChanged(QStandardItem * item);

protected:
	void	AddSonItems(Entity* _pEntity, QStandardItem* _pRootItem, S32 _itemIdx);
END_EDITOR_CLASS

//..

BEGIN_SUPER_CLASS(EditorEntitiesItem, QStandardItem)
public:
	EditorEntitiesItem() :SUPER() { m_pEntity = NULL; m_pScriptRoot = NULL; }
	EditorEntitiesItem(const QString& _title, Entity* _pEntity) :SUPER(_title) { SetEntity(_pEntity); }

	void		SetEntity(Entity* _entity)
	{
		m_pEntity = _entity;
		m_pScriptRoot = NULL;
		if (_entity)
		{
			FlowScript* pScript = Application::GetInstance().GetFlowScriptManager()->GetScriptForEntity(_entity);
			if(pScript)
				m_pScriptRoot = &(pScript->GetRootEntity());
		}
			
	}
	Entity*	GetEntity() { return m_pEntity; }
	Entity*	GetScriptRoot() { return m_pScriptRoot; }
	virtual void setData(const QVariant& value, int role) OVERRIDE;
	virtual int type() const OVERRIDE  { return QStandardItem::UserType;}
	virtual QStandardItem * clone() const OVERRIDE  { return NEW EditorEntitiesItem(text(), m_pEntity); }

	virtual void read(QDataStream & in) OVERRIDE
	{
		SUPER::read(in);

		U64 ptr = 0;
		in >> ptr;
		SetEntity((Entity*)ptr);
	}

	virtual void write(QDataStream & out) const OVERRIDE
	{
		SUPER::write(out);

		U64 ptr = (U64)m_pEntity;
		out << ptr;
	}
	
protected:
	Entity*	m_pEntity;
	Entity*	m_pScriptRoot;
END_SUPER_CLASS

//..

BEGIN_SUPER_CLASS(EditorEntitiesNotSaved, EditorEntitiesItem)
public:
	EditorEntitiesNotSaved(const Char* _title, Entity* _pEntity) :SUPER(_title, _pEntity) {}
END_SUPER_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesSearchFilter, QSortFilterProxyModel)
public:
	virtual bool filterAcceptsRow(int row, const QModelIndex &parent) const OVERRIDE;
END_EDITOR_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesTree,QTreeView)
public :    
	EditorEntitiesTree(QWidget* _parent);

	virtual void	dragEnterEvent(QDragEnterEvent * _event) OVERRIDE;
	virtual void dragMoveEvent(QDragMoveEvent *_event) OVERRIDE;

public slots:
	void itemSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void updateTree();
	void onSearchChanged(const QString& _value);
	void onCustomContextMenu(const QPoint &_point);
	void AddChild();
	void RemoveEntity();
	void CopyEntity();
	void PasteEntity();
	void PasteHierarchy();

signals:
	void selectedEntityChanged(Entity* _entity);

protected:
	Bool HasEntityInClipboard() const;

	EditorEntitiesTreeModel	m_SourceModel;
	EditorEntitiesSearchFilter	m_FilterModel;
	QMenu * m_pContextMenu;
	QAction* m_pMenuAdd;
	QAction* m_pMenuDelete;
	QAction* m_pMenuCopy;
	QAction* m_pMenuPasteEntity;
	QAction* m_pMenuPasteHierarchy;
	Entity* m_pContextMenuEntity;
	Entity* m_pDragDropEntity;
	EntityPtrArray m_AllSelectedEntities;
END_EDITOR_CLASS

//..

BEGIN_EDITOR_CLASS(EditorEntitiesSearch, QLineEdit)
public:
	EditorEntitiesSearch(QWidget *_parent);

	SELECTALL_ONFOCUS

protected:
	void resizeEvent(QResizeEvent *);

private slots:
	void updateCloseButton(const QString &text);

private:
     QToolButton *m_ClearButton;
END_EDITOR_CLASS

#endif // USE_EDITOR

#endif // __EDITOR_ENTITIES_TREE_H__