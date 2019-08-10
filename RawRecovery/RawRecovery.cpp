#include "stdafx.h"
#include "RawRecovery.h"

#include "io/iodevice.h"
#include "deviceitem.h"


RawRecovery::RawRecovery(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	auto physical_drives = IO::ReadPhysicalDrives();
	//physical_drives.


}
