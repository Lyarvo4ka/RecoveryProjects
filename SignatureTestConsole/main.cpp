#include <QtCore/QCoreApplication>

#include "../JsonReader/JsonReader.h"
#include "../JsonReader/SignatureReader.h"

#include "IO/Finder.h"
#include "raw/AbstractRaw.h"

#include <filesystem>

namespace fs = std::filesystem;

void rename_to_bad_file(const IO::path_string & file_path)
{
	fs::rename(file_path, file_path + L".bad_file");
}

void testMp3(const RAW::FileStruct & Mp3FileStruct, const IO::path_string & file_to_test)
{
	IO::File file(file_to_test);
	file.OpenRead();
	auto file_size = file.Size();

	const uint32_t cmp_size = 4096;
	const uint32_t cmp_nulls_size = 16;

	if (file_size < cmp_size)
	{
		file.Close();
		rename_to_bad_file(file_to_test);
		return;
	}

	IO::DataArray block_data(cmp_size);
	file.ReadData(block_data);
	file.Close();


	bool isNullsFromStart = true;

	uint32_t not_null_pos = 0;
	for (not_null_pos = 0; not_null_pos < cmp_size; ++not_null_pos)
	{
		if (block_data.data()[not_null_pos]!= 0)
		{
			isNullsFromStart = false;
			break;
		}
	}

	IO::ByteArray pMp3Start = block_data.data();

	uint32_t size_to_compare = cmp_size;
	if (not_null_pos > cmp_nulls_size)
	{
		pMp3Start = block_data.data() + not_null_pos;
		size_to_compare = cmp_size - not_null_pos;
	}


	auto bFound = Mp3FileStruct.compareWithAllHeaders(pMp3Start, size_to_compare);
	
	if (!bFound)
	{
		rename_to_bad_file(file_to_test);
	}
	

}

void testSignature(const RAW::FileStruct& fileStruct, const IO::path_string& file_to_test)
{
	IO::File file(file_to_test);
	file.OpenRead();
	auto file_size = file.Size();
	auto cmp_size = default_sector_size;

	if (file_size < cmp_size)
	{
		file.Close();
		rename_to_bad_file(file_to_test);
		return;
	}
	IO::DataArray buffer(cmp_size);
	file.ReadData(buffer);
	file.Close();
	auto bFound = fileStruct.compareWithAllHeaders(buffer.data(), buffer.size());

	if (!bFound)
	{
		rename_to_bad_file(file_to_test);
	}



}

inline void ReadSignatures(SignatureReader & signatureReader, RAW::HeaderBase::Ptr headeBase , const IO::path_string & folder)
{
	if (headeBase)
	{
		signatureReader.loadAllSignatures(folder , L".json");
		for (auto json_signature : signatureReader.getAllSignatures())
		{
			headeBase->addFileFormat(toFileStruct(json_signature));
		}
	}
}

class ExtensionExtractor
{
	const IO::path_string signaturePath_;
	RAW::HeaderBase::Ptr headerBase_ = std::make_shared< RAW::HeaderBase>();
	SignatureReader signatureReader;
public:
	ExtensionExtractor(const IO::path_string signaturePath)
		:signaturePath_(signaturePath)
	{
	}

	void extract_extensions(const IO::path_list& listFiles)
	{
		const uint32_t DefaultReadSize = 33280;
		DataArray buffer(DefaultReadSize);
		for (auto filepath : listFiles)
		{
			uint32_t read_size = DefaultReadSize;
			File file(filepath);
			file.OpenRead();
			if (file.Size() < buffer.size())
				read_size = file.Size();

			file.ReadData(buffer.data(), read_size);
			file.Close();

			auto file_struct = headerBase_->find(buffer.data(), read_size);
			if (file_struct)
			{
				qInfo() << filepath << "-->" << QString::fromStdWString(file_struct->getExtension()) ;
				auto ext = file_struct->getExtension();
				auto filePathWithExt = filepath + file_struct->getExtension();
				fs::rename(filepath, filePathWithExt);
			}
		}
	}
};



