#include <unistd.h>
#include "i_shm.h"
#include <fcntl.h>
#include <sys/msg.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <regex>
#include <signal.h>
#include <typeinfo>
#include <cassert>
#include "i_string.h"
#include "msgbuffer.h"
#include "channel.h"
#include "flit.h"
#include "shr_mem_manager.h"

#define _PRINT_DATA_INFO_
using namespace std;

void shmManager::initialize()
{
    cout << "initializing......" << endl;
    partitionId = gHandle->getPartitionId();

    gProcId = gHandle->getGlobalProcId();
//each process has unique message queue ID

    uint_64 routerNumInProc = gHandle->getRoutersNumInPartion(partitionId);
    routerSpool = new Router[routerNumInProc];

    cout << "routerNumInProc" << routerNumInProc << endl;
    uint_64 coreNumInProc = gHandle->getCoresNumInPartition(partitionId);
    coreSpool = new Core[coreNumInProc];

    cout << "coreNumInProc" << coreNumInProc << endl;
    int routerGateNum = gHandle->getRouterGateNum();
    int coreGateNum = gHandle->getCoreGateNum();
    for(uint_64 i = 0; i < routerNumInProc ; ++i)
    {
        routerSpool[i].gHandle = gHandle;
        routerSpool[i].cxtInputChannel.resize(routerGateNum);
        routerSpool[i].cxtOutputChannel.resize(routerGateNum);
    }

    for(uint_64 i = 0; i < coreNumInProc; ++i)
    {
        coreSpool[i].gHandle = gHandle;
        coreSpool[i].cxtInputChannel.resize(coreGateNum);
        coreSpool[i].cxtOutputChannel.resize(coreGateNum);
    }

    uint_64 channelNum = gHandle->getTotalChannelNum();
    outputChannelSpool = new Channel[channelNum];

    inputChannelSpool = new Channel[channelNum];

    cout << __FUNCTION__ << endl;
    gShmId = gHandle->getgShmId();
    gPtr = (unsigned char*)( shmat(gShmId, NULL, 0) );
    if(gPtr == nullptr)
        throw runtime_error("shmat() failed.");
}

void shmManager::sendMsgToOtherProc(Flit* f)
{
#ifdef _PRINT_DATA_INFO_
    cout << "Pack Message..." << endl;
    cout << "message srcId:" << f->srcId << "  message destId:" << f->destId << endl;
#endif
    //int nodePart = gHandle->getRouterPartitionId(f->nextRouterId);
    int _mqid = gHandle->getMqId(1);
    msgBuffer *ptr = new msgBuffer;

    ptr->type = 1;
    i_shm_write(ptr->head, f->head);
    i_shm_write(ptr->tail, f->tail);
    i_shm_write(ptr->state, f->state);
    i_shm_write(ptr->arrivalGate, f->arrivalGate);
    i_shm_write(ptr->timer, f->timer);
    i_shm_write(ptr->hopCnt, f->hopCnt);
    i_shm_write(ptr->srcId, f->srcId);
    i_shm_write(ptr->destId, f->destId);
    i_shm_write(ptr->id, f->id);
    i_shm_write(ptr->pid, f->pid);
    i_shm_write(ptr->lastRouterId, f->lastRouterId);
    i_shm_write(ptr->currentRouterId, f->currentRouterId);
    i_shm_write(ptr->nextRouterId, f->nextRouterId);
    i_shm_write(ptr->actSimTime, f->actSimTime);
    i_shm_write(ptr->currentChannelId, f->currentChannelId);
    int tmp = msgsnd(_mqid, ptr, sizeof(msgBuffer), 0);
    if(tmp == -1)
        throw runtime_error("msgsnd failed in shmManager::sendMsgToOtherProc().");
}

void shmManager::recvMsgFromOtherProc()
{
    while(1)
    {
        Flit *f = new Flit;

        int _mqid = gHandle->getMqId(partitionId);
        msgBuffer *ptr = new msgBuffer;
        int tmp = msgrcv(_mqid, ptr, sizeof(msgBuffer), 0, O_NONBLOCK | MSG_NOERROR);
        if(tmp == -1)
            break;
        assert(ptr->type == 1);
        i_shm_read(ptr->head, f->head);
        i_shm_read(ptr->tail, f->tail);
        i_shm_read(ptr->state, f->state);
        i_shm_read(ptr->arrivalGate, f->arrivalGate);
        i_shm_read(ptr->timer, f->timer);
        i_shm_read(ptr->hopCnt, f->hopCnt);
        i_shm_read(ptr->srcId, f->srcId);
        i_shm_read(ptr->destId, f->destId);
        i_shm_read(ptr->id, f->id);
        i_shm_read(ptr->pid, f->pid);
        i_shm_read(ptr->lastRouterId, f->lastRouterId);
        i_shm_read(ptr->currentRouterId, f->currentRouterId);
        i_shm_read(ptr->nextRouterId, f->nextRouterId);
        i_shm_read(ptr->actSimTime, f->actSimTime);
        i_shm_read(ptr->currentChannelId, f->currentChannelId);
        cout << "curChannelIID" << f->currentChannelId << endl;
        uint_64 curChannelId = f->currentChannelId;
        Channel * _cPtr = allChannelPtr[curChannelId];
        if(partitionId == 2)
            cout << "FUCK" << curChannelId << endl;
        _cPtr->pushFlitIntoStack(f);
        //Router *nextRouterPtr = allRouterPtr[f->nextRouterId];
        //nextRouterPtr->cxtInputChannel[]
        cout << "Unpack Message..." << endl;
        cout << "message srcId:" << f->srcId << "  message destId:" << f->destId << endl;
    }
}

