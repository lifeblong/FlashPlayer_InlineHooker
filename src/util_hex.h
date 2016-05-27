#ifndef _TMBEP_UTIL_HEX_H_
#define _TMBEP_UTIL_HEX_H_


#include <string>
#include <sstream>
#include <iomanip>



class HexUtil {
public:
    static std::string ToHex(const char* buffer, unsigned int length);
    static bool ToBinary(const std::string& hex_str,char*& out_data,unsigned int& out_length);
};


template< typename T >
static std::string Int2Hex(T i) {
    std::stringstream stream;
    stream << "0x" 
        << std::setfill ('0') << std::setw(sizeof(T)*2) 
        << std::hex << i;
    return stream.str();
}





#endif
