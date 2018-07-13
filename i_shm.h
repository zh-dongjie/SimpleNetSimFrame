#ifndef  _I_SHM_H
#define  _I_SHM_H

#include <string>

#include <iostream>
#include <unistd.h>
#include <sys/shm.h>
#include <bitset>
constexpr unsigned long ipow(const int x, const int y)
{
    return (y==0) ? 1 : x * ipow(x,y-1);
}

template<typename T>
void i_shm_write( unsigned char* ptr, T data)
{
    std::string str;
    const size_t s = sizeof(data);
    const size_t n = sizeof(data) * 8;
    std::string binary = std::bitset<n>(data).to_string();

    for(uint i = 0; i < s; ++i)
    {
        str = binary.substr(0 + i * 8, 8 );
        unsigned int decimal = std::bitset<8>(str).to_ulong();
        *(ptr++) = static_cast<unsigned char>( decimal );
    }
}

template<typename T>
void i_shm_read( unsigned char* ptr, T& data)
{
    const size_t s = sizeof(data);
    const size_t n = sizeof(data) * 8;
    unsigned int decimal;
    std::string binary;
    for(uint i = 0; i < s; ++i)
    {
         decimal = static_cast<unsigned int>(*(ptr+i));
         //std::cout << "decimal = " << decimal << std::endl;
         binary = binary + std::bitset<8>(decimal).to_string();
    }

    data = std::bitset<n>(binary).to_ulong();
}

#endif
