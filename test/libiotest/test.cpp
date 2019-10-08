#include "pch.h"

#include "raw/QuickTime.h"

TEST(TestCaseName, TestfindTextTnBlock) {
	IO::DataArray block(20);
	uint32_t text_offset = 5;
	memcpy(block.data() + text_offset, "test", 4);
	uint32_t pos = 0;
	auto foundResult = RAW::findTextTnBlock(block, "test", pos);
	EXPECT_TRUE(foundResult);
	EXPECT_EQ(pos, text_offset);
}

TEST(TestCaseName, TestfindTextTnBlockFromEnd) {
	IO::DataArray block(20);
	uint32_t text_offset = 5;
	memcpy(block.data() + text_offset, "test", 4);
	uint32_t pos = 0;
	auto foundResult = RAW::findTextTnBlockFromEnd(block, "test", pos);
	EXPECT_TRUE(foundResult);
	EXPECT_EQ(pos, text_offset);
}