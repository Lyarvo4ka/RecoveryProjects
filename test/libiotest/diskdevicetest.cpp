#include "mocioengine.h"
#include "io/diskdevice.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::DoAll;


class IODiskDeviceTest
	: public ::testing::Test
{
protected:
	//IOFileTest()
	//	: fileToTest(L"")
	//{
	//}
	void SetUp() override
	{
		DiskDevice.setIOEngine(mockIOEngine_ptr);
	}

	DiskDevice fileToTest = L"";
	std::shared_ptr< MockIOEngine> mockIOEngine_ptr = std::make_shared<MockIOEngine>();

};
