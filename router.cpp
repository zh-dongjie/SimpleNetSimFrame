#include <iostream>
#include <cstdlib>
#include <ctime>
#include "msgbuffer.h"
#include "routing_func.h"
#include "channel.h"
#include "global.h"
#include "router.h"

void Router::initialize()
{
    totalRouters = gHandle->getTotalRouterNum();
    gatesNum = gHandle->getRouterGateNum();
   // cxtInputChannel.resize(gateNum);
   // cxtOutputChannel.resize(gateNum);

    _rfName = gHandle->getRoutingFunc();
    _rfPtr = gHandle->getRfPtr();
    cout << "router" << routerId << "initialized." << endl;
}

void Router::readInput()
{
    for(int i = 0; i < gatesNum; ++i)
    {
        Channel* ptr = cxtInputChannel[i];
        if(!ptr->empty())
        {
            for(unsigned int n = 0; n < ptr->pre.flitsStack.size(); ++n)
            {
                Flit* f = ptr->pre.flitsStack.back();
                if(getSimTime() >= f->actSimTime )
                {
                    f->arrivalGate = i;
                    sourceFlits.push_back(f);
                    ptr->pre.flitsStack.pop_back();
                }
                else
                    break;
            }
        }
    }
}

void Router::evaluate()
{
    if(!sourceFlits.empty())
    {
        Flit* f = sourceFlits.back();
        handleMessage(f);
    }
}

void Router::handleMessage(Flit*f)
{
    int outGateId ;
	if(f->head)
	{
        outGateId = _rfPtr->getOutputGateId(_rfName, f, this);
		if(!f->tail)
            gateIdHead.insert(make_pair(f->pid, outGateId));
    }
    else if(!f->head && f->tail)
	{
        outGateId = gateIdHead[f->pid];
	    gateIdHead.erase(f->pid);
	}
    else if(!f->head && !f->tail)
    {
        outGateId = gateIdHead[f->pid];
    }
    if(outGateId != -1)
    {
        f->actSimTime = getSimTime() + 10;
        f->hopCnt++;
        f->crossRouterId.push_back(routerId);
        send(f, outGateId);
    }
    else
        killFlit(f);
}

uint_64 Router::getRouterId()
{
    return routerId;
}

void Router::send(Flit* f, int gateId)
{
    f->actSimTime = getSimTime() + 10;
    f->lastRouterId = routerId;
    cxtOutputChannel[gateId]->pushFlitIntoStack(f);
    sourceFlits.pop_back();
}

void Router::sendDirect(Flit* f, uint_64 _routerId)
{
    Router*const tmpPtr = gHandle->getRouterPtr(_routerId);
    tmpPtr->putFlitIntoStack(f);
}

void Router::putFlitIntoStack(Flit* f)
{
    sourceFlits.push_back(f);   
}

Flit* Router::popFlitFromStack()
{
    Flit* tmpPtr = sourceFlits.back();
    sourceFlits.pop_back();
	return tmpPtr;
}

void Router::killFlit(Flit* f)
{
    cout << "flit sinks." << endl;
    cout << "srcId:" << f->srcId << "  destId" << f->destId << endl;
    cout << "id:" << f->id << "  pid:" << f->pid << endl;
    delete f;
}

void Router::finish()
{

}

Router::~Router()
{
    for(auto iter:sourceFlits)
	{
	    delete iter;
	}
    for(auto iter:curFlit)
    {
        delete iter;
    }
}
