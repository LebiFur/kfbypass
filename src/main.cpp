#include <Windows.h>

HANDLE handle = 0;

void WriteNops(void* address, size_t offset, size_t length)
{
    byte* bytes = new byte[length];
    for (size_t i = 0; i < length; i++) bytes[i] = 0x90;
    
    WriteProcessMemory(handle, (byte*)address + offset, bytes, length, 0);
}

void Write(void* address, size_t offset, byte value)
{
    WriteProcessMemory(handle, (byte*)address + offset, &value, 1, 0);
}

void Start()
{
    handle = GetCurrentProcess();
    void* engine = (void*)GetModuleHandleA("Engine.dll");

    //Engine.ASteamStatsAndAchievementsBase::execFlushStatToSteamInt
    WriteNops(engine, 0x000BF390 + 0xE4, 6);

    //Engine.ASteamStatsAndAchievementsBase::execFlushStatToSteamFloat
    WriteNops(engine, 0x000BF5D0 + 0xE4, 6);

    //Engine.ASteamStatsAndAchievementsBase::execFlushStatsToSteamDatabase
    WriteNops(engine, 0x000BFAD0 + 0x55, 2);

    //Engine.ASteamStatsAndAchievementsBase::FlushAchievementToSteam
    WriteNops(engine, 0x000BD250 + 0x37, 6);

    //Engine.Trip_Steam::IsDLCAvailable
    Write(engine, 0x0024D860, 0xC2);
    Write(engine, 0x0024D860 + 1, 0x01);
    Write(engine, 0x0024D860 + 2, 0x00);
    WriteNops(engine, 0x0024D860 + 3, 26);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
    switch(dwReason)
    { 
        case DLL_PROCESS_ATTACH:
            Start();
            break;
    }

    return TRUE;
}