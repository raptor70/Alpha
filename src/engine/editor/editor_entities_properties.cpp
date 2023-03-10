#include "editor_entities_properties.h"
#include "core/name_static.h"
#ifdef USE_EDITOR

EditorEntitiesPropertiesComponentFrame::EditorEntitiesPropertiesComponentFrame(QWidget *_parent):SUPER(_parent) 
{
	m_UiComponent.setupUi(this);
}

//..

void	EditorEntitiesPropertiesComponentFrame::setComponent(EntityComponent* _component)
{
	m_Component = _component;
	m_UiComponent.ComponentName->setText(_component->GetComponentName().GetStr().GetArray());
	connect(m_UiComponent.ComponentName, SIGNAL(clicked()), this, SLOT(toggleParamsVisibility()));

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
}

//..

void EditorEntitiesPropertiesComponentFrame::toggleParamsVisibility()
{
	m_UiComponent.EntitiesPropertiesParams->setVisible(!m_UiComponent.EntitiesPropertiesParams->isVisible());
}

//.......................................................................

void EditorEntitiesPropertiesComponentFrame::onCustomContextMenu(const QPoint & _point)
{
	emit NeedContextMenu(m_Component, mapToGlobal(_point));
}

//.......................................................................

EditorEntitiesPropertiesBoolCheckBox::EditorEntitiesPropertiesBoolCheckBox(QWidget *_parent) :SUPER(_parent)
{
	m_nParam = Name::Null;

	connect(this, SIGNAL(stateChanged(int)), this, SLOT(onValueChanged(int)));
}

//..

void EditorEntitiesPropertiesBoolCheckBox::setParam(const Name& _name, Bool _value)
{
	m_nParam = _name;
	setChecked(_value);
}

//..

void EditorEntitiesPropertiesBoolCheckBox::onValueChanged(int _state)
{
	if(_state == Qt::Unchecked)
		emit onBoolChanged(m_nParam, FALSE);
	else if( _state == Qt::Checked)
		emit onBoolChanged(m_nParam, TRUE);
	emit onChanged();
}

//.......................................................................

EditorEntitiesPropertiesStrEdit::EditorEntitiesPropertiesStrEdit(QWidget *_parent) :SUPER(_parent)
{
	m_nParam = Name::Null;

	connect(this, SIGNAL(returnPressed()), this, SLOT(onValueChanged()));
}

//..

void EditorEntitiesPropertiesStrEdit::setParam(const Name& _name, const Str& _value)
{
	m_nParam = _name;

	setText(_value.GetArray());
}

//..

void EditorEntitiesPropertiesStrEdit::onValueChanged()
{
	QByteArray ba = text().toLatin1();
	const char *cba = ba.data();
	Str value(cba);
	emit onStrChanged(m_nParam, value);
	emit onChanged();
}

//.......................................................................

