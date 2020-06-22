#include "pch.h"
#include "mocioengine.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;

TEST(ioengineSuit, openreadTest)
{
	MockIOEngine mockioengine;
	EXPECT_CALL(mockioengine, OpenRead(_))
		.WillOnce( Return(IO::IOErrorsType::OK));

	EXPECT_EQ(mockioengine.OpenRead(L"test"), IO::IOErrorsType::OK);
	//mockioengine.
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
TEST(ioengineSuit, ReadTest)
{
	const uint32_t READ_SIZE = 10;
	DataArray buffer(READ_SIZE);

	MockIOEngine mockioengine;
	ON_CALL(mockioengine, read_data(_,_,_))
		.WillByDefault(DoAll(SetArgReferee<2>(READ_SIZE) , Return(IO::IOErrorsType::OK)));
	
	uint32_t bytesRead = 0;
	auto read_result = mockioengine.base_Read(buffer.data(), buffer.size(), bytesRead);
	EXPECT_EQ(bytesRead, READ_SIZE);
	EXPECT_EQ(read_result, IO::IOErrorsType::OK);

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

TEST(ioengineSuit, ReadTestTransferSizeIsNull)
{
	const uint32_t READ_SIZE = 0;

	MockIOEngine mockioengine;
	mockioengine.setTranserSize(1);
	ON_CALL(mockioengine, read_data(_, _, _))
		.WillByDefault(DoAll(SetArgReferee<2>(READ_SIZE), Return(IO::IOErrorsType::kReadData)));

	DataArray buffer(READ_SIZE);

	uint32_t bytesRead = 0;
	auto read_result = mockioengine.base_Read(buffer.data(), READ_SIZE, bytesRead);
	EXPECT_EQ(bytesRead, 0);

}

TEST(ioengineSuit, ReadTestDataParamNullptr)
{
	const uint32_t READ_SIZE = 0;

	MockIOEngine mockioengine;
	mockioengine.setTranserSize(1);
	ON_CALL(mockioengine, read_data(_, _, _))
		.WillByDefault(DoAll(SetArgReferee<2>(READ_SIZE), Return(IO::IOErrorsType::kReadData)));

	ByteArray tmp = nullptr;
	uint32_t bytesRead = 0;
	auto read_result = mockioengine.base_Read(tmp, READ_SIZE, bytesRead);
	EXPECT_EQ(bytesRead, 0);

}