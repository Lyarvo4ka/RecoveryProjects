// ConsoleApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "raw/oleraw.h"
#include "io/functions.h"
//
//const char s_ftyp[] = "ftyp";
//const char s_moov[] = "moov";
//const char s_mdat[] = "mdat";
//const char s_wide[] = "wide";
//const char s_skip[] = "skip";
//const char s_free[] = "free";
//const char s_pnot[] = "pnot";
//const char s_prfl[] = "prfl";
//const char s_mvhd[] = "mvhd";
//const char s_clip[] = "clip";
//const char s_trak[] = "trak";
//const char s_udta[] = "udta";
//const char s_ctab[] = "ctab";
//const char s_cmov[] = "cmov";
//const char s_rmra[] = "rmra";
//const char s_uuid[] = "uuid";
//const char s_meta[] = "meta";
//
//
//inline bool isQtSignature(const uint8_t* qt_header, const uint32_t size)
//{
//	if (memcmp(qt_header, s_ftyp, size) == 0)
//		return true;
//	else if (memcmp(qt_header, s_moov, size) == 0)
//		return true;
//	else if (memcmp(qt_header, s_mdat, size) == 0)
//		return true;
//	else if (memcmp(qt_header, s_wide, size) == 0)
//		return true;
//
//
//	return false;
//}
//
//inline void testSignatureMP4(const IO::path_string& filePath)
//{
//	auto test_file = IO::makeFilePtr(filePath);
//	const uint32_t offset = 4;
//	const uint32_t header_size = 4;
//	uint8_t buff[header_size];
//	ZeroMemory(buff, header_size);
//	if (test_file->Open(IO::OpenMode::OpenRead))
//	{
//		if (test_file->Size() >= header_size)
//		{
//			test_file->setPosition(offset);
//			test_file->ReadData(buff, header_size);
//			test_file->Close();
//
//			if (isQtSignature(buff, header_size) == false)
//			{
//				auto new_file_name = filePath + L".bad_file";
//				std::wcout << new_file_name.c_str() << std::endl;
//				fs::rename(filePath, new_file_name);
//			}
//		}
//
//	}
//
//
//}
//
//
//void testMP4_signature(const IO::path_string& folder)
//{
//	IO::Finder finder;
//	finder.add_extension(L".mov");
//	finder.add_extension(L".mp4");
//	finder.add_extension(L".3gp");
//	finder.add_extension(L".3gpp");
//	finder.add_extension(L".m4a");
//	//finder.add_extension(L".msdoc");
//	finder.FindFiles(folder);
//	auto fileList = finder.getFiles();
//
//	for (auto& theFile : fileList)
//	{
//		try {
//			std::wcout << theFile.c_str();
//			testSignatureMP4(theFile);
//			std::wcout <<L" - OK" <<std::endl;
//		}
//		catch (IO::Error::IOErrorException& ex)
//		{
//			const char* text = ex.what();
//			std::cout << " Cougth exception " << text;
//
//		}
//		catch (fs::filesystem_error &fs_error)
//		{
//			std::cout << " Cougth exception " << fs_error.what();
//			int k = 1;
//			k = 2;
//
//		}
//
//
//	}
//}

void testDCM(const IO::path_string& filePath)
{
	const uint32_t offset = 128;
	const uint8_t dcm_header[] = { 0x44, 0x49 , 0x43 ,0x4D };
	const uint32_t dcm_header_size = SIZEOF_ARRAY(dcm_header);

	IO::File src_file(filePath);
	src_file.OpenRead();

	uint8_t buff[dcm_header_size];
	memset(buff, 0, dcm_header_size);

	if (src_file.Size() > (offset + dcm_header_size))
	{
		src_file.setPosition(offset);
		src_file.ReadData((IO::ByteArray)buff, dcm_header_size);

		src_file.Close();

		if (memcmp(buff, dcm_header, dcm_header_size) != 0)
		{
			auto new_file_name = filePath + L".bad_file";
			std::wcout << new_file_name.c_str() << std::endl;
			fs::rename(filePath, new_file_name);
		}

	}

}

#pragma pack(1)
struct dcm_type_t
{
	uint8_t DICM[4];
	uint8_t reserved[8];
	uint8_t size;
};
#pragma pack()

#pragma pack(1)
struct dcm_name_t
{
	uint8_t UI[2];
	uint16_t size;
	uint8_t pName[];
};
#pragma pack()


