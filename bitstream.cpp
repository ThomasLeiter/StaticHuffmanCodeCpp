#include "bitstream.hpp"

#include <sstream>

int main()
{
    bitstream::obitstream obs{std::cout};
    unsigned char c = 0x23u;
    unsigned int i = 0x63534333u;
    unsigned long long l = 0x8474645444342413ull;
    std::vector<bool> v{{0, 0, 1}};
    obs << true << false << false << false;
    obs << c << i << l << v;
    std::cout << "\nClosing obs:\n";
    obs.close();
    std::cout << "\n\n";

    std::istringstream iss{"123456ABCDEFGH"};
    bitstream::ibitstream ibs{iss};
    bool b1,b2,b3,b4;
    std::cout << std::boolalpha;
    ibs >> b1 >> b2 >> b3 >> b4 >> c >> i >> l;
    std::cout << b1 << " " << b2 << " " << b3 << " " << b4 << "\n";
    std::cout << std::hex;
    std::cout << (int)c << "\n";
    std::cout << i << "\n";
    std::cout << l << "\n";
    return 0;
}