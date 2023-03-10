#include "editor_entities_tree.h"
#include "core/name_static.h"
#ifdef USE_EDITOR

#include "world\world.h"

S32 g_entityIndex = 0;

EditorEntitiesTreeModel::EditorEntitiesTreeModel():SUPER()
{
	setItemPrototype(NEW EditorEntitiesItem("none", NULL));
	CreateTree();

	connect(this, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(onItemChanged(QStandardItem *)));
}

//..

void	EditorEntitiesTreeModel::CreateTree()
{
	g_entityIndex = 0;
	Entity* pCur = &(World::GetInstance().GetRoot());
	AddSonItems(pCur, invisibleRootItem(),0);
	World::GetInstance().SetEditorTreeDone(TRUE);
}

//..

void	EditorEntitiesTreeModel::AddSonItems(Entity* _pEntity, QStandardItem* _pRootItem, S32 _itemIdx)
{
	Str name;
	if(_pEntity->GetName() != Name::Null)
		name = _pEntity->GetName().GetStr();
	else
	{
		name.SetFrom("noname_%d", g_entityIndex); // todo _pEntity->GetName()
		g_entityIndex++;
	}

	EditorEntitiesItem* currentNode = (EditorEntitiesItem*)_pRootItem->child(_itemIdx);
	if (!currentNode || currentNode->GetEntity() != _pEntity)
	{
		currentNode = NEW EditorEntitiesItem(name.GetArray(), _pEntity);
		Bool bIsRootScript = Application::GetInstance().GetFlowScriptManager()->IsRootScriptEntity(_pEntity);
		Bool bIsSavable = _pEntity->ToSave();
		if (!bIsSavable)
		{
			currentNode->setEditable(false);
			currentNode->setForeground(Qt::red);
			currentNode->setDragEnabled(false);
			currentNode->setDropEnabled(false);
		}

		_pRootItem->insertRow(_itemIdx,currentNode);
	}
	
	for(U32 i = 0; i < _pEntity->GetNbSons(); i++)
	{
		AddSonItems(_pEntity->GetSon(i), currentNode, i);
	}

	currentNode->removeRows(_pEntity->GetNbSons(), currentNode->rowCount() - _pEntity->GetNbSons());
}

//..

void EditorEntitiesTreeModel::onItemChanged(QStandardItem * item)
{
	// check parent
	QStandardItem* pParent = item->parent();
	if (pParent && pParent->type() == QStandardItem::UserType)
	{
		EditorEntitiesItem* pCurrentEntityItem = (EditorEntitiesItem*)item;
		EditorEntitiesItem* pParentEntityItem = (EditorEntitiesItem*)pParent;
		if (pCurrentEntityItem->GetEntity()->GetParent() != pParentEntityItem->GetEntity())
		{
			Mat44 entityMat = pCurrentEntityItem->GetEntity()->GetWorldMatrix();
			World::GetInstance().SetParent(pCurrentEntityItem->GetEntity(), pParentEntityItem->GetEntity());
			pCurrentEntityItem->GetEntity()->SetWorldMatrix(entityMat);
		}
	}
}

//..

void EditorEntitiesItem::setData(const QVariant& _value, int _role)
{
	if (_role == Qt::EditRole && _value.isValid()) 
	{
		QByteArray ba = _value.toString().toLatin1();
		m_pEntity->SetName(ba.data());
	}
	
	QStandardItem::setData(_value, _role);


}

//..

bool EditorEntitiesSearchFilter::filterAcceptsRow(int row, const QModelIndex &parent) const
{
	QModelIndex index = sourceModel()->index(row, 0, parent);

	if (!index.isValid())
		return false;

	if (index.data().toString().contains(filterRegExp()))
		return true;

	int rows = sourceModel()->rowCount(index);
	for (row = 0; row < rows; row++)
		if (filterAcceptsRow(row, index))
			return true;

	return false;
}

//..