std::string getDCM_DateString(const IO::path_string& filePath)
{
	std::string date_string;
	const uint32_t offset = 128;
	const uint32_t max_length = 128;

	const uint16_t UI_ = 0x4955;

	dcm_type_t dcm_type = {};

	IO::File src_file(filePath);
	src_file.OpenRead();

	src_file.setPosition(offset);
	src_file.ReadData((IO::ByteArray)&dcm_type, sizeof(dcm_type_t));

	if (dcm_type.size == 0)
		return date_string;

	IO::DataArray buffer(dcm_type.size);
	src_file.setPosition(offset);
	uint64_t new_offset = offset + sizeof(dcm_type_t);
	src_file.setPosition(offset);
	src_file.ReadData(buffer);
	bool bFound = false;

	uint16_t string_size = 0;
	for (uint32_t i = 0; i < buffer.size()-1; ++i)
	{
		uint16_t* pUI = (uint16_t*)& buffer.data()[i];
		if (*pUI == UI_)
		{
			if (bFound)
			{
				dcm_name_t * dcm_name = (dcm_name_t * )&buffer.data()[i];
				string_size = dcm_name->size;
				if (string_size == 0)
					return date_string;

				IO::DataArray string_data(string_size +1);
				ZeroMemory(string_data.data(), string_size + 1);
				std::memcpy(string_data.data(), dcm_name->pName, string_size);
				std::string tmp((char*)string_data.data());
				return tmp;
			}
			bFound = true;
		}
	}

	return date_string;
}


void testDCM_signatrue(const IO::path_string& folder)
{
	IO::Finder finder;
	finder.add_extension(L".dcm");

	finder.FindFiles(folder);
	auto fileList = finder.getFiles();

	for (auto& theFile : fileList)
	{
		try {
			std::wcout << theFile.c_str();
			testDCM(theFile);
			std::wcout <<L" - OK" <<std::endl;
		}
		catch (IO::Error::IOErrorException& ex)
		{
			const char* text = ex.what();
			std::cout << " Cougth exception " << text;

		}
		catch (fs::filesystem_error &fs_error)
		{
			std::cout << " Cougth exception " << fs_error.what();
			int k = 1;
			k = 2;

		}


	}
}
void getDCMFiles_date(const IO::path_string& src_folder, const IO::path_string& target_folder)
{
	IO::Finder finder;
	finder.add_extension(L".dcm");

	finder.FindFiles(src_folder);
	auto fileList = finder.getFiles();

	for (auto& theFile : fileList)
	{
			std::wcout << theFile.c_str();
			fs::path filePath(theFile);
			
			//auto ext = theFile.extension().generic_wstring();

			auto new_file_name = target_folder + filePath.filename().generic_wstring();
			auto date_string = getDCM_DateString(theFile);

			if (!date_string.empty())
			{
				std::wstring file_name(date_string.begin(), date_string.end());
 				new_file_name = target_folder + file_name + L".dcm";
			}
			uint64_t iNumber = 1;
			auto firstfilename = new_file_name;
			while (fs::exists(new_file_name))
			{
				auto strNumber = std::to_wstring(iNumber++);
				fs::path tmp_path(firstfilename);
				auto tmp_filename = tmp_path.stem().generic_wstring();
				new_file_name = target_folder + tmp_filename + L"_" + strNumber + L".dcm";
			}
 			fs::copy(theFile, new_file_name);

	}
}

void add_service(const IO::path_string& src_filename, const IO::path_string& dst_filename)
{
	const uint32_t fullpage_size = 18432;
	const uint32_t numof_pages = 16;
	const uint32_t data_size = 1024;

	const uint32_t src_page_size = 16384;

	IO::DataArray target(fullpage_size);
	IO::DataArray source(src_page_size);

	IO::File src(src_filename);
	src.OpenRead();

	IO::File dst(dst_filename);
	dst.OpenCreate();

	const uint32_t first_offset = data_size + 113;
	uint32_t cur_offset = 0;
	while (cur_offset < src.Size())
	{
		src.ReadData(source);

		memset(target.data(), 0xFF, fullpage_size);
		memcpy(target.data(), source.data(), data_size);

		for (auto i = 0; i < 15; ++i)
		{
			auto src_offset = data_size * (i + 1);
			auto dst_offset = first_offset + (data_size + 109) * i ;
			memcpy(target.data() + dst_offset, source.data() + src_offset, data_size);

		}
		dst.WriteData(target.data(), target.size());
		cur_offset += source.size();
	}





}

#include "raw/GoPro.h"
#include "io/dbf.h"

#include "io/utility.h"
#include "raw/ext4_raw.h"