void shmManager::buildNetwork()
{
    cout << "building network...... " << endl;
    string str, tmpStr;
    string startStr, endStr;
    uint_64 startId, endId;
    int startGate, endGate;
    int leftEnd, rightEnd;
    int flag;

    uint_64 itmp;
    ifstream ifs;
    string network = gHandle->getNetwork();
    string nedFile = "/home/mytest/qtProject/netSim/topology/" + network + ".ned";
    ifs.open(nedFile);
    size_t tmp;
    uint_64 cnt = 0, channelId = 0, routerCnt = 0, coreCnt = 0;
    uint_64 channelN = gHandle->getTotalChannelNum();

    map<uint_64, uint_64> routerPos;
    map<uint_64, uint_64> corePos;
    ofstream ofs;
    ofs.open("fuck.log", ios::app);
    while(getline(ifs, str))
    {
        cout << "cnt:" << cnt << endl;
        cout << str << endl;
        if( nedCheck(str) && nedReady)
        {
            flag = 0;
            cout << "chkNed() success." << endl;
            leftEnd = str.find_first_of("[");
            tmp = leftEnd;
            rightEnd = str.find_first_of("]");
            tmpStr = str.substr(leftEnd + 1, rightEnd - leftEnd - 1);
            startId = stoull(tmpStr);

            leftEnd = rightEnd + 10;
            rightEnd = str.find("<");
            tmpStr = str.substr(leftEnd, rightEnd - leftEnd - 13);
            startGate = stoi(tmpStr);

            startStr = str.substr(0, tmp);

            leftEnd = str.find_last_of("[");
            rightEnd = str.find_last_of("]");
            tmpStr = str.substr(leftEnd + 1, rightEnd - leftEnd - 1);
            endGate = stoi(tmpStr);

            rightEnd = leftEnd - 9;
            itmp = 1;
            while( str[rightEnd - itmp] != '[' )//Node[0] <--> Node[2]
                ++itmp;

            tmpStr = str.substr(rightEnd - itmp + 1, itmp);
            endId = stoull(tmpStr);

            rightEnd = find_front_n_of(str,"[", 3);
            leftEnd = find_front_n_of(str, ">", 1);
            endStr = str.substr(leftEnd + 2, rightEnd - leftEnd - 2);

            if(startStr == "Router" && partitionId == gHandle->getRouterPartitionId(startId))
            {
                uint_64 pos;
                auto it = routerPos.find(startId);
                if(it != routerPos.end())
                    pos = routerPos[startId];
                else
                {
                    pos = routerCnt;
                    routerPos[startId] = routerCnt;
                    ++routerCnt;
                }
                ++flag;                                                                                             // set flag to watch whether this channel is the boundary?
                allRouterPtr[startId] = &(routerSpool[pos]);
                allRouterPtr[startId]->routerId = startId;
                allRouterPtr[startId]->cxtOutputChannel[startGate] = &(outputChannelSpool[cnt]);
                allRouterPtr[startId]->cxtInputChannel[startGate] = &(inputChannelSpool[cnt]);
            }
            if(startStr == "Core" && partitionId == gHandle->getCorePartitionId(startId))
            {
                uint_64 pos;
                auto it = corePos.find(startId);
                if(it != corePos.end())
                    pos = corePos[startId];
                else
                {

                    pos = coreCnt;
                    corePos[startId] = coreCnt;
                    ++coreCnt;
                }
                ++flag;
                allCorePtr[startId] = &coreSpool[pos];
                allCorePtr[startId]->coreId = startId;
                allCorePtr[startId]->cxtOutputChannel[startGate] = &(outputChannelSpool[cnt]);
                allCorePtr[startId]->cxtInputChannel[startGate] = &(inputChannelSpool[cnt]);
            }
            if(endStr == "Router" && partitionId == gHandle->getRouterPartitionId(endId))
            {
                cout << "XXXXXXXXXXXX" << endl;
                uint_64 pos;
                auto it = routerPos.find(endId);
                if(it != routerPos.end())
                    pos = routerPos[endId];
                else
                {
                    pos = routerCnt;
                    routerPos[endId] = routerCnt;
                    ++routerCnt;
                }
                cout << "endId:" << endId << endl;
                cout << "pos:" << pos << endl;
                ++flag;
                allRouterPtr[endId] = &routerSpool[pos];
                allRouterPtr[endId]->routerId = endId;
                allRouterPtr[endId]->cxtInputChannel[endGate] = &outputChannelSpool[cnt];
                allRouterPtr[endId]->cxtOutputChannel[endGate] = &inputChannelSpool[cnt];
                cout << "AAAAAAAAAAAA" << endl;
            }
            if(endStr == "Core" && partitionId == gHandle->getCorePartitionId(endId))
            {
                uint_64 pos;
                auto it = corePos.find(endId);
                if(it != corePos.end())
                    pos = corePos[endId];
                else
                {
                    pos = coreCnt;
                    corePos[endId] = routerCnt;
                    ++coreCnt;
                }
                ++flag;
                allCorePtr[endId] = &coreSpool[pos];
                allCorePtr[endId]->coreId = endId;
                allCorePtr[endId]->cxtInputChannel[endGate] = &outputChannelSpool[cnt];
                allCorePtr[endId]->cxtOutputChannel[endGate] = &inputChannelSpool[cnt];
            }
            outputChannelSpool[cnt].setChannelId(channelId);
            inputChannelSpool[cnt].setChannelId(channelN + channelId);
            if(flag == 1)
            {
                outputChannelSpool[cnt].setBoundary();
                inputChannelSpool[cnt].setBoundary();
            }
            allChannelPtr[channelId] = &outputChannelSpool[cnt];
            allChannelPtr[channelN + channelId] = &inputChannelSpool[cnt];
            outputChannelSpool[cnt].initialize();
            inputChannelSpool[cnt].initialize();
            if(flag)  ++cnt;
            ++channelId;
        }
    }
    cout << "build network success!" << endl;

    uint_64 routerN = gHandle->getTotalRouterNum();
    uint_64 coreN = gHandle->getTotalCoreNum();
    for(uint_64 i = 0; i < routerN; ++i)
    {
        if(allRouterPtr[i] == nullptr && partitionId == gHandle->getRouterPartitionId(i))
        {
            cout << i << " " << i << endl;
            ofs << "Router[" << i << "] hang in the air." << endl;
            throw runtime_error("");
        }
    }
    for(uint_64 i = 0; i < coreN; ++i)
    {
        if(allCorePtr[i] == nullptr && partitionId == gHandle->getCorePartitionId(i))
        {
            cerr << "Core[" << i << "] hang in the air." << endl;
            throw runtime_error("");
        }
    }
}