EditorEntitiesTree::EditorEntitiesTree(QWidget* _parent) :SUPER(_parent)
{
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::InternalMove);

	setModel(&m_FilterModel);
	m_FilterModel.setDynamicSortFilter(true);
	m_FilterModel.setSourceModel(&m_SourceModel);
	connect(selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(itemSelectionChanged(const QItemSelection &, const QItemSelection &)));

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	m_pContextMenu = NEW QMenu;
	m_pMenuAdd = NEW QAction("Add Child");
	m_pContextMenu->addAction(m_pMenuAdd);
	connect(m_pMenuAdd, SIGNAL(triggered()), this, SLOT(AddChild()));
	m_pMenuDelete = NEW QAction("Delete");
	m_pContextMenu->addAction(m_pMenuDelete);
	connect(m_pMenuDelete, SIGNAL(triggered()), this, SLOT(RemoveEntity()));
	m_pContextMenu->addSeparator();
	m_pMenuCopy = NEW QAction("Copy");
	m_pContextMenu->addAction(m_pMenuCopy);
	connect(m_pMenuCopy, SIGNAL(triggered()), this, SLOT(CopyEntity()));
	m_pMenuPasteEntity = NEW QAction("Paste Entity");
	m_pContextMenu->addAction(m_pMenuPasteEntity);
	connect(m_pMenuPasteEntity, SIGNAL(triggered()), this, SLOT(PasteEntity()));
	m_pMenuPasteHierarchy = NEW QAction("Paste Hierarchy");
	m_pContextMenu->addAction(m_pMenuPasteHierarchy);
	connect(m_pMenuPasteHierarchy, SIGNAL(triggered()), this, SLOT(PasteHierarchy()));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

	m_pDragDropEntity = NULL;
}

//..

void	EditorEntitiesTree::dragEnterEvent(QDragEnterEvent * _event)
{
	Bool bAllSameScript = TRUE;
	Entity* pRoot = NULL;
	QModelIndexList items = selectedIndexes();
	for(QModelIndex item : items)
	{
		const QModelIndex& idx = m_FilterModel.mapToSource(item);
		QStandardItem* pBaseitem = m_SourceModel.itemFromIndex(idx);
		Bool bOk = FALSE;
		if (pBaseitem->type() == QStandardItem::UserType)
		{
			EditorEntitiesItem* pItem = (EditorEntitiesItem*)pBaseitem;
			if (pItem->GetEntity()->ToSave())
			{
				if (pItem->GetScriptRoot())
				{
					if (!pRoot)
					{
						pRoot = pItem->GetScriptRoot();
						bOk = TRUE;
					}
					else if (pRoot == pItem->GetScriptRoot())
					{
						bOk = TRUE;
					}
				}
			}
		}

		if (!bOk)
		{
			bAllSameScript = FALSE;
			break;
		}
	}
	
	if (bAllSameScript)
		SUPER::dragEnterEvent(_event);
}

//..

void EditorEntitiesTree::dragMoveEvent(QDragMoveEvent *_event)
{
	SUPER::dragMoveEvent(_event);

	Bool bValid = FALSE;
	QModelIndexList items = selectedIndexes();
	if (items.size() > 0)
	{
		const QModelIndex& idx = m_FilterModel.mapToSource(items[0]);
		QStandardItem* pBaseitem = m_SourceModel.itemFromIndex(idx);
		if (pBaseitem->type() == QStandardItem::UserType)
		{
			EditorEntitiesItem* pFirstItem = (EditorEntitiesItem*)pBaseitem;
			const QModelIndex& otheridx = m_FilterModel.mapToSource(indexAt(_event->pos()));
			if (otheridx.isValid())
			{
				QStandardItem* pOtherBaseitem = m_SourceModel.itemFromIndex(otheridx);
				if (pOtherBaseitem->type() == QStandardItem::UserType)
				{
					EditorEntitiesItem* pOtherItem = (EditorEntitiesItem*)pOtherBaseitem;
					if (pOtherItem->GetEntity() 
						&& pOtherItem->GetEntity()->ToSave()
						&& pFirstItem->GetScriptRoot() == pOtherItem->GetScriptRoot())
					{
						bValid = TRUE;
					}
				}
			}
		}
	}

	if (bValid)
		_event->accept();
	else
		_event->ignore();
}

