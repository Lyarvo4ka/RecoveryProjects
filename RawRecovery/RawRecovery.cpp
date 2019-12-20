#include "stdafx.h"
#include "RawRecovery.h"

#include "io/iodevice.h"
#include "deviceitem.h"


RawRecovery::RawRecovery(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	//ui.
	auto physical_drives = IO::ReadPhysicalDrives();
	auto rootAdapter = std::make_unique< RootAdapter>();

	DeviceItem* root_item = new DeviceItem(std::move(rootAdapter));
	for (auto i = 0; i < physical_drives.getSize(); ++i)
	{
		auto physicalAdapter = std::make_unique<PhysicalAdapter>(physical_drives.find_by_number(i));
		DeviceItem* disk_item = new DeviceItem(std::move(physicalAdapter ), root_item);
		root_item->appendChild(disk_item);
	}
	auto device_model = new DeviceModel(root_item, this);
	ui.treeView->setModel(device_model);
	for (auto iColumn = 0; iColumn < device_model->columnCount(); ++iColumn)
	{
		ui.treeView->resizeColumnToContents(iColumn);
	}
	//physical_drives.


}
