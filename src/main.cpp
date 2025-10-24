#include <Windows.h>
#include <inicpp.hpp>

void WriteNops(HANDLE handle, char *address, size_t length)
{
    char *bytes = new char[length];

    memset(bytes, 0x90, length);

    WriteProcessMemory(handle, address, bytes, length, 0);

    delete[] bytes;
}

void Write(HANDLE handle, char *address, const char *bytes, size_t length)
{
    WriteProcessMemory(handle, address, bytes, length, 0);
}

void Start()
{
    inicpp::IniManager config("kfbypass.ini");
    bool unlockDlcs = config["config"]["unlockDlcs"];
    config["config"]["unlockDlcs"] = unlockDlcs;

    HANDLE handle = GetCurrentProcess();
    char *engine = (char *)GetModuleHandleA("Engine.dll");

    //Engine.ASteamStatsAndAchievementsBase::execFlushStatToSteamInt
    WriteNops(handle, engine + 0x000BF390 + 0xE4, 6);

    //Engine.ASteamStatsAndAchievementsBase::execFlushStatToSteamFloat
    WriteNops(handle, engine + 0x000BF5D0 + 0xE4, 6);

    //Engine.ASteamStatsAndAchievementsBase::execFlushStatsToSteamDatabase
    WriteNops(handle, engine + 0x000BFAD0 + 0x55, 2);

    //Engine.ASteamStatsAndAchievementsBase::FlushAchievementToSteam
    WriteNops(handle, engine + 0x000BD250 + 0x37, 6);

    if (unlockDlcs)
    {
        //Engine.Trip_Steam::IsDLCAvailable
        const char bytes[] = { (char)0xC2, (char)0x01, (char)0x00 };
        Write(handle, engine + 0x0024D860, bytes, 3);
        WriteNops(handle, engine + 0x0024D860 + 3, 26);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID a)
{
    switch(dwReason)
    { 
        case DLL_PROCESS_ATTACH:
            Start();
            break;
    }

    return TRUE;
}