#pragma once

#include "fs.h"
#include <iostream>

#include <locale>
#include <codecvt>
#include <boost/algorithm/string.hpp>
#include <algorithm>

#include "iodevice.h"

namespace IO
{
	using compare_ByteArray_func = std::function<bool(ByteArray, uint32_t size)>;

	class DataFinder
	{
		IODevicePtr device_;
		uint32_t search_size_ = 1;
		uint32_t data_size_ = default_block_size;
		uint64_t pos_ = 0;
	public:
		std::function<bool(ByteArray, uint32_t size)> cmp_func_ = nullptr;
		DataFinder(IODevicePtr & device)
			: device_(device)
		{
			auto func_ptr = std::bind(&DataFinder::returnFalse, this, std::placeholders::_1, std::placeholders::_2);
		}
		uint64_t getFoundPosition()const
		{
			return pos_;
		}
		bool returnFalse(ByteArray data, uint32_t size)
		{
			return false;
		}
		void setSearchSize(uint32_t search_size)
		{
			search_size_ = search_size;
		}
		bool compareData(ByteArray data, uint32_t size)
		{
			return cmp_func_(data, size);
		}
		bool findFromCurrentToEnd(uint64_t curr_offset)
		{
			uint64_t offset = curr_offset;
			uint32_t bytesToRead = 0;
			DataArray buffer(data_size_);

			while (offset < device_->Size())
			{
				bytesToRead = calcBlockSize(offset, device_->Size(), buffer.size());
				if (bytesToRead == 0)
					break;

				device_->setPosition(offset);
				device_->ReadData(buffer.data(), bytesToRead);

				for (uint32_t i = 0; i < bytesToRead; i += search_size_)
				{
					ByteArray pData = buffer.data() + i;
					if (compareData(pData, bytesToRead - i))
					{
						pos_ = offset + i;
						return true;
					}
				}


				offset += bytesToRead;
			}

		}

	};

	class Finder
	{
	private:
		DirectoryNode::Ptr rootFolder_;
		path_list list_ext_;
		path_list files_;

	public:
		void add_extension(path_string ext)
		{
			list_ext_.push_back(ext);
		}
		Finder()
		{

		}
		void FindFiles(const path_string& folder)
		{
			this->rootFolder_ = DirectoryNode::CreateDirectoryNode(folder);
			Find(rootFolder_);
		}
		void FindFiles(const path_string& folder, const path_list& list_extensions)
		{
			this->list_ext_ = list_extensions;
			FindFiles(folder);
		}
		DirectoryNode::Ptr getRoot()
		{
			return rootFolder_;
		}
		path_list getFiles() const
		{
			return files_;
		}
		void printFiles(DirectoryNode::Ptr current_folder)
		{

			if (auto file = current_folder->getFirstFile())
			{
				auto folder_path = current_folder->getFullPath();
				do
				{
					wprintf_s(L"%s\n", file->getFullPath().c_str());
					auto full_path = file->getFullPath();

					files_.push_back(full_path);
					file = current_folder->getNextFile();
				} while (file != nullptr);
			}
			if (auto folder = current_folder->getFirstFolder())
			{
				do
				{
					printFiles(folder);
					folder = current_folder->getNextFolder();
				} while (folder != nullptr);
			}

		}
		void printAll()
		{
			if (!rootFolder_)
				return;

			wprintf_s(L"Root: %s\n", rootFolder_->getFullPath().c_str());
			printFiles(rootFolder_);

		}
	private:
/*
		void Rename_wave(const IO::path_string& filePath)
		{
			auto test_file = IO::makeFilePtr(filePath);
			const uint32_t date_offset = 0x154;
			const uint32_t date_size = 18;
			const uint32_t check_size = 0x16A;
			char buff[date_size + 1];
			ZeroMemory(buff, date_size + 1);

			if (test_file->Open(IO::OpenMode::OpenRead))
			{
				if (check_size > test_file->Size())
					return;

				test_file->setPosition(date_offset);
				test_file->ReadData((ByteArray)buff, date_size);


				if (buff[0] == '2' && buff[1] == '0')
				{

					std::string date_string(buff);
					std::replace(date_string.begin(), date_string.end(), ' ', '-');
					std::replace(date_string.begin(), date_string.end(), '.', '-');
					std::replace(date_string.begin(), date_string.end(), ':', '-');
					date_string.insert(10, 1, '-');

					IO::path_string new_date_str(date_string.begin(), date_string.end());

					fs::path src_path(filePath);
					auto folder_path = src_path.parent_path().generic_wstring();
					auto only_name_path = src_path.stem().generic_wstring();
					auto ext = src_path.extension().generic_wstring();
					auto new_name = folder_path + L"//" + new_date_str + L"-" + only_name_path + ext;
					test_file->Close();
					try
					{
						fs::rename(filePath, new_name);
					}
					catch (const fs::filesystem_error& e)
					{
						std::cout << "Error: " << e.what() << std::endl;
					}

				}


			}
		}
*/
		void Find(DirectoryNode::Ptr folder_node)
		{
			path_string current_folder = folder_node->getFullPath();
			path_string mask_folder(addBackSlash(current_folder) + mask_all);
			WIN32_FIND_DATA findData = { 0 };

			HANDLE hFindFile = FindFirstFile(mask_folder.c_str(), &findData);
			if (hFindFile != INVALID_HANDLE_VALUE)
			{
				do
				{
					path_string current_file(findData.cFileName);

					if (isOneDotOrTwoDots(current_file))
						continue;

					if (isDirectoryAttribute(findData))
					{
						path_string new_folder = findData.cFileName;

						auto new_folder_node = DirectoryNode::CreateDirectoryNode(new_folder);

						folder_node->AddDirectory(new_folder_node);
						Find(new_folder_node);
					}

					// Than it's must be file
					if (!isDirectoryAttribute(findData))
					{
						path_string file_name = findData.cFileName;
						fs::path tmp_path(file_name);
						path_string file_ext = tmp_path.extension().wstring();

						auto full_name = addBackSlash(current_folder) + file_name;


						if (list_ext_.empty())
						{
							folder_node->AddFile(file_name);
							files_.push_back(full_name);
						}
						else
							if (isPresentInList(file_ext, this->list_ext_))
							{

								//TestEndJpg(full_name);
								//zbk_rename(full_name);
								//removeNullsFromEndFile(full_name, 2048);
								//addDateToFile(full_name);
								//testSignatureMP4(full_name);
								//Rename_wave(full_name);
								files_.push_back(full_name);

								folder_node->AddFile(file_name);
							}
					}

					//SearchFiles(
				} while (FindNextFile(hFindFile, &findData));

				FindClose(hFindFile);

			}


		}



	};
}