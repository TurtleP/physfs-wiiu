#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <sysapp/launch.h>

#include <whb/log.h>
#include <whb/log_cafe.h>
#include <whb/log_console.h>
#include <whb/log_udp.h>
#include <whb/proc.h>

#include <vpad/input.h>

#include "filesystem.h"

#include <string.h>
#include <unistd.h>

#include <string>
#include <vector>

static void listDirItems(const char* directory)
{
    std::vector<std::string> items;
    FileSystem::GetDirectoryItems(directory, items);

    WHBLogPrintf("Directory listing of %s", directory);
    for (const auto& filename : items)
        WHBLogPrintf("  -> %s", filename);
}

static const char* getInfoType(const FileSystem::Info& info)
{
    switch (info.type)
    {
        case FileSystem::FileType_File:
            return "file";
        case FileSystem::FileType_Directory:
            return "directory";
        case FileSystem::FileType_SymLink:
            return "symlink";
        case FileSystem::FileType_Other:
            return "other";
        default:
            return "";
    }
}

static void getInfo(const char* filepath)
{
    FileSystem::Info info;

    bool success = FileSystem::GetInfo(filepath, info);

    WHBLogPrintf("File Info of %s", filepath);

    if (success)
        WHBLogPrintf("  FileType: %s\n  File Size: %zu", getInfoType(info), info.size);
    else
        WHBLogPrintf("  Failed to get info for %s!", filepath);
}

int main(int argc, char** argv)
{
    WHBProcInit();
    WHBLogCafeInit();
    WHBLogUdpInit();

    VPADInit();

    WHBLogConsoleInit();

    if (!PHYSFS_init(NULL))
        WHBLogPrintf("physfs failure: %s!", FileSystem::GetPhysfsError());

    FileSystem::SetIdentity("game", true);

    /* create a directory */
    bool success = FileSystem::CreateDirectory("WEEYOU");
    WHBLogPrintf("Directory Created: %d", success);

    /* create a file and write to it */
    FileSystem::File file{};
    FileSystem::OpenFile(file, "ambulance.txt", FileSystem::FileMode_Write);

    const char* message = "HELLO WORLD!";
    size_t length       = strlen(message);

    FileSystem::WriteFile(file, message, length);
    success = FileSystem::CloseFile(file);
    WHBLogPrintf("File Closed: %d", success);

    /* open our file for reading */
    FileSystem::OpenFile(file, "ambulance.txt", FileSystem::FileMode_Read);
    char buffer[file.GetSize()];

    /* read the file contents and print them */
    FileSystem::ReadFile(file, buffer, file.GetSize());
    WHBLogPrintf("File Contents: %s", buffer);

    VPADStatus status;
    VPADReadError error;

    while (WHBProcIsRunning())
    {
        VPADRead(VPAD_CHAN_0, &status, 1, &error);

        if (status.trigger & VPAD_BUTTON_PLUS)
            SYSLaunchMenu();
        else if (status.trigger & VPAD_BUTTON_A)
            listDirItems("");
        else if (status.trigger & VPAD_BUTTON_B)
            getInfo("ambulance.txt");
        else if (status.trigger & VPAD_BUTTON_X)
            getInfo("WEEYOU");

        WHBLogConsoleDraw();
        OSSleepTicks(OSMillisecondsToTicks(100));
    }

    WHBLogPrintf("Exiting... good bye.");
    WHBLogConsoleDraw();
    OSSleepTicks(OSMillisecondsToTicks(1000));

    WHBLogConsoleFree();
    WHBProcShutdown();

    return 0;
}
