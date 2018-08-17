#include <iostream>
#include "routing_func.h"
#include "flit.h"
#include "router.h"
#include "global.h"

int routingFunc::singleFlag = 0;

routingFunc::routingFunc(globalVar *g)
{
    gHandle = g;
    if(singleFlag)
        throw runtime_error("routingFunc constructor function failed.");
    singleFlag++;
}

int routingFunc::getOutputGateId(string rf, Flit *f, Router *node)
{
    if(rf == "flood")
        return flood(f, node);
    else if(rf == "shortest_path")
        return shortest_path(f, node);
    else if(rf == "routing_table")
        return routing_table (f, node);
    else
        throw runtime_error("unknown routing function.");
}

int routingFunc::flood(Flit* f, Router* _router)
{
    //cout << "routingFunction flood is evaluating..." << endl;
    int outputGateId = f->arrivalGate;
    if(_existed(f->crossRouterId, _router->getRouterId()) )
	{
	    f->state = _STOP;
	    return -1;
	}
    else if(!f->timer)
    {
        f->state = _STOP;
        return -1;
        cout << "one flood flit sinked." << endl;
    }
    else
    {
        while(outputGateId != f->arrivalGate)
        {
            int routerGatesNum = _router->getGatesNum();
            outputGateId = gHandle->getRandomUint(0, routerGatesNum - 1);
        }
        --f->timer;
        return outputGateId;
    }
}

int routingFunc::shortest_path(Flit* f, Router* node)
{
    return -1;
}

int routingFunc::routing_table(Flit* f, Router* node)
{
   return _RT[node->getRouterId()][f->destId];
}

bool routingFunc::_existed(vector<uint_64> & vec, uint_64 _routerId)
{
    bool flag = 0;
    for(auto iter : vec)
    {
        if(iter == _routerId)
            flag = 1;
    }
    return flag;
}
