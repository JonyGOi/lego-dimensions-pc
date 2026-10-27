// SPDX-License-Identifier: GPL-2.0-or-later
// Bounded read-ahead for overlapping 64 KiB PFS sectors in an immutable PKG.
#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <span>
#include <stdexcept>
#include <vector>
#include "io_file.h"

class PkgReadWindow {
public:
    static constexpr size_t kWindowBytes = 4 * 1024 * 1024;
    explicit PkgReadWindow(Common::FS::IOFile& input)
        : file(input), file_size(input.GetSize()), bytes(kWindowBytes) {
        if (!file.IsOpen()) throw std::runtime_error("Cannot open source PKG");
    }
    size_t Read(std::uint64_t offset, std::span<std::uint8_t> output) {
        if (output.size() > kWindowBytes) throw std::runtime_error("PKG read request exceeds window limit");
        if (output.empty()) return 0;
        std::fill(output.begin(), output.end(), 0);
        if (offset >= file_size) return 0;
        const size_t count = static_cast<size_t>(std::min<std::uint64_t>(output.size(), file_size-offset));
        if (offset < start || offset-start > valid || count > valid-static_cast<size_t>(offset-start)) {
            if (offset > static_cast<std::uint64_t>(std::numeric_limits<long long>::max()) || !file.Seek(static_cast<long long>(offset))) {
                throw std::runtime_error("Cannot seek source PKG");
            }
            start = offset;
            const size_t wanted = static_cast<size_t>(std::min<std::uint64_t>(bytes.size(),file_size-offset));
            valid = file.ReadRaw<std::uint8_t>(bytes.data(),wanted);
            ++backing_reads;
            if (valid != wanted) throw std::runtime_error("Source PKG changed or could not be read completely");
        }
        std::memcpy(output.data(),bytes.data()+static_cast<size_t>(offset-start),count);
        return count;
    }
    size_t BackingReads() const { return backing_reads; }
private:
    Common::FS::IOFile& file;
    std::uint64_t file_size=0,start=0;
    size_t valid=0,backing_reads=0;
    std::vector<std::uint8_t> bytes;
};
