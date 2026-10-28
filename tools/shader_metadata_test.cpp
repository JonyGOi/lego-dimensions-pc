#include "common/shader_metadata.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

void check(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}

int main(int argc, char** argv) {
    try {
    constexpr auto prefix = "pair vs 0123456789abcdef ps fedcba9876543210 variante 0011223344556677 fetch_sgpr 0 ";
    check(std::string(shader_metadata::DescriptorFlag(true))=="sim", "Compiler output must preserve canonical cache vocabulary");
    check(std::string(shader_metadata::DescriptorFlag(false))=="nao", "Unspecialized output must preserve canonical cache vocabulary");
    check(shader_metadata::HasActualDescriptors(std::string(prefix)+"real "+shader_metadata::DescriptorFlag(true)), "Compiler/runtime metadata round trip");
    check(!shader_metadata::HasActualDescriptors(std::string(prefix)+"real "+shader_metadata::DescriptorFlag(false)), "Unspecialized compiler output stays rejected");
    check(shader_metadata::HasActualDescriptors(std::string(prefix)+"real sim\n"), "Existing prepared shaders must retain actual descriptors");
    check(shader_metadata::HasActualDescriptors(std::string(prefix)+"real yes\r\n"), "Accept caches emitted by the early English-beta compiler");
    for (const auto* suffix : {"real nao", "real no", "real", "unreal sim", "real similar", "actual yes", "real sim real nao"})
        check(!shader_metadata::HasActualDescriptors(std::string(prefix)+suffix), "Reject absent, unsupported or ambiguous descriptor flags");
    check(!shader_metadata::HasActualDescriptors("stage vs real sim"), "Only pair metadata headers declare specialization");
    for (int i=1; i<argc; ++i) {
        std::ifstream input(argv[i]); std::string header;
        check(static_cast<bool>(std::getline(input,header)), "Read owner-supplied shader metadata");
        check(shader_metadata::HasActualDescriptors(header), "Actual prepared cache must be accepted by runtime metadata parser");
    }
    std::cout<<"Shader metadata compatibility checks passed\n";
    return 0;
    } catch (const std::exception& error) {
        std::cerr<<error.what()<<'\n';
        return 1;
    }
}
