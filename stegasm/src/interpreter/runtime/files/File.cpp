//
// Created by Roumite on 23/02/2026.
//

#include "./File.h"
#include "Logger.h"

#include <filesystem>
#include <fstream>

#include "interpreter/exceptions.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <fcntl.h>
#else
#pragma message("Unsupported platform")
#endif

static std::filesystem::path get_available_temp_path()
{
#ifdef _WIN32
    wchar_t tempFile[MAX_PATH];
    UINT result = GetTempFileNameW(
        std::filesystem::temp_directory_path().c_str(),
        L"stg",
        0,
        tempFile
    );
    if (!result)
        throw FileError("Can't create temporary file");
    return std::filesystem::path(tempFile);
#elif __linux__
    std::string tmpl = (std::filesystem::temp_directory_path() / "stgXXXXXX").string();
    int fd = mkstemp(&tmpl[0]);
    if (fd == -1)
        throw FileError("Can't create temporary file");
    close(fd);
    return std::filesystem::path(tmpl);
#endif
}

static std::filesystem::path write_buffer_to_file(ByteBuffer& buffer, const std::string& extension)
{
    auto path = get_available_temp_path();
    if (!extension.empty())
        path.replace_extension("." + extension);

    Logger::log("Temporary file path : " + path.string(), "Files");

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
        throw FileError("Can't open file");

    buffer.reset_cursor();
    while (buffer.remaining_uint8() != 0)
        file.put(buffer.read_uint8());
    file.close();

    return path;
}

void File::read_data_if_needed()
{
    if (_data_was_read || _type == FileType::EMPTY)
        return;

    std::ifstream file_stream(this->_path, std::ios::binary);
    if (!file_stream.is_open())
        throw FileError("Can't read data at path : " + this->_path);

    int byte;
    while ((byte = file_stream.get()) != EOF)
        this->_file_data.write_uint8(static_cast<uint8_t>(byte));

    this->_file_data.reset_cursor();
    this->_modified = false;
    this->_data_was_read = true;
}

File::~File()
{
    if (_type == FileType::BYTECODE && !this->_path.empty())
    {
        if (std::filesystem::exists(this->_path))
        {
            std::filesystem::remove(this->_path);
            Logger::log("Temporary file deleted : " + this->_path, "Files");
        }
    }
}

const std::string& File::get_path()
{
    if (this->_path.empty() && _type == FileType::BYTECODE)
    {
        auto path = write_buffer_to_file(this->_file_data, this->_extension);
        this->_path = path.string();
    }
    return _path;
}

void File::close()
{
    this->call_callbacks();
}

void File::save()
{
    if (_type == FileType::BYTECODE)
        throw FileError("Can't save a file from files section !");

    std::ofstream file(this->_path, std::ios::binary);
    if (!file.is_open())
        throw FileError("Can't open file at path : " + this->_path);

    this->reset_cursor();
    while (this->_file_data.remaining_uint8() != 0)
        file.put(this->_file_data.read_uint8());
    file.close();

    this->_modified = false;
}

void File::delete_file()
{
    if (_type == FileType::BYTECODE)
        throw FileError("Can't delete a file from files section !");
    std::filesystem::remove(this->_path);
}

void File::reset_cursor()
{
    this->read_data_if_needed();
    this->_file_data.reset_cursor();
}

void File::seek_cursor(uint32_t index)
{
    this->read_data_if_needed();
    this->_file_data.seek_cursor(index);
}

void File::clear_data()
{
    this->read_data_if_needed();
    this->_file_data.clear_data();
    this->reset_cursor();
    _modified = true;
}

uint8_t File::read_byte()
{
    this->read_data_if_needed();
    return this->_file_data.read_uint8();
}

uint16_t File::read_word()
{
    this->read_data_if_needed();
    return this->_file_data.read_uint16();
}

uint32_t File::read_doubleword()
{
    this->read_data_if_needed();
    return this->_file_data.read_uint32();
}

void File::append_byte(uint8_t byte)
{
    this->read_data_if_needed();
    this->_file_data.write_uint8(byte);
    this->_modified = true;
}

void File::append_word(uint16_t word)
{
    this->read_data_if_needed();
    this->_file_data.write_uint16(word);
    this->_modified = true;
}

void File::append_doubleword(uint32_t word)
{
    this->read_data_if_needed();
    this->_file_data.write_uint32(word);
    this->_modified = true;
}

bool File::has_byte_remaining()
{
    this->read_data_if_needed();
    return this->_file_data.remaining_uint8() != 0;
}

bool File::has_word_remaining()
{
    this->read_data_if_needed();
    return this->_file_data.remaining_uint16() != 0;
}

bool File::has_doubleword_remaining()
{
    this->read_data_if_needed();
    return this->_file_data.remaining_uint32() != 0;
}
