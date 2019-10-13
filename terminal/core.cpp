#include <unistd.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <cstdlib>
#include <climits>
#include <chrono>
#include <bitset>
#include <vector>
#include <sys/msg.h>
#include "i_shm.h"
#include "channel.h"
#include "core.h"
#include "msgbuffer.h"
using namespace std;

#ifdef _SERIAL_STATISTICS
uint_64 Core::recvFlitsNum = 0;
uint_64 Core::sendFlitsNum = 0;
uint_64 Core::flitIdBase = 0;
uint_64 Core::packetIdBase = 0;
uint_64 Core::flitsLifeTimeSum = 0;
#endif
const char *paraFmt = "sendTotalFlitsNum recvTotalFlitsNum averageDelay realTime\n";
const char *dataFmt = "%lld              %lld              %f           %f\n";

void Core::initialize()
{
    packetFlits = gHandle->getPacketFlits();
    gateNum = gHandle->getCoreGateNum();
    if(!coreId)
        startRealTime = chrono::system_clock::now();
#ifdef _PARALLEL_STATISTICS
    statsMqId = gHandle->getStatsMqId();
    procNum = gHandle->getProcNum();
    partitionId = gHandle->getPartitionId();
    statProcMem = gHandle->getManagerProcMem();

    uint_64 coreNum = gHandle->getTotalCoreNum();
    flitIdBase = ( ULLONG_MAX / coreNum ) * coreId;
    packetIdBase = (ULLONG_MAX / coreNum ) * coreId;
#endif
    networkTotalCores = gHandle->getTotalCoreNum();
    _rf = gHandle->getRfPtr();
    _rfName = gHandle->getRoutingFunc();
#ifdef _PRINT_DATA_STREAM
    cout << "core" << coreId << " initialized." << endl;
#endif
}

void Core::readInput()
{
    for(int n =0; n < gateNum; ++n)
    {
        Channel* cp = cxtInputChannel[n];
        uint numFlitsInChannel = cp->pre.flitsStack.size();
        //cout << "core" << coreId << "readinput....." << endl;
        for(uint i = 0; i < numFlitsInChannel; ++i)
        {
            Flit* f = cp->pre.flitsStack.front();
            if(getSimTime() >= f->actSimTime)
            {
                cp->popFlitFromStack();
                sinkFlits.push_back(f);
                uint_64 _lifeTime = getSimTime() - f->startSimTime;
                flitsLifeTimeSum += _lifeTime;
                ++recvFlitsNum;
            }
        }
    }
}

void Core::evaluate()
{
#ifdef _PRINT_DATA_STREAM
    cout << "core " << coreId << " evaluating..." << endl;
#endif
    for(auto sinkMsg:sinkFlits)
        handleMessage(sinkMsg);
    generateFlits();
    int outGateId;

    for(auto f: sourceFlits)
    {
        if(f->head)
        {
            outGateId = rand()/( (RAND_MAX + 1u)/gateNum );
            gateIdHead[f->pid] = outGateId;
        }
        else
            outGateId = gateIdHead[f->pid];
        assert(outGateId >= 0 && outGateId < gateNum);
        send(f, outGateId);
        f->startSimTime = getSimTime();
        ++sendFlitsNum;
    }
}

void Core::handleMessage(Flit* &f)
{
    //assert(f->srcId == 20);
#ifdef _SHORTEST_PATH_FUNC_
    assert(f->destId == coreId);
    if(f->destId != coreId)
	{
        cerr << "f->dest = " << f->destId << endl;
        cerr << "f->src = " << f->srcId << endl;
        cerr << "coreId = " << coreId << endl;
        //throw runtime_error("The flit arrived at wrong core.");
    }
#endif
	killFlit(f);
}

void Core::killFlit(Flit* &f)
{
#ifdef _PRINT_DATA_STREAM
    cout << "coreId:" << coreId << " The received flit info:" << endl;
    cout << "simTime:" << getSimTime() << " pid:" << f->pid << " id:" << f->id << endl;
    cout << " destId:" << f->destId << " srcId" << f->srcId << endl;
#endif
    sinkFlits.pop_back();
    delete f;
}

void Core::send(Flit* f, int gateId)
{
        cxtOutputChannel[gateId]->pushFlitIntoStack(f);
        sourceFlits.pop_back();
        f->actSimTime = getSimTime() + 10;
#ifdef _PRINT_DATA_STREAM
        cout << "coreId:" << coreId << "send filt info:" << endl;
        cout << "simTime:" << getSimTime() << " pid:" << f->pid << " id:" << f->id << endl;
        cout << "srcId:" << f->srcId << " destId:" << f->destId << endl;
#endif
}

