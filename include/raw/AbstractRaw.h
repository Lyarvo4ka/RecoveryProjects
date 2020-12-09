#ifndef ABSTRACT_RAW_H
#define ABSTRACT_RAW_H


//#include "iofunctions.h"
#include "io/iodevice.h"
#include <memory>

#include "io/utility.h"
#include "io/dataarray.h"

namespace RAW
{
	using namespace IO;



	using SignatureArray = std::vector<DataArray>;
	using SignatureArrayPtr = std::vector<DataArray::Ptr>;
	using FotersArray = SignatureArray;

	class SignatureOffset
	{
		uint32_t signature_offset_ = 0;
		SignatureArray signatureArray_;

	public:
		using Ptr = std::unique_ptr<SignatureOffset>;
		SignatureOffset()
		{

		}
		SignatureOffset(DataArray dataArray, uint32_t signature_offset = 0)
		{
			signature_offset_ = signature_offset;
			addSignature(dataArray);
		}
		SignatureOffset(ByteArray data, uint32_t size, uint32_t signature_offset = 0)
		{
			signature_offset_ = signature_offset;
			addSignature(data , size);
		}
		void setOffset(const uint32_t signature_offset)
		{
			signature_offset_ = signature_offset;
		}
		uint32_t getOffset() const
		{
			return signature_offset_;
		}
		void addSignature(const DataArray & dataArray)
		{
			signatureArray_.emplace_back(dataArray);
		}
		void addSignature(ByteArray data, uint32_t size)
		{
			signatureArray_.emplace_back(DataArray(data, size));
		}
		bool FindSignature(const ByteArray data, uint32_t size)
		{
			for (auto & theSignature : signatureArray_)
			{
				if (theSignature.compareData(data, size, signature_offset_))
					return true;
			}
			return false;
		}
		bool find(const DataArray & data_array)
		{
			auto iter = std::find(signatureArray_.begin(), signatureArray_.end(), &data_array);
			return (iter != signatureArray_.end()) ? true : false;
		}

	};


	using HeaderArray = std::vector<SignatureOffset>;
	using HeaderArrayPtr = std::vector<SignatureOffset::Ptr>;


	// RAWFile
	// RAWHandle
	// RAWDescription
	// RAWType
	//class RAWDescriptor
	class FileStruct
	{
		HeaderArray headers_;
		std::string formatName_;
		std::string algorithmName_;
		std::string algorithType_;
		std::string category_;
		path_string extension_;
		FotersArray footers_;
		uint32_t footerTailEndSize_ = 0;
		uint64_t maxFileSize_ = 0;
		uint64_t minFileSize_ = 0;
		uint32_t footer_offset_ = 0;
		uint32_t search_block_ = 0;
		bool bValid = false;
	public:
		using Ptr = std::unique_ptr<FileStruct>;
		FileStruct(const std::string & formatName)
			: formatName_(formatName)
		{

		}
		bool isValid()
		{
			return !headers_.empty();
		}
		std::string getName() const
		{
			return formatName_;
		}
		void setAlgorithmName(const std::string & algorithmName )
		{
			algorithmName_ = algorithmName;
		}
		std::string getAlgorithmName() const 
		{
			return algorithmName_;
		}
		std::size_t headersCount() const
		{
			return headers_.size();
		}

		void addHeader(const DataArray & dataArray, uint32_t offset)
		{
			auto iter = findByOffset(offset);

			if (iter != headers_.end())
			{
				if (!(*iter)->find(dataArray))
					(*iter).addSignature(dataArray);
				else
				{
					printf("This signature is already present\r\n");
				}
			}
			else
				headers_.emplace_back(SignatureOffset(dataArray, offset));

		}
		void addHeader(const SignatureOffset & signAndOffset)
		{
			headers_.emplace_back(signAndOffset);
		}
		void addFooter( const DataArray & footer)
		{
			footers_.emplace_back(footer);

		}
		const FotersArray& getFooters() const
		{
			return footers_;
		}
		void setFooterTailEndSize(uint32_t footerTailEndSize)
		{
			footerTailEndSize_ = footerTailEndSize;
		}
		uint32_t getFooterTailEndSize() const
		{
			return footerTailEndSize_;
		}
		void setFooterSearchOffset(uint32_t footer_offset, uint32_t search_block)
		{
			footer_offset_ = footer_offset;
			search_block_ = search_block;
		}
		void setMaxFileSize(uint64_t maxFileSize)
		{
			maxFileSize_ = maxFileSize;
		}
		uint64_t getMaxFileSize() const
		{
			return maxFileSize_;
		}
		void setMinFileSize(const uint64_t minFileSize)
		{
			minFileSize_ = minFileSize;
		}
		uint64_t getMinFileSize() const 
		{
			return minFileSize_;
		}

