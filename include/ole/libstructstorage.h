#ifndef LIBSTRUCTSTORAGE_H
#define LIBSTRUCTSTORAGE_H

#pragma warning (disable : 4251)

#include <Windows.h>
#include <string>
#include "libstructstorage_global.h"

#include "Objbase.h"
#include "Objidl.h"

//#include "IOLibrary/IODevice.h"

std::string LIBSTRUCTSTORAGE_API getTimeFromFileTime(const FILETIME & file_time);
std::string LIBSTRUCTSTORAGE_API getDateFromFileTime(const FILETIME & file_time);
std::string LIBSTRUCTSTORAGE_API getDateTimeFromFileTime(const FILETIME & file_time);
SYSTEMTIME LIBSTRUCTSTORAGE_API toSysTime(const FILETIME & file_time);

std::string LIBSTRUCTSTORAGE_API toString(WORD nYear, const int size);


std::string LIBSTRUCTSTORAGE_API toYearString(WORD nYear);
std::string LIBSTRUCTSTORAGE_API toStringDate(WORD nDateTime);


class FileDateTime
{
private:
	FILETIME filetime_;
	SYSTEMTIME systime_;
	bool bConverted_;

public:
	FileDateTime()
		: filetime_({ 0 })
		, systime_({ 0 })
	{
		this->convert();
	}
	FileDateTime(const FILETIME & file_time)
		: filetime_(file_time)
		, systime_({ 0 })
	{
		this->convert();

	}
	void setFileTime(const FILETIME & file_time)
	{
		filetime_ = file_time;
		this->convert();
	}
	bool isValid() const
	{
		return bConverted_;
	}
	SYSTEMTIME getSystime() const
	{
		return systime_;
	}
	FILETIME getFiletime() const
	{
		return filetime_;
	}
	std::string getYear() const
	{
		return toYearString(systime_.wYear);
	}
	std::string getMonth() const
	{
		return toStringDate(systime_.wMonth);
	}
private:
	void convert()
	{
		bConverted_ = ::FileTimeToSystemTime(&filetime_, &systime_);
	}


};



