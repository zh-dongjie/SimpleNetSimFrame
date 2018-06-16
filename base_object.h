#ifndef _BASE_OBJECT_H
#define _BASE_OBJECT_H

#include <iostream>
#include "time_module.h"
using namespace std;
class Flit;


class base_object:public timeModule
{
    public:
        base_object(){}
        virtual void initialize(){}
        virtual void handleMessage(Flit*f){}
        virtual void finish(){}

        virtual void readInput(){}
        virtual void evaluate(){}
        virtual ~base_object(){}
	private:
};
#endif
