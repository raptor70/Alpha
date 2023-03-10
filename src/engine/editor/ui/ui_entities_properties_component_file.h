/********************************************************************************
** Form generated from reading UI file 'ui_entities_properties_component_file.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENTITIES_PROPERTIES_COMPONENT_FILE_H
#define UI_ENTITIES_PROPERTIES_COMPONENT_FILE_H

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

class Ui_EntitiesPropertiesComponentFile
{
public:
    QHBoxLayout *horizontalLayout;
    CEditorEntitiesPropertiesLineEdit *path;
    QPushButton *browse;

    void setupUi(QWidget *EntitiesPropertiesComponentFile)
    {
        if (EntitiesPropertiesComponentFile->objectName().isEmpty())
            EntitiesPropertiesComponentFile->setObjectName(QStringLiteral("EntitiesPropertiesComponentFile"));
        EntitiesPropertiesComponentFile->resize(456, 34);
        horizontalLayout = new QHBoxLayout(EntitiesPropertiesComponentFile);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        path = new CEditorEntitiesPropertiesLineEdit(EntitiesPropertiesComponentFile);
        path->setObjectName(QStringLiteral("path"));

        horizontalLayout->addWidget(path);

        browse = new QPushButton(EntitiesPropertiesComponentFile);
        browse->setObjectName(QStringLiteral("browse"));
        browse->setMaximumSize(QSize(20, 16777215));

        horizontalLayout->addWidget(browse);


        retranslateUi(EntitiesPropertiesComponentFile);

        QMetaObject::connectSlotsByName(EntitiesPropertiesComponentFile);
    } // setupUi

    void retranslateUi(QWidget *EntitiesPropertiesComponentFile)
    {
        EntitiesPropertiesComponentFile->setWindowTitle(QApplication::translate("EntitiesPropertiesComponentFile", "Form", nullptr));
        browse->setText(QApplication::translate("EntitiesPropertiesComponentFile", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EntitiesPropertiesComponentFile: public Ui_EntitiesPropertiesComponentFile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENTITIES_PROPERTIES_COMPONENT_FILE_H
