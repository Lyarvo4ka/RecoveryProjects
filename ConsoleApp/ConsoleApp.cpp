// ConsoleApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "raw/oleraw.h"
#include "io/functions.h"

const char s_ftyp[] = "ftyp";
const char s_moov[] = "moov";
const char s_mdat[] = "mdat";
const char s_wide[] = "wide";
const char s_skip[] = "skip";
const char s_free[] = "free";
const char s_pnot[] = "pnot";
const char s_prfl[] = "prfl";
const char s_mvhd[] = "mvhd";
const char s_clip[] = "clip";
const char s_trak[] = "trak";
const char s_udta[] = "udta";
const char s_ctab[] = "ctab";
const char s_cmov[] = "cmov";
const char s_rmra[] = "rmra";
const char s_uuid[] = "uuid";
const char s_meta[] = "meta";


inline bool isQtSignature(const uint8_t* qt_header, const uint32_t size)
{
	if (memcmp(qt_header, s_ftyp, size) == 0)
		return true;
	else if (memcmp(qt_header, s_moov, size) == 0)
		return true;
	else if (memcmp(qt_header, s_mdat, size) == 0)
		return true;
	else if (memcmp(qt_header, s_wide, size) == 0)
		return true;


	return false;
}

inline void testSignatureMP4(const IO::path_string& filePath)
{
	auto test_file = IO::makeFilePtr(filePath);
	const uint32_t offset = 4;
	const uint32_t header_size = 4;
	uint8_t buff[header_size];
	ZeroMemory(buff, header_size);
	if (test_file->Open(IO::OpenMode::OpenRead))
	{
		if (test_file->Size() >= header_size)
		{
			test_file->setPosition(offset);
			test_file->ReadData(buff, header_size);
			test_file->Close();

			if (isQtSignature(buff, header_size) == false)
			{
				auto new_file_name = filePath + L".bad_file";
				std::wcout << new_file_name.c_str() << std::endl;
				fs::rename(filePath, new_file_name);
			}
		}

	}


}


void testMP4_signature(const IO::path_string& folder)
{
	IO::Finder finder;
	finder.add_extension(L".mov");
	finder.add_extension(L".mp4");
	finder.add_extension(L".3gp");
	finder.add_extension(L".3gpp");
	finder.add_extension(L".m4a");
	//finder.add_extension(L".msdoc");
	finder.FindFiles(folder);
	auto fileList = finder.getFiles();

	for (auto& theFile : fileList)
	{
		try {
			std::wcout << theFile.c_str();
			testSignatureMP4(theFile);
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


int main()
{
	//add_service(LR"(d:\incoming\46907\xor_without_SA.bin)", LR"(d:\incoming\46907\result.bin)");
	IO::calcNullsForFolder(LR"(d:\PaboTa\46950\sample\)" , 131072);
	//RAW::testOLE();

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
