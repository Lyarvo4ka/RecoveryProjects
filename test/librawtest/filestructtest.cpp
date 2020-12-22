#include "pch.h"

#include "raw/filestruct.h"


namespace RAWTest
{
	constexpr uint32_t value_aabbccdd = 0xaabbccdd;
	constexpr uint32_t value_00112233 = 0x00112233;
	uint32_t testSize = 4;

	TEST(SignatureOffsetSuit, addSignatureTest)
	{

		IO::DataArray signData(testSize);
		memcpy(signData.data(), &value_aabbccdd, testSize);

		uint32_t offset = 2;
		RAW::SignatureOffset signOffset(signData, offset);

		IO::DataArray testData(10);
		std::memcpy(testData.data() + offset, &value_aabbccdd, testSize);
		EXPECT_TRUE(signOffset.findSignature(testData.data(), testData.size()));

		memcpy(signData.data(), &value_00112233, testSize);
		signOffset.addSignature(signData);
		std::memcpy(testData.data() + offset, &value_00112233, testSize);

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
		fileStruct.addHeader(IO::DataArray(4) , 0);
		EXPECT_TRUE(fileStruct.isValid());
	}
	TEST(FileStruct, addHeaderTest)
	{
		IO::DataArray nomater_data1(testSize);
		std::memcpy(nomater_data1.data(), &value_aabbccdd , testSize);
		const uint32_t offset1 = 3;

		IO::DataArray nomater_data2(testSize);
		std::memcpy(nomater_data2.data(), &value_00112233, testSize);
		const uint32_t offset2 = 8;

		RAW::SignatureOffset signOffset(nomater_data1 , offset1);

		RAW::FileStruct fileStruct("empty");
		fileStruct.addHeader(nomater_data1, offset1);
		EXPECT_EQ(fileStruct.headersCount(), 1);
		fileStruct.addHeader(nomater_data2, offset1);
		EXPECT_EQ(fileStruct.headersCount(), 1);


		fileStruct.addHeader(nomater_data2, offset2);
		EXPECT_EQ(fileStruct.headersCount(), 2);

	}

	TEST(FileStruct, set_get_AlgorithmName)
	{
		RAW::FileStruct fileStruct("");
		const std::string test_name = "test";
		fileStruct.setAlgorithmName(test_name);
		EXPECT_EQ(test_name, fileStruct.getAlgorithmName());
	}



}	//namespace RAWTest