		bool compareWithAllHeaders(ByteArray data, uint32_t size) const
		{
			for (auto &theHeader : headers_)
			{
				if (!theHeader.FindSignature(data, size))
					return false;
			}
			return true;
		}
		void setExtension(const path_string & extension)
		{
			extension_ = extension;
		}
		path_string getExtension() const
		{
			return extension_;
		}
		void setAlgorithType(const std::string algorithType)
		{
			algorithType_ = algorithType;
		}
		std::string getAlgorithType() const
		{
			return algorithType_;
		}
	private:
		HeaderArray::iterator findByOffset(uint32_t header_offset)
		{
			return std::find_if(headers_.begin(), headers_.end(),
				[header_offset](const SignatureOffset::Ptr & ptr)
			{
				return ptr->getOffset() == header_offset;
			}
			);

		}

	};
	
	inline FileStruct::Ptr makeFileStruct(const std::string & formatName)
	{
		return std::make_shared<FileStruct>(formatName);
	}


	class HeaderBase
	{
	public:
		using Ptr = std::shared_ptr<HeaderBase>;
	private:
		std::list<FileStruct::Ptr> listHeaders_;
		std::list<FileStruct::Ptr>::const_iterator iter_;
	public:
		void addFileFormat(FileStruct::Ptr new_file_format)
		{
			listHeaders_.emplace_back(new_file_format);
		}
		FileStruct * find(const ByteArray data , uint32_t size)
		{
			for (auto & theFileStruct : listHeaders_)
			{
				if (theFileStruct->compareWithAllHeaders(data, size))
					return theFileStruct.get();
			}
			return nullptr;
		}
		FileStruct* findByAlgorithmName(const std::string algorithmName)
		{
			for (auto& file_struct : listHeaders_)
			{
				if (file_struct->getAlgorithmName().compare(algorithmName) == 0)
					return file_struct.get();
			}
			return nullptr;
		}
	};

	class RawAlgorithm;

	class SignatureFinder
	{
		IODevicePtr device_;
		uint32_t block_size_ = default_block_size;
		uint32_t sector_size_ = default_sector_size;
		HeaderBase::Ptr header_base_;
	public:
		SignatureFinder(IODevicePtr device , HeaderBase::Ptr header_base)
			: device_(device)
			, header_base_(header_base)
		{

		}
		~SignatureFinder()
		{

		}
		void setBlockSize(const uint32_t block_size)
		{
			this->block_size_ = block_size;
		}
		FileStruct * findHeader(const uint64_t start_offset, uint64_t & header_pos)
		{
			header_pos = 0;
			if ( !device_->isOpen())
			if (!device_->Open(OpenMode::OpenRead))
			{
				wprintf_s(L"Error open device\n");
				return nullptr;
			}

			uint64_t file_size = device_->Size();
			uint64_t offset = start_offset;
			auto buffer = makeDataArray(block_size_);
			uint32_t bytes_read = 0;
			uint32_t result_offset = 0;

			while (offset < file_size)
			{
				device_->setPosition(offset);
				bytes_read = device_->ReadData(buffer->data(), block_size_);
				if (bytes_read == 0)
				{
					printf("Error read drive\r\n");
					break;
				}
				if (auto header_ptr = cmpHeader(buffer, bytes_read, result_offset))
				{
					header_pos = offset;
					header_pos += result_offset;
					return header_ptr;
				}

				offset += bytes_read;
			}
			return nullptr;
		}

		FileStruct * cmpHeader(const DataArray::Ptr & buffer, const uint32_t size, uint32_t & header_pos)
		{
			for (header_pos = 0; header_pos < size; header_pos += sector_size_)
			{
				if (auto header = header_base_->find(buffer->data() + header_pos, buffer->size()))
					return header;
			}
			return nullptr;
		}


		RawAlgorithm * createRawAlgorithm(FileStruct::Ptr headerPtr)
		{
			
			return nullptr;
		}

	};

