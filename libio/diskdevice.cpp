#include "io/diskdevice.h"

namespace IO
{
	const std::string_view disk_txt = "disk";


	DiskDevice::DiskDevice(PhysicalDrivePtr physical_drive)
		: physical_drive_(physical_drive)
		, io_engine_(makeIOEngine())
	{
		io_engine_->setTranserSize(physical_drive_->getTransferLength());
		deviceInfo_.deviceTypeName = disk_txt;
	}
	bool DiskDevice::Open(OpenMode open_mode)
	{
		if (physical_drive_)
		{
			auto status = makeErrorStatus(*this, io_engine_->OpenPhysicalDrive(physical_drive_->getPath()));
			if (!status.isOK())
				throw Error::IOErrorException(status);
		}
		return io_engine_->isOpen();
	}
	void DiskDevice::Close()
	{
		io_engine_->Close();
	}
	bool DiskDevice::isOpen()
	{
		return io_engine_->isOpen();
	}
	void DiskDevice::setPosition(uint64_t offset)
	{
		io_engine_->setPosition(offset);
	}
	uint64_t DiskDevice::getPosition() const
	{
		return io_engine_->getPosition();
	}
	uint32_t DiskDevice::ReadData(ByteArray data, uint32_t read_size)
	{
		assert(isOpen() == true);
		auto sector_size = physical_drive_->getBytesPerSector();

		if (isMultiple(this->getPosition(), sector_size) && isMultiple(read_size, sector_size))
			return ReadBlock(data, read_size);
		else
			return ReadDataNotAligned(data, read_size);
	}
	uint32_t DiskDevice::WriteData(ByteArray data, uint32_t write_size)
	{
		assert(isOpen() == true);
		return WriteBlock(data, write_size);
	}
	uint64_t DiskDevice::Size() const
	{
		return physical_drive_->getSize();	// return byte, not sectors
	}

	 uint32_t DiskDevice::ReadDataNotAligned(ByteArray data, uint32_t read_size)
	{
		const auto sector_size = physical_drive_->getBytesPerSector();
		const uint32_t data_start = this->getPosition() % sector_size;
		const int sector_to_read = (data_start + read_size) / sector_size + 1;
		const int bytes_to_read = sector_to_read * sector_size;

		DataArray temp_data(bytes_to_read);
		auto bytes_read = ReadBlock(temp_data.data(), temp_data.size());

		memcpy(data, temp_data.data() + data_start, read_size);
		return read_size;
	}
	 uint32_t DiskDevice::ReadBlock(ByteArray data, uint32_t read_size)
	 {
		 assert(data != nullptr);
		 assert(read_size >= 0);

		 uint32_t bytes_read = 0;
		 auto result = io_engine_->Read(data, read_size, bytes_read);
		 if (result != Error::IOErrorsType::OK)
			 throw Error::IOErrorException(makeErrorStatus(*this, result));

		 return bytes_read;
	 }
	 uint32_t DiskDevice::WriteBlock(ByteArray data, uint32_t write_size)
	 {
		 assert(data != nullptr);
		 assert(write_size >= 0);

		 uint32_t bytes_written = 0;
		 auto result = io_engine_->Write(data, write_size, bytes_written);
		 if (result != Error::IOErrorsType::OK)
			 throw Error::IOErrorException(makeErrorStatus(*this, result));

		 return bytes_written;
	 }
}
