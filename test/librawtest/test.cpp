#include "pch.h"

#include "raw/ext4_raw.h"


TEST(ListExtentsCase, testAdd)
{
	RAW::ListExtents listExtents;
	listExtents.add(0, 0);
	EXPECT_FALSE(listExtents.isEmpty());
}

TEST(ListExtentsCase, testRemove)
{
	RAW::ListExtents listExtents;
	listExtents.add(1, 0);
	listExtents.remove(1);
	EXPECT_TRUE(listExtents.isEmpty());

	listExtents.add(2, 0);
	listExtents.remove(1);
	EXPECT_FALSE(listExtents.isEmpty());
}

TEST(ListExtentsCase, testfindByOffset)
{
	RAW::ListExtents listExtents;
	listExtents.add(1, 0);
	listExtents.add(2, 0);
	listExtents.add(3, 0);
	auto extent_handle = listExtents.findByOffset(1);
	EXPECT_EQ(extent_handle.offset, 1);
	
	extent_handle = listExtents.findByOffset(4);
	EXPECT_EQ(0, 0);

}

