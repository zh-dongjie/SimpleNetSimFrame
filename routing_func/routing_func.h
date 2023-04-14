#ifndef _ROUTING_FUN_H
#define _ROUTING_FUN_H
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "router.h"
#include "flit.h"
using namespace std;
typedef unsigned long long int uint_64;

class globalVar;
class routingFunc
{
    public:
        routingFunc(globalVar *g);
        routingFunc(routingFunc &&) = delete;

        routingFunc() = delete;
        routingFunc(const routingFunc &rf) = delete;
        routingFunc & operator=(const routingFunc&) = delete;

        void initialize();

        virtual int getOutputGateId(string rf, Flit* f, Router* node);

        int flood(Flit* f, Router* node);
        int shortest_path(Flit* f, Router* node);
        int routing_table(Flit*f, Router* node);

        unordered_map<uint_64,map<uint_64, int>> _RT;//destId --> gateId
        virtual ~routingFunc(){}

    private:

        static int singleFlag;
        bool _existed(vector<uint_64>&, uint_64);
        globalVar *gHandle;
};

#endif
