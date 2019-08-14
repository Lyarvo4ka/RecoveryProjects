#pragma once

#include "AbstractRaw.h"

namespace RAW
{
	const uint32_t END_OF_CHAIN =	 0xFFFFFFFE;
	const uint32_t FREE_SECTORS =	 0xFFFFFFFF;
	const uint32_t FAT_SECTORS  =	 0xFFFFFFFD;
	const uint32_t DIFSECT		=	 0xFFFFFFFC;

	const uint32_t FAT_VAL_SIZE = 4;
	const uint32_t FIRST_109_SECTORS = 109;


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
		uint32_t number_of_msat_sectors;

		/* The first 109 MSAT sector identifiers
		 * Consists of 436 bytes
		 */
		uint32_t msat[FIRST_109_SECTORS];
	};


	//1. ssat_sector_identifier

	class Sector
	{
		DataArray data_;
		uint64_t number_ = 0;
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
		void setSectorNumber(const uint64_t sector_number)
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

	};

	class DIF_Sector
	{
		DataArray sector_data_;
	public:
		DIF_Sector(const uint32_t sector_size = default_sector_size)
			: sector_data_(sector_size)
		{

		}
		ByteArray data()
		{
			return sector_data_.data();
		}
		uint32_t size() const
		{
			return sector_data_.size() - FAT_VAL_SIZE;
		}
		uint32_t next_chain() const
		{
			return sector_data_.data()[sector_data_.size() - 1];
		}
	};



	class OLE_FAT
	{
		std::vector<uint32_t> table_;
	public:
		void addSector( Sector & sector)
		{
//			std::transform(std::begin()table_.begin(), table_.end(),)
			for (auto i = 0; i < sector.size(); i += FAT_VAL_SIZE)
				table_.push_back(sector.data()[i]);
		}
	};

	void testOLE()
	{
		auto filename = LR"(d:\ole_test\2012-08-05-10-29-0000344.doc)";
		//	auto filename = LR"(d:\ole_test\bigdoc.doc)";

		OLE_FAT fat_table;

		IO::File file(filename);
		file.OpenRead();
		IO::DataArray data_array(default_sector_size);
		file.ReadData(data_array);

		auto ole_header = (RAW::OLE_header*)data_array.data();
		for (auto iSector = 0; iSector < FIRST_109_SECTORS; ++iSector)
		{
			Sector sector(ole_header->msat[iSector]);
			file.setPosition(sector.offset());
			file.ReadData(sector.data(), sector.size());
			fat_table.addSector(sector);
		}
		if (ole_header->dif_sector_start != END_OF_CHAIN)
		{
			
			// read using DIF sectors
		}
	}

	class OleRAW
		: public DefaultRaw
	{
	public:
		OleRAW(IO::IODevicePtr device)
			: DefaultRaw(device)
		{

		}

		uint64_t SaveRawFile(File& target_file, const uint64_t start_offset)   override
		{
			IO::DataArray sector(default_sector_size);

			this->setPosition(start_offset);
			this->ReadData(sector);

			OLE_header* pOLE_header = (OLE_header*)sector.data();



			return 0;
		}
		bool Specify(const uint64_t start_offset)
		{
			return true;
		}



	};

}