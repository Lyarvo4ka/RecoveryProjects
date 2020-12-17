#include "pch.h"

#include "raw/filestruct.h"


namespace RAWTest
{
	TEST(SignatureOffsetSuit, addSignatureTest)
	{
		constexpr uint32_t value1 = 0xaabbccdd;
		constexpr uint32_t value2 = 0x00112233;
		uint32_t testSize = 4;
		IO::DataArray signData(testSize);
		memcpy(signData.data(), &value1, testSize);

		uint32_t offset = 2;
		RAW::SignatureOffset signOffset(signData, offset);

		IO::DataArray testData(10);
		std::memcpy(testData.data() + offset, &value1, testSize);
		EXPECT_TRUE(signOffset.findSignature(testData.data(), testData.size()));

		memcpy(signData.data(), &value2, testSize);
		signOffset.addSignature(signData);
		std::memcpy(testData.data() + offset, &value2, testSize);

		EXPECT_TRUE(signOffset.findSignature(testData.data(), testData.size()));

		memset(testData.data(), 0xFF, testData.size());
		EXPECT_FALSE(signOffset.findSignature(testData.data(), testData.size()));
	}
	TEST(SignatureOffsetSuit, addSignatureInvalidDataArrayTest)
	{
		IO::DataArray invalid_data(0);
		RAW::SignatureOffset signOffset;
		signOffset.addSignature(invalid_data);
		EXPECT_EQ(signOffset.signaturesCount(), 0);
	}

	TEST(SignatureOffsetSuit, setOffsetTest)
	{
		RAW::SignatureOffset signOffset;
		uint32_t val_offset = 10;
		signOffset.setOffset(val_offset);
		EXPECT_EQ(signOffset.getOffset()  ,val_offset);
	}

	TEST(FileStruct, isValidTest)
	{
		RAW::FileStruct fileStruct("empty");
		EXPECT_FALSE(fileStruct.isValid());
		RAW::SignatureOffset signOffset;


	}

}	//namespace RAWTest