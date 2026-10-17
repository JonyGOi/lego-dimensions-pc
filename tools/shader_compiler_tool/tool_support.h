#pragma once

#include <stdexcept>
#include <string>

namespace tool {

// Lancada no lugar de ASSERT / UNREACHABLE do shadPS4.
struct RecompilerAbort : std::runtime_error {
    using std::runtime_error::runtime_error;
};

void InitLogCapture();
std::string TakeLog(); // texto de log (avisos e erros) acumulado desde a ultima chamada

} // namespace tool
