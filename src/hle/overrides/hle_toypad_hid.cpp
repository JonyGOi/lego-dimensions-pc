// Fase 61: Toy Pad real pela API de HID do Windows. Ver hle_toypad_hid.h.
#include "hle/overrides/hle_toypad_hid.h"

#include <windows.h>

#include <hidsdi.h>
#include <setupapi.h>

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "setupapi.lib")

namespace hle::toypadhid {

namespace {

constexpr USHORT kVendor = 0x0E6F, kProduct = 0x0241;

HANDLE g_device = INVALID_HANDLE_VALUE;
u32 g_input_bytes = 33, g_output_bytes = 33; // com o byte do id de relatorio (0)
std::mutex g_write_mutex;
std::atomic<bool> g_open{false};
std::atomic<u64> g_written{0}, g_read{0};

void Reader(PacketCallback callback) {
    std::vector<u8> buffer(g_input_bytes);
    OVERLAPPED overlapped{};
    overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    while (g_open.load()) {
        ResetEvent(overlapped.hEvent);
        DWORD got = 0;
        if (!ReadFile(g_device, buffer.data(), g_input_bytes, &got, &overlapped)) {
            if (GetLastError() != ERROR_IO_PENDING) {
                break;
            }
            if (!GetOverlappedResult(g_device, &overlapped, &got, TRUE)) {
                break;
            }
        }
        if (got >= 2) {
            ++g_read;
            u8 packet[32] = {};
            const u32 n = std::min<u32>(32, got - 1);
            std::memcpy(packet, buffer.data() + 1, n); // pula o id de relatorio
            callback(packet, 32);
        }
    }
    std::printf("[TOYPAD-USB] o portal real parou de responder (desconectado?) depois de %llu leituras\n",
                static_cast<unsigned long long>(g_read.load()));
    g_open = false;
    CloseHandle(overlapped.hEvent);
}

} // namespace

bool Open(std::string* description) {
    GUID hid_guid;
    HidD_GetHidGuid(&hid_guid);
    HDEVINFO devices = SetupDiGetClassDevsW(&hid_guid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (devices == INVALID_HANDLE_VALUE) {
        *description = "lista de aparelhos HID indisponivel";
        return false;
    }
    bool found = false;
    SP_DEVICE_INTERFACE_DATA interface{sizeof(interface)};
    for (DWORD i = 0; !found && SetupDiEnumDeviceInterfaces(devices, nullptr, &hid_guid, i, &interface); ++i) {
        DWORD size = 0;
        SetupDiGetDeviceInterfaceDetailW(devices, &interface, nullptr, 0, &size, nullptr);
        std::vector<u8> storage(size);
        auto* detail = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(storage.data());
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        if (!SetupDiGetDeviceInterfaceDetailW(devices, &interface, detail, size, nullptr, nullptr)) {
            continue;
        }
        // So os atributos (sem acesso de leitura / escrita: funciona ate com o aparelho aberto por outro programa)
        HANDLE probe = CreateFileW(detail->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if (probe == INVALID_HANDLE_VALUE) {
            continue;
        }
        HIDD_ATTRIBUTES attributes{sizeof(attributes)};
        const bool match = HidD_GetAttributes(probe, &attributes) && attributes.VendorID == kVendor && attributes.ProductID == kProduct;
        CloseHandle(probe);
        if (!match) {
            continue;
        }
        HANDLE device = CreateFileW(detail->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                    OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
        if (device == INVALID_HANDLE_VALUE) {
            char text[160];
            std::snprintf(text, sizeof(text), "Toy Pad real achado, mas nao abriu (erro %lu: outro programa usando o portal?)", GetLastError());
            *description = text;
            continue;
        }
        PHIDP_PREPARSED_DATA preparsed = nullptr;
        if (HidD_GetPreparsedData(device, &preparsed)) {
            HIDP_CAPS caps{};
            if (HidP_GetCaps(preparsed, &caps) == HIDP_STATUS_SUCCESS) {
                g_input_bytes = std::max<u32>(caps.InputReportByteLength, 2);
                g_output_bytes = std::max<u32>(caps.OutputReportByteLength, 2);
            }
            HidD_FreePreparsedData(preparsed);
        }
        g_device = device;
        g_open = true;
        found = true;
        char text[200];
        std::snprintf(text, sizeof(text), "Toy Pad real aberto (relatorios de %u / %u bytes)", g_input_bytes, g_output_bytes);
        *description = text;
    }
    SetupDiDestroyDeviceInfoList(devices);
    if (!found && description->empty()) {
        *description = "nenhum Toy Pad real ligado (se ele estiver ligado e o driver WinUSB do Zadig estiver instalado, "
                       "o portal nao aparece como HID: volte o driver para o padrao do Windows)";
    }
    return found;
}

void Start(PacketCallback callback) {
    if (g_open.load()) {
        std::thread(Reader, callback).detach();
    }
}

bool Write(const u8* data, u32 length) {
    if (!g_open.load()) {
        return false;
    }
    std::lock_guard lock(g_write_mutex);
    std::vector<u8> report(g_output_bytes, 0); // [0] = id de relatorio 0
    std::memcpy(report.data() + 1, data, std::min<u32>(length, g_output_bytes - 1));
    OVERLAPPED overlapped{};
    overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    DWORD written = 0;
    bool ok = WriteFile(g_device, report.data(), g_output_bytes, &written, &overlapped) != 0;
    if (!ok && GetLastError() == ERROR_IO_PENDING) {
        ok = WaitForSingleObject(overlapped.hEvent, 1000) == WAIT_OBJECT_0 && GetOverlappedResult(g_device, &overlapped, &written, FALSE);
        if (!ok) {
            CancelIoEx(g_device, &overlapped);
        }
    }
    if (!ok) { // alguns aparelhos HID so aceitam o relatorio de saida pelo canal de controle
        ok = HidD_SetOutputReport(g_device, report.data(), g_output_bytes) != 0;
    }
    CloseHandle(overlapped.hEvent);
    g_written += ok;
    return ok;
}

bool IsOpen() {
    return g_open.load();
}

} // namespace hle::toypadhid