	class RawAlgorithm
	{
	public:
		virtual uint64_t SaveRawFile(File & target_file, const uint64_t start_offset) = 0;
		virtual bool Specify(const uint64_t start_offset) = 0;
		virtual bool Verify(const IO::path_string & file_path) 
		{
			return true;
		}
	};

	class FileAnalyzer
	{
	public:
		virtual void Analyze(const IO::path_string & file_name) = 0;
	};

	class BlockDevice
	{
		IODevicePtr device_;
		uint32_t block_size_ = default_block_size;
		uint32_t sector_size_ = default_sector_size;
	public:
		IODevice * getDevice()
		{
			return device_.get();
		}

	};

	class DefaultRaw : public RawAlgorithm
	{
	private:
		IODevicePtr device_;
		uint32_t block_size_ = default_block_size;
		uint32_t sector_size_ = default_sector_size;
	public:
		DefaultRaw(IODevicePtr device)
			: device_(device)
		{

		}
		IODevicePtr getDevice()
		{
			return device_;
		}
		void setBlockSize(const uint32_t block_size)
		{
			this->block_size_ = block_size;
		}
		uint32_t getBlockSize() const
		{
			return block_size_;
		}
		void setSectorSize(const uint32_t sector_size)
		{
			this->sector_size_ = sector_size;
		}
		uint32_t getSectorSize() const
		{
			return sector_size_;
		}
		uint32_t ReadData(ByteArray data, uint32_t size)
		{
			return device_->ReadData(data, size);
		}
		uint32_t ReadData(DataArray & data_array)
		{
			return device_->ReadData(data_array.data(), data_array.size());
		}
		void setPosition(uint64_t offset)
		{
			device_->setPosition(offset);
		}
		uint64_t getSize() const
		{
			return device_->Size();
		}
		uint64_t appendToFile(File & write_file, const uint64_t source_offset, const uint64_t write_size)
		{
			if (source_offset >= getSize())
			{
				printf("Error append to file. Source offset is more than size.\r\n");
				return 0;
			}

			uint64_t to_write = write_size;
			if (source_offset + write_size > getSize())	// ?????
				to_write = getSize() - write_size;

			if (to_write == 0)
			{
				printf("Error append to file. Write size is 0.\r\n");
			}

			auto target_offset = write_file.Size();
			uint32_t bytes_read = 0;
			uint32_t bytes_written = 0;
			uint64_t cur_pos = 0;
			uint64_t read_pos = source_offset;
			uint32_t bytes_to_write = 0;

			auto buffer = makeDataArray(getBlockSize());
			while (cur_pos < to_write)
			{
				bytes_to_write = calcBlockSize(cur_pos, write_size, getBlockSize());

				setPosition(read_pos);
				bytes_read = ReadData(buffer->data(), bytes_to_write);
				if (bytes_read == 0)
					return 0;

				read_pos += bytes_read;

				write_file.setPosition(target_offset);
				bytes_written = write_file.WriteData(buffer->data(), bytes_read);
				if (bytes_written == 0)
					return 0;

				target_offset += bytes_written;
				cur_pos += bytes_written;
			}

			return cur_pos;
		}


	};

	class SpecialAlgorithm
	{
	public:
		virtual uint64_t Execute(const uint64_t start_offset, const path_string target_folder) = 0;
	};


};

//
//
//class AbstractRaw
//{
//public:
//	AbstractRaw( const std::string & file_name  )
//		: hSource_( INVALID_HANDLE_VALUE )
//		, bReady_( false )
//	{
//		bReady_ = IO::open_read( hSource_ , file_name );
//	}
//	AbstractRaw( const DWORD drive_number  )
//		: hSource_( INVALID_HANDLE_VALUE )
//		, bReady_( false )
//	{
//		std::string drive_path( drivePathFromNumber( drive_number ) );
//		bReady_ = IO::open_read( hSource_ , drive_path );
//	}
//	virtual ~AbstractRaw()
//	{
//		close();
//	}
//	bool isReady() const
//	{
//		return bReady_;
//	}
//	void close()
//	{
//		if ( hSource_ != INVALID_HANDLE_VALUE )
//		{
//			CloseHandle( hSource_ );
//			hSource_ = INVALID_HANDLE_VALUE;
//			bReady_ = false;
//		}
//	}
//	HANDLE * getHandle() 
//	{
//		return &hSource_;
//	}
//	virtual void execute() = 0;
//private:
//	HANDLE hSource_;
//	bool bReady_;
//};

#endif
