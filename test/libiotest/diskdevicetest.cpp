#include "mocioengine.h"
#include "io/diskdevice.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::SaveArgPointee;
using ::testing::DoAll;
using ::testing::Invoke;


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
	constexpr uint32_t dataInsideSize = (((data_size  + offset % default_sector_size)/ default_sector_size) + 1 ) * default_sector_size;
	DataArray expectedData(dataInsideSize);
	generateOrdered(expectedData);

	EXPECT_CALL(*mockIOEngine_ptr, getPosition()).WillOnce(Return(offset));
	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillOnce(Invoke(
		[&](ByteArray byte_array , uint32_t size , uint32_t& bytes_read) {
			memcpy(byte_array, expectedData.data(), expectedData.size());
			bytes_read = data_size;
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
	constexpr uint32_t dif = (data_size + offset) % default_sector_size;
	constexpr uint32_t mod_offset = offset % default_sector_size;
	constexpr uint32_t size_mul_sector = (data_size + mod_offset) / default_sector_size + 1;

	if (dif == 0)
		real_read_size = data_size;
	else
	{
		real_read_size = size_mul_sector * default_sector_size;
	}
	DataArray expectedData(real_read_size);
	generateOrdered(expectedData, offset);

	EXPECT_CALL(*mockIOEngine_ptr, isOpen()).WillOnce(Return(true));
	EXPECT_CALL(*mockIOEngine_ptr, getPosition()).WillOnce(Return(offset));
	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillOnce(Invoke(
		[&](ByteArray byte_array, uint32_t size, uint32_t& bytes_read) {
			memcpy(byte_array, expectedData.data(), expectedData.size());
			bytes_read = data_size;
			return IOErrorsType::OK;
		}));
	diskDevicePtr->setPosition(offset);

	auto actual = diskDevicePtr->ReadData(block.data(), block.size());
	EXPECT_EQ(std::memcmp(block.data(), expectedData.data() + mod_offset, block.size()), 0);
	EXPECT_EQ(actual, data_size);
}
//
//TEST_F(IODiskDeviceTest, ReadData_Use_ReadDataNotAligned_OK)
//{
//	const uint64_t offset = 458;
//	const uint32_t data_size = 580;
//
//	DataArray block(data_size);
//	constexpr uint32_t mod_offset = offset % default_sector_size;
//	constexpr uint32_t size_mul_sector = (data_size + mod_offset) / default_sector_size + 1;
//	constexpr uint32_t dataInsideSize = size_mul_sector * default_sector_size;
//	DataArray expectedData(dataInsideSize);
//	generateOrdered(expectedData, offset);
//
//
//	EXPECT_CALL(*mockIOEngine_ptr, isOpen()).WillOnce(Return(true));
//	EXPECT_CALL(*mockIOEngine_ptr, getPosition()).WillOnce(Return(offset));
//	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillOnce(Invoke(
//		[&](ByteArray byte_array, uint32_t size, uint32_t& bytes_read) {
//			memcpy(byte_array, expectedData.data(), expectedData.size());
//			bytes_read = data_size;
//			return IOErrorsType::OK;
//		}));
//	diskDevicePtr->setPosition(offset);
//
//	auto actual = diskDevicePtr->ReadData(block.data(), block.size());
//	EXPECT_EQ(std::memcmp(block.data(), expectedData.data(), block.size()), 0);
//	EXPECT_EQ(actual, data_size);
//}