void shmManager::run()
{
    //initialize
    uint_64 routerN = gHandle->getRoutersNumInPartion(partitionId);
    uint_64 coreN = gHandle->getCoresNumInPartition(partitionId);
    uint_64 channelN = gHandle->getTotalChannelNum();
    for(uint_64 i = 0; i < routerN; ++i)
    {
        routerSpool[i].initialize();
    }

    for(uint_64 i = 0; i < coreN; ++i)
    {
        coreSpool[i].initialize();
    }
    //clock-cycle handleMessage
    uint_64 uTimeLimit = gHandle->getSimTimeLimit();

    while(curTime < uTimeLimit)
    {
        for(uint_64 i = 0; i < routerN; ++i)
        {
            routerSpool[i].readInput();
            routerSpool[i].evaluate();//reach the set time point,trigger event.
        }
        for(uint_64 i = 0; i < coreN; ++i)
        {
            coreSpool[i].readInput();
            coreSpool[i].evaluate();//reach the set time point,trigger event.
        }

        for(uint_64 i = 0; i < channelN; ++i)
            if(outputChannelSpool[i].isBoundary())
                if(!outputChannelSpool[i].empty())
                {
                    Flit *f = outputChannelSpool[i].popFlitFromStack();
                    sendMsgToOtherProc(f);
                }
        recvMsgFromOtherProc();             //Parse all the message in message queue and push them into the inputChannel.
        for(uint_64 i = 0; i < channelN; ++i)
        {
            outputChannelSpool[i].evaluate();
            inputChannelSpool[i].evaluate();
        }
        if(curTime%10 == 0)
            cout << "The Task Schedule : " << static_cast<double>(curTime)/uTimeLimit << "%......" << endl;
        curTime += 2;
        cout << "pattitionId:" << partitionId << endl;
        *(gPtr + partitionId) = static_cast<unsigned char>(66);        //write "66" to shared memory
        while(static_cast<unsigned int>( *(gPtr + partitionId) ) != 67){} //wait info from manager process to continue next cycle
    }
}

void shmManager::finish()
{
    cout << "call finish()..." << endl;
    uint_64 routerN = gHandle->getRoutersNumInPartion(partitionId);
    uint_64 coreN = gHandle->getCoresNumInPartition(partitionId);
    for(uint_64 i = 0; i < routerN; ++i)
        routerSpool[i].finish();
    for(uint_64 i = 0; i < coreN; ++i)
        coreSpool[i].finish();
}

shmManager::~shmManager()
{
    delete [] routerSpool;
    delete [] coreSpool;
    delete [] outputChannelSpool;
    delete [] inputChannelSpool;
}
