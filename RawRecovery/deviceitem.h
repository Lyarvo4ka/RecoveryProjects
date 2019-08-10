#pragma once

#include <qvariant.h>

class DeviceItem
{
public:
	explicit DeviceItem(const QVector<QVariant>& data, DeviceItem* parentItem = nullptr);
	~DeviceItem();

	void appendChild(DeviceItem* child);

	DeviceItem* child(int row);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	int row() const;
	DeviceItem* parentItem();

private:
	QString name_;
	QString label_;
	QString fs_;
	uint64_t start_ = 0;
	uint64_t size_ = 0;
	//QVector<TreeItem*> m_childItems;
	QVector<DeviceItem> itemData_;
	DeviceItem* parentItem_ ;
};