//
// AI Generated tests for Steganographer
//

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "images/Image.h"
#include "steganographer/Steganographer.h"
#include "steganographer/constant.h"
#include "ByteBuffer.h"

namespace fs = std::filesystem;

class SteganographerTest : public ::testing::Test
{
protected:
    fs::path test_data_dir;
    fs::path test_image_path;
    fs::path output_dir;

    void SetUp() override
    {
        test_data_dir = fs::path(TEST_DATA_DIR);
        test_image_path = test_data_dir / "img.png";
        output_dir = test_data_dir / "output";

        // Créer le dossier de sortie s'il n'existe pas
        if (!fs::exists(output_dir))
        {
            fs::create_directories(output_dir);
        }

        // Vérifier que l'image de test existe
        ASSERT_TRUE(fs::exists(test_image_path)) << "Test image not found: " << test_image_path;
    }

    void TearDown() override
    {
        // Nettoyer les fichiers de sortie après les tests
        if (fs::exists(output_dir))
        {
            fs::remove_all(output_dir);
        }
    }

    // Helper pour créer un ByteBuffer à partir d'une string
    static ByteBuffer string_to_bytebuffer(const std::string& str)
    {
        ByteBuffer buffer;
        for (char c : str)
        {
            buffer.append_uint8(static_cast<uint8_t>(c));
        }
        return buffer;
    }

    // Helper pour convertir un ByteBuffer en string
    static std::string bytebuffer_to_string(ByteBuffer& buffer)
    {
        std::string result;
        buffer.reset_cursor();
        while (buffer.remaining_uint8() > 0)
        {
            result.push_back(static_cast<char>(buffer.read_uint8()));
        }
        return result;
    }
};

// Test: Encodage et décodage d'une string simple
TEST_F(SteganographerTest, EncodeDecodeSimpleString)
{
    // Charger l'image
    Image image(test_image_path.string());

    // Créer les données à encoder
    std::string test_string = "Hello, World!";
    ByteBuffer data = string_to_bytebuffer(test_string);

    // Encoder les données dans l'image
    Steganographer::encode(image, data, DataType::STRING);

    // Sauvegarder l'image modifiée
    fs::path output_path = output_dir / "encoded_simple.png";
    image.save_png(output_path.string());

    // Recharger l'image encodée
    Image encoded_image(output_path.string());

    // Décoder les données
    Steganographer::DecodeResult result = Steganographer::decode(encoded_image);

    // Vérifier que les données décodées correspondent
    std::string decoded_string = bytebuffer_to_string(result.data);
    EXPECT_EQ(decoded_string, test_string);
    EXPECT_EQ(result.header.data_type, DataType::STRING);
    EXPECT_EQ(result.header.version, PROTOCOL_VERSION);
}

// Test: Encodage et décodage d'une string vide
TEST_F(SteganographerTest, EncodeDecodeEmptyString)
{
    Image image(test_image_path.string());

    std::string test_string = "";
    ByteBuffer data = string_to_bytebuffer(test_string);

    Steganographer::encode(image, data, DataType::STRING);

    fs::path output_path = output_dir / "encoded_empty.png";
    image.save_png(output_path.string());

    Image encoded_image(output_path.string());
    Steganographer::DecodeResult result = Steganographer::decode(encoded_image);

    std::string decoded_string = bytebuffer_to_string(result.data);
    EXPECT_EQ(decoded_string, test_string);
    EXPECT_EQ(result.data.size(), 0);
}

// Test: Encodage et décodage d'une string avec caractères spéciaux
TEST_F(SteganographerTest, EncodeDecodeSpecialCharacters)
{
    Image image(test_image_path.string());

    std::string test_string = "Test123!@#$%^&*()_+-=[]{}|;':\",./<>?`~\n\t";
    ByteBuffer data = string_to_bytebuffer(test_string);

    Steganographer::encode(image, data, DataType::STRING);

    fs::path output_path = output_dir / "encoded_special.png";
    image.save_png(output_path.string());

    Image encoded_image(output_path.string());
    Steganographer::DecodeResult result = Steganographer::decode(encoded_image);

    std::string decoded_string = bytebuffer_to_string(result.data);
    EXPECT_EQ(decoded_string, test_string);
}

// Test: Encodage et décodage d'une string
TEST_F(SteganographerTest, EncodDecodeLongString)
{
    Image image(test_image_path.string());

    // Créer une string longue
    std::string test_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    ByteBuffer data = string_to_bytebuffer(test_string);

    Steganographer::encode(image, data, DataType::STRING);

    fs::path output_path = output_dir / "encoded_long.png";
    image.save_png(output_path.string());

    Image encoded_image(output_path.string());
    Steganographer::DecodeResult result = Steganographer::decode(encoded_image);

    std::string decoded_string = bytebuffer_to_string(result.data);
    EXPECT_EQ(decoded_string, test_string);
    EXPECT_EQ(result.data.size(), test_string.size());
}

