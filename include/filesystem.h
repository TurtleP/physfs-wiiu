#pragma once

#include <physfs.h>

#include <string>

namespace FileSystem
{
    enum FileMode
    {
        FileMode_Open,
        FileMode_Read,
        FileMode_Write,
        FileMode_Closed
    };

    struct File
    {
        PHYSFS_file* handle;
        FileMode mode;

        File()
        {
            this->handle = nullptr;
            this->mode   = FileMode_Closed;
        }

        int64_t GetSize()
        {
            if (this->handle == nullptr)
                return 0;

            return (int64_t)PHYSFS_fileLength(this->handle);
        }
    };

    /* gets the last physfs error */
    const char* GetPhysfsError();

    /* strips any duplicate slashes */
    std::string Normalize(const std::string& input);

    /* gets the user directory from physfs */
    std::string GetUserDirectory();

    /* gets the save directory */
    std::string GetSaveDirectory();

    /* sets up the writing directory for physfs */
    bool SetupWriteDirectory();

    /* creates a new directory */
    bool CreateDirectory(const char* name);

    bool CloseFile(File& file);

    /* creates a new file */
    bool OpenFile(File& file, const char* name, FileMode mode);

    /* writes to a file */
    bool WriteFile(File& file, const void* data, int64_t size);

    /* reads a file's content */
    int64_t ReadFile(File& file, void* destination, int64_t size);
}
