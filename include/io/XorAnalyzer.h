#pragma once

#include <vector>

#include "utility.h"
#include "IODevice.h"

#pragma warning(disable:4251)
namespace IO
{
	const int BYTE_SIZE = 256;

	uint64_t NumBytesForBlock(DWORD block_size)
	{
		return (uint64_t)(sizeof(WORD) * BYTE_SIZE) * block_size;
	}

	int chunksPrerBlock(uint64_t block_size)
	{
		return (int)(block_size / BS::GB);
	}
	class ByteCount
	{
	public:
		ByteCount(void)
		{
			bytes_ = new WORD[BYTE_SIZE];
		}
		~ByteCount(void)
		{
			delete bytes_;
		}
		void add(BYTE _byte)
		{
			bytes_[_byte]++;
		}
		BYTE getMax()
		{
			BYTE popularByte = 0;
			WORD dwMax = bytes_[0];
			for (BYTE i = 1; i < UCHAR_MAX; ++i)
			{
				if (bytes_[i] > dwMax)
				{
					popularByte = i;
					dwMax = bytes_[i];
				}
			}
			return popularByte;
		}
	private:
		WORD* bytes_;
	};


	class XorAnalyzer
	{
		File dump_file_;

	public:
		XorAnalyzer(const path_string& dump_file)
			:dump_file_(dump_file)
		{
		}
		~XorAnalyzer(void)
		{
		}
		void Analize(const path_string& result_xor, DWORD xor_size)
		{
			dump_file_.OpenRead();

			if (xor_size <= 0)
			{
				printf("Error block size must be more than 0.\r\n");
				return;
			}

			File target_file(result_xor);
			target_file.OpenCreate();


			ULONGLONG needMemory = NumBytesForBlock(xor_size);

			int chunks = chunksPrerBlock(needMemory);
			DWORD chunk_size = xor_size / chunks;

			DWORD buffer_size = 0;
			DWORD xor_offset = 0;
			//DWORD bytesRead = 0;
			DWORD bytesWritten = 0;
			LONGLONG read_offset = 0;

			BYTE* xor_data = new BYTE[xor_size];

			for (auto nChunk = 0; nChunk < chunks; ++nChunk)
			{
				printf("Start analyzing #%d chunk of %dn\r\n", nChunk, chunks);
				xor_offset = chunk_size * nChunk;
				if (nChunk == chunks - 1)
					buffer_size = xor_size - xor_offset;
				else
					buffer_size = getChunckBufferSize(chunk_size, nChunk, xor_size);
				read_offset = xor_offset;
				ByteCount* pByteCounts = new ByteCount[buffer_size];
				DataArray buffer(buffer_size);


				while (read_offset < dump_file_.Size())
				{
					dump_file_.setPosition(read_offset);
					dump_file_.ReadData(buffer);


					if (IO::isNot00orFF(buffer.data(), buffer.size()))
					{
						for (DWORD nByte = 0; nByte < buffer.size(); ++nByte)
							pByteCounts[nByte].add(buffer[nByte]);
					}

					read_offset += xor_size;
				}

				for (DWORD nByte = 0; nByte < buffer_size; ++nByte)
					xor_data[nByte + xor_offset] = pByteCounts[nByte].getMax();

				ByteArray pWriteData = xor_data + xor_offset;

				target_file.WriteData(pWriteData, buffer_size);

				delete[] pByteCounts;
			}

			delete[] xor_data;

			dump_file_.Close();
		}

		DWORD getChunckBufferSize(DWORD chunck_size, int nChunck, DWORD xor_size)
		{
			DWORD buffer_size = 0;
			DWORD xor_offset = chunck_size * nChunck;
			if ((xor_size - xor_offset) < chunck_size)
				buffer_size = xor_size - xor_offset;
			else
				buffer_size = chunck_size;
			return buffer_size;
		}


	};



}