EditorEntitiesPropertiesS32Edit::EditorEntitiesPropertiesS32Edit(QWidget *_parent) :SUPER(_parent)
{
	m_nParam = Name::Null;
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

//..

void EditorEntitiesPropertiesS32Edit::setParam(const Name& _name, S32 _value)
{
	m_nParam = _name;

	setMinimum(INT_MIN);
	setMaximum(INT_MAX);
	setValue(_value);
}

//..

void EditorEntitiesPropertiesS32Edit::onValueChanged(int _value)
{
	emit onS32Changed(m_nParam, (S32)_value);
	emit onChanged();
}

//.......................................................................

EditorEntitiesPropertiesFloatEdit::EditorEntitiesPropertiesFloatEdit(QWidget *_parent) :SUPER(_parent)
{
	m_nParam = Name::Null;

	connect(this, SIGNAL(returnPressed()), this, SLOT(onValueChanged()));
}

//..

void EditorEntitiesPropertiesFloatEdit::setParam(const Name& _name, Float _value)
{
	m_nParam = _name;

	QLocale locale;
	setText(locale.toString(_value));
	setValidator(NEW QDoubleValidator());
}

//..

void EditorEntitiesPropertiesFloatEdit::onValueChanged()
{
	emit onFloatChanged(m_nParam, text().toFloat());
	emit onChanged();
}

//.......................................................................

EditorEntitiesPropertiesColorButton::EditorEntitiesPropertiesColorButton(QWidget *_parent):SUPER(_parent)
{
	m_nParam = Name::Null;

	connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

//..

void EditorEntitiesPropertiesColorButton::setParam(const Name& _name, const Color& _color)
{
	m_nParam = _name;
	setColor(_color);
}

//..

void EditorEntitiesPropertiesColorButton::setColor(const Color& _color)
{
	QColor color;
	color.setRgbF(_color.r, _color.g, _color.b, _color.a);
	setColor(color);
}

//..

void EditorEntitiesPropertiesColorButton::setColor(const QColor& _color, Bool _emit /*= FALSE*/)
{
	m_CurrentColor = _color;
	QString text;
	text.sprintf("%d;%d;%d;%d", m_CurrentColor.red(), m_CurrentColor.green(), m_CurrentColor.blue(), m_CurrentColor.alpha());
	setText(text);
	text.sprintf("* {background-color: rgb(%d,%d,%d);",m_CurrentColor.red(), m_CurrentColor.green(), m_CurrentColor.blue());
	text.append(QString::asprintf("color:%s;}", m_CurrentColor.value()>128 ? "black" : "white"));
	setStyleSheet(text);

	if (_emit)
	{
		emit onColorChanged(m_nParam, getColor());
		emit onChanged();
	}
}

//..

Color EditorEntitiesPropertiesColorButton::getColor()
{
	return Color((Float)m_CurrentColor.redF(), m_CurrentColor.greenF(), m_CurrentColor.blueF(), m_CurrentColor.alphaF());
}

//..

void EditorEntitiesPropertiesColorButton::onClicked()
{
	QColor selected = QColorDialog::getColor(m_CurrentColor, this, "Choose Color",QColorDialog::ShowAlphaChannel);
	if (selected.isValid())
		setColor(selected,TRUE);
}

//.......................................................................

EditorEntitiesPropertiesEnumCombo::EditorEntitiesPropertiesEnumCombo(QWidget *_parent) :SUPER(_parent)
{
	m_nParam = Name::Null;

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueChanged(int)));
}

//..

void EditorEntitiesPropertiesEnumCombo::setParam(const Name& _name, const EntityComponentFactory::Register::SEditorEnum& _enum, S32 _value)
{
	m_nParam = _name;

	S32 currentValue = 0;
	for (U32 i = 0; i < _enum.m_ValueNames.GetCount();i++)
	{
		addItem(_enum.m_ValueNames[i].GetArray(), QVariant((int)_enum.m_Values[i]));
		if (_value == _enum.m_Values[i])
		{
			currentValue = i;
		}
	}

	setCurrentIndex(currentValue);
}

//..

void EditorEntitiesPropertiesEnumCombo::onValueChanged(int _index)
{
	S32 value = (S32)itemData(_index).toInt();
	emit onEnumChanged(m_nParam, value);
	emit onChanged();
}

//.......................................................................

EditorEntitiesPropertiesSound::EditorEntitiesPropertiesSound(QWidget *_parent) :SUPER(_parent)
{
	m_nParam = Name::Null;
	
	m_UISound.setupUi(this);
	connect(m_UISound.path, SIGNAL(returnPressed()), this, SLOT(onValueChanged()));
	connect(m_UISound.test, SIGNAL(clicked()), this, SLOT(testSound()));
}

//..

void EditorEntitiesPropertiesSound::setParam(const Name& _name, const Str& _value)
{
	m_nParam = _name;

	m_UISound.path->setText(_value.GetArray());
}

//..

void EditorEntitiesPropertiesSound::onValueChanged()
{
	QByteArray ba = m_UISound.path->text().toLatin1();
	const char *cba = ba.data();
	Str value(cba);
	emit onSoundChanged(m_nParam, value);
	emit onChanged();
}

//..

void EditorEntitiesPropertiesSound::testSound()
{
	QByteArray ba = m_UISound.path->text().toLatin1();
	const char *cba = ba.data();
	Str value(cba);
	SoundManager::GetInstance().PlaySound(value);
}

//.......................................................................

EditorEntitiesPropertiesFile::EditorEntitiesPropertiesFile(QWidget *_parent) :SUPER(_parent)
{
	m_nParam = Name::Null;

	m_UIFile.setupUi(this);
	connect(m_UIFile.path, SIGNAL(returnPressed()), this, SLOT(onValueChanged()));
	connect(m_UIFile.browse, SIGNAL(clicked()), this, SLOT(browseFile()));
}

//..

