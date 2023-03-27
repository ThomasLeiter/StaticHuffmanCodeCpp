#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>

/// @brief Namespace containing methods for the 
/// lossless compression of data with huffman codes.
namespace huffman
{

    /// @brief Encode and compress a string of data 
    /// @param  data The string of bytes to encode
    /// @return The encoded data as a string of bytes
    std::string encode(const std::string &);
    /// @brief Decode a string of compressed byte data
    /// @param  data The string of bytes to decode
    /// @return The decoded data as a string of bytes
    std::string decode(const std::string &);


    /// @brief Encode and compress a file 
    /// @param filename The file to encode
    /// @param output Where to store the encoded data
    void encode_file(const char *, const char *);
    /// @brief Decode a compressed file
    /// @param filename The file to decode
    /// @param output Where to store the decoded data
    void decode_file(const char *, const char *);

} // namespace huffman

#endif // HUFFMAN_H