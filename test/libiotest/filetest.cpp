#include "mocioengine.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;

TEST(ioengineSuit, ReadDataTest11)
{
	auto engine_mock = std::make_unique<MockIOEngine>();


}


/*
#include "io/file.h"
#include "mocioengine.h"


using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;

TEST(fileTest, OpenRead_Test)
{

	//auto engine_mock = std::make_unique<MockIOEngine>();;
	//MockIOEngine engine_mock;
	//EXPECT_CALL(*engine_mock.get(), OpenRead(_)).WillRepeatedly(Return(IOErrorsType::OK));

//	File fileToTest(L"somename");
//	fileToTest.setIOEngine(std::move(engine_mock));
	//EXPECT_TRUE(fileToTest.OpenRead());

}

*/