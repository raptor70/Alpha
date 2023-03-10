/********************************************************************************
** Form generated from reading UI file 'ui_entities_properties_component.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENTITIES_PROPERTIES_COMPONENT_H
#define UI_ENTITIES_PROPERTIES_COMPONENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_EntitiesPropertiesComponent
{
public:
    QVBoxLayout *verticalLayout;
    QPushButton *ComponentName;
    QFrame *EntitiesPropertiesParams;
    QFormLayout *formLayout;

    void setupUi(QFrame *EntitiesPropertiesComponent)
    {
        if (EntitiesPropertiesComponent->objectName().isEmpty())
            EntitiesPropertiesComponent->setObjectName(QStringLiteral("EntitiesPropertiesComponent"));
        EntitiesPropertiesComponent->resize(346, 387);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(EntitiesPropertiesComponent->sizePolicy().hasHeightForWidth());
        EntitiesPropertiesComponent->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(EntitiesPropertiesComponent);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        ComponentName = new QPushButton(EntitiesPropertiesComponent);
        ComponentName->setObjectName(QStringLiteral("ComponentName"));

        verticalLayout->addWidget(ComponentName);

        EntitiesPropertiesParams = new QFrame(EntitiesPropertiesComponent);
        EntitiesPropertiesParams->setObjectName(QStringLiteral("EntitiesPropertiesParams"));
        formLayout = new QFormLayout(EntitiesPropertiesParams);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        formLayout->setContentsMargins(0, 2, 0, 2);

        verticalLayout->addWidget(EntitiesPropertiesParams);


        retranslateUi(EntitiesPropertiesComponent);

        QMetaObject::connectSlotsByName(EntitiesPropertiesComponent);
    } // setupUi

    void retranslateUi(QFrame *EntitiesPropertiesComponent)
    {
        EntitiesPropertiesComponent->setWindowTitle(QApplication::translate("EntitiesPropertiesComponent", "Frame", nullptr));
        ComponentName->setText(QApplication::translate("EntitiesPropertiesComponent", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EntitiesPropertiesComponent: public Ui_EntitiesPropertiesComponent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENTITIES_PROPERTIES_COMPONENT_H
