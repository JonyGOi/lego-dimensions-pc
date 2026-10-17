// GERADO por tools/gen_hle_stubs.py - NAO EDITAR. Alteracoes manuais serao perdidas.

#include "hle_symbol_table.h"

#include <vector>

namespace hle {
namespace gen {
namespace libc { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libkernel { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceAppContent { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceAudioOut { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceAvPlayer { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceCommonDialog { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceCoredump { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceFios2 { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceGameCustomDataDialog { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceGameLiveStreaming { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceGnmDriver { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceHttp { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceInvitationDialog { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceMsgDialog { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNet { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNetCtl { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNgs2 { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpAuth { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpCommerce { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpCommon { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpManager { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpMatching2 { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpScore { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpSns { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpSnsFacebookDialog { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpTrophy { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpTus { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpUtility { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceNpWebApi { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libScePad { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libScePlayGo { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libScePosix { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceRemoteplay { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceRtc { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceSaveData { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceSaveDataDialog { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceScreenShot { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceSsl { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceSysmodule { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceSystemService { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceUsbd { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceUserService { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceVideoOut { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceVideoRecording { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
namespace libSceWebBrowserDialog { extern const HleSymbol kSymbols[]; extern const std::size_t kSymbolCount; }
} // namespace gen

namespace {

struct LibraryTable {
    const HleSymbol* symbols;
    std::size_t count;
};

const LibraryTable kLibraries[] = {
    {gen::libc::kSymbols, gen::libc::kSymbolCount},
    {gen::libkernel::kSymbols, gen::libkernel::kSymbolCount},
    {gen::libSceAppContent::kSymbols, gen::libSceAppContent::kSymbolCount},
    {gen::libSceAudioOut::kSymbols, gen::libSceAudioOut::kSymbolCount},
    {gen::libSceAvPlayer::kSymbols, gen::libSceAvPlayer::kSymbolCount},
    {gen::libSceCommonDialog::kSymbols, gen::libSceCommonDialog::kSymbolCount},
    {gen::libSceCoredump::kSymbols, gen::libSceCoredump::kSymbolCount},
    {gen::libSceFios2::kSymbols, gen::libSceFios2::kSymbolCount},
    {gen::libSceGameCustomDataDialog::kSymbols, gen::libSceGameCustomDataDialog::kSymbolCount},
    {gen::libSceGameLiveStreaming::kSymbols, gen::libSceGameLiveStreaming::kSymbolCount},
    {gen::libSceGnmDriver::kSymbols, gen::libSceGnmDriver::kSymbolCount},
    {gen::libSceHttp::kSymbols, gen::libSceHttp::kSymbolCount},
    {gen::libSceInvitationDialog::kSymbols, gen::libSceInvitationDialog::kSymbolCount},
    {gen::libSceMsgDialog::kSymbols, gen::libSceMsgDialog::kSymbolCount},
    {gen::libSceNet::kSymbols, gen::libSceNet::kSymbolCount},
    {gen::libSceNetCtl::kSymbols, gen::libSceNetCtl::kSymbolCount},
    {gen::libSceNgs2::kSymbols, gen::libSceNgs2::kSymbolCount},
    {gen::libSceNpAuth::kSymbols, gen::libSceNpAuth::kSymbolCount},
    {gen::libSceNpCommerce::kSymbols, gen::libSceNpCommerce::kSymbolCount},
    {gen::libSceNpCommon::kSymbols, gen::libSceNpCommon::kSymbolCount},
    {gen::libSceNpManager::kSymbols, gen::libSceNpManager::kSymbolCount},
    {gen::libSceNpMatching2::kSymbols, gen::libSceNpMatching2::kSymbolCount},
    {gen::libSceNpScore::kSymbols, gen::libSceNpScore::kSymbolCount},
    {gen::libSceNpSns::kSymbols, gen::libSceNpSns::kSymbolCount},
    {gen::libSceNpSnsFacebookDialog::kSymbols, gen::libSceNpSnsFacebookDialog::kSymbolCount},
    {gen::libSceNpTrophy::kSymbols, gen::libSceNpTrophy::kSymbolCount},
    {gen::libSceNpTus::kSymbols, gen::libSceNpTus::kSymbolCount},
    {gen::libSceNpUtility::kSymbols, gen::libSceNpUtility::kSymbolCount},
    {gen::libSceNpWebApi::kSymbols, gen::libSceNpWebApi::kSymbolCount},
    {gen::libScePad::kSymbols, gen::libScePad::kSymbolCount},
    {gen::libScePlayGo::kSymbols, gen::libScePlayGo::kSymbolCount},
    {gen::libScePosix::kSymbols, gen::libScePosix::kSymbolCount},
    {gen::libSceRemoteplay::kSymbols, gen::libSceRemoteplay::kSymbolCount},
    {gen::libSceRtc::kSymbols, gen::libSceRtc::kSymbolCount},
    {gen::libSceSaveData::kSymbols, gen::libSceSaveData::kSymbolCount},
    {gen::libSceSaveDataDialog::kSymbols, gen::libSceSaveDataDialog::kSymbolCount},
    {gen::libSceScreenShot::kSymbols, gen::libSceScreenShot::kSymbolCount},
    {gen::libSceSsl::kSymbols, gen::libSceSsl::kSymbolCount},
    {gen::libSceSysmodule::kSymbols, gen::libSceSysmodule::kSymbolCount},
    {gen::libSceSystemService::kSymbols, gen::libSceSystemService::kSymbolCount},
    {gen::libSceUsbd::kSymbols, gen::libSceUsbd::kSymbolCount},
    {gen::libSceUserService::kSymbols, gen::libSceUserService::kSymbolCount},
    {gen::libSceVideoOut::kSymbols, gen::libSceVideoOut::kSymbolCount},
    {gen::libSceVideoRecording::kSymbols, gen::libSceVideoRecording::kSymbolCount},
    {gen::libSceWebBrowserDialog::kSymbols, gen::libSceWebBrowserDialog::kSymbolCount},
};

constexpr ModuleAlias kModuleAliases[] = {
    {"libSceGnmDriver_padebug", "libSceGnmDriver"},
};

std::vector<HleSymbol>& AllSymbols() {
    static std::vector<HleSymbol> all = [] {
        std::vector<HleSymbol> v;
        v.reserve(596);
        for (const auto& lib : kLibraries) {
            v.insert(v.end(), lib.symbols, lib.symbols + lib.count);
        }
        return v;
    }();
    return all;
}

} // namespace

std::span<const HleSymbol> Symbols() {
    return AllSymbols();
}

const std::unordered_map<std::string_view, const HleSymbol*>& SymbolMap() {
    static const std::unordered_map<std::string_view, const HleSymbol*> map = [] {
        std::unordered_map<std::string_view, const HleSymbol*> m;
        m.reserve(AllSymbols().size());
        for (const auto& sym : AllSymbols()) {
            m.emplace(sym.nid, &sym);
        }
        return m;
    }();
    return map;
}

const HleSymbol* FindSymbolByName(std::string_view name) {
    for (const auto& sym : AllSymbols()) {
        if (sym.name == name) {
            return &sym;
        }
    }
    return nullptr;
}

bool OverrideSymbol(std::string_view name, void* address) {
    for (auto& sym : AllSymbols()) {
        if (sym.name == name) {
            sym.address = address;
            sym.overridden = true;
            return true;
        }
    }
    return false;
}

const HleSymbol* FindSymbol(std::string_view nid) {
    const auto& map = SymbolMap();
    const auto it = map.find(nid);
    return it != map.end() ? it->second : nullptr;
}

void* ResolveNid(std::string_view nid) {
    const HleSymbol* sym = FindSymbol(nid);
    return sym ? sym->address : nullptr;
}

std::span<const ModuleAlias> ModuleAliases() {
    return std::span<const ModuleAlias>(kModuleAliases, 1);
}

std::string_view CanonicalModule(std::string_view needed) {
    for (const auto& alias : ModuleAliases()) {
        if (alias.needed == needed) {
            return alias.canonical;
        }
    }
    return needed;
}

} // namespace hle
