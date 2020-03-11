#include "pch.h"

#include "raw/ext4_raw.h"


TEST(ListExtentsCase, testAdd)
{
	RAW::ListExtents listExtents;
	listExtents.add(0, 0, 0, 0);
	EXPECT_FALSE(listExtents.isEmpty());
}

TEST(ListExtentsCase, testRemove)
{
	RAW::ListExtents listExtents;
	listExtents.add(1, 0, 0, 0);
	listExtents.remove(1);
	EXPECT_TRUE(listExtents.isEmpty());

	listExtents.add(2, 0, 0, 0);
	listExtents.remove(1);
	EXPECT_FALSE(listExtents.isEmpty());
}

TEST(ListExtentsCase, testfindByOffset)
{
	RAW::ListExtents listExtents;
	listExtents.add(1, 0, 0, 0);
	listExtents.add(2, 0, 0, 0);
	listExtents.add(3, 0, 0, 0);
	auto extent_handle = listExtents.findByOffset(1);
	EXPECT_EQ(extent_handle.offset, 1);

	extent_handle = listExtents.findByOffset(4);
	EXPECT_EQ(extent_handle.offset, 0);

}

TEST(ListExtentsCase, testfindBySize)
{
	RAW::ListExtents listExtents;
	listExtents.add(1, 2, 0 ,0);
	listExtents.add(2, 2, 0, 0);
	listExtents.add(3, 1, 0, 0);
	listExtents.add(4, 3, 0, 0);


	auto found_list = listExtents.findBySize(2);
	EXPECT_EQ(found_list.size(), 2);

	found_list = listExtents.findBySize(1);
	EXPECT_EQ(found_list.size(), 1);

	found_list = listExtents.findBySize(4);
	EXPECT_TRUE(found_list.empty());
		

}

TEST(ListExtentsCase, testfindByFirstOffset)
{
	RAW::ListExtents listExtents;
	listExtents.add(1, 0, 0 , 2);
	listExtents.add(2, 0, 0 , 2);
	listExtents.add(3, 0, 0 , 1);
	listExtents.add(4, 0, 0 , 3);


	auto found_list = listExtents.findByFirstOffset(2);
	EXPECT_EQ(found_list.size(), 2);

	found_list = listExtents.findByFirstOffset(1);
	EXPECT_EQ(found_list.size(), 1);

	found_list = listExtents.findByFirstOffset(4);
	EXPECT_TRUE(found_list.empty());


}

TEST(ListExtentsCase, testfindWithMaxEntries)
{
	RAW::ExtentHandle extHandle1{ 1 , 2 , 3 };
	RAW::ExtentHandle extHandle2{ 2 , 2 , 2 };
	RAW::ExtentHandle extHandle3{ 3 , 2 , 4 };
	RAW::ExtentHandle extHandle4{ 4 , 2 , 2 };

	std::list< RAW::ExtentHandle> listExtentHandle;
	auto extent_handle = RAW::findWithMaxEntries(listExtentHandle);
	EXPECT_EQ(extent_handle.number_entries, 0);

	listExtentHandle.emplace_back(extHandle1);
	listExtentHandle.emplace_back(extHandle2);
	listExtentHandle.emplace_back(extHandle3);
	listExtentHandle.emplace_back(extHandle4);

	extent_handle = RAW::findWithMaxEntries(listExtentHandle);
	EXPECT_EQ(extent_handle.number_entries, 4);
}