//namespace StructStorage
//{

	namespace idNames
	{
		const std::string Title = "Title";
		const std::string Subject = "Subject";
		const std::string Author = "Author";
		const std::string Keywords = "Keywords";
		const std::string Comments = "Comments";
		const std::string Template = "Template";
		const std::string LastSavedBy = "Last Saved By";
		const std::string RevisionNumber = "Revision Number";
		const std::string TotalEditingTime = "Total Editing Time";
		const std::string LastPrinted = "Last Printed";
		const std::string CreateTime = "Create Time/Date";
		const std::string LastSavedTime = "Last saved Time/Date";
		const std::string NumberOfPages = "Number of Pages";
		const std::string NumberOfWords = "Number of Words";
		const std::string NumberOfCharacters = "Number of Characters";
		const std::string Thumbnail = "Thumbnail";
		const std::string NameOfCreatingApplication = "Name of Creating Application";
		const std::string Security = "Security";

		const std::string Unknown = "Unknown";
	};

	inline std::string SummaryInformationName(PROPID prop_id)
	{
		switch (prop_id)
		{
		case PIDSI_TITLE:
			return idNames::Title;
		case PIDSI_SUBJECT:
			return idNames::Subject;
		case PIDSI_AUTHOR:
			return idNames::Author;
		case PIDSI_KEYWORDS:
			return idNames::Keywords;
		case PIDSI_COMMENTS:
			return idNames::Comments;
		case PIDSI_TEMPLATE:
			return idNames::Template;
		case PIDSI_LASTAUTHOR:
			return idNames::LastSavedBy;
		case PIDSI_REVNUMBER:
			return idNames::RevisionNumber;
		case PIDSI_EDITTIME:
			return idNames::TotalEditingTime;
		case PIDSI_LASTPRINTED:
			return idNames::LastPrinted;
		case PIDSI_CREATE_DTM:
			return idNames::CreateTime;
		case PIDSI_LASTSAVE_DTM:
			return idNames::LastSavedTime;
		case PIDSI_PAGECOUNT:
			return idNames::NumberOfPages;
		case PIDSI_WORDCOUNT:
			return idNames::NumberOfWords;
		case PIDSI_CHARCOUNT:
			return idNames::NumberOfCharacters;
		case PIDSI_THUMBNAIL:
			return idNames::Thumbnail;
		case PIDSI_APPNAME:
			return idNames::NameOfCreatingApplication;
		case PIDSI_DOC_SECURITY:
			return idNames::Security;
		default:
			return idNames::Unknown;

		};

	}



	class LIBSTRUCTSTORAGE_API SummaryInformation
	{
	public:

		SummaryInformation();
		void setTitile(const std::string & title);
		std::string title() const;
		void setSubject(const std::string & subject);
		std::string subject() const;
		void setAuthor(const std::string & author);
		std::string author() const;
		void setKeywords(const std::string & keywords);
		std::string keywords() const;
		void setComments(const std::string & comments);
		std::string comments() const;
		void setTemplate(const std::string & strTemplate);
		std::string getTemplate() const;
		void setLastSavedBy(const std::string & LastSavedBy);
		std::string LastSavedBy() const;
		void setRevisionNumber(const std::string & RevisionNumber);
		std::string RevisionNumber() const;
		void setTotalEditingTime(const FILETIME & file_time);
		FileDateTime totalEditTime() const;
		void setLastPrintedTime(const FILETIME & file_time);
		FileDateTime lastPrintedTime() const;
		void setCreateTime(const FILETIME & file_time);
		FileDateTime createTime() const;
		void setLastSavedTime(const FILETIME & file_time);
		FileDateTime lastSavedTime() const;
	private:
		std::string Title_;
		std::string Subject_;
		std::string Author_;
		std::string Keywords_;
		std::string Comments_;
		std::string Template_;
		std::string LastSavedBy_;
		std::string RevisionNumber_;
		FileDateTime TotalEditTime_;
		FileDateTime LastPrinted_;
		FileDateTime CreateTime_;
		FileDateTime LastSavedTime_;

	};


	class LIBSTRUCTSTORAGE_API SSReader
	{
	private:
		IPropertySetStorage * toPropertySetStorage(IStorage * pStorage);

		// Open the document as an OLE compound document.

	public:
		SSReader();
		~SSReader();
		IStorage * open_storage(const std::wstring & file);
		bool read_storage(IStorage * pStorage)
		{
			IEnumSTATSTG *iEnumStg = NULL;
			if (FAILED(pStorage->EnumElements(0, nullptr, 0, &iEnumStg)))
				return false;

			const OLECHAR * wordDocumentName = L"WordDocument";

			STATSTG statstg = { 0 };
			wprintf(L"\n\nSTART ENUM\n");
			auto hr = iEnumStg->Next(1, &statstg, NULL);
			bool bOpenResult = false;
			IStream *pStream = NULL;
			while (hr == S_OK)
			{
				wprintf(L"%s\n", statstg.pwcsName);

				switch (statstg.type)
				{
				case STGTY_STREAM:
					if  (wcscmp(statstg.pwcsName , wordDocumentName) == 0 )
					{
						if (SUCCEEDED(pStorage->OpenStream(statstg.pwcsName, NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream)))
						{
							auto strream_size = statstg.cbSize.QuadPart;
							if (SUCCEEDED(ReadStream(pStream, strream_size)))
							{
								
								bOpenResult = true;
								return bOpenResult;
							}
							else
							{
								wprintf(L"Error read stream.\n");
								return false;
							}

						}
						else
							return false;
					}
					break;
				//case STGTY_STORAGE:
				//	// read storage inside
				//	break;

				default:
					break;
				}

				hr = iEnumStg->Next(1, &statstg, nullptr);
			}
			return bOpenResult;
		}

		HRESULT ReadStream( IStream * pStream, uint64_t size )
		{
			//IO::Buffer buffer(size);
			uint8_t * data = new uint8_t[size];
			DWORD bytes_read = 0;
			auto hr = pStream->Read(data, size, &bytes_read);
			delete[] data;
			return hr;
		}

		// read properties of a property storage
		bool read_properties(const std::string & file_path, SummaryInformation & summary_information);

		void setSummaryInformation(const PROPSPEC * pPropSpec, const PROPVARIANT * pPropVar, int nCount, SummaryInformation & summary_info);

	};

//}


#endif
