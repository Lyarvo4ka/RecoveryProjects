#pragma once

#include "AbstractRaw.h"
#include "factories.h"

#include <algorithm>

#include "io/finder.h"

namespace RAW
{
	const uint32_t END_OF_CHAIN =	 0xFFFFFFFE;
	const uint32_t FREE_SECTORS =	 0xFFFFFFFF;
	const uint32_t FAT_SECTORS  =	 0xFFFFFFFD;
	const uint32_t DIFSECT		=	 0xFFFFFFFC;

	const uint32_t FAT_VAL_SIZE = 4;
	const uint32_t FIRST_109_SECTORS = 109;

	const uint32_t OLE_header_size = 512;
	const uint32_t DEFAULT_DIR_ENTRY_SIZE = 128;
	struct OLE_header
	{
		/* The magic signature of the OLECF compound file
		 * Consists of 8 bytes
		 * This should be 0xd0 0xcf 0x11 0xe0 0xa1 0xb1 0x1a 0xe1
		 * or 0x0e 0x11 0xfc 0x0d 0xd0 0xcf 0x11 0x0e
		 */
		uint8_t signature[8];

		/* The class identifier
		 * Consists of 16 bytes
		 */
		uint8_t class_identifier[16];

		/* The minor version of the OLECF format used
		 * Consists of 2 bytes
		 */
		uint16_t minor_format_version;

		/* The major verion of the OLECF format used
		 * also used refered to as DLL version
		 * Consists of 2 bytes
		 */
		uint16_t major_format_version;

		/* The byte order within the file
		 * 0xfffe indicates little endian
		 * 0xfeff indicates big endian
		 * Consists of 2 bytes
		 */
		uint16_t byte_order;

		/* The sector (block) size within the file
		 * also refered to as sector shift
		 * Consists of 2 bytes
		 * default is 9 indicating 2^9 = 512
		 */
		uint16_t sector_shift;

		/* The short (mini) sector (block) size within the file
		 * also refered to as mini sector shift
		 * Consists of 2 bytes
		 * default is 6 indicating 2^6 = 64
		 */
		uint16_t minifat_sector_shift;

		/* Reserved
		 * Consists of 2 bytes
		 * Should contain 0 bytes
		 */
		uint8_t reserved1[2];

		/* Reserved
		 * Consists of 4 bytes
		 * Should contain 0 bytes
		 */
		uint8_t reserved2[4];

		/* Reserved
		 * Consists of 4 bytes
		 * Should contain 0 bytes
		 */
		uint8_t reserved3[4];

		/* The number of SAT sectors
		 * Consists of 4 bytes
		 */
		uint32_t number_of_fat_sectors;

		/* The sector identifier of the sector that contains the root directory
		 * Consists of 4 bytes
		 */
		uint32_t root_directory_sector_start;

		/* Signature used for transactioning
		 * Consists of 4 bytes
		 */
		uint8_t transactioning_signature[4];

		/* The minimum data size in a sector stream
		 * otherwise a short-sector stream is used
		 * Consists of 4 bytes
		 */
		uint32_t sector_stream_minimum_data_size;

		/* The sector identifier of the sector that contains the first SSAT
		 * Consists of 4 bytes
		 */
		uint32_t minifat_sector_start;

		/* The number of SSAT sectors
		 * Consists of 4 bytes
		 */
		uint32_t number_of_minifat_sectors;

		/* The sector identifier of the sector that contains the first MSAT
		 * Consists of 4 bytes. If file is smaller than 7 Mb the value must 0xFFFFFFFE(END_OF_CHAIN)
		 */
		uint32_t dif_sector_start;

		/* The number of MSAT sectors
		 * Consists of 4 bytes
		 */
		uint32_t number_of_dif_sectors;

		/* The first 109 MSAT sector identifiers
		 * Consists of 436 bytes
		 */
		uint32_t msat[FIRST_109_SECTORS];
	};


	const uint32_t DIR_NAME_MAXSIZE = 32*2;

#pragma pack( 1 )
	struct OleDirectoryEntry
	{
		/* The directory name
		 */
		uint8_t name[DIR_NAME_MAXSIZE];

		/* The size of the directory name
		 */
		uint16_t name_size;

		/* The type
		 */
		uint8_t type;

