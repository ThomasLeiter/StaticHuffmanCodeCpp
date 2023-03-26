#include "huffman.h"

#include <iostream>
#include <fstream>
#include <sstream>

void test_encoding(const char *file_name, const char *output_file)
{
    std::ifstream iff{file_name};
    std::ostringstream buffer;
    buffer << iff.rdbuf();
    std::string decoded_data = buffer.str();
    std::cout << "Encoding " << decoded_data.size() << " bytes of data\n";
    std::string encoded_data = huffman::encode(decoded_data);
    std::cout << "Encoded " << encoded_data.size() << " bytes of data\n";
    std::ofstream off{output_file, std::ofstream::binary};
    off << encoded_data;
}

void test_decoding(const char *file_name, const char *output_file)
{
    std::ifstream iff{file_name, std::ifstream::binary};
    std::ostringstream buffer;
    buffer << iff.rdbuf();
    std::string encoded_data = buffer.str();
    std::cout << "Decoding " << encoded_data.size() << " bytes of data\n";
    std::string decoded_data = huffman::decode(encoded_data);
    std::cout << "Decoded " << decoded_data.size() << " bytes of data\n";
    std::ofstream off{output_file};
    off << decoded_data;
}

void print_binary(unsigned char character)
{
    for (int i = 0; i < 8; ++i)
    {
        std::cout << ((character & 1 << i) ? "1" : "0");
    }
}

void print_bits(const std::string &str)
{
    for (unsigned char c : str)
        print_binary(c);
    std::cout << "\n";
}

int main()
{
    std::string msg = "MISSISSIPPI";
    std::cout << msg << "\n";
    print_bits(msg);
    std::string enc = huffman::encode(msg);
    print_bits(enc);
    std::string dec = huffman::decode(enc);
    std::cout << dec << "\n";
    print_bits(dec);

    test_encoding("lorem_ipsum.txt", "lorem_ipsum.enc");
    test_decoding("lorem_ipsum.enc", "lorem_ipsum.dec");
}