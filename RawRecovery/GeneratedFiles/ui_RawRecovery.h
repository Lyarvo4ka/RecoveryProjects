/********************************************************************************
** Form generated from reading UI file 'RawRecovery.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RAWRECOVERY_H
#define UI_RAWRECOVERY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RawRecoveryClass
{
public:
    QAction *actionOpen;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QTreeView *treeView;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QTreeView *signatureTree;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;
    QWidget *tab_2;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RawRecoveryClass)
    {
        if (RawRecoveryClass->objectName().isEmpty())
            RawRecoveryClass->setObjectName(QString::fromUtf8("RawRecoveryClass"));
        RawRecoveryClass->resize(1124, 819);
        actionOpen = new QAction(RawRecoveryClass);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        centralWidget = new QWidget(RawRecoveryClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        treeView = new QTreeView(centralWidget);
        treeView->setObjectName(QString::fromUtf8("treeView"));

        horizontalLayout->addWidget(treeView);

        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        signatureTree = new QTreeView(tab);
        signatureTree->setObjectName(QString::fromUtf8("signatureTree"));

        verticalLayout_2->addWidget(signatureTree);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton = new QPushButton(tab);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout_2->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout_2);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());

        horizontalLayout->addWidget(tabWidget);


        verticalLayout->addLayout(horizontalLayout);

        RawRecoveryClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(RawRecoveryClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1124, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        RawRecoveryClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(RawRecoveryClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        RawRecoveryClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(RawRecoveryClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        RawRecoveryClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);

        retranslateUi(RawRecoveryClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(RawRecoveryClass);
    } // setupUi

    void retranslateUi(QMainWindow *RawRecoveryClass)
    {
        RawRecoveryClass->setWindowTitle(QCoreApplication::translate("RawRecoveryClass", "RawRecovery", nullptr));
        actionOpen->setText(QCoreApplication::translate("RawRecoveryClass", "Open", nullptr));
        pushButton->setText(QCoreApplication::translate("RawRecoveryClass", "Start", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("RawRecoveryClass", "Tab 1", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("RawRecoveryClass", "Tab 2", nullptr));
        menuFile->setTitle(QCoreApplication::translate("RawRecoveryClass", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RawRecoveryClass: public Ui_RawRecoveryClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RAWRECOVERY_H
