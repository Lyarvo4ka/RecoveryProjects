#pragma once

#include "io/finder.h"


namespace IO
{

	inline void calcNullsForFile(const path_string& file_name, uint32_t block_size)
	{
		IO::File src_file(file_name);
		src_file.OpenRead();

		IO::path_string entropy_name = file_name + L".nulls";


		IO::File entrory_file(entropy_name);
		entrory_file.OpenCreate();

		IO::DataArray data_array(block_size);

		uint32_t bytesRead = 0;
		DWORD byteWritten = 0;
		uint32_t number_nulls = 0;
		bool bResult = false;
		DWORD cluster_number = 0;



		while (true)
		{
			try {
				bResult = true;
				bytesRead = src_file.ReadData(data_array);
			}
			catch (Error::IOErrorException ex)
			{
				std::cout << "Cougth exception" << ex.what() << std::endl;
				bResult = false;
			}
			if (!bResult)
				break;

			number_nulls = 0;
			for (uint32_t i = 0; i < bytesRead; ++i)
			{
				if (data_array.data()[i] == 0)
					++number_nulls;
			}
			std::string write_string(boost::lexical_cast<std::string>(number_nulls));
			write_string.append("\r\n");

			entrory_file.WriteData((IO::ByteArray)write_string.data(), (DWORD)write_string.size());
			++cluster_number;

		}
	}
	inline void calcNullsForFolder(const path_string& folder, uint32_t block_size)
	{
		Finder finder;
		finder.FindFiles(folder);
		auto files = finder.getFiles();
		for (auto theFile : files)
		{
			calcNullsForFile(theFile, block_size);
		}
	}
}
