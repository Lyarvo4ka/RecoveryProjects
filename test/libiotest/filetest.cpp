#include "mocioengine.h"
#include "io/file.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::DoAll;


class IOFileTest
	: public ::testing::Test
{
protected:
	//IOFileTest()
	//	: fileToTest(L"")
	//{
	//}
	void SetUp() override
	{
		fileToTest.setIOEngine(mockIOEngine_ptr);
	}

	File fileToTest = L"";
	std::shared_ptr< MockIOEngine> mockIOEngine_ptr = std::make_shared<MockIOEngine>();
	
};

TEST_F(IOFileTest , OpenReadTest_OK)
{
	EXPECT_CALL(*mockIOEngine_ptr, OpenRead(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*mockIOEngine_ptr, readFileSize(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*mockIOEngine_ptr, isOpen()).WillRepeatedly(Return(true));

	EXPECT_TRUE(fileToTest.OpenRead());

 }

TEST_F(IOFileTest, OpenReadTest_kOpenRead)
{
	EXPECT_CALL(*mockIOEngine_ptr, OpenRead(_)).WillRepeatedly(Return(IOErrorsType::kOpenRead));

	EXPECT_THROW(fileToTest.OpenRead(), IOErrorException);
}

TEST_F(IOFileTest, OpenReadTest_kGetFileSize)
{
	EXPECT_CALL(*mockIOEngine_ptr, OpenRead(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*mockIOEngine_ptr, readFileSize(_)).WillRepeatedly(Return(IOErrorsType::kGetFileSize));

	EXPECT_THROW(fileToTest.OpenRead(), IOErrorException);
}

TEST_F(IOFileTest, OpenWriteTest_OK)
{
	EXPECT_CALL(*mockIOEngine_ptr, OpenWrite(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*mockIOEngine_ptr, readFileSize(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*mockIOEngine_ptr, isOpen()).WillRepeatedly(Return(true));

	EXPECT_TRUE(fileToTest.OpenWrite());
}

TEST_F(IOFileTest, OpenWriteTest_kOpenWrite)
{
	EXPECT_CALL(*mockIOEngine_ptr, OpenWrite(_)).WillRepeatedly(Return(IOErrorsType::kOpenWrite));

	EXPECT_THROW(fileToTest.OpenWrite() , IOErrorException);
}

TEST_F(IOFileTest, OpenWriteTest_kGetFileSize)
{
	EXPECT_CALL(*mockIOEngine_ptr, OpenWrite(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*mockIOEngine_ptr, readFileSize(_)).WillRepeatedly(Return(IOErrorsType::kGetFileSize));

	EXPECT_THROW(fileToTest.OpenWrite(), IOErrorException);
}


TEST_F(IOFileTest, OpenCreate_OK)
{
	EXPECT_CALL(*mockIOEngine_ptr,Create(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*mockIOEngine_ptr, readFileSize(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*mockIOEngine_ptr, isOpen()).WillRepeatedly(Return(true));

	EXPECT_TRUE(fileToTest.OpenCreate());
}

TEST_F(IOFileTest, OpenCreateTest_kOpenWrite)
{
	EXPECT_CALL(*mockIOEngine_ptr, Create(_)).WillRepeatedly(Return(IOErrorsType::kCreate));

	EXPECT_THROW(fileToTest.OpenCreate(), IOErrorException);
}

TEST_F(IOFileTest, OpenCreateTest_kGetFileSize)
{
	EXPECT_CALL(*mockIOEngine_ptr, Create(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*mockIOEngine_ptr, readFileSize(_)).WillRepeatedly(Return(IOErrorsType::kGetFileSize));

	EXPECT_THROW(fileToTest.OpenCreate(), IOErrorException);
}

TEST_F(IOFileTest, setPositionTest)
{
	uint64_t pos_value = 10;
	EXPECT_CALL(*mockIOEngine_ptr, setPosition(_));
	EXPECT_CALL(*mockIOEngine_ptr, getPosition()).WillRepeatedly(Return(pos_value));

	fileToTest.setPosition(pos_value);
	EXPECT_EQ(fileToTest.getPosition(), pos_value);
}

TEST_F(IOFileTest, ReadDataTest_OK)
{
	const uint32_t data_size = 1;
	DataArray buf(data_size);

	EXPECT_CALL(*mockIOEngine_ptr, Read(_,_, _)).WillRepeatedly(DoAll(SetArgReferee<2>(data_size), Return(IOErrorsType::OK)));
	EXPECT_EQ( fileToTest.ReadData(buf.data() , buf.size() ), data_size);
}

TEST_F(IOFileTest, ReadDataTest_kReadData)
{
	const uint32_t data_size = 1;
	DataArray buf(data_size);

	EXPECT_CALL(*mockIOEngine_ptr, Read(_, _, _)).WillRepeatedly( Return(IOErrorsType::kReadData));
	EXPECT_THROW(fileToTest.ReadData(buf.data(), buf.size()), IOErrorException);
}

TEST_F(IOFileTest, WriteDataTest_OK)
{
	const uint32_t data_size = 1;
	DataArray buf(data_size);

	EXPECT_CALL(*mockIOEngine_ptr, Write(_, _, _)).WillRepeatedly(DoAll(SetArgReferee<2>(data_size), Return(IOErrorsType::OK)));
	EXPECT_EQ(fileToTest.WriteData(buf.data(), buf.size()), data_size);
}

TEST_F(IOFileTest, WriteDataTest_kReadData)
{
	const uint32_t data_size = 1;
	DataArray buf(data_size);

	EXPECT_CALL(*mockIOEngine_ptr, Write(_, _, _)).WillRepeatedly(Return(IOErrorsType::kWriteData));
	EXPECT_THROW(fileToTest.WriteData(buf.data(), buf.size()), IOErrorException);
}

TEST_F(IOFileTest, setSizeTest_OK)
{
	uint64_t size_value = 10;
	EXPECT_CALL(*mockIOEngine_ptr, SetFileSize(_)).WillRepeatedly(Return(IOErrorsType::OK));

	fileToTest.setSize(size_value);
	EXPECT_EQ(fileToTest.Size(), size_value);
}

TEST_F(IOFileTest, setSizeTest_kSetFileSize)
{
	uint64_t size_value = 10;
	EXPECT_CALL(*mockIOEngine_ptr, SetFileSize(_)).WillRepeatedly(Return(IOErrorsType::kSetFileSize));

	EXPECT_THROW(fileToTest.setSize(size_value); , IOErrorException);
}

TEST_F(IOFileTest, readFileSize_OK)
{
	uint64_t size_value = 0;
	const uint64_t const_value = 10;
	EXPECT_CALL(*mockIOEngine_ptr, readFileSize(_)).WillRepeatedly(DoAll(SetArgReferee<0>(const_value), Return(IOErrorsType::OK)));

	fileToTest.readFileSize(size_value);
	EXPECT_EQ(const_value, size_value);
}

TEST_F(IOFileTest, readFileSize_kGetFileSize)
{
	uint64_t size_value = 0;
	EXPECT_CALL(*mockIOEngine_ptr, readFileSize(_)).WillRepeatedly(Return(IOErrorsType::kGetFileSize));
	
	EXPECT_THROW(fileToTest.readFileSize(size_value), IOErrorException);
}