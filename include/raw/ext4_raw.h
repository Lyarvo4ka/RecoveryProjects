#pragma once

#include "io/iodevice.h"
#include "AbstractRaw.h"

namespace RAW
{

	const uint16_t EXTENT_HEADER_MAGIC = (uint16_t)0xF30A;
	
	const uint16_t INODE_SIZE = 256;

#pragma pack(push)
#pragma pack(1)

	typedef struct _ext4_extent {
		uint32_t block;		//	First file block number that this extent covers.
		uint16_t len;
		uint16_t start_hi;	//	Upper 16-bits of the block number to which this extent points.
		uint32_t start_lo;	//	Lower 32-bits of the block number to which this extent points.
		uint64_t PysicalBlock(void) { return ((uint64_t)start_lo) | (((uint64_t)start_hi) << 32); }
	} ext4_extent;

	typedef struct _ext4_extent_idx {
		uint32_t block;		//	This index node covers file blocks from 'block' onward.
		uint32_t leaf_lo;		//	Lower 32-bits of the block number of the extent node that is the next level lower in the tree. 
							//	The tree node pointed to can be either another internal node or a leaf node, described below.
		uint16_t leaf_hi;		//	Upper 16-bits of the previous field.
		uint16_t unused;
		uint64_t PysicalBlock(void) { return ((uint64_t)leaf_lo) | (((uint64_t)leaf_hi) << 32); }
	} ext4_extent_idx;


	typedef struct _ext4_extent_header {
		uint16_t magic;
		uint16_t entries;
		uint16_t max;
		uint16_t depth;
		uint32_t generation;
	} ext4_extent_header;

	typedef struct _EXTENT_BLOCK {
		ext4_extent_header header;
		union {
			ext4_extent extent[1];
			ext4_extent_idx extent_index[1];
		};
	} EXTENT_BLOCK;


	struct ext4_inode
	{
		uint8_t skip;
		EXTENT_BLOCK extent_block;
	};

	class ExtentStruct
	{
		DataArray block_;
		bool bValid_ = false;
		uint64_t block_num_ = 0;
	public:
		ExtentStruct(const uint32_t block_size)
			: block_(block_size)
		{

		}
		void copyData(ByteArray source_data)
		{
			memcpy(block_.data(), source_data, block_.size());
			bValid_ = true;
		}
		bool isValid()
		{
			return bValid_;
		}
		EXTENT_BLOCK* getExtentBlock() const
		{
			return (EXTENT_BLOCK*)block_.data();
		}
		void setBlockNumber(uint64_t block_num)
		{
			block_num_ = block_num;
		}
		uint64_t getBlockNumber() const
		{
			return block_num_;
		}
	};

