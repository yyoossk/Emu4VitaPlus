#pragma once
#include "archive_reader_zip.h"
#include "archive_reader_7z.h"

class ArchiveReaderFactory
{
public:
    ArchiveReaderFactory();
    virtual ~ArchiveReaderFactory();

    ArchiveReader *Get(const char *path);

private:
    ArchiveReaderZip *_zip;
    ArchiveReader7z *_7z;
};

extern ArchiveReaderFactory *gArchiveReaderFactory;