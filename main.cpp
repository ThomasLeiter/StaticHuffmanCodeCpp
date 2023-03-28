#include "huffman.h"

#include <iostream>

void print_binary(unsigned char character)
{
    for (int i = 0; i < 8; ++i)
    {
        std::cout << ((character & 1 << (7 - i)) ? "1" : "0");
    }
    std::cout << " ";
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

    huffman::encode_file("lorem_ipsum.txt", "lorem_ipsum.enc");
    huffman::decode_file("lorem_ipsum.enc", "lorem_ipsum.dec");
}