	class ext4_raw
		: public SpecialAlgorithm
	{
		IODevicePtr device_;
		uint64_t volume_offset_ = 0;
		uint32_t block_size_ = 4096;
		uint16_t max_extents_in_block_ = 0;
		uint64_t value_to_cmp_ = 0;
	public:
		ext4_raw(IODevicePtr device)
			: device_(device)
		{
			max_extents_in_block_ = (block_size_ - sizeof(ext4_extent_header)) / sizeof(ext4_extent);
		}
		uint64_t Execute(const uint64_t inode_offset, const path_string target_folder) override
		{
			if (!device_->isOpen())
				return 0;

			//DataArray inode(256);
			//device_->setPosition(inode_offset);
			//device_->ReadData(inode.data(), inode.size());
			//EXTENT_BLOCK *pExtBlock = (EXTENT_BLOCK*)(inode.data());

			//auto ext_offset = pExtBlock->extent_index->PysicalBlock();
			
			//DataArray next_inode(4096);
			//device_->setPosition(inode_offset);
			//device_->ReadData(next_inode.data(), next_inode.size());
			//EXTENT_BLOCK *pNext = (EXTENT_BLOCK*)(next_inode.data());

			//int k = 1;
			//k = 2;
			auto ext_offset = inode_offset / 4096;
			uint64_t inode_block = ext_offset/* / 4096*/;
			File target_file(target_folder);
			target_file.OpenWrite();


			saveToFile(inode_block , target_file);
			//auto inode = read_inode(inode_offset);
			//ext4_inode * pInode = (ext4_inode*)inode.data();
			//if (pInode->extent_block.header.magic == EXTENT_HEADER_MAGIC)
			//{
			//}
			return 0;
		}

		DataArray readInode(const uint64_t inode_offset)
		{
			DataArray inode(INODE_SIZE);
			device_->setPosition(inode_offset);
			device_->ReadData(inode.data(), inode.size());
			return inode;
		}


		// if (first_offset == size_to_cmp)
		bool firstExtentOffsetEqualTo(ByteArray data, uint32_t size)
		{
			EXTENT_BLOCK* extent_block = (EXTENT_BLOCK*)(data);
			if (isValidExtentWithNullDepth(*extent_block))
			{
				uint64_t first_offset = (uint64_t)extent_block->extent[0].block * block_size_;
				if (first_offset == value_to_cmp_)
					return true;
			}
			return false;
		}

		ExtentStruct findExtentEqualToSize(uint64_t block_start, uint64_t size_to_cmp)
		{
			uint64_t offset = block_start * block_size_;
			uint32_t bytesToRead = 0;
			value_to_cmp_ = size_to_cmp;	// ?????

			ExtentStruct block_struct(block_size_);

			DataFinder data_finder(device_);
			data_finder.setSearchSize(block_size_);
			auto func_ptr = std::bind(&ext4_raw::firstExtentOffsetEqualTo, this, std::placeholders::_1, std::placeholders::_2);
			data_finder.compareFunctionPtr_ = func_ptr;
			if (data_finder.searchÑircle(offset))
			{
				auto extent_pos = data_finder.getFoundPosition();
				DataArray ext_block(block_size_);
				device_->setPosition(extent_pos);
				device_->ReadData(ext_block.data(), ext_block.size());
				block_struct.copyData(ext_block.data());
				uint64_t block_number = extent_pos / block_size_;
				block_struct.setBlockNumber(block_number);
				return block_struct;

			}
			return block_struct;
		}
		void searchExtends(uint64_t block_start)
		{
			uint64_t target_offset = 0;
			uint64_t current_block = block_start;

			auto current_size = calculateSize(current_block);

			std::cout << "Offset " << target_offset << " size " << current_size <<  std::endl;

			// search next extent
			while(true)
			{

				// to do: determine and search only in 1 Tb range
				target_offset += current_size;
				auto block_struct = findExtentEqualToSize(current_block, target_offset);
				if (!block_struct.isValid())
					break;
				
				auto extent_block = block_struct.getExtentBlock();
				current_block = block_struct.getBlockNumber();
				current_size = calculateSize(current_block);
				if (current_size == 0)
					break;
				std::cout << "Offset " << target_offset << " size " << current_size << std::endl;

				int k = 1;
				k = 2;
			} 


		}
		void readExtent(const uint64_t block_num, DataArray& buffer)
		{

			uint64_t extent_offset = volume_offset_ + block_num * block_size_;
			device_->setPosition(extent_offset);
			device_->ReadData(buffer.data(), buffer.size());
		}

		uint64_t getOffsetFromPhysicalBlock(const uint64_t physical_block)
		{
			return (volume_offset_ + physical_block * block_size_);
		}
		
		uint64_t calculateSize(const uint64_t block_num)
		{
			const uint16_t MIN_REQUIRE_ENTRIES = 2;

			DataArray extent(block_size_);
			readExtent(block_num, extent);

			EXTENT_BLOCK* extent_block = (EXTENT_BLOCK*)extent.data();

			if (!isValidExtent(*extent_block))
				return 0;

			if (extent_block->header.depth != 0)
				return 0;

			uint64_t extent_size = 0;

			if (extent_block->header.entries > MIN_REQUIRE_ENTRIES)
			{
				auto first_offset = getOffsetFromPhysicalBlock(extent_block->extent[0].block);

				auto last_extent = extent_block->extent[extent_block->header.entries - 1];

				auto last_offset = getOffsetFromPhysicalBlock(last_extent.block);
				auto last_size = last_extent.len;

				extent_size = last_offset + determineSize(last_size) - first_offset;
				
			}

			return extent_size;

		}
		
		bool isValidExtent(EXTENT_BLOCK& extent_block)
		{
			if ((extent_block.header.magic != EXTENT_HEADER_MAGIC) ||
				(extent_block.header.max != max_extents_in_block_) ||
				(extent_block.header.entries > max_extents_in_block_)) {
				return false;
			}
			return true;
		}
		bool isValidExtentWithNullDepth(EXTENT_BLOCK& extent_block)
		{
			if (isValidExtent(extent_block))
				if (extent_block.header.depth == 0)
					return true;
			return false;
		}
		void toResize(DataArray & data_array , uint32_t new_size)
		{
			if (data_array.size() < new_size) {
				data_array.resize(new_size);
			}

		}

		uint32_t determineSize(const uint16_t len)
		{
			return (len <= 0x8000)
				? (len * block_size_)
				: ((len - 0x8000) * block_size_);
		}

		uint64_t saveToFile(const uint64_t block_num, File &target_file)
		{
			if (!target_file.isOpen())
				return 0;

			DataArray buffer(block_size_);
			EXTENT_BLOCK *extent_block = (EXTENT_BLOCK *)buffer.data();

			uint64_t extent_offset = volume_offset_ + block_num * block_size_;
			device_->setPosition(extent_offset);
			device_->ReadData(buffer.data(),buffer.size());
			
			if (!isValidExtent(*extent_block))
				return 0;

			//std::vector<BYTE> data_buff;
			DataArray data_array(default_block_size);
			uint64_t offset = 0;
			uint32_t size = 0;
			if (extent_block->header.depth == 0) {
				for (int i = 0; i < extent_block->header.entries; i++) {
					offset = volume_offset_ + extent_block->extent[i].PysicalBlock() * block_size_;

					size = determineSize(extent_block->extent[i].len);

					toResize(data_array, size);

					if (extent_block->extent[i].len <= 0x8000) 
					{
						device_->setPosition(offset);
						device_->ReadData(data_array.data(), size);
					}
					else {
						memset(data_array.data(), 0x00, size);
					}

					//extent_block->extent[i].block += 0x10000000;
					uint64_t target_offset = (uint64_t)extent_block->extent[i].block * block_size_;
					target_file.setPosition(target_offset);
					target_file.WriteData(data_array.data(), size);
				}
			}
			else {
				for (int i = 0; i < extent_block->header.entries; i++) {
					saveToFile(extent_block->extent_index[i].PysicalBlock(), target_file);
					int x = 0;
				}
			}
			return 0;
		}

	//	uint64_t SaveRawFile(uint64_t iNode_offset)
	};

#pragma pack(pop)
}