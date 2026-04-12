//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <memory>
#include <string>

#include "ByteBuffer.h"
#include "FileBase.h"

enum class FileType { EMPTY, NORMAL, BYTECODE };

class File final : public FileBase
{
private:
    FileType _type;
    bool _data_was_read = false;
    bool _modified = false;
    std::string _path;
    std::string _extension;
    ByteBuffer _file_data{};

private:
    void read_data_if_needed();

public:
    File(FileType type, const std::string& path) : _type(type), _path(path)
    {
    }

    File(const ByteBuffer& buffer, const std::string& extension)
        : _type(FileType::BYTECODE), _extension(extension),
          _data_was_read(true), _file_data(buffer)
    {
    }

    ~File() override;

    static std::shared_ptr<File> create_empty_file(const std::string& path)
    {
        return std::make_shared<File>(FileType::EMPTY, path);
    }

    static std::shared_ptr<File> open_file(const std::string& path)
    {
        return std::make_shared<File>(FileType::NORMAL, path);
    }

    static std::shared_ptr<File> from_bytecode(const ByteBuffer& buffer, const std::string& extension)
    {
        return std::make_shared<File>(buffer, extension);
    }

    const std::string& get_path() override;

    void close();

    void save();
    void delete_file();

    void reset_cursor();
    void seek_cursor(uint32_t);
    uint32_t get_cursor();
    void clear_data();

    uint8_t read_byte();
    uint16_t read_word();
    uint16_t read_word_little();
    uint32_t read_doubleword();
    uint32_t read_doubleword_little();

    void append_byte(uint8_t byte);
    void append_word_big(uint16_t word);
    void append_word_little(uint16_t word);
    void append_doubleword_big(uint32_t word);
    void append_doubleword_little(uint32_t word);

    void write_byte(uint8_t byte);
    void write_word_big(uint16_t word);
    void write_word_little(uint16_t word);
    void write_doubleword_big(uint32_t word);
    void write_doubleword_little(uint32_t word);

    bool has_byte_remaining();
    bool has_word_remaining();
    bool has_doubleword_remaining();

    uint32_t get_size();
    FileBaseType get_file_type() override { return FILE_TYPE; }
    [[nodiscard]] const ByteBuffer &get_file_data();
};
