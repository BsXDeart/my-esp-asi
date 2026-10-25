#include <windows.h>
#include <psapi.h>

bool g_bESPEnabled = false;
bool g_bKeyHandled = false;

// ฟังก์ชันค้นหา Memory Pattern (Pattern Scanning) เพื่อรองรับ SAMP
BYTE* FindPattern(HMODULE hModule, const char* pattern, const char* mask) {
    MODULEINFO modInfo;
    GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
    DWORD base = (DWORD)hModule;
    DWORD size = modInfo.SizeOfImage;
    DWORD patternLength = (DWORD)strlen(mask);

    for (DWORD i = 0; i < size - patternLength; i++) {
        bool found = true;
        for (DWORD j = 0; j < patternLength; j++) {
            found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
        }
        if (found) return (BYTE*)(base + i);
    }
    return nullptr;
}

void ProcessESPName() {
    // เช็คการกดปุ่ม Ctrl + L
    if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState('L') & 0x8000)) {
        if (!g_bKeyHandled) {
            g_bESPEnabled = !g_bESPEnabled;
            g_bKeyHandled = true;
        }
    } else {
        g_bKeyHandled = false;
    }

    HMODULE hSamp = GetModuleHandleA("samp.dll");
    if (!hSamp) return;

    // ค้นหาจุด Render Distance/Line of Sight ของ NameTag ใน samp.dll
    static BYTE* pAddress = nullptr;
    if (!pAddress) {
        pAddress = FindPattern(hSamp, "\x75\x36\x8B\x44\x24\x14", "xxxxxx");
    }

    // แก้ไข Memory (Nop / Patch) เพื่อบังคับให้แสดงชื่อตลอดเวลาเมื่อเปิด g_bESPEnabled
    if (pAddress) {
        DWORD oldProtect;
        VirtualProtect(pAddress, 2, PAGE_EXECUTE_READWRITE, &oldProtect);
        if (g_bESPEnabled) {
            *(WORD*)pAddress = 0x9090; // NOP (เปิด ESP แสดงชื่อทะลุกำแพง/ระยะไกล)
        } else {
            *(WORD*)pAddress = 0x3675; // Restore Original Bytes (คืนค่าเดิม)
        }
        VirtualProtect(pAddress, 2, oldProtect, &oldProtect);
    }
}

// Thread หลักสำหรับรันลูปตรวจสอบ
DWORD WINAPI MainThread(LPVOID lpParam) {
    while (true) {
        ProcessESPName();
        Sleep(10);
    }
    return 0;
}

// จุดเริ่มต้นของไฟล์ .asi (DLL Entry Point)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