class SignatureTest
{
	using ListFormatName = std::list<std::string>;
	using ExtensionName = std::string;
	using FormatName = std::string;
	std::map<ExtensionName, ListFormatName > extensionsMap_;
	std::map< FormatName, RAW::FileStruct> headerBase_;
	using 
public:
	IO::path_string getExtension(const IO::path_string & filename)
	{
		fs::path filePath(filename);
		return filePath.extension().generic_wstring();
	}

	std::list<RAW::FileStruct> getListFileStructFromListFormatName( const ListFormatName & listFormatName)
	{
		std::list<RAW::FileStruct> listFileStruct;
		for (auto formatName : listFormatName)
		{
			auto findIter =	headerBase_.find(formatName);
			if (findIter != headerBase_.end())
				listFileStruct.push_back(findIter->second);
		}
		return listFileStruct;
	}
	

	void testSigantures(const IO::path_string & filename , )
	{
		File file(filename);
		file.OpenRead();
		if (file.Size() < buffer.size())
			read_size = file.Size();

		file.ReadData(buffer.data(), read_size);
		file.Close();


	}



	//void testSigantures(const IO::path_list& listFiles)
	//{
	//	const uint32_t DefaultReadSize = 33280;
	//	DataArray buffer(DefaultReadSize);
	//	for (auto filepath : listFiles)
	//	{
	//		uint32_t read_size = DefaultReadSize;
	//		File file(filepath);
	//		file.OpenRead();
	//		if (file.Size() < buffer.size())
	//			read_size = file.Size();

	//		file.ReadData(buffer.data(), read_size);
	//		file.Close();

	//		auto file_struct = headerBase_->find(buffer.data(), read_size);
	//		if (file_struct)
	//		{
	//			qInfo() << filepath << "-->" << QString::fromStdWString(file_struct->getExtension());
	//			auto ext = file_struct->getExtension();
	//			auto filePathWithExt = filepath + file_struct->getExtension();
	//			fs::rename(filepath, filePathWithExt);
	//		}
	//	}

	//}
};




int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	IO::path_string singFolder = LR"(d:\develop\RecoveryProjects\SignatureTestConsole\signatures\)";
	SignatureReader signReader;
	signReader.loadAllSignatures(singFolder, L".json");

	IO::path_string extFolder = LR"(d:\develop\RecoveryProjects\SignatureTestConsole\extensions\)";
	ExtensionReader extReader;
	extReader.loadAllExtensions(extFolder, L".json");

	//ExtensionExtractor extExtractor;
	//extExtractor.loadAllSignatures(LR"(d:\develop\RecoveryProjects\SignatureTestConsole\signatures\)");

	//IO::Finder finder;
	//finder.FindFiles(LR"(f:\Root\!NoName\0\)");
	//auto listFiles = getFilesWithoutExtension(finder.getFiles());

	//extExtractor.extract_extensions(listFiles);


	qDebug() << "Finished.";
	return a.exec();
}

//
//int main(int argc, char *argv[])
//{
//	QCoreApplication a(argc, argv);
//
//	QString jsonFile("mts.json");
//
//	QFile file(jsonFile);
//	if (!file.open(QIODevice::ReadOnly))
//	{
//		qInfo() << "Error to open file. \"" << file.fileName() << "\"";
//		return -1;
//	}
//
//	auto json_str = file.readAll();
//	QList<JsonFileStruct> listFileStruct;
//	ReadJsonFIle(json_str, listFileStruct);
//
//	IO::Finder finder;
//	finder.add_extension(L".m2ts");
//
//	finder.FindFiles(LR"(f:\46976\)");
//	auto fileList = finder.getFiles();
//
//	if (!listFileStruct.empty())
//	{
//		auto fileStructQt = listFileStruct.first();
//		auto fileStruct = toFileStruct(fileStructQt);
//
//		for (auto & theFile : fileList)
//		{
//			testSignature(*fileStruct.get(),theFile);
//		}
//	}
//	
//
//	qDebug() << "Finished.";
//	return a.exec();
//}
