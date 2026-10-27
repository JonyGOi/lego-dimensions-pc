"""Asset-free configuration checks: bundled headers, SDK target, missing input."""
import argparse
from pathlib import Path
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cmake', default='cmake')
    args = parser.parse_args()
    module = (Path(__file__).resolve().parents[1] / 'cmake/VulkanHeaders.cmake').as_posix()
    with tempfile.TemporaryDirectory(prefix='ld-vulkan-') as temporary:
        root = Path(temporary)
        for name, settings, succeeds, expected in (
            ('bundled', 'set(CMAKE_DISABLE_FIND_PACKAGE_Vulkan TRUE)', True, 'Vulkan headers: bundled'),
            ('missing', 'set(CMAKE_DISABLE_FIND_PACKAGE_Vulkan TRUE)\nset(LD_BUNDLED_VULKAN_HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/missing")', False, 'Vulkan headers are missing'),
            ('sdk', 'list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")', True, 'Vulkan headers: test-sdk'),
        ):
            source = root/name; source.mkdir()
            if name == 'sdk':
                (source/'modules').mkdir()
                (source/'modules/FindVulkan.cmake').write_text('add_library(Vulkan::Headers INTERFACE IMPORTED)\nset(Vulkan_INCLUDE_DIRS test-sdk)\nset(Vulkan_VERSION test-version)\n')
            (source/'CMakeLists.txt').write_text('cmake_minimum_required(VERSION 3.24)\nproject(HeaderCheck NONE)\n'+settings+'\ninclude("'+module+'")\nif(NOT TARGET Vulkan::Headers)\nmessage(FATAL_ERROR "No header target")\nendif()\n')
            result = subprocess.run([args.cmake,'-S',str(source),'-B',str(source/'build')],capture_output=True,text=True)
            output = result.stdout+result.stderr
            if (result.returncode == 0) != succeeds or expected not in output:
                raise RuntimeError(name+' failed:\n'+output)
            print(name+': passed')


if __name__ == '__main__':
    main()