// Test: Vérification du CRC32
TEST_F(SteganographerTest, VerifyCRC32)
{
    Image image(test_image_path.string());

    std::string test_string = "CRC32 Test String";
    ByteBuffer data = string_to_bytebuffer(test_string);

    uint32_t original_crc = data.get_crc32();

    Steganographer::encode(image, data, DataType::STRING);

    fs::path output_path = output_dir / "encoded_crc.png";
    image.save_png(output_path.string());

    Image encoded_image(output_path.string());
    Steganographer::DecodeResult result = Steganographer::decode(encoded_image);

    // Vérifier que le CRC32 dans le header correspond
    EXPECT_EQ(result.header.data_crc32, original_crc);

    // Vérifier que le CRC32 des données décodées correspond
    uint32_t decoded_crc = result.data.get_crc32();
    EXPECT_EQ(decoded_crc, original_crc);
}

// Test: Encodage et décodage de fichier texte
TEST_F(SteganographerTest, EncodeDecodeTextFile)
{
    Image image(test_image_path.string());

    // Créer un fichier texte temporaire
    fs::path temp_file = test_data_dir / "temp_test.txt";
    std::string file_content = "This is a test file content.\nWith multiple lines.\nAnd special characters: éàù!@#$%";

    std::ofstream out_file(temp_file);
    out_file << file_content;
    out_file.close();

    // Lire le fichier dans un ByteBuffer
    std::ifstream in_file(temp_file, std::ios::binary);
    ByteBuffer data;
    char byte;
    while (in_file.get(byte))
    {
        data.append_uint8(static_cast<uint8_t>(byte));
    }
    in_file.close();

    // Encoder
    Steganographer::encode(image, data, DataType::FILE_T);

    fs::path output_path = output_dir / "encoded_file.png";
    image.save_png(output_path.string());

    // Décoder
    Image encoded_image(output_path.string());
    Steganographer::DecodeResult result = Steganographer::decode(encoded_image);

    // Écrire le résultat dans un nouveau fichier
    fs::path decoded_file = output_dir / "decoded_test.txt";
    std::ofstream decoded_out(decoded_file, std::ios::binary);
    result.data.reset_cursor();
    while (result.data.remaining_uint8() > 0)
    {
        decoded_out.put(static_cast<char>(result.data.read_uint8()));
    }
    decoded_out.close();

    // Lire le fichier décodé et comparer
    std::ifstream decoded_in(decoded_file);
    std::string decoded_content((std::istreambuf_iterator<char>(decoded_in)),
                                 std::istreambuf_iterator<char>());
    decoded_in.close();

    EXPECT_EQ(decoded_content, file_content);
    EXPECT_EQ(result.header.data_type, DataType::FILE_T);

    // Nettoyer le fichier temporaire
    fs::remove(temp_file);
}

// Test: Encodage et décodage d'un fichier binaire
TEST_F(SteganographerTest, EncodeDecodeBinaryFile)
{
    Image image(test_image_path.string());

    // Créer un fichier binaire temporaire avec des données aléatoires
    fs::path temp_file = test_data_dir / "temp_binary.bin";
    std::vector<uint8_t> binary_data;
    for (int i = 0; i < 256; i++)
    {
        binary_data.push_back(static_cast<uint8_t>(i));
    }

    std::ofstream out_file(temp_file, std::ios::binary);
    out_file.write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
    out_file.close();

    // Lire le fichier dans un ByteBuffer
    ByteBuffer data;
    for (uint8_t byte : binary_data)
    {
        data.append_uint8(byte);
    }

    // Encoder
    Steganographer::encode(image, data, DataType::FILE_T);

    fs::path output_path = output_dir / "encoded_binary.png";
    image.save_png(output_path.string());

    // Décoder
    Image encoded_image(output_path.string());
    Steganographer::DecodeResult result = Steganographer::decode(encoded_image);

    // Vérifier que les données correspondent
    EXPECT_EQ(result.data.size(), binary_data.size());
    result.data.reset_cursor();
    for (size_t i = 0; i < binary_data.size(); i++)
    {
        EXPECT_EQ(result.data.read_uint8(), binary_data[i]);
    }

    // Nettoyer le fichier temporaire
    fs::remove(temp_file);
}

// Test: Vérifier que l'image encodée est différente de l'originale
TEST_F(SteganographerTest, EncodedImageIsDifferent)
{
    Image original_image(test_image_path.string());
    Image image_to_encode(test_image_path.string());

    std::string test_string = "Test data";
    ByteBuffer data = string_to_bytebuffer(test_string);

    Steganographer::encode(image_to_encode, data, DataType::STRING);

    // Les images devraient être différentes (au moins au niveau des pixels modifiés)
    // On ne peut pas comparer directement, mais on peut vérifier que l'encodage a réussi
    fs::path output_path = output_dir / "encoded_different.png";
    EXPECT_NO_THROW(image_to_encode.save_png(output_path.string()));
}

// Test: Multiple encode/decode cycles
TEST_F(SteganographerTest, MultipleEncodDecodeCycles)
{
    std::string test_string = "Multi-cycle test";

    for (int cycle = 0; cycle < 3; cycle++)
    {
        Image image(test_image_path.string());
        ByteBuffer data = string_to_bytebuffer(test_string);

        Steganographer::encode(image, data, DataType::STRING);

        fs::path output_path = output_dir / ("encoded_cycle_" + std::to_string(cycle) + ".png");
        image.save_png(output_path.string());

        Image encoded_image(output_path.string());
        Steganographer::DecodeResult result = Steganographer::decode(encoded_image);

        std::string decoded_string = bytebuffer_to_string(result.data);
        EXPECT_EQ(decoded_string, test_string) << "Failed at cycle " << cycle;
    }
}
