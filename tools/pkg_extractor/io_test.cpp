// SPDX-License-Identifier: GPL-2.0-or-later
#include "read_window.h"
#include <cstdio>
#include <filesystem>
#include <random>

int main() {
    const auto root=std::filesystem::temp_directory_path()/std::filesystem::path(L"ld-pkg-io-"+std::to_wstring(std::random_device{}()));
    if(!std::filesystem::create_directory(root)) {
        std::fputs("IO test directory already exists; preserved\n",stderr);return 1;
    }
    const auto path=root/L"unicode-\u03a9.bin";
    try {
        std::vector<std::uint8_t> expected(PkgReadWindow::kWindowBytes*2+193);
        for(size_t i=0;i<expected.size();++i)expected[i]=static_cast<std::uint8_t>((i*31u)^((i>>12)+13u));
        {
            Common::FS::IOFile out(path,Common::FS::FileAccessMode::Write);
            for(size_t at=0;at<expected.size();) {
                const size_t n=std::min<size_t>(65536,expected.size()-at);
                if(out.WriteRaw<std::uint8_t>(expected.data()+at,n)!=n)throw std::runtime_error("fixture write");
                at+=n;
            }
            if(!out.Close())throw std::runtime_error("fixture flush");
        }
        {
            Common::FS::IOFile file(path,Common::FS::FileAccessMode::Read);
            PkgReadWindow reader(file);
            std::vector<std::uint8_t> buffer(0x11000);
            for(size_t i=0;i<100;++i) {
                const size_t at=i*65536;
                if(reader.Read(at,buffer)!=buffer.size() || !std::equal(buffer.begin(),buffer.end(),expected.begin()+at))throw std::runtime_error("overlapping read mismatch");
            }
            if(reader.BackingReads()>3)throw std::runtime_error("sequential reads did not use read-ahead");
            for(size_t at : {size_t(15),PkgReadWindow::kWindowBytes-13,expected.size()-11,expected.size()}) {
                const size_t n=std::min(buffer.size(),expected.size()-at);
                if(reader.Read(at,buffer)!=n || !std::equal(buffer.begin(),buffer.begin()+n,expected.begin()+at) || !std::all_of(buffer.begin()+n,buffer.end(),[](auto v){return v==0;}))throw std::runtime_error("seek/EOF mismatch");
            }
            std::vector<std::uint8_t> too_large(PkgReadWindow::kWindowBytes+1);
            bool refused=false;try{reader.Read(0,too_large);}catch(const std::runtime_error&){refused=true;}
            if(!refused)throw std::runtime_error("oversized read accepted");
        }
        // Remove only the two exact test-created paths after handles close.
        std::filesystem::remove(path);std::filesystem::remove(root);
        std::puts("Buffered writes, Unicode paths, overlapping/cross-window reads, EOF and limits: passed");
        return 0;
    } catch(const std::exception& error) {
        std::fprintf(stderr,"IO test failed: %s\n",error.what());return 1;
    }
}
