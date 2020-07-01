#include "mocioengine.h"
#include "io/file.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;

TEST(fileSuite, OpenReadTest_OK)
{
	auto engine_mock = std::make_unique<MockIOEngine>();
	EXPECT_CALL(*engine_mock.get(), OpenRead(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*engine_mock.get(), readFileSize(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*engine_mock.get(), isOpen()).WillRepeatedly(Return(true));

 }

TEST(fileSuite, OpenReadTest_kOpenRead)
{
	auto engine_mock = std::make_unique<MockIOEngine>();
	EXPECT_CALL(*engine_mock.get(), OpenRead(_)).WillRepeatedly(Return(IOErrorsType::kOpenRead));

	File fileToTest(L"somename");
	fileToTest.setIOEngine(std::move(engine_mock));
	EXPECT_THROW(fileToTest.OpenRead(), IOErrorException);
}

TEST(fileSuite, OpenReadTest_kGetFileSize)
{
	auto engine_mock = std::make_unique<MockIOEngine>();
	EXPECT_CALL(*engine_mock.get(), OpenRead(_)).WillRepeatedly(Return(IOErrorsType::OK));
	EXPECT_CALL(*engine_mock.get(), readFileSize(_)).WillRepeatedly(Return(IOErrorsType::kGetFileSize));

	File fileToTest(L"somename");
	fileToTest.setIOEngine(std::move(engine_mock));
	EXPECT_THROW(fileToTest.OpenRead(), IOErrorException);
}
