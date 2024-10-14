#include "archive_reader_factory.h"

ArchiveReaderFactory *gArchiveReaderFactory;

ArchiveReaderFactory::ArchiveReaderFactory()
{
    _zip = new ArchiveReaderZip();
    _7z = new ArchiveReader7z();
}

ArchiveReaderFactory::~ArchiveReaderFactory()
{
    delete _zip;
    delete _7z;
}

ArchiveReader *ArchiveReaderFactory::Get(const char *path)
{
    std::string ext = File::GetExt(path);
    if (ext == "zip")
    {
        return _zip;
    }
    else if (ext == "7z")
    {
        return _7z;
    }
    return nullptr;
};