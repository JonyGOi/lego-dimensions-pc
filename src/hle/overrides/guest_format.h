#pragma once

#include <string>

#include "hle/overrides/sysv_va.h"

namespace hle {

// printf do jogo: tipos LP64 (long = 64 bits) e argumentos lidos de um va_list System V.
std::string GuestFormat(const char* fmt, SysVVaList& ap);

// sscanf do jogo. Retorna o numero de conversoes atribuidas, ou -1 (EOF).
int GuestScan(const char* input, const char* fmt, SysVVaList& ap);

} // namespace hle
