#ifndef _I_STRING_H
#define _I_STRING_H
#include <string>
namespace std
{
        extern string::size_type find_front_n_of(const string& str, const string& sub, const int n);

        extern string::size_type find_back_n_of(const string& str, const string& sub, const int n);
	
}
#endif
