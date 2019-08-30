#pragma once

#include "io/constants.h"

namespace OLE
{
	const uint32_t END_OF_CHAIN = 0xFFFFFFFE;
	const uint32_t FREE_SECTORS = 0xFFFFFFFF;
	const uint32_t FAT_SECTORS = 0xFFFFFFFD;
	const uint32_t DIFSECT = 0xFFFFFFFC;

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


	const uint32_t DIR_NAME_MAXSIZE = 32 * 2;

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

}
