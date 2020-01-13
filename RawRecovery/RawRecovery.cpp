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

	auto root_item = new DeviceItem(std::move(rootAdapter));
	for (auto i = 0; i < physical_drives.getSize(); ++i)
	{
		auto physicalAdapter = std::make_unique<PhysicalAdapter>(physical_drives.index(i));
		auto disk_item = new DeviceItem(std::move(physicalAdapter ), root_item);
		root_item->appendChild(disk_item);
	}
	auto device_model = new DeviceModel(root_item, this);
	ui.treeView->setModel(device_model);
	for (auto iColumn = 0; iColumn < device_model->columnCount(); ++iColumn)
	{
		ui.treeView->resizeColumnToContents(iColumn);
	}
	contectMenu_ = new QAction(tr("&TEST SELECT DEVICE!!!!"), this);
	ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);
//	ui.treeView->addAction(contectMenu_);

	connect(ui.treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnDeviceContextMenu(const QPoint&)));

	//physical_drives.


}

void RawRecovery::OnDeviceContextMenu(const QPoint& point_pos)
{
	QModelIndex device_cell = ui.treeView->indexAt(point_pos);
	if (device_cell.isValid())
	{
		auto seclected_index = static_cast<DeviceItem*>(device_cell.internalPointer());
		if (seclected_index)
		{
			auto selected_device = seclected_index->getDeviceAdapter()->createDevice();
			if (selected_device)
			{
				auto disk_device = std::dynamic_pointer_cast<IO::DiskDevice>(selected_device);
				if (disk_device)
				{
					QMessageBox msgBox;
					QString msg_string = "Selected " + QString::fromWCharArray(disk_device->getDiskName().c_str());
					msgBox.setText(msg_string);
					msgBox.exec();
				}
			}
		}
	}
}
