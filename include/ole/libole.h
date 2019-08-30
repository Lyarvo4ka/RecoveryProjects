#pragma once

#include "io/iodevice.h"
#include "oleconstants.h"


namespace OLE
{
	using namespace IO;


	class DIF_Sector_ref;

	class Sector
	{
		DataArray data_;
		uint64_t number_ = 0;
		bool valid_ = false;
	public:
		Sector(const uint32_t sector_number = 0, const uint32_t sector_size = default_sector_size)
			:data_(sector_size)
			, number_(sector_number)
		{

		}

		ByteArray data()
		{
			return data_.data();
		}
		void setNumber(const uint64_t sector_number)
		{
			number_ = sector_number;
		}
		uint64_t number() const
		{
			return number_;
		}
		uint64_t offset() const
		{
			return number() * data_.size();
		}
		uint32_t size() const
		{
			return data_.size();
		}
		void setValid(bool valid = true)
		{
			valid_ = valid;
		}
		bool isValid() const
		{
			return valid_;
		}

		DIF_Sector_ref toDifSector();


	};

	class DIF_Sector_ref
	{
		ByteArray data_;
		uint32_t size_;
	public:
		//DIF_Sector(const uint32_t sector_size = default_sector_size)
		//	: sector_data_(sector_size)
		//{

		//}
		DIF_Sector_ref(Sector& sector)
			: data_(sector.data())
			, size_(sector.size())
		{

		}
		uint32_t* data() const
		{
			return (uint32_t*)data_;
		}
		uint32_t size() const
		{
			return (size_ - FAT_VAL_SIZE) / FAT_VAL_SIZE;
		}
		uint32_t next_chain() const
		{
			auto val = (uint32_t*)(data_ + size_ - FAT_VAL_SIZE);
			return  *val;
		}
	};

	DIF_Sector_ref Sector::toDifSector()
	{
		return DIF_Sector_ref(*this);
	}


	class OLE_FAT
	{
		std::vector<uint32_t> table_;
	public:
		void addSector(Sector& sector)
		{
			//			std::transform(std::begin()table_.begin(), table_.end(),)
			for (auto i = 0; i < sector.size(); i += FAT_VAL_SIZE)
			{
				auto pVal = (uint32_t*)(sector.data() + i);
				table_.push_back(*pVal);
			}
		}
		uint32_t getNumberSectorsFromFATTable()
		{
			uint32_t file_size = 0;
			auto last = table_.size() - 1;
			if (table_.at(last) == FREE_SECTORS)
			{
				auto findIter = std::find_if_not(std::rbegin(table_), std::rend(table_), [](int i) {return i == FREE_SECTORS; });
				if (findIter != table_.rend())
				{
					auto numberOfSector = std::distance(std::begin(table_), findIter.base());
					return numberOfSector;
				}
			}
			return table_.size();
		}
		uint32_t getNextSector(const uint32_t current_sector) const
		{
			if (current_sector >= table_.size())
				return END_OF_CHAIN;
			return table_[current_sector];
		}

	};

	class OLEReader
	{
		IO::IODevicePtr device_;
		uint64_t offset_ = 0;
		OLE_header ole_header_ = { 0 };
		OLE_FAT ole_fat_table_;
		uint64_t file_size_ = 0;
		std::list<std::wstring> listEnries_;
	public:
		OLEReader(IO::IODevicePtr device, uint64_t offset = 0)
			:device_(device)
			, offset_(offset)
		{}
		void read()
		{
			readHeader(ole_header_);
			readFatTable(ole_fat_table_);
			auto sectors_count = ole_fat_table_.getNumberSectorsFromFATTable();
			file_size_ = (uint64_t)sectors_count * OLE_header_size + OLE_header_size;
			int k = 0;
			k = 1;

		}
		bool findKeywordEntry(std::wstring_view entry_keyword)
		{
			auto findIter = std::find(std::begin(listEnries_), std::end(listEnries_), entry_keyword);
			return findIter != listEnries_.end();
		}
		uint64_t getFileSize() const
		{
			return file_size_;
		}
		uint64_t sectorToOffset(const Sector& sector)
		{
			return sector.offset() + OLE_header_size;
		}
		void readHeader(OLE_header& ole_header)
		{
			device_->setPosition(offset_);
			device_->ReadData((ByteArray)& ole_header, OLE_header_size);
		}

		void readSector(Sector& sector)
		{
			auto offset = sectorToOffset(sector) + offset_;
			if (offset < device_->Size())
			{
				device_->setPosition(offset);
				device_->ReadData(sector.data(), sector.size());
				sector.setValid();
			}
			else
				sector.setValid(false);

		}
		bool readAndAddSectorToFat(Sector& sector, OLE_FAT& ole_fat_table)
		{
			if (sector.number() == FREE_SECTORS)
				return false;
			readSector(sector);
			ole_fat_table_.addSector(sector);
			return true;
		}
		void readFatTable(OLE_FAT& ole_fat_table)
		{
			Sector sector;

			for (auto iSector = 0; iSector < FIRST_109_SECTORS; ++iSector)
			{
				sector.setNumber(ole_header_.msat[iSector]);
				if (!readAndAddSectorToFat(sector, ole_fat_table))
					break;
			}
			Sector difSector;
			if (ole_header_.dif_sector_start != END_OF_CHAIN)
			{
				uint32_t sector_number = ole_header_.dif_sector_start;
				for (auto iDifSector = 0; iDifSector < ole_header_.number_of_dif_sectors; ++iDifSector)
				{
					difSector.setNumber(sector_number);
					readSector(difSector);
					auto dif_sector = difSector.toDifSector();
					auto num_dif_sector = dif_sector.size();
					for (auto iSector = 0; iSector < num_dif_sector; ++iSector)
					{
						auto number = dif_sector.data() + iSector;
						auto offset = sectorToOffset(*number);
						if (offset >= device_->Size())
						{
							int k = 1;
							k = 1;
						}
						sector.setNumber(*number);
						readAndAddSectorToFat(sector, ole_fat_table);
					}
					sector_number = dif_sector.next_chain();
					// read DIF sector
				}
			}
		}
		const std::list<std::wstring>& getListEntries() const
		{
			return listEnries_;
		}
		void readRoot()
		{
			Sector sector(ole_header_.root_directory_sector_start);

			while (true)
			{
				readSector(sector);
				if (!sector.isValid())
					break;

				for (auto i = 0; i < sector.size(); i += DEFAULT_DIR_ENTRY_SIZE)
				{
					auto pDirData = sector.data() + i;
					OleDirectoryEntry* pOleDirEntry = (OleDirectoryEntry*)pDirData;
					auto ptrWchar = (wchar_t*)pOleDirEntry->name;
					std::wstring name(ptrWchar);
					listEnries_.push_back(name);
					//std::wcout << name << std::endl;
				}

				auto next_sector = ole_fat_table_.getNextSector(sector.number());
				if (next_sector == END_OF_CHAIN)
					break;
				sector.setNumber(next_sector);

			}



		}
	};



}