void EditorEntitiesPropertiesFile::setParam(const Name& _name, const EntityComponentFactory::Register::SEditorFile& _file, const Str& _value)
{
	m_nParam = _name;

	switch (_file.m_Type)
	{
	case EntityComponentFactory::Register::SEditorFile::TYPE_Texture: m_sFilter = "Textures (*.png *.bmp *.tif)"; break;
	case EntityComponentFactory::Register::SEditorFile::TYPE_Mesh: m_sFilter = "Meshs (*.dae *.obj)"; break;
	case EntityComponentFactory::Register::SEditorFile::TYPE_Script: m_sFilter = "Scripts (*.lua)"; break;
	default: m_sFilter = "All (*.*)"; break;
	}

	m_UIFile.path->setText(_value.GetArray());
}

//..

void EditorEntitiesPropertiesFile::onValueChanged()
{
	QByteArray ba = m_UIFile.path->text().toLatin1();
	const char *cba = ba.data();
	Str value(cba);
	emit onFileChanged(m_nParam, value);
	emit onChanged();
}

//..

void EditorEntitiesPropertiesFile::browseFile()
{
	QString fileName = QFileDialog::getOpenFileName(
		this,
		"Open File",
		"./",
		m_sFilter);

	if (!fileName.isEmpty())
	{
		QDir dir;
		m_UIFile.path->setText(dir.relativeFilePath(fileName));
		onValueChanged();
	}
}

//.......................................................................

EditorEntitiesProperties::EditorEntitiesProperties()
{
	m_pEntity = NULL;
	m_UiEntitiesProperties.setupUi(this);

	connect(m_UiEntitiesProperties.PXValue, SIGNAL(returnPressed()), this, SLOT(onPosChanged()));
	connect(m_UiEntitiesProperties.PYValue, SIGNAL(returnPressed()), this, SLOT(onPosChanged()));
	connect(m_UiEntitiesProperties.PZValue, SIGNAL(returnPressed()), this, SLOT(onPosChanged()));
	connect(m_UiEntitiesProperties.RXValue, SIGNAL(returnPressed()), this, SLOT(onRotChanged()));
	connect(m_UiEntitiesProperties.RYValue, SIGNAL(returnPressed()), this, SLOT(onRotChanged()));
	connect(m_UiEntitiesProperties.RZValue, SIGNAL(returnPressed()), this, SLOT(onRotChanged()));
	connect(m_UiEntitiesProperties.SXValue, SIGNAL(returnPressed()), this, SLOT(onScaleChanged()));
	connect(m_UiEntitiesProperties.SYValue, SIGNAL(returnPressed()), this, SLOT(onScaleChanged()));
	connect(m_UiEntitiesProperties.SZValue, SIGNAL(returnPressed()), this, SLOT(onScaleChanged()));
	connect(m_UiEntitiesProperties.HideValue, SIGNAL(onChanged()), this, SLOT(onHideChanged()));
	connect(m_UiEntitiesProperties.ColorValue, SIGNAL(onChanged()), this, SLOT(onColorChanged()));

	QDoubleValidator* pVal = NEW QDoubleValidator();
	m_UiEntitiesProperties.PXValue->setValidator(pVal);
	m_UiEntitiesProperties.PYValue->setValidator(pVal);
	m_UiEntitiesProperties.PZValue->setValidator(pVal);
	m_UiEntitiesProperties.RXValue->setValidator(pVal);
	m_UiEntitiesProperties.RYValue->setValidator(pVal);
	m_UiEntitiesProperties.RZValue->setValidator(pVal);
	m_UiEntitiesProperties.SXValue->setValidator(pVal);
	m_UiEntitiesProperties.SYValue->setValidator(pVal);
	m_UiEntitiesProperties.SZValue->setValidator(pVal);

	// context menu
	m_pContextMenu = NEW QMenu;
	AddComponentsContextMenu();
	m_pMenuDelete = NEW QAction("Delete");
	m_pContextMenu->addAction(m_pMenuDelete);
	connect(m_pMenuDelete, SIGNAL(triggered()), this, SLOT(RemoveComponent()));
	m_pContextMenu->addSeparator();
	m_pMenuCopy = NEW QAction("Copy");
	m_pContextMenu->addAction(m_pMenuCopy);
	connect(m_pMenuCopy, SIGNAL(triggered()), this, SLOT(CopyComponent()));
	m_pMenuPasteComponents = NEW QAction("Paste Component (Add)");
	m_pContextMenu->addAction(m_pMenuPasteComponents);
	connect(m_pMenuPasteComponents, SIGNAL(triggered()), this, SLOT(PasteComponent()));
	m_pMenuPasteValues = NEW QAction("Paste Values (Replace)");
	m_pContextMenu->addAction(m_pMenuPasteValues);
	connect(m_pMenuPasteValues, SIGNAL(triggered()), this, SLOT(ReplaceComponent()));
	m_pLastComponentOnContextMenu = NULL;
	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
}

