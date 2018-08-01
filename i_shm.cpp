/*
 * i_shm.cpp
 *
 *  Created on: 2018年7月13日
 *      Author: mytest
 */
#include "i_shm.h"

void i_shm_write(unsigned char *ptr, double data)
{
    long long myInt = ((long long *)&data)[0];
    i_shm_write(ptr, myInt);
}

void i_shm_read(unsigned char *ptr, double &data)
{
    long long myInt;
    i_shm_read(ptr, myInt);
    data = ((double *)&myInt)[0];
 // cout << "world" << endl;
}
