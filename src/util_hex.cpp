#include "stdafx.h"
#include "util_hex.h"

//helper function
unsigned HexCharToInt ( char c ) {
    if ( c >= '0' && c <= '9' ) return c - '0';
    if ( c >= 'A' && c <= 'F' ) return c - 'A' + 10;
    if ( c >= 'a' && c <= 'f' ) return c - 'a' + 10;
    return 0;
}

std::string HexUtil::ToHex(const char* buffer, unsigned int length) {
    unsigned int num_hex_digits = 2*length;
    char* res = new char[num_hex_digits];
    if(!res) {
        return "";
    }
    ::memset(res,0,num_hex_digits);
    for(unsigned int index=0; index<length; index++) {
        *(res+index*2) = "0123456789ABCDEF"[((*(buffer+index)>>4) & 0x0F)];
        *(res+index*2+1) = "0123456789ABCDEF"[(*(buffer+index) & 0x0F)];
    }
    std::string hex_string(res,num_hex_digits);
    delete[] res;
    return hex_string;
}

bool HexUtil::ToBinary(const std::string& hex_str,char*& out_data,unsigned int& out_length) {
    if(hex_str.size()%2 != 0) {
        return false;
    }
    out_length = hex_str.size() / 2;
    out_data = new char[out_length];
    if(!out_data) {
        return false;
    }
    ::memset(out_data,0,out_length);
    for(unsigned int index=0; index<out_length; index++) {
        out_data[index] = HexCharToInt(hex_str[index*2])*16 +
            HexCharToInt(hex_str[index*2+1]);
    }
    return true;
}