void findNullsBlock()
{
	IO::File file(LR"(d:\PaboTa\47240\1C\original\1Cv8.1CD )");
	file.OpenRead();
	uint64_t offset = 512*512;
	IO::DataArray buff(4096);
	while (offset < file.Size())
	{
		file.setPosition(offset);
		file.ReadData(buff);
		bool bFound = false;
		for (auto i = 0; i < buff.size(); ++i)
		{
			if (buff[i] != 0)
			{
				bFound = true;
				break;
			}
		}

		if (bFound == false)
		{
			int k = 1;
			k = 2;
		}

		offset += buff.size();
	}
}

#include "io/utility.h"
#include "raw/QuickTime.h"

inline void Xor2Files(const IO::path_string& sourceFileName, const IO::path_string& xorFileName, const IO::path_string& targetFileName)
{
	IO::File src_file(sourceFileName);
	src_file.OpenRead();
	IO::File xor_file(xorFileName);
	xor_file.OpenRead();
	IO::File target_file(targetFileName);
	target_file.OpenCreate();

	auto xor_size = xor_file.Size();
	if (xor_size == 0)
	{
		printf_s("Xor file size is 0.\r\n");
		return;
	}


	auto source_size = src_file.Size();
	if ((source_size % xor_size) != 0)
	{
		printf_s("Source size is not multiple by xor size.\r\n");
		return;
	}

	IO::DataArray xor_buffer(xor_size);
	xor_file.ReadData(xor_buffer);

	IO::DataArray src_buffer(xor_size);
	IO::DataArray dst_buffer(xor_size);



	uint64_t offset = 0;

	while (offset < source_size)
	{
		src_file.setPosition(offset);
		src_file.ReadData(src_buffer);

		for (uint32_t iByte = 0; iByte < xor_size; ++iByte)
		{
			dst_buffer[iByte] = src_buffer[iByte] ^ xor_buffer[iByte];
		}

		target_file.WriteData(dst_buffer.data(), dst_buffer.size());

		offset += xor_size;
	}

	printf_s("\r\nFinished\r\n");
}

void testQtFiles(const IO::path_string& folder_path)
{
	IO::Finder finder;
	finder.add_extension(L".mov");
	finder.add_extension(L".mp4");
	finder.FindFiles(folder_path);

	for (auto filepath : finder.getFiles())
	{
		bool bGood = false;
		{
			auto filePtr = IO::makeFilePtr(filepath);
			filePtr->OpenRead();
			RAW::QuickTimeRaw qt_raw(filePtr);
			RAW::QuickTimeList atomsList;

			auto sizeKeywords = qt_raw.readAllQtAtoms(0, atomsList);
			if (qt_raw.isPresentMainKeywords(atomsList))
			{
				bGood = true;
			}

		}
		if (!bGood)
		{
			auto new_file_name = filepath + L".bad_file";
			//std::wcout << new_file_name.c_str() << std::endl;
			fs::rename(filepath, new_file_name);
		}
	}


}



/*
Xor2Files
	if (argc == 4)
	{
		std::wstring srcFileName = argv[1];
		std::wstring xorFileName = argv[2];
		std::wstring dstFileName = argv[3];

		printf_s("It's starting xor ... \r\n");
		Xor2Files(srcFileName, xorFileName, dstFileName);

	}
	else
	{
		printf_s("Wrong params\r\n");
		printf_s("1 - source file\r\n");
		printf_s("2 - xor file\r\n");
		printf_s("3 - target file\r\n");

	}

*/

#include "io/XorAnalyzer.h"

