/********************************************************************************
** Form generated from reading UI file 'ui_entities_properties.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENTITIES_PROPERTIES_H
#define UI_ENTITIES_PROPERTIES_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "editor/editor_entities_properties.h"

QT_BEGIN_NAMESPACE

class Ui_EntitiesProperties
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QFrame *position;
    QHBoxLayout *horizontalLayout;
    QLabel *PX;
    EditorEntitiesPropertiesLineEdit *PXValue;
    QLabel *PY;
    EditorEntitiesPropertiesLineEdit *PYValue;
    QLabel *PZ;
    EditorEntitiesPropertiesLineEdit *PZValue;
    QFrame *rotation;
    QHBoxLayout *horizontalLayout_3;
    QLabel *RX;
    EditorEntitiesPropertiesLineEdit *RXValue;
    QLabel *RY;
    EditorEntitiesPropertiesLineEdit *RYValue;
    QLabel *RZ;
    EditorEntitiesPropertiesLineEdit *RZValue;
    QFrame *scale;
    QHBoxLayout *horizontalLayout_2;
    QLabel *SX;
    EditorEntitiesPropertiesLineEdit *SXValue;
    QLabel *SY;
    EditorEntitiesPropertiesLineEdit *SYValue;
    QLabel *SZ;
    EditorEntitiesPropertiesLineEdit *SZValue;
    QFrame *property;
    QHBoxLayout *horizontalLayout_4;
    QLabel *Hide;
    EditorEntitiesPropertiesBoolCheckBox *HideValue;
    QSpacerItem *horizontalSpacer;
    QLabel *Color;
    EditorEntitiesPropertiesColorButton *ColorValue;
    QFrame *components;
    QVBoxLayout *componentsLayout;
    QSpacerItem *verticalSpacer;

    void setupUi(QDockWidget *EntitiesProperties)
    {
        if (EntitiesProperties->objectName().isEmpty())
            EntitiesProperties->setObjectName(QStringLiteral("EntitiesProperties"));
        EntitiesProperties->resize(350, 646);
        EntitiesProperties->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        EntitiesProperties->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dockWidgetContents->sizePolicy().hasHeightForWidth());
        dockWidgetContents->setSizePolicy(sizePolicy);
        verticalLayout_2 = new QVBoxLayout(dockWidgetContents);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(dockWidgetContents);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy1);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 348, 622));
        verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(scrollAreaWidgetContents);
        frame->setObjectName(QStringLiteral("frame"));
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        position = new QFrame(frame);
        position->setObjectName(QStringLiteral("position"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(position->sizePolicy().hasHeightForWidth());
        position->setSizePolicy(sizePolicy2);
        position->setFrameShape(QFrame::StyledPanel);
        position->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(position);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 0, 5, 0);
        PX = new QLabel(position);
        PX->setObjectName(QStringLiteral("PX"));

        horizontalLayout->addWidget(PX);

        PXValue = new EditorEntitiesPropertiesLineEdit(position);
        PXValue->setObjectName(QStringLiteral("PXValue"));
        PXValue->setEnabled(false);

        horizontalLayout->addWidget(PXValue);

        PY = new QLabel(position);
        PY->setObjectName(QStringLiteral("PY"));

        horizontalLayout->addWidget(PY);

        PYValue = new EditorEntitiesPropertiesLineEdit(position);
        PYValue->setObjectName(QStringLiteral("PYValue"));
        PYValue->setEnabled(false);

        horizontalLayout->addWidget(PYValue);

        PZ = new QLabel(position);
        PZ->setObjectName(QStringLiteral("PZ"));

        horizontalLayout->addWidget(PZ);

        PZValue = new EditorEntitiesPropertiesLineEdit(position);
        PZValue->setObjectName(QStringLiteral("PZValue"));
        PZValue->setEnabled(false);

        horizontalLayout->addWidget(PZValue);


        verticalLayout->addWidget(position);

        rotation = new QFrame(frame);
        rotation->setObjectName(QStringLiteral("rotation"));
        sizePolicy2.setHeightForWidth(rotation->sizePolicy().hasHeightForWidth());
        rotation->setSizePolicy(sizePolicy2);
        rotation->setFrameShape(QFrame::StyledPanel);
        rotation->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(rotation);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(5, 0, 5, 0);
        RX = new QLabel(rotation);
        RX->setObjectName(QStringLiteral("RX"));

        horizontalLayout_3->addWidget(RX);

        RXValue = new EditorEntitiesPropertiesLineEdit(rotation);
        RXValue->setObjectName(QStringLiteral("RXValue"));
        RXValue->setEnabled(false);

        horizontalLayout_3->addWidget(RXValue);

        RY = new QLabel(rotation);
        RY->setObjectName(QStringLiteral("RY"));

        horizontalLayout_3->addWidget(RY);

        RYValue = new EditorEntitiesPropertiesLineEdit(rotation);
        RYValue->setObjectName(QStringLiteral("RYValue"));
        RYValue->setEnabled(false);

        horizontalLayout_3->addWidget(RYValue);

        RZ = new QLabel(rotation);
        RZ->setObjectName(QStringLiteral("RZ"));

        horizontalLayout_3->addWidget(RZ);

        RZValue = new EditorEntitiesPropertiesLineEdit(rotation);
        RZValue->setObjectName(QStringLiteral("RZValue"));
        RZValue->setEnabled(false);

        horizontalLayout_3->addWidget(RZValue);


        verticalLayout->addWidget(rotation);

        scale = new QFrame(frame);
        scale->setObjectName(QStringLiteral("scale"));
        sizePolicy2.setHeightForWidth(scale->sizePolicy().hasHeightForWidth());
        scale->setSizePolicy(sizePolicy2);
        scale->setFrameShape(QFrame::StyledPanel);
        scale->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(scale);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 0, 5, 0);
        SX = new QLabel(scale);
        SX->setObjectName(QStringLiteral("SX"));

        horizontalLayout_2->addWidget(SX);

        SXValue = new EditorEntitiesPropertiesLineEdit(scale);
        SXValue->setObjectName(QStringLiteral("SXValue"));
        SXValue->setEnabled(false);

        horizontalLayout_2->addWidget(SXValue);

        SY = new QLabel(scale);
        SY->setObjectName(QStringLiteral("SY"));

        horizontalLayout_2->addWidget(SY);

        SYValue = new EditorEntitiesPropertiesLineEdit(scale);
        SYValue->setObjectName(QStringLiteral("SYValue"));
        SYValue->setEnabled(false);

        horizontalLayout_2->addWidget(SYValue);

        SZ = new QLabel(scale);
        SZ->setObjectName(QStringLiteral("SZ"));

        horizontalLayout_2->addWidget(SZ);

        SZValue = new EditorEntitiesPropertiesLineEdit(scale);
        SZValue->setObjectName(QStringLiteral("SZValue"));
        SZValue->setEnabled(false);

        horizontalLayout_2->addWidget(SZValue);


        verticalLayout->addWidget(scale);

        property = new QFrame(frame);
        property->setObjectName(QStringLiteral("property"));
        property->setCursor(QCursor(Qt::ArrowCursor));
        property->setFrameShape(QFrame::StyledPanel);
        property->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(property);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(5, 0, 5, 0);
        Hide = new QLabel(property);
        Hide->setObjectName(QStringLiteral("Hide"));

        horizontalLayout_4->addWidget(Hide);

        HideValue = new EditorEntitiesPropertiesBoolCheckBox(property);
        HideValue->setObjectName(QStringLiteral("HideValue"));
        HideValue->setEnabled(false);

        horizontalLayout_4->addWidget(HideValue);

        horizontalSpacer = new QSpacerItem(200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        Color = new QLabel(property);
        Color->setObjectName(QStringLiteral("Color"));

        horizontalLayout_4->addWidget(Color);

        ColorValue = new EditorEntitiesPropertiesColorButton(property);
        ColorValue->setObjectName(QStringLiteral("ColorValue"));
        ColorValue->setEnabled(false);

        horizontalLayout_4->addWidget(ColorValue);


        verticalLayout->addWidget(property);

        components = new QFrame(frame);
        components->setObjectName(QStringLiteral("components"));
        componentsLayout = new QVBoxLayout(components);
        componentsLayout->setSpacing(0);
        componentsLayout->setObjectName(QStringLiteral("componentsLayout"));
        componentsLayout->setContentsMargins(0, 0, 0, 0);

        verticalLayout->addWidget(components);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        verticalLayout_3->addWidget(frame);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_2->addWidget(scrollArea);

        EntitiesProperties->setWidget(dockWidgetContents);

        retranslateUi(EntitiesProperties);

        QMetaObject::connectSlotsByName(EntitiesProperties);
    } // setupUi

    void retranslateUi(QDockWidget *EntitiesProperties)
    {
        EntitiesProperties->setWindowTitle(QApplication::translate("EntitiesProperties", "Properties", nullptr));
        PX->setText(QApplication::translate("EntitiesProperties", "PX", nullptr));
        PY->setText(QApplication::translate("EntitiesProperties", "PY", nullptr));
        PZ->setText(QApplication::translate("EntitiesProperties", "PZ", nullptr));
        RX->setText(QApplication::translate("EntitiesProperties", "RX", nullptr));
        RY->setText(QApplication::translate("EntitiesProperties", "RY", nullptr));
        RZ->setText(QApplication::translate("EntitiesProperties", "RZ", nullptr));
        SX->setText(QApplication::translate("EntitiesProperties", "SX", nullptr));
        SY->setText(QApplication::translate("EntitiesProperties", "SY", nullptr));
        SZ->setText(QApplication::translate("EntitiesProperties", "SZ", nullptr));
        Hide->setText(QApplication::translate("EntitiesProperties", "Hide", nullptr));
        HideValue->setText(QString());
        Color->setText(QApplication::translate("EntitiesProperties", "Color", nullptr));
        ColorValue->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class EntitiesProperties: public Ui_EntitiesProperties {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENTITIES_PROPERTIES_H
