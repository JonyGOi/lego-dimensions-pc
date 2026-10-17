# shader_compiler_tool - recompilacao estatica (AOT) dos shaders GCN extraidos para SPIR-V.
#
# Usa o shader_recompiler do shadPS4 direto do checkout local (SHADPS4_DIR): os fontes do
# recompilador sao compilados aqui, e as bibliotecas externas dele (sirit, fmt, spdlog, Zydis)
# sao as ja compiladas na pasta build/ do shadPS4 (Release, clang-cl, /MD). O shadPS4 e GPL-2.0;
# este alvo liga codigo dele, entao o executavel resultante tambem e GPL-2.0.
cmake_minimum_required(VERSION 3.24)
project(shader_compiler_tool CXX)

set(SHADPS4_DIR "C:/src/shadPS4" CACHE PATH "checkout do shadPS4 (com a pasta build/ ja compilada)")
set(SHADPS4_BUILD "${SHADPS4_DIR}/build" CACHE PATH "pasta de build do shadPS4")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

# O pouco do resto do shadPS4 que o recompilador chama de verdade.
set(SHADPS4_EXTRA_SOURCES
    ${SHADPS4_DIR}/src/video_core/amdgpu/pixel_format.cpp
    ${SHADPS4_DIR}/src/common/decoder.cpp
    ${SHADPS4_DIR}/src/common/signal_context.cpp
)

file(GLOB_RECURSE RECOMPILER_SOURCES CONFIGURE_DEPENDS "${SHADPS4_DIR}/src/shader_recompiler/*.cpp")
# Fase 51: arquivos do recompilador com mudancas do port (pasta override/) no lugar dos originais
list(FILTER RECOMPILER_SOURCES EXCLUDE REGEX "/ir/passes/resource_patching_pass\\.cpp$")
list(APPEND RECOMPILER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/override/resource_patching_pass.cpp)

add_executable(shader_compiler_tool
    main.cpp
    shadps4_stubs.cpp
    ${RECOMPILER_SOURCES}
    ${SHADPS4_EXTRA_SOURCES}
)

target_include_directories(shader_compiler_tool PRIVATE
    ${SHADPS4_DIR}/src
    ${SHADPS4_DIR}
    ${SHADPS4_BUILD}/src/video_core/host_shaders/include
    ${SHADPS4_BUILD}/include
)
target_include_directories(shader_compiler_tool SYSTEM PRIVATE
    ${SHADPS4_DIR}/externals/spdlog/include
    ${SHADPS4_BUILD}/_deps/fmt-src/include
    ${SHADPS4_DIR}/externals/magic_enum/include
    ${SHADPS4_DIR}/externals/toml11/include
    ${SHADPS4_DIR}/externals/json/include
    ${SHADPS4_DIR}/externals/robin-map/include
    ${SHADPS4_DIR}/externals/xbyak
    ${SHADPS4_DIR}/externals/gcn/include
    ${SHADPS4_DIR}/externals/ext-boost
    ${SHADPS4_DIR}/externals/vma/include
    ${SHADPS4_DIR}/externals/sirit/include
    ${SHADPS4_DIR}/externals/sirit/externals/SPIRV-Headers/include
    ${SHADPS4_DIR}/externals/vulkan-headers/include
    ${SHADPS4_DIR}/externals/xxhash
    ${SHADPS4_DIR}/externals/zydis/include
    ${SHADPS4_BUILD}/externals/zydis
    ${SHADPS4_DIR}/externals/zydis/dependencies/zycore/include
    ${SHADPS4_BUILD}/externals/zydis/zycore
    ${SHADPS4_DIR}/externals/half/include
    ${SHADPS4_DIR}/externals/tracy/public
)

target_compile_definitions(shader_compiler_tool PRIVATE
    NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE _SCL_SECURE_NO_WARNINGS
    NTDDI_VERSION=0x0A000006 WINVER=0x0A00 _WIN32_WINNT=0x0A00 _TIMESPEC_DEFINED
    SPDLOG_FUNCTION=__func__
    ZYCORE_STATIC_BUILD ZYDIS_STATIC_BUILD BOOST_ASIO_STANDALONE
)
target_compile_options(shader_compiler_tool PRIVATE /EHsc /utf-8 /wd4201 -march=x86-64-v3 -w)

target_link_libraries(shader_compiler_tool PRIVATE
    ${SHADPS4_BUILD}/externals/sirit/src/sirit.lib
    ${SHADPS4_BUILD}/_deps/fmt-build/fmt.lib
    ${SHADPS4_BUILD}/externals/spdlog/spdlog.lib
    ${SHADPS4_BUILD}/externals/zydis/Zydis.lib
    ${SHADPS4_BUILD}/externals/zydis/zycore/Zycore.lib
)