//..

void EditorEntitiesProperties::AddComponentsContextMenu()
{
	QMenu* pMenu = m_pContextMenu->addMenu("Add");
	EntityComponentFactory& factory = EntityComponentFactory::GetInstance();

	struct Category
	{
		Category(const QString& _name) :m_Category(_name) {}
		QString	m_Category;
		QList<QAction*>	m_Actions;

		static bool actionLessThan(const QAction* _pA1, const QAction* _pA2)
		{
			return _pA1->text() < _pA2->text();
		}

		static bool categoryLessThan(const Category& _pC1, const Category& _pC2)
		{
			return _pC1.m_Category < _pC2.m_Category;
		}

		bool operator==(const Category& _other)  const { return m_Category == _other.m_Category; }
	};
	

	Category othersCat("Others");
	QList<Category> categoryList;
	for (U32 i = 0; i < factory.GetNbComponents(); i++)
	{
		QAction* pAct = NEW QAction(factory.GetComponentName(i).GetStr().GetArray());
		pAct->setData(QVariant((int)i));

		Name category = factory.GetComponentCategoryName(i);
		if (category == Name::Null)
		{
			othersCat.m_Actions.append(pAct);
		}
		else
		{
			Category newCat(category.GetStr().GetArray());
			int idx = categoryList.indexOf(newCat);
			if (idx >= 0)
			{
				categoryList[idx].m_Actions.append(pAct);
			}
			else
			{
				newCat.m_Actions.append(pAct);
				categoryList.append(newCat);
			}
		}
	}

	qSort(categoryList.begin(), categoryList.end(), Category::categoryLessThan);
	categoryList.append(othersCat);
	for (Category& pCat : categoryList)
	{
		qSort(pCat.m_Actions.begin(), pCat.m_Actions.end(), Category::actionLessThan);

		QMenu* pCatMenu = pMenu->addMenu(pCat.m_Category);

		for (QAction* pCur : pCat.m_Actions)
		{
			pCatMenu->addAction(pCur);
			connect(pCur, SIGNAL(triggered()), this, SLOT(AddComponent()));
		}
	}
}

//..

