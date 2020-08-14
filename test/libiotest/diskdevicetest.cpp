#include "mocioengine.h"
#include "io/diskdevice.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::SaveArgPointee;
using ::testing::DoAll;
using ::testing::Invoke;


class OffsetsSizes
{
	uint64_t offset_ = 0;
	uint32_t size_ = 0;
	uint32_t sector_size_ = 0;
	uint32_t offset_mod_ = 0;
	uint32_t buffer_size_ = 0;
public:
	OffsetsSizes(uint64_t offset, uint32_t size, uint32_t sector_size = default_sector_size)
		: offset_(offset)
		, size_(size)
		, sector_size_(sector_size)
	{
		if (sector_size == 0)
			return;

		offset_mod_ = offset_ % sector_size_;

		uint32_t dif = (size_ + offset_) % sector_size_;
		if (dif == 0)
			buffer_size_ = size;
		else
		{
			uint32_t size_mul_sector = (size + offset_mod_) / sector_size_ + 1;
			buffer_size_ = size_mul_sector * sector_size_;
		}



	}
	uint64_t getOffsetMod() const
	{
		return offset_mod_;
	}
	uint32_t getBufferSize() const
	{
		return buffer_size_;
	}
};

class IODiskDeviceTest
	: public ::testing::Test
{
protected:

	void SetUp() override
	{
		PhysicalDrivePtr physicalDrivePtr = std::make_shared< PhysicalDrive>();
		physicalDrivePtr->setBytesPerSector(default_sector_size);
		diskDevicePtr = std::make_unique<DiskDevice>(physicalDrivePtr);
		diskDevicePtr->setIOEngine(mockIOEngine_ptr);
	}

	std::unique_ptr<DiskDevice> diskDevicePtr;
	std::shared_ptr< MockIOEngine> mockIOEngine_ptr = std::make_shared<MockIOEngine>();

};

