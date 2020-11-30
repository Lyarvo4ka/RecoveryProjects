#pragma once

#include <map>
#include "io/file.h"

#include <filesystem>
namespace fs = std::filesystem;

class SignatureTester
{
	//std::map<
public:
	void testSignature(IO::path_string & filename)
	{
		IO::File file(filename);
		file.OpenRead();

		fs::path filepath(filename);
		auto ext = filepath.extension();


		
		
	}
};