void EditorEntitiesProperties::entitiesTreeSelectedEntityChanged(Entity* _entity)
{
	if (m_pEntity)
		m_pEntity->SetIsSelected(FALSE);
	m_pEntity = _entity;
	if (m_pEntity)
		m_pEntity->SetIsSelected(TRUE);

	if (_entity)
	{
		QLocale locale;

		//pos
		Vec3f& pos = _entity->GetWorldPos();
		m_UiEntitiesProperties.PXValue->setEnabled(TRUE);
		m_UiEntitiesProperties.PXValue->setText(locale.toString(pos.x));
		m_UiEntitiesProperties.PYValue->setEnabled(TRUE);
		m_UiEntitiesProperties.PYValue->setText(locale.toString(pos.y));
		m_UiEntitiesProperties.PZValue->setEnabled(TRUE);
		m_UiEntitiesProperties.PZValue->setText(locale.toString(pos.z));
		//rot
		Vec3f& rot = _entity->GetWorldRot().GetEulerAngles();
		m_UiEntitiesProperties.RXValue->setEnabled(TRUE);
		m_UiEntitiesProperties.RXValue->setText(locale.toString(MATH_RAD_TO_DEG(rot.x)));
		m_UiEntitiesProperties.RYValue->setEnabled(TRUE);
		m_UiEntitiesProperties.RYValue->setText(locale.toString(MATH_RAD_TO_DEG(rot.y)));
		m_UiEntitiesProperties.RZValue->setEnabled(TRUE);
		m_UiEntitiesProperties.RZValue->setText(locale.toString(MATH_RAD_TO_DEG(rot.z)));
		//scale
		Vec3f& scale = _entity->GetWorldScale();
		m_UiEntitiesProperties.SXValue->setEnabled(TRUE);
		m_UiEntitiesProperties.SXValue->setText(locale.toString(scale.x));
		m_UiEntitiesProperties.SYValue->setEnabled(TRUE);
		m_UiEntitiesProperties.SYValue->setText(locale.toString(scale.y));
		m_UiEntitiesProperties.SZValue->setEnabled(TRUE);
		m_UiEntitiesProperties.SZValue->setText(locale.toString(scale.z));
		//hide
		m_UiEntitiesProperties.HideValue->setEnabled(TRUE);
		m_UiEntitiesProperties.HideValue->setChecked(_entity->IsHidden());
		//color
		m_UiEntitiesProperties.ColorValue->setEnabled(TRUE);
		m_UiEntitiesProperties.ColorValue->setColor(_entity->GetColor());

		setWindowTitle(QString::asprintf("Properties - %s", _entity->GetName().GetStr().GetArray()));
	}
	else
	{
		m_UiEntitiesProperties.PXValue->setEnabled(FALSE);
		m_UiEntitiesProperties.PXValue->setText("");
		m_UiEntitiesProperties.PYValue->setEnabled(FALSE);
		m_UiEntitiesProperties.PYValue->setText("");
		m_UiEntitiesProperties.PZValue->setEnabled(FALSE);
		m_UiEntitiesProperties.PZValue->setText("");
		m_UiEntitiesProperties.RXValue->setEnabled(FALSE);
		m_UiEntitiesProperties.RXValue->setText("");
		m_UiEntitiesProperties.RYValue->setEnabled(FALSE);
		m_UiEntitiesProperties.RYValue->setText("");
		m_UiEntitiesProperties.RZValue->setEnabled(FALSE);
		m_UiEntitiesProperties.RZValue->setText("");
		m_UiEntitiesProperties.SXValue->setEnabled(FALSE);
		m_UiEntitiesProperties.SXValue->setText("");
		m_UiEntitiesProperties.SYValue->setEnabled(FALSE);
		m_UiEntitiesProperties.SYValue->setText("");
		m_UiEntitiesProperties.SZValue->setEnabled(FALSE);
		m_UiEntitiesProperties.SZValue->setText("");
		m_UiEntitiesProperties.HideValue->setEnabled(FALSE);
		m_UiEntitiesProperties.HideValue->setChecked(FALSE);
		m_UiEntitiesProperties.ColorValue->setEnabled(FALSE);
		m_UiEntitiesProperties.ColorValue->setColor(Color::White);

		setWindowTitle(QString("Properties"));
	}

	AddComponents(_entity);
}

//..

