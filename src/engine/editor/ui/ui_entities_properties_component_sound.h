/********************************************************************************
** Form generated from reading UI file 'ui_entities_properties_component_sound.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENTITIES_PROPERTIES_COMPONENT_SOUND_H
#define UI_ENTITIES_PROPERTIES_COMPONENT_SOUND_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "editor/editor_entities_properties.h"

QT_BEGIN_NAMESPACE

class Ui_EntitiesPropertiesComponentSound
{
public:
    QHBoxLayout *horizontalLayout;
    EditorEntitiesPropertiesLineEdit *path;
    QPushButton *test;

    void setupUi(QWidget *EntitiesPropertiesComponentSound)
    {
        if (EntitiesPropertiesComponentSound->objectName().isEmpty())
            EntitiesPropertiesComponentSound->setObjectName(QStringLiteral("EntitiesPropertiesComponentSound"));
        EntitiesPropertiesComponentSound->resize(464, 42);
        horizontalLayout = new QHBoxLayout(EntitiesPropertiesComponentSound);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        path = new EditorEntitiesPropertiesLineEdit(EntitiesPropertiesComponentSound);
        path->setObjectName(QStringLiteral("path"));

        horizontalLayout->addWidget(path);

        test = new QPushButton(EntitiesPropertiesComponentSound);
        test->setObjectName(QStringLiteral("test"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(test->sizePolicy().hasHeightForWidth());
        test->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QStringLiteral(":/speaker"), QSize(), QIcon::Normal, QIcon::Off);
        test->setIcon(icon);
        test->setIconSize(QSize(16, 16));

        horizontalLayout->addWidget(test);


        retranslateUi(EntitiesPropertiesComponentSound);

        QMetaObject::connectSlotsByName(EntitiesPropertiesComponentSound);
    } // setupUi

    void retranslateUi(QWidget *EntitiesPropertiesComponentSound)
    {
        EntitiesPropertiesComponentSound->setWindowTitle(QApplication::translate("EntitiesPropertiesComponentSound", "Form", nullptr));
        test->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class EntitiesPropertiesComponentSound: public Ui_EntitiesPropertiesComponentSound {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENTITIES_PROPERTIES_COMPONENT_SOUND_H
