//
// Created by Roumite on 18/02/2026.
//

#include "FileHandler.h"

#include <filesystem>
#include <fstream>

#include "Logger.h"

std::string FileHandler::get_name()
{
    return "File";
}

ByteBuffer FileHandler::encode(const Parameters& parameters)
{
    ByteBuffer end;
    if (parameters.get_parameters().empty())
        throw HandlerError("Missing file to encode !");

    std::filesystem::path path = parameters.get_parameters()[0];
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        throw HandlerError("Can't open file at path : " + path.string());
    Logger::log("File " + path.string() + " opened", "FileHandler");

    // Write file name size
    std::string file_name = path.filename().string();
    end.append_uint32_big(file_name.size());

    // Write file name
    for (const auto & c : file_name) {
        end.append_uint8(c);
    }

    // Write file data
    Logger::log("Prepare file data", "FileHandler");

    int byte; // EOF is int need int to finish loop
    while ((byte = file.get()) != EOF)
    {
        end.append_uint8(byte);
    }

    Logger::log("File ready to be encoded", "FileHandler");

    file.close();
    return end;
}

/*
 * IA Generated function get new next name to file
 * EX : test.png -> test_1.png
 * EX2: test_1.png -> test_2.png
 * EX3: test123.png -> test_124.png
 */
std::string get_next_name(const std::string& name)
{
    namespace fs = std::filesystem;

    fs::path p{name};
    fs::path parent = p.parent_path();
    std::string ext = p.extension().string();
    std::string stem = p.stem().string();

    std::size_t underscore = stem.rfind('_');
    if (underscore != std::string::npos && underscore + 1 < stem.size())
    {
        bool all_digits = true;
        for (std::size_t i = underscore + 1; i < stem.size(); ++i)
        {
            if (!std::isdigit(static_cast<unsigned char>(stem[i])))
            {
                all_digits = false;
                break;
            }
        }

        if (all_digits)
        {
            std::string prefix = stem.substr(0, underscore);
            std::string number_str = stem.substr(underscore + 1);

            unsigned long long number = 0;
            try {
                number = std::stoull(number_str);
            } catch (...) {
                number = 0;
                prefix = stem;
            }

            if (prefix != stem)
            {
                stem = prefix + "_" + std::to_string(number + 1);
            }
            else
            {
                stem = stem + "_1";
            }
        }
        else
        {
            stem = stem + "_1";
        }
    }
    else
    {
        stem = stem + "_1";
    }

    fs::path out = parent / fs::path(stem + ext);
    return out.string();
}

void FileHandler::decode(const Parameters& parameters, ByteBuffer& buffer)
{
    uint32_t file_name_size = buffer.read_uint32();

    std::string file_name;
    for (int i = 0; i < file_name_size; ++i)
    {
        file_name.push_back(static_cast<char>(buffer.read_uint8()));
    }

    while (std::filesystem::exists(file_name))
        file_name = get_next_name(file_name);

    std::ofstream file(file_name, std::ios::binary);
    if (!file.is_open())
        throw HandlerError("Can't open file at path : " + file_name);

    while (buffer.remaining_uint8() != 0)
    {
        file.put(buffer.read_uint8());
    }
    file.close();

    Logger::log("File " + file_name + " created successfully !", "FileHandler");
}
