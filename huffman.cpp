#include "huffman.h"

#include <vector>
#include <map>
#include <queue>
#include <sstream>
#include <iostream>

struct node
{
    node *_left, *_right;

    unsigned char _character;
    unsigned int _weight;

    node(node *left, node *right)
    {
        _left = left;
        _right = right;
        _weight = left->_weight + right->_weight;
    }

    node(unsigned char character, int weight)
    {
        _weight = weight;
        _character = character;
        _left = nullptr;
        _right = nullptr;
    }

    ~node()
    {
        delete _left;
        delete _right;
    }
};

std::ostream &operator<<(std::ostream &, node *);
std::ostream &operator<<(std::ostream &, const std::vector<bool> &);

template <typename T>
void append(std::vector<T> &, const std::vector<T> &);

node *build_huffman_tree(const std::string &);
std::map<unsigned char, std::vector<bool>> build_codebook(node *);

std::vector<bool> encode_tree(node *);
template <typename bool_iterator>
node *decode_tree(bool_iterator &);

std::vector<bool> encode_data(const std::string &, const std::map<unsigned char, std::vector<bool>> &);
template <typename bool_iterator>
std::string decode_data(bool_iterator &, node *, size_t);

std::string encode_size(size_t);
size_t decode_size(const std::string &);

std::string bits_to_bytes(const std::vector<bool> &);
std::vector<bool> bytes_to_bits(const std::string &);

std::string huffman::encode(const std::string &data)
{
    node *root = build_huffman_tree(data);
    std::cout << root << "\n";
    std::map<unsigned char, std::vector<bool>> code_book = build_codebook(root);
    std::vector<bool> encoded_tree = encode_tree(root);
    std::vector<bool> encoded_data = encode_data(data, code_book);
    std::vector<bool> huffman_code;
    append(huffman_code, encoded_tree);
    append(huffman_code, encoded_data);
    while (huffman_code.size() % 8 != 0)
        huffman_code.push_back(0);

    std::ostringstream oss;
    oss << encode_size(data.size()) << bits_to_bytes(huffman_code);
    return oss.str();
}

std::string huffman::decode(const std::string &data)
{
    std::istringstream iss{data};
    size_t size = decode_size(data);
    std::vector<bool> bits = bytes_to_bits(data);
    auto it = bits.begin();
    it += 32;
    node *root = decode_tree(it);
    std::cout << root << "\n";
    return decode_data(it, root, size);
}

template <typename T>
void append(std::vector<T> &head, const std::vector<T> &tail)
{
    head.insert(head.end(), tail.begin(), tail.end());
}

bool is_leaf(node *n)
{
    return n->_left == nullptr;
}

struct node_priority_comparator
{
    bool operator()(node *left, node *right) { return left->_weight > right->_weight; }
};

std::map<unsigned char, int> count_characters(const std::string &data)
{
    std::map<unsigned char, int> counter;
    for (unsigned char c : data)
    {
        if (counter.count(c) == 0)
            counter[c] = 0;
        ++counter[c];
    }
    return counter;
}

node *build_huffman_tree(const std::string &data)
{
    std::map<unsigned char, int> counter = count_characters(data);
    std::priority_queue<node *, std::vector<node *>, node_priority_comparator> pq;
    for (const auto &[character, weight] : counter)
    {
        pq.push(new node(character, weight));
    }
    while (pq.size() > 1)
    {
        node *left = pq.top();
        pq.pop();
        node *right = pq.top();
        pq.pop();
        pq.push(new node(left, right));
    }
    return pq.top();
}

void build_codebook_recursive(
    node *current_node,
    std::map<unsigned char, std::vector<bool>> &code_book,
    std::vector<bool> &prefix)
{
    if (is_leaf(current_node))
    {
        code_book[current_node->_character] = prefix;
    }
    else
    {
        prefix.push_back(0);
        build_codebook_recursive(current_node->_left, code_book, prefix);
        prefix.pop_back();
        prefix.push_back(1);
        build_codebook_recursive(current_node->_right, code_book, prefix);
        prefix.pop_back();
    }
}

std::map<unsigned char, std::vector<bool>> build_codebook(node *root)
{
    std::map<unsigned char, std::vector<bool>> code_book;
    std::vector<bool> prefix;
    build_codebook_recursive(root, code_book, prefix);
    return code_book;
}

