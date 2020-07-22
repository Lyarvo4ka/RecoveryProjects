#pragma once

#include "io/ioengine.h"
#include "gmock/gmock.h"

using namespace IO;

class MockIOEngine
	: public IOEngine
{
public:
	MOCK_METHOD(IOErrorsType , OpenRead, (const path_string&), (override));
	MOCK_METHOD(IOErrorsType , OpenWrite, (const path_string& path) , (override));
	MOCK_METHOD(IOErrorsType , Create , (const path_string& path) , (override));
	MOCK_METHOD(void , Close , () , (override));
	MOCK_METHOD(bool , isOpen , () , (const , override));
	MOCK_METHOD(void , setPosition, (uint64_t position) , (override));
	MOCK_METHOD(uint64_t , getPosition, () , ( const , override));
	MOCK_METHOD(IOErrorsType , Read , (ByteArray data, const uint32_t read_size, uint32_t& bytes_read) , (override));
	MOCK_METHOD(IOErrorsType , Write , (ByteArray data, const uint32_t write_size, uint32_t& bytes_written) , (override));
	MOCK_METHOD(IOErrorsType , SetFileSize , (uint64_t new_size) , (override));
	MOCK_METHOD(IOErrorsType , readFileSize , (uint64_t& file_size) , (override));
	//MOCK_METHOD(void , setTranserSize , (const uint32_t transfer_size) , (override));
	//MOCK_METHOD(uint32_t , getTranferSize , () , ( const , override));

	MOCK_METHOD(IOErrorsType, read_data , (ByteArray data, uint32_t read_size, uint32_t& bytes_read), (override) );
	MOCK_METHOD(IOErrorsType, write_data , (ByteArray data, uint32_t write_size, uint32_t& bytes_written) , (override));

	MOCK_METHOD( BOOL , read_device , (HANDLE& hDevice, ByteArray data, const uint32_t bytes_to_read, uint32_t& bytes_read) , (override));
	MOCK_METHOD( BOOL , write_device , (HANDLE& hDevice, ByteArray data, const uint32_t bytes_to_write, uint32_t& bytes_written) , (override));

	IOErrorsType base_Read(ByteArray data, const uint32_t read_size, uint32_t& bytes_read)
	{
		return IOEngine::Read(data , read_size , bytes_read);
	}
	IOErrorsType base_Write(ByteArray data, const uint32_t read_size, uint32_t& bytes_read)
	{
		return IOEngine::Write(data, read_size, bytes_read);
	}
	IOErrorsType base_ReadOrWriteData(ByteArray data, const uint32_t read_size, uint32_t& bytes_read, enReadWrite read_write)
	{
		return IOEngine::ReadOrWriteData(data, read_size, bytes_read, read_write);
	}
};
