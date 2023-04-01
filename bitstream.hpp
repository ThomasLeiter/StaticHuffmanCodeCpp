#ifndef BITSTREAM_HPP
#define BITSTREAM_HPP

#include <iostream>
#include <vector>

namespace bitstream
{

    class obitstream
    {
    public:
        obitstream(std::ostream &ostr) : _ostr{ostr}
        {
            _next_byte = 0;
            _bit_index = 0;
        }

        obitstream &operator<<(bool bit)
        {
            return write_bit(bit);
        }

        obitstream &operator<<(unsigned char bits)
        {
            return write_bits<unsigned char, 8>(bits);
        }

        obitstream &operator<<(unsigned int bits)
        {
            return write_bits<unsigned int, 32>(bits);
        }

        obitstream &operator<<(unsigned long long bits)
        {
            return write_bits<unsigned long long, 64>(bits);
        }

        obitstream &operator<<(const std::vector<bool> &bits)
        {
            for (bool bit : bits)
                write_bit(bit);
            return *this;
        }

        void close(bool padding = false)
        {
            while (_bit_index != 0)
                write_bit(padding);
        }

    private:
        unsigned char _next_byte;
        int _bit_index;
        std::ostream &_ostr;

        obitstream &write_bit(bool bit)
        {
            if (bit)
                _next_byte += 1 << _bit_index;
            _bit_index = (_bit_index + 1) % 8;
            if (_bit_index == 0)
            {
                _ostr << _next_byte;
                _next_byte = 0;
            }
            return *this;
        }

        template <typename uint, int size>
        obitstream &write_bits(uint bits)
        {
            for (int i = 0; i < size; ++i)
            {
                bool bit = bits & (1ull << i);
                write_bit(bit);
            }
            return *this;
        }
    }; // class obitstream

    class ibitstream
    {
    public:
        ibitstream(std::istream &istr) : _istr{istr}
        {
            _next_byte = 0;
            _bit_index = 0;
        }

        ibitstream &operator>>(bool &bit)
        {
            bit = read_bit();
            return *this;
        }

        ibitstream &operator>>(unsigned char &bits)
        {
            bits = read_bits<unsigned char, 8>();
            return *this;
        }

        ibitstream &operator>>(unsigned int &bits)
        {
            bits = read_bits<unsigned int, 32>();
            return *this;
        }

        ibitstream &operator>>(unsigned long long &bits)
        {
            bits = read_bits<unsigned long long, 64>();
            return *this;
        }

    private:
        std::istream &_istr;
        unsigned char _next_byte;
        int _bit_index;

        bool read_bit()
        {
            if (_bit_index % 8 == 0)
            {
                _bit_index %= 8;
                _istr >> _next_byte;
            }
            return (bool)(_next_byte & (1 << _bit_index++));
        }

        template <typename uint, int size>
        uint read_bits()
        {
            uint n = 0;
            for (int i = 0; i < size; ++i)
                if (read_bit())
                    n += 1ull << i;
            return n;
        }
    }; // class ibitstream

} // namespace bitstream

#endif // BITSTREAM_HPP