//..

void EditorEntitiesTree::onCustomContextMenu(const QPoint &_point)
{
	const QModelIndex& idx = m_FilterModel.mapToSource(indexAt(_point));
	EditorEntitiesItem* pItem = (EditorEntitiesItem*)(m_SourceModel.itemFromIndex(idx));
	if (pItem)
	{
		const Entity* pEntity = pItem->GetEntity();
		Bool bIsRootScript = Application::GetInstance().GetFlowScriptManager()->IsRootScriptEntity(pEntity);
		Bool bIsSavable = pEntity->ToSave();
		if (bIsRootScript|| bIsSavable)
		{
			if (bIsRootScript)
			{
				m_pMenuDelete->setDisabled(TRUE);
				m_pMenuCopy->setDisabled(TRUE);
			}
			else
			{
				m_pMenuDelete->setEnabled(TRUE);
				m_pMenuCopy->setEnabled(TRUE);
			}
			
			if (HasEntityInClipboard())
			{
				m_pMenuPasteEntity->setEnabled(TRUE);
				m_pMenuPasteHierarchy->setEnabled(TRUE);
			}
			else
			{
				m_pMenuPasteEntity->setDisabled(TRUE);
				m_pMenuPasteHierarchy->setDisabled(TRUE);
			}
			m_pContextMenuEntity = pItem->GetEntity();
			m_pContextMenu->exec(mapToGlobal(_point));
		}
	}
}

//..

void EditorEntitiesTree::updateTree()
{
	m_SourceModel.CreateTree();
}

//..

void EditorEntitiesTree::itemSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	Entity* currentSelected = NULL;
	
	for (auto& current = deselected.begin(); current != deselected.end(); current++)
	{
		QModelIndexList range = current->indexes();
		for (auto& index = range.begin(); index != range.end(); index++)
		{
			const QModelIndex& idx = m_FilterModel.mapToSource(*index);
			QStandardItem* pBaseitem = m_SourceModel.itemFromIndex(idx);
			if (pBaseitem->type() == QStandardItem::UserType)
			{
				EditorEntitiesItem* pItem = (EditorEntitiesItem*)pBaseitem;
				if (pItem->GetEntity())
				{
					pItem->GetEntity()->SetIsSelected(FALSE);
					m_AllSelectedEntities.RemoveItems(m_AllSelectedEntities.GetIndexOfItem(pItem->GetEntity()), 1);
				}
			}
		}
	}

	for (auto& current = selected.begin(); current != selected.end(); current++)
	{
		QModelIndexList range = current->indexes();
		for (auto& index = range.begin(); index != range.end(); index++)
		{
			const QModelIndex& idx = m_FilterModel.mapToSource(*index);
			QStandardItem* pBaseitem = m_SourceModel.itemFromIndex(idx);
			if (pBaseitem->type() == QStandardItem::UserType)
			{
				EditorEntitiesItem* pItem = (EditorEntitiesItem*)pBaseitem;
				if (pItem->GetEntity())
				{
					pItem->GetEntity()->SetIsSelected(TRUE);

					m_AllSelectedEntities.AddLastItem(pItem->GetEntity());
					if (!currentSelected)
						currentSelected = pItem->GetEntity();
				}
			}
		}
	}

	emit selectedEntityChanged(currentSelected);
}

//..

void EditorEntitiesTree::onSearchChanged(const QString& _value)
{
	QRegExp::PatternSyntax syntax = QRegExp::RegExp;
	Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;
	QRegExp regExp(_value, caseSensitivity);
	m_FilterModel.setFilterRegExp(regExp);
	expandAll();
}

