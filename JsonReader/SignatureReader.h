#pragma once

#include "io/finder.h"
#include "JsonReader.h"


class SignatureReader
{
	QList<JsonFileStruct> listSignatures_;
	IO::path_string signaturesFolder_;
public:
	SignatureReader(const IO::path_string& signaturesFolder)
		:signaturesFolder_(signaturesFolder)
	{

	}
	void loadAllSignatures()
	{
		IO::Finder finder;
		finder.add_extension(L".json");
		finder.FindFiles(signaturesFolder_);
		auto listFilesOfSignatures = finder.getFiles();
		for (auto signFile : listFilesOfSignatures)
			addSignatures(signFile);
	}
	void addSignatures(const IO::path_string& signaturesFile)
	{
		QList<JsonFileStruct> listFileStruct;

		QString json_file = QString::fromStdWString(signaturesFile);
		QFile file(json_file);
		if (!file.open(QIODevice::ReadOnly))
		{
			qInfo() << "Error to open file. \"" << file.fileName() << "\"";
			return;
		}

		auto json_str = file.readAll();
		ReadJsonFIle(json_str, listFileStruct);
		if (listFileStruct.empty())
		{
			qInfo() << "Error to read" << file.fileName() << "file. Wrong syntax.";
			return;
		}

		for (auto theFileStruct : listFileStruct)
			listSignatures_.append(theFileStruct);
	}

	QList<JsonFileStruct> getAllSignatures() const
	{
		return listSignatures_;
	}


};
