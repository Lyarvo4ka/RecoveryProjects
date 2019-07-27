#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RawRecovery.h"

class RawRecovery : public QMainWindow
{
	Q_OBJECT

public:
	RawRecovery(QWidget *parent = Q_NULLPTR);

private:
	Ui::RawRecoveryClass ui;
};