void Core::sendDirect(Flit *f, string destModule, uint_64 destModuleId, string gateType, int gateId, uint_64 delay)
{
    assert(destModule == "Router" || destModule == "Core");
    assert(gateType == "inputGate");
    if(destModule == "Router")
    {
        Router *_rPtr = gHandle->getRouterPtr(destModuleId);
        Channel *_channelPtr = _rPtr->getInputChannelPtr(gateId);
        _channelPtr->pushFlitIntoStack(f);
        sourceFlits.pop_back();
    }
    if(destModule == "Core")
    {
        Core *_cPtr = gHandle->getCorePtr(destModuleId);
        Channel *_channelPtr = _cPtr->getInputChannelPtr(gateId);
        _channelPtr->pushFlitIntoStack(f);
        sourceFlits.pop_back();
    }
    f->actSimTime = getSimTime() + delay;
    //Core* tmpPtr = gHandle->getCore(coreId);
    //tmpPtr->pushFlitIntoStack(f);
}

void Core::generateFlits()
{
#ifdef _PRINT_DATA_STREAM
    cout << "isGenFlits(): " << isGenFlits() << endl;
#endif
    if(isGenFlits())
    {
#ifdef _PRINT_DATA_STREAM
        cout << "core " << coreId << " generating Flits..." << endl;
#endif
        for (int i = 0; i < packetFlits; ++i)
        {
            Flit *f = new Flit;
            if (i == 0)
            {
                f->head = 1;
                ++packetIdBase;
            }
            if (i == packetFlits - 1)
                f->tail = 1;
            f->srcId = coreId;
            f->destId = getDestId();

            f->id = flitIdBase;
            f->pid = packetIdBase;
            ++flitIdBase;
            sourceFlits.push_back(f);
         }
    }
}

bool Core::isGenFlits()
{
    return gHandle->getRandomUint(0, 1);
      //0.5 of probability to generate Flits
}

uint_64 Core::getDestId()
{
    bool flag = 0;
    uint_64 destCoreId;
    while(!flag)
    {
        destCoreId = gHandle->getRandomUint(0, networkTotalCores - 1);
        if(destCoreId != coreId)
            flag = 1;
    }
    return destCoreId;
}

void Core::finish()
{
    cout << "core[" << coreId << "] is excecuting finish()..." << endl;
#ifdef _SERIAL_STATISTICS
    if(coreId == 0)
    {
        using _sclk = chrono::system_clock;
        ofstream ofs;
        _sclk::time_point now_t = _sclk::now();
        auto _t = now_t.time_since_epoch().count();
        string str = to_string(_t);
        string randomStr = str.substr(str.size() - 15, str.size() - 1);
        const string resultFile = "serial_" + randomStr + ".log";
        endRealTime = _sclk::now();
        ofs.open(resultFile, ios::app);

        ofs << "sendFlitsNum:  " << sendFlitsNum << endl;
        ofs << "recvFlitsNum:  " << recvFlitsNum << endl;
        double lifeTime = static_cast<double>(flitsLifeTimeSum);
        double averageDelay;
        averageDelay = lifeTime / recvFlitsNum;
        double timeSec = (endRealTime - startRealTime).count() * pow(10, -9);
        ofs << "averageDelay:  " << averageDelay << endl;
        ofs << "realTime:" << timeSec << "s" << endl;
        ofs.close();
    }
#endif

#ifdef _PARALLEL_STATISTICS
        statsMsgBuffer *_ptr = new statsMsgBuffer;
        _ptr->type = StatsMsg;
        i_shm_write(_ptr->sendFlitsNum, sendFlitsNum);
        i_shm_write(_ptr->recvFlitsNum, recvFlitsNum);
        i_shm_write(_ptr->sumLifeTime, flitsLifeTimeSum);
        int _mqId = gHandle->getStatsMqId();
        int tmp = msgsnd(_mqId, _ptr, sizeof(statsMsgBuffer), 0);
        if(tmp == -1)
            throw runtime_error("msgsnd() failed in finish() function");
#endif
}

uint_64 Core::getCoreId()const
{
    return this->coreId;
}

Core::~Core()
{
    for(auto iter : sourceFlits)
        delete iter;
    for(auto iter : sinkFlits)
        delete iter;
}
