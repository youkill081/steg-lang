//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <map>
#include <string>

#include "ByteBuffer.h"
#include "interpreter/exceptions.h"

using descriptor = uint16_t;

class File
{
private:
    bool modified = false; // True if file was modified
    bool new_file = false; // True if file was create with create_empty_file
    bool data_was_read = false; // True if _file_data has been filed up
    bool file_from_bytebuffer = false; // True if file was created from a FileBuffer

    std::string _path;

    ByteBuffer _file_data{};

    void read_data_if_needed();
public:
    File(const bool new_file, const std::string &path) : new_file(new_file), _path(path) {}
    explicit File(const ByteBuffer& buffer) : file_from_bytebuffer(true), _file_data(buffer), data_was_read(true) {};
    ~File();

    [[nodiscard]] const std::string &get_path();

    static File create_empty_file(const std::string &path);
    static File open_file(const std::string &path);

    void save();
    void delete_file();

    void reset_cursor();
    void clear_data();

    uint8_t read_byte();
    uint16_t read_word();

    void append_byte(uint8_t byte);
    void append_word(uint16_t word);

    bool has_byte_remaining();
    bool has_word_remaining();
};

class Files
{
private:
    descriptor current_descriptor = 0;
    std::map<descriptor, File> files;
public:
    descriptor open_file(const std::string &path);
    descriptor create_file(const std::string &path);
    void push_file(uint16_t descriptor, File file);

    void close_file(descriptor descriptor);
    void delete_file(descriptor descriptor);

    File &operator[](const descriptor &desc)
    {
        if (!this->files.contains(desc))
            throw FileError("File descriptor " + std::to_string(desc) + " not found !");
        return this->files.at(desc);
    }
};
