#ifndef _FLIT_H
#define _FLIT_H
#include <iostream>
#include <chrono>
#include "time_module.h"
#include "global.h"

#define MAX_HOPS 20
#define _RUN 1
#define _STOP 0
typedef unsigned long long int uint_64;
using namespace std;

class Flit
{
    public:
        //typedef chrono::steady_clock::time_point timePoint;
        Flit(){}
        bool head = 0;
        bool tail = 0;
        uint_64 srcId = 0;
        uint_64 destId;

	    int timer = MAX_HOPS; //added for flood routing algorithm
		bool state = _RUN;
		int arrivalGate; // added for flood routing algorithm
        vector<uint_64> crossRouterId;

        uint_64 id;
		uint_64 pid;
        uint_64 lastRouterId;
        uint_64 currentRouterId;
        uint_64 nextRouterId;
        uint_64 currentChannelId = 0;
        int hopCnt;
		int cycle;//clock-cycle,record SimTime when the flit should be handled.
        //timePoint realTimeStamp;

        uint_64 actSimTime = 0;
        uint_64 startSimTime;
        uint_64 endSimTime;
        virtual ~Flit(){}
};
#endif
