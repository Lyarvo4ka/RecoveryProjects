#pragma once

#include <qvariant.h>

template < class AdapterType >
class TreeItem
{
	std::unique_ptr<AdapterType> adapter_;
	QVector<TreeItem*> childItems_;
	TreeItem* parentItem_ = nullptr;
public:
	explicit TreeItem(std::unique_ptr<AdapterType> adapter, TreeItem* parentItem = nullptr)
		: adapter_(std::move(adapter))
		, parentItem_(parentItem)
	{

	}
	~TreeItem()
	{
		qDeleteAll(childItems_);
	}

	void appendChild(TreeItem* child)
	{
		childItems_.append(child);
	}
	TreeItem* child(int row)
	{
		if (row < 0 || row >= childItems_.size())
			return nullptr;
		return childItems_.at(row);

	}
	int childCount() const
	{
		return childItems_.count();
	}

	AdapterType* getAdapter()
	{
		return adapter_.get();
	}

	int row() const
	{
		if (parentItem_ != nullptr)
			return parentItem_->childItems_.indexOf(const_cast<TreeItem*>(this));

		return 0;

	}
	TreeItem* parentItem()
	{
		return parentItem_;
	}


};
