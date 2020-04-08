#pragma once

#include "treeitem.h"
#include "raw/AbstractRaw.h"
#include <QAbstractItemModel>
#include <QFileIconProvider>

enum SignatureItemType : int
{
	kSignatureItem, kCategoryItem
};

class SignatureAdapter
{
	QString name_;
protected:
	SignatureItemType signItemType_;

public:
	void setName(const QString& name)
	{
		name_ = name;
	}
	QString getName() const
	{
		return name_;
	}
	SignatureItemType getItemType() const
	{
		return signItemType_;
	}
	virtual RAW::FileStruct::Ptr createFileStruct() = 0;
};

class CategoryFolderAdapter
	: public SignatureAdapter
{
public:
	CategoryFolderAdapter()
	{
		signItemType_ = SignatureItemType::kCategoryItem;
	}
	RAW::FileStruct::Ptr createFileStruct() override
	{
		return nullptr;
	}
};

class SignatureItemAdapter
	: public SignatureAdapter
{
	RAW::FileStruct::Ptr file_struct_;
public:
	SignatureItemAdapter(RAW::FileStruct::Ptr file_struct)
		: file_struct_(file_struct)
	{
		signItemType_ = (SignatureItemType::kSignatureItem);
		setName(QString::fromLocal8Bit(file_struct->getAlgorithmName().c_str()));
	}
	RAW::FileStruct::Ptr createFileStruct() override
	{
		return file_struct_;
	}
};

class SignatureItem 
	: public TreeItem<SignatureAdapter>
{
public:
	explicit SignatureItem(std::unique_ptr<SignatureAdapter> adapter, SignatureItem* parentItem = nullptr)
		:TreeItem(std::move(adapter), parentItem)
	{
		
	}	
	TreeItem* findWithName(const QString & name_txt)
	{
		for (auto i = 0; i < this->childCount(); ++i)
		{
			auto treeItem = this->child(i);
			if (treeItem->getAdapter()->getName().compare(name_txt) == 0)
				return treeItem;
		}
		return nullptr;
	}

};

//using SignatureItem = TreeItem< SignatureAdapter>;


class SignatureTreeModel
	: public QAbstractItemModel
{
	Q_OBJECT
private:
	SignatureItem* rootItem_;
	QFileIconProvider iconProvider_;
public:
	explicit SignatureTreeModel(SignatureItem* root_item, QObject* parent = nullptr)
		:QAbstractItemModel(parent)
		, rootItem_(root_item)
	{

	}
	~SignatureTreeModel()
	{
		delete rootItem_;
	}


	QVariant data(const QModelIndex& index, int role) const override
	{
		if (!index.isValid())
			return QVariant();

		auto item = static_cast<SignatureItem*>(index.internalPointer());

		if (role == Qt::DisplayRole)
		{
			auto tmp_name = item->getAdapter()->getName();
			return tmp_name;
		}
		if (index.column() == 0)
		{
			//if (role == Qt::CheckStateRole)
			//	return static_cast<int>(model_index->checked());
			//else
				if (role == Qt::DecorationRole)
				{
					switch (item->getAdapter()->getItemType())
					{
					case SignatureItemType::kCategoryItem:
						iconProvider_.icon(QFileIconProvider::Folder);
						break;
					case SignatureItemType::kSignatureItem:
						iconProvider_.icon(QFileIconProvider::File);
						break;

					}
				}
		}
		return QVariant();
	}
	Qt::ItemFlags flags(const QModelIndex& index) const override
	{
		if (!index.isValid())
			return Qt::NoItemFlags;

		return QAbstractItemModel::flags(index) /* | Qt::ItemIsUserCheckable */;

	}

	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override
	{
		return QVariant();
	}

	QModelIndex index(int row, int column,
		const QModelIndex& parent = QModelIndex()) const override
	{
		if (!hasIndex(row, column, parent))
			return QModelIndex();

		SignatureItem* parentItem = nullptr;

		if (!parent.isValid())
			parentItem = rootItem_;
		else
			parentItem = static_cast<SignatureItem*>(parent.internalPointer());

		auto childItem = parentItem->child(row);
		if (childItem)
			return createIndex(row, column, childItem);
		return QModelIndex();
	}

	QModelIndex parent(const QModelIndex& index) const override
	{
		if (!index.isValid())
			return QModelIndex();

		auto childItem = static_cast<SignatureItem*>(index.internalPointer());
		auto parentItem = childItem->parentItem();

		if (parentItem == rootItem_)
			return QModelIndex();

		return createIndex(parentItem->row(), 0, parentItem);
	}

	int rowCount(const QModelIndex& parent = QModelIndex()) const override
	{
		SignatureItem* parentItem = nullptr;
		if (parent.column() > 0)
			return 0;

		if (!parent.isValid())
			parentItem = rootItem_;
		else
			parentItem = static_cast<SignatureItem*>(parent.internalPointer());

		return parentItem->childCount();
	}
	int columnCount(const QModelIndex& parent = QModelIndex()) const override
	{
		return /*(parent.isValid() && parent.column() != 0) ? 0 :*/ 1;
	}

};