std::vector<bool> encode_data(const std::string &data, const std::map<unsigned char, std::vector<bool>> &code_book)
{
    std::vector<bool> bits;
    for (unsigned char character : data)
    {
        append(bits, code_book.at(character)); // No [] operator possible because of const specifier
    }
    return bits;
}

std::vector<bool> encode_character(unsigned char character)
{
    std::vector<bool> bits;
    for (int i = 0; i < 8; ++i)
        bits.push_back(character & (1 << i));
    return bits;
}

unsigned char decode_character(const std::vector<bool> &bits)
{
    unsigned char character = 0;
    for (int i = 0; i < 8; ++i)
        character += bits[i] ? 1 << i : 0;
    return character;
}

std::vector<bool> encode_int(unsigned int character)
{
    std::vector<bool> bits;
    for (int i = 0; i < 32; ++i)
        bits.push_back(character & (1ull << i));
    return bits;
}

unsigned int decode_int(const std::vector<bool> &bits)
{
    unsigned int number = 0;
    for (int i = 0; i < 32; ++i)
        number += bits[i] ? 1ull << i : 0;
    return number;
}

std::string bits_to_bytes(const std::vector<bool> &bits)
{
    std::ostringstream oss;
    for (auto it = bits.begin(), end = bits.end(); it != end; it += 8)
    {
        oss << decode_character(std::vector<bool>{it, it + 8});
    }
    return oss.str();
}

std::vector<bool> bytes_to_bits(const std::string &bytes)
{
    std::vector<bool> bits;
    for (unsigned char character : bytes)
    {
        std::vector<bool> code = encode_character(character);
        bits.insert(bits.end(), code.begin(), code.end());
    }
    return bits;
}

void encode_tree_recursion(node *current_node, std::vector<bool> &tree_encoding)
{
    if (is_leaf(current_node))
    {
        tree_encoding.push_back(1);
        append(tree_encoding, encode_character(current_node->_character));
    }
    else
    {
        encode_tree_recursion(current_node->_left, tree_encoding);
        encode_tree_recursion(current_node->_right, tree_encoding);
        tree_encoding.push_back(0);
    }
}

std::vector<bool> encode_tree(node *root)
{
    std::vector<bool> tree_encoding;
    encode_tree_recursion(root, tree_encoding);
    tree_encoding.push_back(0);
    return tree_encoding;
}

std::string encode_size(size_t size)
{
    std::ostringstream oss;
    unsigned char mask = 0xff;
    for (int i = 0; i < 4; ++i)
    {
        unsigned char byte = mask & size;
        oss << byte;
        size >>= 8;
    }
    return oss.str();
}

size_t decode_size(const std::string &str)
{
    size_t size = 0ull;
    for (int i = 3; i >= 0; --i)
    {
        unsigned char byte = str[i];
        size <<= 8;
        size += byte;
    }
    return size;
}

template <typename bool_iterator>
node *decode_tree(bool_iterator &it)
{
    std::vector<node *> stack;
    while (true)
    {
        if (*it++)
        {
            stack.push_back(new node(decode_character(std::vector<bool>{it, it + 8}), 0));
            it += 8;
        }
        else
        {
            if (stack.size() == 1)
                return stack[0];
            node *right = stack.back();
            stack.pop_back();
            node *left = stack.back();
            stack.pop_back();
            stack.push_back(new node(left, right));
        }
    }
}

template <typename bool_iterator>
std::string decode_data(bool_iterator &it, node *root, size_t size)
{
    std::ostringstream oss;
    node *n;
    for (size_t i = 0; i < size; ++i)
    {
        n = root;
        while (!is_leaf(n))
            n = *it++ ? n->_right : n->_left;
        oss << n->_character;
    }
    return oss.str();
}

void print_tree(std::ostream &str, node *root, std::string prefix, std::string children_prefix)
{
    if (is_leaf(root))
    {
        str << prefix << root->_character << "\n";
    }
    else
    {
        str << prefix << root->_weight << "\n";
        print_tree(str, root->_left, children_prefix + "+---", children_prefix + "|   ");
        print_tree(str, root->_right, children_prefix + "\\---", children_prefix + "    ");
    }
}

std::ostream &operator<<(std::ostream &str, node *root)
{
    print_tree(str, root, "", "");
    return str;
}

std::ostream &operator<<(std::ostream &str, const std::vector<bool> &bits)
{
    for (bool bit : bits)
        str << (bit ? "1" : "0");
    return str;
}