TEST_F(IODiskDeviceTest, OpenTest_OK)
{
	EXPECT_CALL(*mockIOEngine_ptr, OpenPhysicalDrive(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*mockIOEngine_ptr, isOpen()).WillRepeatedly(Return(true));

	EXPECT_TRUE(diskDevicePtr->Open(IO::OpenMode::OpenRead));
}

TEST_F(IODiskDeviceTest, OpenTest_kOpenPhysicalDrive)
{
	EXPECT_CALL(*mockIOEngine_ptr, OpenPhysicalDrive(_)).WillRepeatedly(Return(IOErrorsType::kOpenPhysicalDrive));
	EXPECT_CALL(*mockIOEngine_ptr, isOpen()).WillRepeatedly(Return(true));
	try 
	{
		diskDevicePtr->Open(IO::OpenMode::OpenRead);
	}
	catch (const IOErrorException& ex)
	{
		EXPECT_EQ(ex.getStatus().code(), IOErrorsType::kOpenPhysicalDrive);
	}
}

TEST_F(IODiskDeviceTest, ReadBlock_OK)
{
	const uint32_t data_size = 1;
	DataArray block(data_size);

	EXPECT_CALL(*mockIOEngine_ptr, Read(_,_,_)).WillRepeatedly(DoAll(SetArgReferee<2>(data_size), Return(IOErrorsType::OK)));

	EXPECT_EQ(diskDevicePtr->ReadBlock(block.data(), block.size()), data_size);

}

TEST_F(IODiskDeviceTest, ReadBlock_kReadData)
{
	const uint32_t data_size = 1;
	DataArray block(data_size);

	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillRepeatedly( Return(IOErrorsType::kReadData));

	try
	{
		diskDevicePtr->ReadBlock(block.data(), block.size());
	}
	catch (const IOErrorException& ex)
	{
		EXPECT_EQ(ex.getStatus().code(), IOErrorsType::kReadData);
	}

}

void generateOrdered(DataArray& block , uint32_t start_value = 0)
{
	for (uint32_t i = 0; i < block.size(); ++i)
	{
		block[i] = ( i + start_value ) % UINT8_MAX;
	}
}
TEST_F(IODiskDeviceTest, ReadDataNotAligned_OK)
{
	const uint64_t offset = 10;
	const uint32_t data_size = 510;
	DataArray block(data_size);
	OffsetsSizes offsets_sizes(offset, data_size);

	DataArray expectedData(offsets_sizes.getBufferSize());
	generateOrdered(expectedData);

	EXPECT_CALL(*mockIOEngine_ptr, getPosition()).WillRepeatedly(Return(offset));
	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillOnce(Invoke(
		[&](ByteArray byte_array , uint32_t size , uint32_t& bytes_read) {
			memcpy(byte_array, expectedData.data(), expectedData.size());
			bytes_read = expectedData.size();
			return IOErrorsType::OK;
		}));
	diskDevicePtr->setPosition(offset);
	auto actual = diskDevicePtr->ReadDataNotAligned(block.data(), block.size());
	EXPECT_EQ(std::memcmp(block.data() , expectedData.data() + offset, block.size()) , 0);
	EXPECT_EQ(actual, data_size);
}

TEST_F(IODiskDeviceTest, ReadDataNotAligned_kReadData)
{
	const uint32_t data_size = 1;
	DataArray block(data_size);

	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillRepeatedly(Return(IOErrorsType::kReadData));

	try
	{
		diskDevicePtr->ReadDataNotAligned(block.data(), block.size());
	}
	catch (const IOErrorException& ex)
	{
		EXPECT_EQ(ex.getStatus().code(), IOErrorsType::kReadData);
	}

}

TEST_F(IODiskDeviceTest, ReadData_Use_ReadBlock_OK)
{
	uint32_t real_read_size = 0;

	const uint64_t offset = 1024;
	const uint32_t data_size = 512;
	DataArray block(data_size);

	OffsetsSizes offset_sizes(offset, data_size);
	DataArray expectedData(offset_sizes.getBufferSize());
	generateOrdered(expectedData, offset);

	EXPECT_CALL(*mockIOEngine_ptr, isOpen()).WillOnce(Return(true));
	EXPECT_CALL(*mockIOEngine_ptr, getPosition()).WillOnce(Return(offset));
	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillOnce(Invoke(
		[&](ByteArray byte_array, uint32_t size, uint32_t& bytes_read) {
			memcpy(byte_array, expectedData.data(), expectedData.size());
			bytes_read = expectedData.size();
			return IOErrorsType::OK;
		}));
	diskDevicePtr->setPosition(offset);

	auto actual = diskDevicePtr->ReadData(block.data(), block.size());
	EXPECT_EQ(std::memcmp(block.data(), expectedData.data() + offset_sizes.getOffsetMod(), block.size()), 0);
	EXPECT_EQ(actual, data_size);
}

TEST_F(IODiskDeviceTest, ReadData_Use_ReadDataNotAligned_OK)
{
	const uint64_t offset = 458;
	const uint32_t data_size = 580;

	DataArray block(data_size);

	OffsetsSizes offset_sizes(offset, data_size);

	DataArray expectedData(offset_sizes.getBufferSize());
	generateOrdered(expectedData, offset);


	EXPECT_CALL(*mockIOEngine_ptr, isOpen()).WillRepeatedly(Return(true));
	EXPECT_CALL(*mockIOEngine_ptr, getPosition()).WillRepeatedly(Return(offset));
	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillOnce(Invoke(
		[&](ByteArray byte_array, uint32_t size, uint32_t& bytes_read) {
			memcpy(byte_array, expectedData.data(), expectedData.size());
			bytes_read = expectedData.size();
			return IOErrorsType::OK;
		}));
	diskDevicePtr->setPosition(offset);

	auto actual = diskDevicePtr->ReadData(block.data(), block.size());
	EXPECT_EQ(std::memcmp(block.data(), expectedData.data()+offset_sizes.getOffsetMod(), block.size()), 0);
	EXPECT_EQ(actual, data_size);
}