void EditorEntitiesProperties::AddComponents(Entity* _entity)
{
	QLayoutItem *wItem;
	while ((wItem = m_UiEntitiesProperties.componentsLayout->takeAt(0)) != 0)
	{
		if (wItem->widget())
			wItem->widget()->setParent(NULL);
		DELETE wItem;
	}

	if (_entity)
	{
		QLocale locale;
		for (U32 c = 0; c < _entity->GetNbComponents(); c++)
		{
			EntityComponent* pComp = _entity->GetComponent(c);
			const EntityComponentFactory::Register* pRegister = EntityComponentFactory::GetInstance().GetComponentRegister(pComp->GetComponentName());
			EditorEntitiesPropertiesComponentFrame* newFrame = NEW EditorEntitiesPropertiesComponentFrame(m_UiEntitiesProperties.components);
			newFrame->setComponent(pComp);
			m_UiEntitiesProperties.componentsLayout->addWidget(newFrame);
			connect(newFrame, SIGNAL(NeedContextMenu(EntityComponent*, const QPoint&)), this, SLOT(launchContextMenu(EntityComponent*, const QPoint&)));
			
			ParamsTable& table = _entity->GetParamsTable();
			NameArray orgComps;
			NameArray paramNames;
			pRegister->GetParamNames(paramNames, orgComps);
			QWidget* paramParent = newFrame->m_UiComponent.EntitiesPropertiesParams;

			for (U32 p = 0; p < paramNames.GetCount(); p++)
			{
				const Params* param = table.GetValue(paramNames[p]);
				EditorEntitiesPropertiesLabel* newLabel = new EditorEntitiesPropertiesLabel(paramParent);
				U32 iCompNameSize = orgComps[p].GetStr().GetLength();
				const Str& orgName = param->GetName().GetStr();
				Str name = orgName.GetTextInsideRange(iCompNameSize, orgName.GetLength()-1);
				newLabel->setText(name.GetArray());
				newLabel->setFixedWidth(120);
				newLabel->setAlignment(newFrame->m_UiComponent.formLayout->labelAlignment());
				if(param->ChangedByEditor())
					newLabel->Changed();
				newFrame->m_UiComponent.formLayout->setWidget(p, QFormLayout::LabelRole, newLabel);

				QWidget* newWidget = NULL;
				const EntityComponentFactory::Register::SEditorEnum* pEnum = NULL;
				const EntityComponentFactory::Register::SEditorFile* pFile = NULL;
				if (pRegister->FindEnumParam(param->GetName(), &pEnum)) // enum
				{
					EditorEntitiesPropertiesEnumCombo* newCombo = new EditorEntitiesPropertiesEnumCombo(paramParent);
					newCombo->setParam(param->GetName(), *pEnum, param->GetS32());
					newWidget = newCombo;
					connect(newWidget, SIGNAL(onEnumChanged(const Name&, S32)), this, SLOT(onS32Changed(const Name&, S32)));
				}
				else if (pRegister->IsSoundParam(param->GetName())) // sound
				{
					EditorEntitiesPropertiesSound* newSound = new EditorEntitiesPropertiesSound(paramParent);
					newSound->setParam(param->GetName(), param->GetStr());
					newWidget = newSound;
					connect(newWidget, SIGNAL(onSoundChanged(const Name&, const Str&)), this, SLOT(onStrChanged(const Name&, const Str&)));
				}
				else if (pRegister->FindFileParam(param->GetName(), &pFile)) // file
				{
					EditorEntitiesPropertiesFile* newFile = new EditorEntitiesPropertiesFile(paramParent);
					newFile->setParam(param->GetName(), *pFile, param->GetStr());
					newWidget = newFile;
					connect(newWidget, SIGNAL(onFileChanged(const Name&, const Str&)), this, SLOT(onStrChanged(const Name&, const Str&)));
				}
				else // default
				{
					switch (param->GetType())
					{
						case Params::TYPE_Bool:
						{
							EditorEntitiesPropertiesBoolCheckBox* newCheckBox = new EditorEntitiesPropertiesBoolCheckBox(paramParent);
							newCheckBox->setParam(param->GetName(), param->GetBool());
							newWidget = newCheckBox;
							connect(newWidget, SIGNAL(onBoolChanged(const Name&, Bool)), this, SLOT(onBoolChanged(const Name&, Bool)));
							break;
						}
						case Params::TYPE_Float:
						{
							EditorEntitiesPropertiesFloatEdit* newEdit = new EditorEntitiesPropertiesFloatEdit(paramParent);
							newEdit->setParam(param->GetName(), param->GetFloat());
							newWidget = newEdit;
							connect(newWidget, SIGNAL(onFloatChanged(const Name&, Float)), this, SLOT(onFloatChanged(const Name&, Float)));
							break;
						}
						case Params::TYPE_S32:
						{
							EditorEntitiesPropertiesS32Edit* newEdit = new EditorEntitiesPropertiesS32Edit(paramParent);
							newEdit->setParam(param->GetName(), param->GetS32());
							newWidget = newEdit;
							connect(newWidget, SIGNAL(onS32Changed(const Name&, S32)), this, SLOT(onS32Changed(const Name&, S32)));
							break;
						}
						case Params::TYPE_Color:
						{
							EditorEntitiesPropertiesColorButton* newButton = new EditorEntitiesPropertiesColorButton(paramParent);
							newButton->setParam(param->GetName(), param->GetColor());
							newWidget = newButton;
							connect(newWidget, SIGNAL(onColorChanged(const Name&, const Color&)), this, SLOT(onColorChanged(const Name&, const Color&)));
							break;
						}
						case Params::TYPE_Str:
						{
							EditorEntitiesPropertiesStrEdit* newEdit = new EditorEntitiesPropertiesStrEdit(paramParent);
							newEdit->setParam(param->GetName(), param->GetStr());
							newWidget = newEdit;
							connect(newWidget, SIGNAL(onStrChanged(const Name&, const Str&)), this, SLOT(onStrChanged(const Name&, const Str&)));
							break;
						}
						default:
						{
							QLineEdit* newEdit = new QLineEdit(paramParent);
							newEdit->setText(param->ToStr().GetArray());
							newWidget = newEdit;
							break;
						}
					}
				}
				newFrame->m_UiComponent.formLayout->setWidget(p, QFormLayout::FieldRole, newWidget);
				connect(newWidget, SIGNAL(onChanged()), newLabel, SLOT(Changed()));
			}
		}
	}
}


//..

void EditorEntitiesProperties::onCustomContextMenu(const QPoint & _point)
{
	launchContextMenu(NULL, mapToGlobal(_point));
}

//..

