/********************************************************************************
** Form generated from reading UI file 'ui_top_bar.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TOP_BAR_H
#define UI_TOP_BAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TopBar
{
public:
    QWidget *dockWidgetContents;
    QHBoxLayout *horizontalLayout_2;
    QFrame *left_frame;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *newscript;
    QPushButton *open;
    QPushButton *save;
    QSpacerItem *horizontalSpacer_4;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QPushButton *refresh;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pause;
    QPushButton *play;
    QSpacerItem *horizontalSpacer;
    QPushButton *rewind;
    QLabel *factor;
    QPushButton *forward;
    QSpacerItem *horizontalSpacer_5;
    QFrame *right_frame;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *translate;
    QPushButton *rotate;
    QPushButton *scale;

    void setupUi(QDockWidget *TopBar)
    {
        if (TopBar->objectName().isEmpty())
            TopBar->setObjectName(QStringLiteral("TopBar"));
        TopBar->resize(1094, 42);
        TopBar->setMinimumSize(QSize(594, 42));
        TopBar->setMaximumSize(QSize(524287, 42));
        TopBar->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
        TopBar->setAllowedAreas(Qt::TopDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        dockWidgetContents->setMinimumSize(QSize(0, 42));
        dockWidgetContents->setMaximumSize(QSize(16777215, 42));
        horizontalLayout_2 = new QHBoxLayout(dockWidgetContents);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        left_frame = new QFrame(dockWidgetContents);
        left_frame->setObjectName(QStringLiteral("left_frame"));
        left_frame->setFrameShape(QFrame::StyledPanel);
        left_frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(left_frame);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        newscript = new QPushButton(left_frame);
        newscript->setObjectName(QStringLiteral("newscript"));
        newscript->setMaximumSize(QSize(40, 40));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/new.png"), QSize(), QIcon::Normal, QIcon::Off);
        newscript->setIcon(icon);
        newscript->setIconSize(QSize(32, 32));
        newscript->setFlat(true);

        horizontalLayout_3->addWidget(newscript);

        open = new QPushButton(left_frame);
        open->setObjectName(QStringLiteral("open"));
        open->setMaximumSize(QSize(40, 40));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        open->setIcon(icon1);
        open->setIconSize(QSize(32, 32));
        open->setFlat(true);

        horizontalLayout_3->addWidget(open);

        save = new QPushButton(left_frame);
        save->setObjectName(QStringLiteral("save"));
        save->setMaximumSize(QSize(40, 40));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        save->setIcon(icon2);
        save->setIconSize(QSize(32, 32));
        save->setFlat(true);

        horizontalLayout_3->addWidget(save);


        horizontalLayout_2->addWidget(left_frame);

        horizontalSpacer_4 = new QSpacerItem(354, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        frame = new QFrame(dockWidgetContents);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        refresh = new QPushButton(frame);
        refresh->setObjectName(QStringLiteral("refresh"));
        refresh->setMaximumSize(QSize(40, 40));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/refresh"), QSize(), QIcon::Normal, QIcon::Off);
        refresh->setIcon(icon3);
        refresh->setIconSize(QSize(32, 32));
        refresh->setFlat(true);

        horizontalLayout->addWidget(refresh);

        horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        pause = new QPushButton(frame);
        pause->setObjectName(QStringLiteral("pause"));
        pause->setMaximumSize(QSize(40, 40));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/pause"), QSize(), QIcon::Normal, QIcon::Off);
        pause->setIcon(icon4);
        pause->setIconSize(QSize(32, 32));
        pause->setCheckable(true);
        pause->setFlat(true);

        horizontalLayout->addWidget(pause);

        play = new QPushButton(frame);
        play->setObjectName(QStringLiteral("play"));
        play->setMaximumSize(QSize(40, 40));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/play"), QSize(), QIcon::Normal, QIcon::Off);
        play->setIcon(icon5);
        play->setIconSize(QSize(32, 32));
        play->setCheckable(true);
        play->setChecked(true);
        play->setFlat(true);

        horizontalLayout->addWidget(play);

        horizontalSpacer = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        rewind = new QPushButton(frame);
        rewind->setObjectName(QStringLiteral("rewind"));
        rewind->setMaximumSize(QSize(40, 40));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/rewind"), QSize(), QIcon::Normal, QIcon::Off);
        rewind->setIcon(icon6);
        rewind->setIconSize(QSize(32, 32));
        rewind->setFlat(true);

        horizontalLayout->addWidget(rewind);

        factor = new QLabel(frame);
        factor->setObjectName(QStringLiteral("factor"));
        sizePolicy.setHeightForWidth(factor->sizePolicy().hasHeightForWidth());
        factor->setSizePolicy(sizePolicy);
        factor->setMinimumSize(QSize(40, 0));
        factor->setMaximumSize(QSize(40, 16777215));
        factor->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(factor);

        forward = new QPushButton(frame);
        forward->setObjectName(QStringLiteral("forward"));
        forward->setMaximumSize(QSize(40, 40));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/forward"), QSize(), QIcon::Normal, QIcon::Off);
        forward->setIcon(icon7);
        forward->setIconSize(QSize(32, 32));
        forward->setFlat(true);

        horizontalLayout->addWidget(forward);


        horizontalLayout_2->addWidget(frame);

        horizontalSpacer_5 = new QSpacerItem(355, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_5);

        right_frame = new QFrame(dockWidgetContents);
        right_frame->setObjectName(QStringLiteral("right_frame"));
        right_frame->setFrameShape(QFrame::StyledPanel);
        right_frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(right_frame);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        translate = new QPushButton(right_frame);
        translate->setObjectName(QStringLiteral("translate"));
        translate->setMaximumSize(QSize(40, 40));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/images/translation.png"), QSize(), QIcon::Normal, QIcon::Off);
        translate->setIcon(icon8);
        translate->setIconSize(QSize(32, 32));
        translate->setCheckable(true);
        translate->setChecked(true);
        translate->setFlat(true);

        horizontalLayout_4->addWidget(translate);

        rotate = new QPushButton(right_frame);
        rotate->setObjectName(QStringLiteral("rotate"));
        rotate->setMaximumSize(QSize(40, 40));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/images/rotate.png"), QSize(), QIcon::Normal, QIcon::Off);
        rotate->setIcon(icon9);
        rotate->setIconSize(QSize(32, 32));
        rotate->setCheckable(true);
        rotate->setChecked(false);
        rotate->setFlat(true);

        horizontalLayout_4->addWidget(rotate);

        scale = new QPushButton(right_frame);
        scale->setObjectName(QStringLiteral("scale"));
        scale->setMaximumSize(QSize(40, 40));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/images/scale.png"), QSize(), QIcon::Normal, QIcon::Off);
        scale->setIcon(icon10);
        scale->setIconSize(QSize(32, 32));
        scale->setCheckable(true);
        scale->setFlat(true);

        horizontalLayout_4->addWidget(scale);


        horizontalLayout_2->addWidget(right_frame);

        TopBar->setWidget(dockWidgetContents);

        retranslateUi(TopBar);

        QMetaObject::connectSlotsByName(TopBar);
    } // setupUi

    void retranslateUi(QDockWidget *TopBar)
    {
        TopBar->setWindowTitle(QApplication::translate("TopBar", "Tool", nullptr));
        newscript->setText(QString());
        open->setText(QString());
        save->setText(QString());
        refresh->setText(QString());
        pause->setText(QString());
        play->setText(QString());
        rewind->setText(QString());
        factor->setText(QApplication::translate("TopBar", "x1", nullptr));
        forward->setText(QString());
        translate->setText(QString());
        rotate->setText(QString());
        scale->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TopBar: public Ui_TopBar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOP_BAR_H
