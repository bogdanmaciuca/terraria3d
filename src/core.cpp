#include "core.hpp"
#include <string>
#include <cstdio>
#include <iostream>
#include "clue/clue.hpp"

namespace core {
    File::File(const char* filename, const char* options) {
        Open(filename, options);
    }
    File::~File() {
        LOG_INFO("File: Destructor called!");
        std::fclose(_handle);
    }
    void File::Open(const char* filename, const char* options) {
        _handle = std::fopen(filename, options);
        if (_handle == nullptr) {
            std::cout << "_handle == nullptr for " << filename << "\n";
            exit(1);
        }
    }
    void File::Close() {
        std::fclose(_handle);
    }
    void File::MoveAt(u32 offset, u32 origin) {
        std::fseek(_handle, (i32)offset, (i32)origin);
    }
    std::size_t File::Tell() {
        return ftell(_handle);
    }
    void File::Read(void* data, std::size_t byte_size) {
        std::fread(data, 1, byte_size, _handle);
    }
    void File::Write(const void* data, std::size_t byte_size) {
        std::fwrite(data, 1, byte_size, _handle);
    }
    void File::ReadAt(std::size_t pos, void* data, std::size_t byte_size) {
        MoveAt(pos);
        Read(data, byte_size);
    }
    void File::WriteAt(std::size_t pos, const void* data, std::size_t byte_size) {
        MoveAt(pos);
        Write(data, byte_size);
    }
    void File::ReadAll(std::string* str) {
        MoveAt(0, SEEK_END);
        std::size_t size = Tell();
        str->resize(size);
        MoveAt(0);
        Read(str->data(), size);
    }
}

