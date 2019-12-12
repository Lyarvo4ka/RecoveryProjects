#pragma once

#include <qvariant.h>
#include "io/physicaldrive.h"
#include "io/iodevice.h"

enum DeviceItemNames : int
{
	kName , kLabel , kFS , kStart , kSize , kCount
};

const QString Name_txt = "Name";
const QString Label_txt = "Label";
const QString FS_txt = "FS";
const QString Start_txt = "Start";
const QString Size_txt = "Size";

//const std::vector<const char *> array

struct DeviceItemData
{
	QString name;
	QString label;
	QString fs;
	uint64_t start = 0;
	uint64_t size = 0;
};

class DeviceAdapter
{
protected:
	DeviceItemData itemData_;
public:
	virtual IO::IODevicePtr createDevice() = 0;
	const DeviceItemData & getDeviceItemData() const
	{
		return itemData_;
	}
};

class RootAdapter
	: public DeviceAdapter 
{
public:
	IO::IODevicePtr createDevice() override
	{
		return nullptr;
	}
};

class PhysicalAdapter
	: public DeviceAdapter
{
	IO::PhysicalDrivePtr physical_drive_;
public:
	PhysicalAdapter(IO::PhysicalDrivePtr physical_drive_ptr)
		: physical_drive_(physical_drive_ptr)
	{
		itemData_.name.fromStdWString(physical_drive_->getDriveName());
		itemData_.label.fromStdString(physical_drive_->getSerialNumber());
		itemData_.fs = QString::number(physical_drive_->getDriveNumber());
		itemData_.start = 0;
		itemData_.size = physical_drive_->getSize();


	}
	IO::IODevicePtr createDevice() override
	{
		return std::make_shared<IO::DiskDevice>(physical_drive_);
	}
};

class DeviceItem
{
public:
	explicit DeviceItem(std::unique_ptr<DeviceAdapter> device_adapter, DeviceItem* parentItem = nullptr)
		: device_adapter_(std::move(device_adapter))
		, parentItem_(parentItem)
	{

	}
	~DeviceItem()
	{
		qDeleteAll(childItems_);
	}

	void appendChild(DeviceItem* child)
	{
		childItems_.append(child);
	}
	DeviceItem* child(int row)
	{
		if (row < 0 || row >= childItems_.size())
			return nullptr;
		return childItems_.at(row);

	}
	int childCount() const
	{
		return childItems_.count();
	}
	int columnCount() const
	{
		return DeviceItemNames::kCount;
	}
	DeviceAdapter* getDeviceAdapter()
	{
		return device_adapter_.get();
	}

	int row() const
	{
		if (parentItem_ != nullptr)
			return parentItem_->childItems_.indexOf(const_cast<DeviceItem*>(this));

		return 0;

	}
	DeviceItem* parentItem()
	{
		return parentItem_;
	}

private:
	std::unique_ptr<DeviceAdapter> device_adapter_;
	QVector<DeviceItem*> childItems_;
	DeviceItem* parentItem_ = nullptr ;
};


class DeviceModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit DeviceModel(DeviceItem* root_item, QObject* parent = nullptr)
		:QAbstractItemModel(parent)
		, rootItem_(root_item)
	{

	}
	~DeviceModel()
	{
		delete rootItem_;
	}

	QVariant data(const QModelIndex& index, int role) const override
	{
		if (!index.isValid())
			return QVariant();

		if (role != Qt::DisplayRole)
			return QVariant();

		auto item = static_cast<DeviceItem*>(index.internalPointer());
		auto itemData = item->getDeviceAdapter()->getDeviceItemData();
		//itemData.name
		switch (index.column())
		{
		case DeviceItemNames::kName: return itemData.name;
		case DeviceItemNames::kLabel: return itemData.label;
		case DeviceItemNames::kFS: return itemData.fs;
		case DeviceItemNames::kStart: return itemData.start;
		case DeviceItemNames::kSize: return itemData.size;
		}

		return QVariant();
	}
	Qt::ItemFlags flags(const QModelIndex& index) const override
	{
		if (!index.isValid())
			return Qt::NoItemFlags;

		return QAbstractItemModel::flags(index);

	}
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override
	{
		if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		{
			switch (section)
			{
			case DeviceItemNames::kName: return Name_txt;
			case DeviceItemNames::kLabel: return Label_txt;
			case DeviceItemNames::kFS: return FS_txt;
			case DeviceItemNames::kStart: return Start_txt;
			case DeviceItemNames::kSize: return Size_txt;
			}
		}

		return QVariant();
	}

	QModelIndex index(int row, int column,
		const QModelIndex& parent = QModelIndex()) const override
	{
		if (!hasIndex(row, column, parent))
			return QModelIndex();

		DeviceItem* parentItem = nullptr;

		if (!parent.isValid())
			parentItem = rootItem_;
		else
			parentItem = static_cast<DeviceItem*>(parent.internalPointer());

		auto childItem = parentItem->child(row);
		if (childItem)
			return createIndex(row, column, childItem);
		return QModelIndex();
	}
	QModelIndex parent(const QModelIndex& index) const override
	{
		if (!index.isValid())
			return QModelIndex();

		auto childItem = static_cast<DeviceItem*>(index.internalPointer());
		auto parentItem = childItem->parentItem();

		if (parentItem == rootItem_)
			return QModelIndex();

		return createIndex(parentItem->row(), 0, parentItem);
	}
	int rowCount(const QModelIndex& parent = QModelIndex()) const override
	{
		DeviceItem* parentItem = nullptr;
		if (parent.column() > 0)
			return 0;

		if (!parent.isValid())
			parentItem = rootItem_;
		else
			parentItem = static_cast<DeviceItem*>(parent.internalPointer());

		return parentItem->childCount();
	}
	int columnCount(const QModelIndex& parent = QModelIndex()) const override
	{
		if (parent.isValid())
			return static_cast<DeviceItem*>(parent.internalPointer())->columnCount();
		return rootItem_->columnCount();
	}

private:

	DeviceItem* rootItem_;
};