		/* Value taken from DECOLOR
		*/
		uint8_t bFlag;

		uint32_t leftSID;
		uint32_t rigthSID;
		uint32_t childSID;

		uint8_t guid[16];
		uint32_t userFlags;
		/* Creation time
		 */
		uint64_t creation_time;

		/* Modification time
		 */
		uint64_t modification_time;

		/* The sector identifier
		 */
		uint32_t sector_start;

		/* The size
		 */
		uint32_t size;

		uint16_t propType;


	};
#pragma pack()
	constexpr uint32_t sizeOfOleDirectoryEntry = sizeof(OleDirectoryEntry);


	class DIF_Sector_ref;

	class Sector
	{
		DataArray data_;
		uint64_t number_ = 0;
		bool valid_ = false;
	public:
		Sector(const uint32_t sector_number = 0 , const uint32_t sector_size = default_sector_size)
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
		void setValid(bool valid =true)
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
		uint32_t * data() const
		{
			return (uint32_t*)data_;
		}
		uint32_t size() const
		{
			return (size_ - FAT_VAL_SIZE) / FAT_VAL_SIZE;
		}
		uint32_t next_chain() const
		{
			auto val = (uint32_t *) (data_ + size_ - FAT_VAL_SIZE);
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
		void addSector( Sector & sector)
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
		OLE_header ole_header_ = {0};
		OLE_FAT ole_fat_table_;
		uint64_t file_size_ = 0;
		std::list<std::wstring> listEnries_;
	public:
		OLEReader(IO::IODevicePtr device , uint64_t offset = 0)
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
		uint64_t getFileSize() const
		{
			return file_size_;
		}
		uint64_t sectorToOffset(const Sector& sector)
		{
			return sector.offset() + OLE_header_size;
		}
		void readHeader(OLE_header &ole_header)
		{
			device_->setPosition(offset_);
			device_->ReadData((ByteArray)&ole_header, OLE_header_size);
		}

		void readSector(Sector & sector)
		{
			auto offset = sectorToOffset(sector) + offset_;
			if (offset >= device_->Size())
				sector.setValid(false);
			device_->setPosition(offset);
			device_->ReadData(sector.data(), sector.size());
			sector.setValid();
		}
		bool readAndAddSectorToFat(Sector & sector,OLE_FAT & ole_fat_table )
		{
			if (sector.number() == FREE_SECTORS)
				return false;
			readSector(sector);
			ole_fat_table_.addSector(sector);
			return true;
		}
		void readFatTable(OLE_FAT & ole_fat_table)
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

			while(true)
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

	void testOLE()
	{
		auto foldername = LR"(d:\incoming\46460\raw\)";

		io::Finder finder;
		finder.FindFiles(foldername);

		for (auto &fileName : finder.getFiles())
		{

			auto filename = LR"(d:\ole_test\2012-08-05-10-29-0000344.doc)";
			//auto filename = LR"(d:\ole_test\Assy.dft)";

			auto filePtr = IO::makeFilePtr(filename);
			filePtr->OpenRead();

			OLEReader oleReader(filePtr);
			oleReader.read();
			oleReader.readRoot();
			filePtr->Close();

			for (auto& valName : oleReader.getListEntries())
			{
				if (valName.compare(L"Workbook") == 0)
				{

					int k = 0;
					k = 1;
				}
			}

		}

	}

	class OleRAW
		: public DefaultRaw
	{
		uint64_t sizeToWrite = 0;
	public:
		OleRAW(IO::IODevicePtr device)
			: DefaultRaw(device)
		{

		}

		uint64_t SaveRawFile(File& target_file, const uint64_t start_offset)   override
		{
			if (sizeToWrite > 0)
				return appendToFile(target_file, start_offset, sizeToWrite);

			return 0;
		}
		bool Specify(const uint64_t start_offset)
		{

			OLEReader ole_reader(this->getDevice() , start_offset);
			ole_reader.read();
			sizeToWrite = ole_reader.getFileSize();
			if (sizeToWrite == 0)
				return false;
			return true;
		}



	};

	class OleRawFactory :
		public RawFactory
	{
	public:
		RawAlgorithm* createRawAlgorithm(IO::IODevicePtr device) override
		{
			return new OleRAW(device);
		}
	};

}