void EditorEntitiesProperties::launchContextMenu(EntityComponent* _lastComp, const QPoint& _point)
{
	if (!m_pEntity)
		return;

	m_pLastComponentOnContextMenu = _lastComp;
	if (_lastComp)
	{
		m_pMenuDelete->setEnabled(TRUE);
		m_pMenuCopy->setEnabled(TRUE);
	}
	else
	{
		m_pMenuDelete->setDisabled(TRUE);
		m_pMenuCopy->setDisabled(TRUE);
	}

	if (HasComponentInClipboard())
		m_pMenuPasteComponents->setEnabled(TRUE);
	else
		m_pMenuPasteComponents->setDisabled(TRUE);

	if (ClipboardComponentIsSameType())
		m_pMenuPasteValues->setEnabled(TRUE);
	else
		m_pMenuPasteValues->setDisabled(TRUE);
	//
	m_pContextMenu->exec(_point);
}

//..

void EditorEntitiesProperties::onPosChanged()
{
	if (m_pEntity)
	{
		Vec3f pos = Vec3f::Zero;
		pos.x = m_UiEntitiesProperties.PXValue->text().toFloat();
		pos.y = m_UiEntitiesProperties.PYValue->text().toFloat();
		pos.z = m_UiEntitiesProperties.PZValue->text().toFloat();
		m_pEntity->SetWorldPos(pos);
	}
}

//..

void EditorEntitiesProperties::onRotChanged()
{
	if (m_pEntity)
	{
		Vec3f rot = Vec3f::Zero;
		rot.x = MATH_DEG_TO_RAD(m_UiEntitiesProperties.RXValue->text().toFloat());
		rot.y = MATH_DEG_TO_RAD(m_UiEntitiesProperties.RYValue->text().toFloat());
		rot.z = MATH_DEG_TO_RAD(m_UiEntitiesProperties.RZValue->text().toFloat());
		Quat qrot;
		qrot.SetFromEulerAngles(rot);
		m_pEntity->SetWorldRot(qrot);
	}
}

//..

void EditorEntitiesProperties::onScaleChanged()
{
	if (m_pEntity)
	{
		Vec3f scale = Vec3f::One;
		scale.x = m_UiEntitiesProperties.SXValue->text().toFloat();
		scale.y = m_UiEntitiesProperties.SYValue->text().toFloat();
		scale.z = m_UiEntitiesProperties.SZValue->text().toFloat();
		m_pEntity->SetWorldScale(scale);
	}
}

//..

void EditorEntitiesProperties::onHideChanged()
{
	if (m_pEntity)
	{
		EditorEntitiesPropertiesBoolCheckBox* checkbox = (EditorEntitiesPropertiesBoolCheckBox*)sender();
		if(checkbox->isChecked())
			m_pEntity->Hide();
		else
			m_pEntity->Show();
	}
}

//..

void EditorEntitiesProperties::onColorChanged()
{
	if (m_pEntity)
	{
		EditorEntitiesPropertiesColorButton* button = (EditorEntitiesPropertiesColorButton*)sender();
		m_pEntity->SetColor(button->getColor());
	}
}

//..

void EditorEntitiesProperties::AddComponent()
{
	QAction* pAction = (QAction*)sender();
	const Name& compName = EntityComponentFactory::GetInstance().GetComponentName((U32)pAction->data().toInt());

	NameArray requirements;
	EntityComponentFactory::GetInstance().GetComponentRegister(compName)->GetRequirements(requirements);
	for (U32 i = 0; i < requirements.GetCount(); i++)
	{
		const Name& needComp = requirements[i];
		if (!m_pEntity->GetComponent(needComp))
		{
			World::GetInstance().AddNewComponentToEntity(*m_pEntity, needComp);
		}
	}

	World::GetInstance().AddNewComponentToEntity(*m_pEntity, compName);

	QTimer::singleShot(1, this, SLOT(entitiesTreeSelectedEntityChanged()));
}

//..

void EditorEntitiesProperties::RemoveComponent()
{
	m_pEntity->RemoveComponent(m_pLastComponentOnContextMenu);
	QTimer::singleShot(1, this, SLOT(entitiesTreeSelectedEntityChanged()));
}

//..

