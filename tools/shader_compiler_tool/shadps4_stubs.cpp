// Substitutos minimos do resto do shadPS4 de que o shader_recompiler depende (log, asserts,
// configuracao, sinais). Nada aqui emula: so permite ligar o recompilador fora do emulador.
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>

#include <spdlog/sinks/ostream_sink.h>

#include "common/assert.h"
#include "common/io_file.h"
#include "common/logging/log.h"
#include "common/path_util.h"
#include "common/thread.h"
#include "core/emulator_settings.h"
#include "core/signals.h"

#include "tool_support.h"

// ------------------------------- log ---------------------------------------
// Todas as classes de log usadas pelo recompilador escrevem num buffer em memoria, que a
// ferramenta le depois de cada shader para saber o motivo de uma falha.
namespace Common::Log {
bool g_should_append = false;
std::unordered_map<std::string_view, std::shared_ptr<spdlog::logger>> ALL_LOGGERS{};
} // namespace Common::Log

namespace tool {

static std::ostringstream g_log_stream;

void InitLogCapture() {
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(g_log_stream);
    sink->set_pattern("%v");
    for (const char* name : {Common::Log::Class::Debug, Common::Log::Class::Render, Common::Log::Class::Render_Recompiler,
                             Common::Log::Class::Render_Vulkan, Common::Log::Class::Common}) {
        auto logger = std::make_shared<spdlog::logger>(name, sink);
        logger->set_level(spdlog::level::warn);
        Common::Log::ALL_LOGGERS[name] = logger;
    }
}

std::string TakeLog() {
    for (auto& [name, logger] : Common::Log::ALL_LOGGERS) {
        if (logger) {
            logger->flush();
        }
    }
    std::string text = g_log_stream.str();
    g_log_stream.str({});
    g_log_stream.clear();
    return text;
}

} // namespace tool

// ------------------------------ asserts ------------------------------------
// No emulador um assert derruba o processo; aqui vira excecao, e o shader conta como falha.
void assert_fail_impl() {
    throw tool::RecompilerAbort("assert");
}

[[noreturn]] void unreachable_impl() {
    throw tool::RecompilerAbort("unreachable");
}

// ---------------------------- configuracao ---------------------------------
std::shared_ptr<EmulatorSettingsImpl> EmulatorSettingsImpl::s_instance;
std::mutex EmulatorSettingsImpl::s_mutex;

EmulatorSettingsImpl::EmulatorSettingsImpl() = default;
EmulatorSettingsImpl::~EmulatorSettingsImpl() = default;

std::shared_ptr<EmulatorSettingsImpl> EmulatorSettingsImpl::GetInstance() {
    std::lock_guard lock(s_mutex);
    if (!s_instance) {
        s_instance = std::make_shared<EmulatorSettingsImpl>();
    }
    return s_instance;
}

// ------------------------------- diversos ----------------------------------
namespace Common {
std::string GetCurrentThreadName() {
    return "tool";
}
} // namespace Common

namespace Common::FS {
const std::filesystem::path& GetUserPath(PathType) {
    static const std::filesystem::path path = std::filesystem::temp_directory_path() / "shader_compiler_tool";
    return path;
}
} // namespace Common::FS

// IOFile so e usado pelo recompilador para gravar dumps de depuracao (desligados aqui).
namespace Common::FS {
IOFile::IOFile(const std::filesystem::path&, FileAccessMode, FileType, FileShareFlag) {}
IOFile::~IOFile() = default;
} // namespace Common::FS

namespace Core {
SignalDispatch::SignalDispatch() = default;
SignalDispatch::~SignalDispatch() = default;
} // namespace Core
