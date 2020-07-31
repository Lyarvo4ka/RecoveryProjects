#include "mocioengine.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;


TEST(ioengineSuit, calcBlockSizeTest)
{
	auto result = calcBlockSize(9, 10, 30);
	EXPECT_EQ(result , 1);
}
TEST(ioengineSuit, currentMoreThanSizeTest)
{
	auto result = calcBlockSize(10, 5, 30);
	EXPECT_EQ(result, 0);
}
TEST(ioengineSuit, calcBlockSizeNormalTest)
{
	auto result = calcBlockSize(2, 10, 5);
	EXPECT_EQ(result, 5);
}

TEST(ioengineSuit, testIsParamsValid)
{
	IOEngine io_engine;
	EXPECT_FALSE(io_engine.isParamsValid(nullptr, 0));
	EXPECT_FALSE(io_engine.isParamsValid(nullptr, 1));

	DataArray tmp(1);
	EXPECT_FALSE(io_engine.isParamsValid(tmp.data(), 0));
	EXPECT_TRUE(io_engine.isParamsValid(tmp.data(), tmp.size()));

}
TEST(ioengineSuit, ReadDataTest)
{
	const uint32_t READ_SIZE = 10;
	DataArray buffer(READ_SIZE);
	uint32_t bytes_read = 0;

	MockIOEngine mockioengine;
	ON_CALL(mockioengine, read_data(_, _, _))
		.WillByDefault(DoAll(SetArgReferee<2>(READ_SIZE), Return(IO::IOErrorsType::OK)));
	auto result = mockioengine.base_ReadOrWriteData(buffer.data(), READ_SIZE, bytes_read, enReadWrite::kREAD);
	EXPECT_EQ(result, IO::IOErrorsType::OK);
	EXPECT_EQ(bytes_read, READ_SIZE);
}

TEST(ioengineSuit, WriteDataTest)
{
	const uint32_t WRITE_SIZE = 10;
	DataArray buffer(WRITE_SIZE);
	uint32_t bytes_written = 0;

	MockIOEngine mockioengine;
	ON_CALL(mockioengine, write_data(_, _, _))
		.WillByDefault(DoAll(SetArgReferee<2>(WRITE_SIZE), Return(IO::IOErrorsType::OK)));
	auto result = mockioengine.base_ReadOrWriteData(buffer.data(), WRITE_SIZE, bytes_written, enReadWrite::kWRITE);
	EXPECT_EQ(result, IO::IOErrorsType::OK);
	EXPECT_EQ(bytes_written, WRITE_SIZE);

}

TEST(ioengineSuit, ReadDataErrorTest)
{
	const uint32_t READ_SIZE = 10;
	DataArray buffer(READ_SIZE);
	uint32_t bytes_read = 0;

	MockIOEngine mockioengine;
	ON_CALL(mockioengine, read_data(_, _, _))
		.WillByDefault(DoAll(SetArgReferee<2>(READ_SIZE), Return(IO::IOErrorsType::kReadData)));
	auto result = mockioengine.base_ReadOrWriteData(buffer.data(), READ_SIZE, bytes_read, enReadWrite::kREAD);
	EXPECT_EQ(result, IO::IOErrorsType::kReadData);
}

TEST(ioengineSuit, ReadBlockMoreTransferSizeTest)
{
	const uint32_t READ_SIZE = 10;
	DataArray buffer(READ_SIZE);
	MockIOEngine mockioengine;
	mockioengine.setTranserSize(1);
	ON_CALL(mockioengine, read_data(_, _, _))
		.WillByDefault(DoAll(SetArgReferee<2>(READ_SIZE), Return(IO::IOErrorsType::OK)));

	uint32_t bytesRead = 0;
	auto read_result = mockioengine.base_Read(buffer.data(), buffer.size(), bytesRead);
	EXPECT_EQ(bytesRead, READ_SIZE);
	EXPECT_EQ(read_result, IO::IOErrorsType::OK);

}

TEST(ioengineSuit, ReadSizeisNull)
{
	const uint32_t READ_SIZE = 10;

	MockIOEngine mockioengine;
	mockioengine.setTranserSize(1);
	ON_CALL(mockioengine, read_data(_, _, _))
		.WillByDefault(DoAll(SetArgReferee<2>(READ_SIZE), Return(IO::IOErrorsType::kReadData)));

	DataArray buffer(READ_SIZE);

	uint32_t bytesRead = 0;
	auto read_result = mockioengine.base_Read(buffer.data(), 0, bytesRead);
	EXPECT_EQ(bytesRead, 0);
	EXPECT_EQ(read_result, IO::IOErrorsType::kWrongParam);

}

TEST(ioengineSuit, TranferSizeisNull)
{
	const uint32_t READ_SIZE = 10;

	MockIOEngine mockioengine;
	mockioengine.setTranserSize(0);

	DataArray buffer(READ_SIZE);
	uint32_t bytesRead = 0;
	auto read_result = mockioengine.base_Read(buffer.data(), READ_SIZE, bytesRead);
	EXPECT_EQ(bytesRead, 0);
	EXPECT_EQ(read_result, IO::IOErrorsType::kWrongParam);
}

TEST(ioengineSuit, ReadTestDataParamNullptr)
{
	const uint32_t READ_SIZE = 10;

	MockIOEngine mockioengine;
	mockioengine.setTranserSize(1);
	ON_CALL(mockioengine, read_data(_, _, _))
		.WillByDefault(DoAll(SetArgReferee<2>(READ_SIZE), Return(IO::IOErrorsType::kReadData)));

	ByteArray tmp = nullptr;
	uint32_t bytesRead = 0;
	auto read_result = mockioengine.base_Read(tmp, READ_SIZE, bytesRead);
	EXPECT_EQ(bytesRead, 0);

}