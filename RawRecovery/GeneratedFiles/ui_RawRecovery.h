/********************************************************************************
** Form generated from reading UI file 'RawRecovery.ui'
**
** Created by: Qt User Interface Compiler version 5.12.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RAWRECOVERY_H
#define UI_RAWRECOVERY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RawRecoveryClass
{
public:
    QAction *actionOpen;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RawRecoveryClass)
    {
        if (RawRecoveryClass->objectName().isEmpty())
            RawRecoveryClass->setObjectName(QString::fromUtf8("RawRecoveryClass"));
        RawRecoveryClass->resize(770, 548);
        actionOpen = new QAction(RawRecoveryClass);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        centralWidget = new QWidget(RawRecoveryClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        RawRecoveryClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(RawRecoveryClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 770, 21));
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

        QMetaObject::connectSlotsByName(RawRecoveryClass);
    } // setupUi

    void retranslateUi(QMainWindow *RawRecoveryClass)
    {
        RawRecoveryClass->setWindowTitle(QApplication::translate("RawRecoveryClass", "RawRecovery", nullptr));
        actionOpen->setText(QApplication::translate("RawRecoveryClass", "Open", nullptr));
        menuFile->setTitle(QApplication::translate("RawRecoveryClass", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RawRecoveryClass: public Ui_RawRecoveryClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RAWRECOVERY_H
