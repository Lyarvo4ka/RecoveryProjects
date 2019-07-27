#include "stdafx.h"
#include "RawRecovery.h"
#include <QtWidgets/QApplication>

#include "io/iodevice.h"
#include "io/dbf.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//auto list_drives = IO::ReadPhysicalDrives();
	auto filename = LR"(d:\PaboTa\46566\dbf\joined\RG295.DBF)";

	IO::fixDBF(filename);

	RawRecovery w;
	w.show();
	return a.exec();
}