//..

void EditorEntitiesTree::AddChild()
{
	FlowScript* pScript = Application::GetInstance().GetFlowScriptManager()->GetScriptForEntity(m_pContextMenuEntity);
	Entity& entity = pScript->CreateNewEntityFromParent(Name::Null, *m_pContextMenuEntity);
	entity.SetToSave(TRUE);
}

//..

void EditorEntitiesTree::RemoveEntity()
{
	for(Entity* entity : m_AllSelectedEntities)
	{ 
		World::GetInstance().RemoveEntity(entity);
	}
}

//..

struct EntityNode
{
	Entity* m_pWorldEntity;
	rapidxml::xml_node<>* m_Node;
};

//..

void EditorEntitiesTree::CopyEntity()
{
	// entities
	rapidxml::xml_document<> doc;
	World::GetInstance().SaveEntitiesToXml(&doc, &doc, m_pContextMenuEntity, TRUE);
	std::string s;
	print(std::back_inserter(s), doc, 0);
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(s.c_str());
}

//..

void EditorEntitiesTree::PasteEntity()
{

	QClipboard *clipboard = QApplication::clipboard();
	QByteArray ba = clipboard->text().toLatin1();
	char *cba = ba.data();

	rapidxml::xml_document<> doc;
	doc.parse<0>(cba);

	World::GetInstance().LoadEntitiesFromXml(&doc, m_pContextMenuEntity, TRUE, FALSE);
}

//..

void EditorEntitiesTree::PasteHierarchy()
{
	QClipboard *clipboard = QApplication::clipboard();
	QByteArray ba = clipboard->text().toLatin1();
	char *cba = ba.data();

	rapidxml::xml_document<> doc;
	doc.parse<0>(cba);

	World::GetInstance().LoadEntitiesFromXml(&doc, m_pContextMenuEntity, TRUE, TRUE);
}

//..

Bool EditorEntitiesTree::HasEntityInClipboard() const
{
	QClipboard *clipboard = QApplication::clipboard();
	QByteArray ba = clipboard->text().toLatin1();
	char *cba = ba.data();

	rapidxml::xml_document<> doc;

	try
	{
		doc.parse<0>(cba);
	}
	catch (...)
	{
		return FALSE;
	}

	rapidxml::xml_node<>* pComponent = doc.first_node();
	if (pComponent && Name(pComponent->name()) == NAME(Entity))
	{
		return TRUE;
	}

	return FALSE;
}

//..

EditorEntitiesSearch::EditorEntitiesSearch(QWidget *_parent):SUPER(_parent)
{
	m_ClearButton = new QToolButton(this);
	QPixmap pixmap(":/cross");
	m_ClearButton->setIcon(QIcon(pixmap));
	m_ClearButton->setIconSize(QSize(12,12));
	m_ClearButton->setCursor(Qt::ArrowCursor);
	m_ClearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	m_ClearButton->hide();
	connect(m_ClearButton, SIGNAL(clicked()), this, SLOT(clear()));
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateCloseButton(const QString&)));
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(m_ClearButton->sizeHint().width() + frameWidth + 1));
	QSize msz = minimumSizeHint();
	setMinimumSize(qMax(msz.width(), m_ClearButton->sizeHint().height() + frameWidth * 2 + 2),
	qMax(msz.height(), m_ClearButton->sizeHint().height() + frameWidth * 2 + 2));
}

void EditorEntitiesSearch::resizeEvent(QResizeEvent *)
{
    QSize sz = m_ClearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	m_ClearButton->move(rect().right() - frameWidth - sz.width(),
    (rect().bottom() + 1 - sz.height()) / 2);
}

void EditorEntitiesSearch::updateCloseButton(const QString& text)
{
	m_ClearButton->setVisible(!text.isEmpty());
}


#endif // USE_EDITOR