void EditorEntitiesProperties::CopyComponent()
{
	if (m_pLastComponentOnContextMenu)
	{
		rapidxml::xml_document<> doc;
		rapidxml::xml_node<>* component = doc.allocate_node(rapidxml::node_element, "Component");
		doc.append_node(component);
		Name nComponentName = m_pLastComponentOnContextMenu->GetComponentName();
		component->append_attribute(doc.allocate_attribute("Type", doc.allocate_string(nComponentName.GetStr().GetArray())));
		const EntityComponentFactory::Register* pRegister = EntityComponentFactory::GetInstance().GetComponentRegister(nComponentName);
		NameArray orgComps;
		NameArray paramNames;
		pRegister->GetParamNames(paramNames, orgComps);
		ParamsTable& table = m_pLastComponentOnContextMenu->GetEntity()->GetParamsTable();
		for (U32 p = 0; p < paramNames.GetCount(); p++)
		{
			const Params* param = table.GetValue(paramNames[p]);
			rapidxml::xml_node<>* newParam = doc.allocate_node(rapidxml::node_element, "Param");
			component->append_node(newParam);
			newParam->append_attribute(doc.allocate_attribute("Name", doc.allocate_string(param->GetName().GetStr().GetArray())));
			newParam->append_attribute(doc.allocate_attribute("Type", doc.allocate_string(Params::TypeToChar(param->GetType()))));
			Str value = param->ToStr();
			newParam->append_attribute(doc.allocate_attribute("Value", value.IsEmpty() ? "" : doc.allocate_string(value.GetArray())));
			if(param->ChangedByEditor())
				newParam->append_node(doc.allocate_node(rapidxml::node_element, "Changed"));
		}

		std::string s;
		print(std::back_inserter(s), doc, 0);
		QClipboard *clipboard = QApplication::clipboard();
		clipboard->setText(s.c_str());
	}
}

//..

Bool EditorEntitiesProperties::HasComponentInClipboard() const
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
	if (pComponent && Name(pComponent->name()) == NAME(Component))
	{
		return TRUE;
	}

	return FALSE;
}

//..

Bool EditorEntitiesProperties::ClipboardComponentIsSameType()
{
	if (!m_pLastComponentOnContextMenu)
		return FALSE;

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
	if (pComponent && Name(pComponent->name()) == NAME(Component))
	{
		Name compType = pComponent->first_attribute("Type")->value(); // type
		if(m_pLastComponentOnContextMenu->GetComponentName() == compType)
			return TRUE;
	}

	return FALSE;
}

//..

void EditorEntitiesProperties::PasteComponent()
{
	QClipboard *clipboard = QApplication::clipboard();
	QByteArray ba = clipboard->text().toLatin1();
	char *cba = ba.data();

	rapidxml::xml_document<> doc;
	doc.parse<0>(cba);

	rapidxml::xml_node<>* pComponent = doc.first_node();
	if (Name(pComponent->name()) == NAME(Component))
	{
		// create component
		Name compType = pComponent->first_attribute("Type")->value(); // type
		World::GetInstance().AddNewComponentToEntity(*m_pEntity, compType);
		// params
		rapidxml::xml_node<>* pParam = pComponent->first_node("Param");
		while (pParam)
		{
			Name paramName = pParam->first_attribute("Name")->value();
			Params::Type paramType = Params::CharToType(pParam->first_attribute("Type")->value());
			Params* pNewParam = m_pEntity->GetParamsTable().GetOrAddValue(paramName);
			pNewParam->FromStr(paramType, pParam->first_attribute("Value")->value());
			pNewParam->SetChangedByEditor(pParam->first_node("Changed")!=0);

			pParam = pParam->next_sibling("Param");
		}
	}

	entitiesTreeSelectedEntityChanged(m_pEntity);
}

void EditorEntitiesProperties::ReplaceComponent()
{
	QClipboard *clipboard = QApplication::clipboard();
	QByteArray ba = clipboard->text().toLatin1();
	char *cba = ba.data();

	rapidxml::xml_document<> doc;
	doc.parse<0>(cba);

	rapidxml::xml_node<>* pComponent = doc.first_node();
	if (Name(pComponent->name()) == NAME(Component))
	{
		// params
		rapidxml::xml_node<>* pParam = pComponent->first_node("Param");
		while (pParam)
		{
			Name paramName = pParam->first_attribute("Name")->value();
			Params::Type paramType = Params::CharToType(pParam->first_attribute("Type")->value());
			Params* pNewParam = m_pEntity->GetParamsTable().GetOrAddValue(paramName);
			pNewParam->FromStr(paramType, pParam->first_attribute("Value")->value());
			pNewParam->SetChangedByEditor(pParam->first_node("Changed") != 0);

			pParam = pParam->next_sibling("Param");
		}
	}

	entitiesTreeSelectedEntityChanged(m_pEntity);
}


#endif // USE_EDITOR