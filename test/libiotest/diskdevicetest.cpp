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

void generateOrdered(DataArray& block)
{
	for (uint32_t i = 0; i < block.size(); ++i)
	{
		block[i] = i % UINT8_MAX;
	}
}
TEST_F(IODiskDeviceTest, ReadDataNotAligned_OK)
{
	const uint32_t data_size = 10;
	DataArray block(data_size);
	DataArray sectorData(data_size);
	generateOrdered(sectorData);
	//memset(sectorData.data(), 0xAA, sectorData.size());

	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillOnce(Invoke(
		[=](ByteArray byte_array , uint32_t size , uint32_t& bytes_read) {
			memcpy(byte_array, sectorData.data(), sectorData.size());
			bytes_read = data_size;
			return IOErrorsType::OK;
		}));
	diskDevicePtr->setPosition(data_size);
	auto actual = diskDevicePtr->ReadDataNotAligned(block.data(), block.size());
	EXPECT_EQ(std::memcmp(block.data() , sectorData.data() , block.size()) , 0);
	EXPECT_EQ(actual, data_size);

//////////////////////////
	/*
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Mock;
using ::testing::_;
using ::testing::Invoke;

class actual_reader
{
   public:
   virtual void read(char* data)
   {
	  std::cout << "This should not be called" << std::endl;
   }
};
void mock_read_function(char* str)
{
   strcpy(str, "Mocked text");
}

class class_under_test
{
   public:
	  void read(actual_reader* obj, char *str)
	  {
		 obj->read(str);
	  }
};

class mock_reader : public actual_reader
{
   public:
   MOCK_METHOD1(read, void(char* str));
};


TEST(test_class_under_test, test_read)
{
   char text[100];

   mock_reader mock_obj;
   class_under_test cut;

   EXPECT_CALL(mock_obj, read(_)).WillOnce(Invoke(mock_read_function));
   cut.read(&mock_obj, text);
   std::cout << "Output : " << text << std::endl;

   // Lambda example
   EXPECT_CALL(mock_obj, read(_)).WillOnce(Invoke(
			[=](char* str){
			   strcpy(str, "Mocked text(lambda)");
			}));
   cut.read(&mock_obj, text);
   std::cout << "Output : " << text << std::endl;
	*/

}
//
//TEST_F(IODiskDeviceTest, ReadDataNotAligned_3Sectors_OK)
//{
//	const uint32_t numSectors = 3;
//	const uint32_t data_size = numSectors * diskDevicePtr->getPhysicalDrive()->getBytesPerSector();
//	DataArray block(data_size);
//	DataArray returnData(data_size);
//	memset(returnData.data(), 0xAA, returnData.size());
//
//	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillOnce(Invoke(
//		[=](ByteArray byte_array, uint32_t size, uint32_t& bytes_read) {
//			memcpy(byte_array, returnData.data(), returnData.size());
//			bytes_read = data_size;
//			return IOErrorsType::OK;
//		}));
//	diskDevicePtr->setPosition(data_size);
//	auto actual = diskDevicePtr->ReadDataNotAligned(block.data(), block.size());
//	EXPECT_EQ(std::memcmp(block.data(), returnData.data(), block.size()), 0);
//	EXPECT_EQ(actual, data_size);
//}