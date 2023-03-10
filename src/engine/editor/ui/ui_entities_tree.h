/********************************************************************************
** Form generated from reading UI file 'ui_entities_tree.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENTITIES_TREE_H
#define UI_ENTITIES_TREE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include "editor/editor_entities_tree.h"

QT_BEGIN_NAMESPACE

class Ui_EntitiesTree
{
public:
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    EditorEntitiesTree *treeView;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    EditorEntitiesSearch *search;

    void setupUi(QDockWidget *EntitiesTree)
    {
        if (EntitiesTree->objectName().isEmpty())
            EntitiesTree->setObjectName(QStringLiteral("EntitiesTree"));
        EntitiesTree->resize(640, 659);
        EntitiesTree->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        EntitiesTree->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dockWidgetContents->sizePolicy().hasHeightForWidth());
        dockWidgetContents->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setVerticalSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        treeView = new EditorEntitiesTree(dockWidgetContents);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        treeView->header()->setVisible(false);

        gridLayout->addWidget(treeView, 7, 0, 1, 1);

        widget = new QWidget(dockWidgetContents);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        search = new EditorEntitiesSearch(widget);
        search->setObjectName(QStringLiteral("search"));

        horizontalLayout->addWidget(search);


        gridLayout->addWidget(widget, 5, 0, 1, 1);

        EntitiesTree->setWidget(dockWidgetContents);

        retranslateUi(EntitiesTree);

        QMetaObject::connectSlotsByName(EntitiesTree);
    } // setupUi

    void retranslateUi(QDockWidget *EntitiesTree)
    {
        EntitiesTree->setWindowTitle(QApplication::translate("EntitiesTree", "Entities", nullptr));
        label->setText(QApplication::translate("EntitiesTree", "  Search : ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EntitiesTree: public Ui_EntitiesTree {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENTITIES_TREE_H
