//
// Created by Roumite on 23/02/2026.
//

#include <fstream>

#include "Files.h"

#include <filesystem>

#include "Logger.h"

void File::read_data_if_needed()
{
    if (data_was_read || new_file)
        return;

    std::ifstream file_stream(this->_path, std::ios::binary);
    if (not file_stream.is_open())
        throw FileError("Can't read data at path : " + this->_path);

    int byte;
    while ((byte = file_stream.get()) != EOF)
    {
        this->_file_data.write_uint8(byte);
    }

    this->_file_data.reset_cursor();
    this->modified = false;
    this->data_was_read = true;
}

File::~File()
{
    if (this->modified)
        Logger::log("Warning: Modifier file was closed without saving !");
}

const std::string& File::get_path()
{
    return _path;
}

File File::create_empty_file(const std::string& path)
{
    return File(true, path);
}

File File::open_file(const std::string& path)
{
    if (not std::filesystem::exists(path))
        throw FileError("File not found at path : " + path);
    return File(false, path);
}

void File::save()
{
    if (this->file_from_bytebuffer)
        throw FileError("Can't save a file from files section !");

    std::ofstream file(this->_path, std::ios::binary);
    if (!file.is_open())
        throw FileError("Can't open file at path : " + this->_path);

    this->reset_cursor();
    while (this->_file_data.remaining_uint8() != 0)
    {
        file.put(this->_file_data.read_uint8());
    }
    file.close();

    this->modified = false;
}

void File::delete_file()
{
    if (this->file_from_bytebuffer)
        throw FileError("Can't delete a file from files section !");
    std::filesystem::remove(this->_path);
}

void File::reset_cursor()
{
    this->read_data_if_needed();

    this->_file_data.reset_cursor();
}

void File::clear_data()
{
    this->read_data_if_needed(); // To ensure coherence

    this->_file_data.clear_data();
    this->reset_cursor();
    modified = true;
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

void File::append_byte(uint8_t byte)
{
    this->read_data_if_needed();

    this->_file_data.write_uint8(byte);
    this->modified = true;
}

void File::append_word(uint16_t word)
{
    this->read_data_if_needed();

    this->_file_data.write_uint16(word);
    this->modified = true;
}

bool File::has_byte_remaining()
{
    this->read_data_if_needed();
    return this->_file_data.remaining_uint8() != 0;
}

bool File::has_word_remaining()
{
    if (not this->new_file && not this->data_was_read)
        this->read_data_if_needed();
    return this->_file_data.remaining_uint16() != 0;
}

descriptor Files::open_file(const std::string& path)
{
    while (this->files.contains(this->current_descriptor))
        this->current_descriptor++;
    this->files.emplace(this->current_descriptor, File::open_file(path));
    return this->current_descriptor;
}

descriptor Files::create_file(const std::string& path)
{
    while (this->files.contains(this->current_descriptor))
        this->current_descriptor++;
    this->files.emplace(this->current_descriptor, File::create_empty_file(path));
    return this->current_descriptor;
}

void Files::push_file(uint16_t descriptor, File file)
{
    this->files.emplace(descriptor, std::move(file));
}

void Files::close_file(descriptor descriptor)
{
    this->files.erase(descriptor);
}

void Files::delete_file(descriptor descriptor)
{
    this->files.at(descriptor).delete_file();
    this->files.erase(descriptor);
}


