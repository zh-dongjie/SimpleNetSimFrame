#include "i_string.h"
namespace std
{
    string::size_type find_front_n_of(const string& str,const string& sub, int n)
    {
        string::size_type key = str.find_first_of(sub), iTmp;
        string sTmp;
        while(n > 1)
        {
            sTmp = str.substr(key + 1, str.size() - 1 - key);
            iTmp = sTmp.find_first_of( sub );
            key = key + iTmp + 1;
            --n;
        }
        return key;
    }

    string::size_type find_back_n_of(const string& str, const string& sub, int n)
    {
        string::size_type key = str.find_last_of(sub), iTmp;
        string sTmp;
        while(n > 1)
        {
            sTmp = str.substr(0, key);
            iTmp = sTmp.find_last_of( sub );
            key = iTmp;
            --n;
        }
        return key;
    }
}