int wmain(int argc, wchar_t* argv[])
{

	if (argc == 4)
	{
		std::wstring srcFileName = argv[1];
		std::wstring xorFileName = argv[2];
		int block_size = std::stoi(argv[3]);


		IO::XorAnalyzer xor_analyzer(srcFileName);
		xor_analyzer.Analize(xorFileName, block_size);

	}
	else
	{
		printf_s("Wrong params\r\n");
		printf_s("1 - source file\r\n");
		printf_s("2 - xor file\r\n");
		printf_s("3 - xor size\r\n");

	}



/*
	IO::path_string src_folder = LR"(d:\PaboTa\47651\NoName\DCM\)";
	IO::path_string target_folder = LR"(z:\47651\)";
	getDCMFiles_date(src_folder, target_folder);
	//testDCM_signatrue(LR"(d:\PaboTa\47651\)");
*/

	// segment 1
	// 1 - 0x8FA000000;
	// 2 - 0xFA08626000	---- not
	// 3 - 0xFA087EE000
	// 4 - 0xFA08A16000
	// 5 - 0xFA08C4A000
	// ...
	// 0xFA0FEA9000

	// 0x115DD000000


	// segment 2
	// 1 - 0x18401000	// 0 bytes
	// 2 - 0x3CC800000	// 19 Gb
	// 3 - 0x3CC801000	// 15208247296 bytes
	// 4 - 0xB58800000	//
	// 5 - 0x16A8801000 //
	// 6 - 0x21F0800000
	// 7 - 0x2D40800000
	// 8 - 0x3948800000
	// 9 - 0x4498800000
	// 10- 0x4FE0800000
	// 11- 0x5AF8800000
	// 12- 0x6618800000
	// 13- 0x7160800000
	// 14- 0x7CB0800000
	// 15- 0x87F0800000
	// 16- 0x9340800000
	// 17- 0x9E90800000
	// 18- 0xA9D8800000
	// 19- 0xB528800000
	// 20- 0xC040800000
	// 21- 0xC5087CA000 , 0xC5087E0000
	// 22- 0xC50865A000 , 0xC5086AB000
	// 23- 0xC5087E0000
	// 24- 0xD6E0800000
	// 25- 0xE230800000
	// 26- 0xED78800000
	// 27- 0xF8B8800000
	// 28- 0x10402020000
	// 29- 0x10F58800000

	// add 0x10000000
	// 30- 0x1116D800000	-- 0(bytes)
	// 31- 0x11CA0800000	-- 44736446464(bytes)
	// 32- 0x127D8800000	-- 89766494208(bytes)
	// 33- 0x13328800000
	// 34- 0x13E70801000
	// 35- 0x149C0800000
	//154F8800000
	//16040800000
	//16B70800000


	// skip 
	// 0xB5D6EC1000

	//0x748DAA9000; -- 300 GB	// depth == 2
	//0x748ED0C000; -- 100 GB entries = 0x22
/*
	auto src_file = IO::makeFilePtr(LR"(f:\segment_v2\47555_v2.tmp)");
	src_file->OpenRead();
	RAW::ext4_raw ext4_recovery(src_file);

	uint64_t inode_offset = 0x18401000;
	IO::path_string target_name = LR"(d:\PaboTa\47555\111.tmp)";
	//ext4_recovery.Execute(inode_offset, target_name);
	uint64_t inode_block = inode_offset / 4096;
	ext4_recovery.searchExtends(inode_block);
*/
	//ext4_recovery.findExtentsWithDepth(0);
	//auto size = ext4_recovery.calculateSize(inode_block);
	//ext4_recovery.readOffsetsFromFile();
	_CrtDumpMemoryLeaks();

// 773807144960 - 0xC508942000
// 729162973184 - 0xC508ABA000
// 683671552000 - 0xC508C24000

	//auto filePath = LR"(z:\47186\47186.bin)";
	//IO::replaceBadMarkerToNulls(filePath);


	//auto with_bads = LR"(d:\PaboTa\47248\1Cv8.1CD)";
	//auto without_bads = LR"(d:\PaboTa\47248\old\1Cv8.1CD)";
	//auto targe_filename = LR"(d:\PaboTa\47248\result)";

	//IO::replaceBadsFromOtherFile(with_bads, without_bads, targe_filename);

	//int k = 1;
	//k = 0;

	//findNullsBlock();
	/*
	uint64_t start_offset = 949495159;
	auto fileName = LR"(d:\PaboTa\47240\Root\Мои резервные копии\Мои разделы\Мои разделы_inc_b1_s47_v1.tib.id-7429198B.[bitlocker@foxmail.com ].wiki)";
	IO::AcronisDecompress arcDecompress(fileName);
	auto targetFileName = LR"(f:\47240\2)";
	auto next_offset = arcDecompress.saveToFile(targetFileName, start_offset);

	*/
	/*
	auto fileName = LR"(e:\46950\Free_space.bin)";
	auto file_ptr = IO::makeFilePtr(fileName);
	file_ptr->OpenRead();
	IO::File target_file(LR"(d:\PaboTa\46950\test.mp4)");
	target_file.OpenCreate();
	uint64_t start_offset = 10675814400;
	RAW::GP_Analyzer gp_analyzer(file_ptr);
	gp_analyzer.AnalyzeGP(target_file, start_offset);

	
	auto fileName = LR"(f:\47301\!Problem\bases1\7-ка\Флоремикс 2019\1SBUKR\DT4419.DBF)";
	IO::fixDBF(fileName);
	*/


	//add_service(LR"(d:\incoming\46907\xor_without_SA.bin)", LR"(d:\incoming\46907\result.bin)");
	//IO::calcNullsForFolder(LR"(d:\PaboTa\46950\sample\)" , 131072);
	//RAW::testOLE();
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
