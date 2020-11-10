#pragma once

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>


struct Extensions
{
	QString extensionName;
	QList<QString> listFormatName;
};

void ReadAllExtensions(const QByteArray& byte_data)
{
	QJsonDocument json_doc = QJsonDocument::fromJson(byte_data);
	if (json_doc.isNull())
	{
		qInfo() << "Error to parse json file.";
		return;
	}
	auto root = json_doc.object();
	auto extensions = root.keys();

	QList<Extensions> listExtensions;

	for (auto ext : extensions)
	{
		Extensions extension;
		extension.extensionName = ext;
		auto object_value = root.value(ext);
		if (object_value.isArray())
		{
			auto formatNameArray = object_value.toArray();
			for (auto formatName : formatNameArray)
			{
				extension.listFormatName.append(formatName.toString());
			}
		}

		listExtensions.append(extension)
	}



}
