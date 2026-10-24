// SPDX-License-Identifier: GPL-2.0-or-later
// LEGO Dimensions PC: o minimo de common/io_file.h do shadPS4 que o extrator de PKG usa (arquivos grandes, Unicode).
#pragma once

#include <cstdio>
#include <filesystem>
#include <type_traits>
#include <vector>

namespace Common::FS {

enum class FileAccessMode { Read, Write };
enum class SeekOrigin { SetOrigin, CurrentPosition, End };

template <typename T>
struct IsVector : std::false_type {};
template <typename T>
struct IsVector<std::vector<T>> : std::true_type {};

class IOFile {
public:
    IOFile() = default;
    IOFile(const std::filesystem::path& path, FileAccessMode mode) {
        Open(path, mode);
    }
    ~IOFile() {
        Close();
    }
    IOFile(const IOFile&) = delete;
    IOFile& operator=(const IOFile&) = delete;

    void Open(const std::filesystem::path& path, FileAccessMode mode) {
        Close();
        _wfopen_s(&file, path.c_str(), mode == FileAccessMode::Read ? L"rb" : L"wb");
    }
    void Close() {
        if (file) {
            std::fclose(file);
            file = nullptr;
        }
    }
    bool IsOpen() const {
        return file != nullptr;
    }
    unsigned long long GetSize() const {
        if (!file) {
            return 0;
        }
        const long long here = _ftelli64(file);
        _fseeki64(file, 0, SEEK_END);
        const long long size = _ftelli64(file);
        _fseeki64(file, here, SEEK_SET);
        return static_cast<unsigned long long>(size);
    }
    bool Seek(long long offset, SeekOrigin origin = SeekOrigin::SetOrigin) const {
        const int whence = origin == SeekOrigin::SetOrigin ? SEEK_SET : origin == SeekOrigin::CurrentPosition ? SEEK_CUR : SEEK_END;
        return file && _fseeki64(file, offset, whence) == 0;
    }
    long long Tell() const {
        return file ? _ftelli64(file) : -1;
    }

    template <typename T>
    size_t ReadRaw(void* data, size_t count) const {
        return file ? std::fread(data, sizeof(T), count, file) : 0;
    }
    template <typename T>
    size_t WriteRaw(const void* data, size_t count) const {
        return file ? std::fwrite(data, sizeof(T), count, file) : 0;
    }
    // Le / grava um valor (estrutura, array) ou o conteudo de um std::vector, byte a byte como esta no arquivo.
    template <typename T>
    size_t Read(T& value) const {
        if constexpr (IsVector<T>::value) {
            return ReadRaw<typename T::value_type>(value.data(), value.size());
        } else {
            return ReadRaw<T>(&value, 1);
        }
    }
    template <typename T>
    size_t Write(const T& value) const {
        if constexpr (IsVector<T>::value) {
            return WriteRaw<typename T::value_type>(value.data(), value.size());
        } else {
            return WriteRaw<T>(&value, 1);
        }
    }

private:
    std::FILE* file = nullptr;
};

} // namespace Common::FS
