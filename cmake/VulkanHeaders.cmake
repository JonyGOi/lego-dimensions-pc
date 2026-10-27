# The game loads vulkan-1.dll at runtime. Compilation needs headers only;
# the shader translator has separate SDK/toolchain requirements.
find_package(Vulkan QUIET)
if(TARGET Vulkan::Headers)
    message(STATUS "Vulkan headers: ${Vulkan_INCLUDE_DIRS} (SDK ${Vulkan_VERSION})")
else()
    set(LD_BUNDLED_VULKAN_HEADERS_DIR
        "${CMAKE_CURRENT_LIST_DIR}/../externals/vulkan-headers/include"
        CACHE PATH "Bundled Vulkan header directory used when no SDK headers target is available")
    if(NOT EXISTS "${LD_BUNDLED_VULKAN_HEADERS_DIR}/vulkan/vulkan.h" OR
       NOT EXISTS "${LD_BUNDLED_VULKAN_HEADERS_DIR}/vulkan/vulkan_core.h")
        message(FATAL_ERROR "Vulkan headers are missing. Restore externals/vulkan-headers or install the Vulkan SDK.")
    endif()
    add_library(ld_vulkan_headers INTERFACE)
    target_include_directories(ld_vulkan_headers SYSTEM INTERFACE "${LD_BUNDLED_VULKAN_HEADERS_DIR}")
    add_library(Vulkan::Headers ALIAS ld_vulkan_headers)
    message(STATUS "Vulkan headers: bundled (${LD_BUNDLED_VULKAN_HEADERS_DIR}); no SDK library is linked")
endif()
