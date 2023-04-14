#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>
#include <regex>
#include <ctype.h>
#include <experimental/filesystem>
#include "i_string.h"
#include "channel.h"
#include "router.h"
#include "global.h"
#include "serial_manager.h"

using namespace std;
void serialManager::initialize()
{
    uint_64 routerNum = gHandle->getTotalRouterNum();
    int routerGateNum = gHandle->getRouterGateNum();
    int coreGateNum = gHandle->getCoreGateNum();
    routerSpool = new Router[routerNum];
    for(uint_64 i = 0; i < routerNum; ++i)
    {
        routerSpool[i].gHandle = gHandle;
        routerSpool[i].cxtInputChannel.resize(routerGateNum);
        routerSpool[i].cxtOutputChannel.resize(routerGateNum);
    }

    uint_64 coreNum = gHandle->getTotalCoreNum();
    coreSpool = new Core[coreNum];
    for(uint_64 i = 0; i < coreNum; ++i)
    {
        coreSpool[i].gHandle = gHandle;
        coreSpool[i].cxtInputChannel.resize(coreGateNum);
        coreSpool[i].cxtOutputChannel.resize(coreGateNum);
    }
    uint_64 channelNum = gHandle->getTotalChannelNum();
    cout << "channelNum:" << channelNum << endl;
    outputChannelSpool = new Channel[channelNum];
    inputChannelSpool = new Channel[channelNum];
}


void serialManager::buildNetwork()
{
    cout << "hello " << endl;
    string str, tmpStr;
    string startStr, endStr;
    uint_64 startId, endId;
    int startGate, endGate;
    int leftEnd, rightEnd;

    uint_64 itmp;
    uint tmp;
    ifstream ifs;
    string network = gHandle->getNetwork();
    string nedFile = "./topology/" + network + ".ned";
    if(!std::experimental::filesystem::exists(nedFile))
    {
        throw std::runtime_error("cannot find " + nedFile);
    }
    ifs.open(nedFile, ios::ate);
    auto size = ifs.tellg();
    string content(size, '\0');
    ifs.seekg(0);
    ifs.read(&content[0], size);
    istringstream iss(content);
    uint_64 cnt = -1;
    string readNedStyle = gHandle->getReadNedStyle();

    while(getline(iss, str))
    {
        ++cnt;
        cout << "cnt:" << cnt << endl;
        if( nedCheck(str) && nedReady)
        {
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
            cout << "startStr:" << startStr << endl;
            rightEnd = find_front_n_of(str,"[", 3);
            leftEnd = find_front_n_of(str, ">", 1);
            endStr = str.substr(leftEnd + 2, rightEnd - leftEnd - 2);

            cout << "endStr" << endStr << endl;
            if(startStr == "Router")
            {
                allRouterPtr[startId] = &(routerSpool[startId]);
                allRouterPtr[startId]->routerId = startId;
                allRouterPtr[startId]->cxtOutputChannel[startGate] = &outputChannelSpool[cnt];
                allRouterPtr[startId]->cxtInputChannel[startGate] = &inputChannelSpool[cnt];
            }

            if(startStr == "Core")
            {
                cout << "core startId: " << startId << "core start gate: " << startGate << endl;
                allCorePtr[startId] = &coreSpool[startId];
                allCorePtr[startId]->coreId = startId;
                allCorePtr[startId]->cxtOutputChannel[startGate] = &(outputChannelSpool[cnt]);
                allCorePtr[startId]->cxtInputChannel[startGate] = &(inputChannelSpool[cnt]);
            }
            if(endStr == "Router")
            {
                allRouterPtr[endId] = &routerSpool[endId];
                allRouterPtr[endId]->routerId = endId;
                allRouterPtr[endId]->cxtInputChannel[endGate] = &outputChannelSpool[cnt];
                allRouterPtr[endId]->cxtOutputChannel[endGate] = &inputChannelSpool[cnt];
            }
            if(endStr == "Core")
            {
                allCorePtr[endId] = &coreSpool[endId];
                allCorePtr[endId]->coreId = endId;
                allCorePtr[endId]->cxtInputChannel[endGate] = &outputChannelSpool[cnt];
                allCorePtr[endId]->cxtOutputChannel[endGate] = &inputChannelSpool[cnt];
            }

        }
    }
    cout << "build network success!" << endl;
    uint_64 routerN = gHandle->getTotalRouterNum();
    uint_64 coreN = gHandle->getTotalCoreNum();
    for(uint_64 i = 0; i < routerN; ++i)
    {
        if(allRouterPtr[i] == nullptr)
        {
            cerr << "Router[" << i << "] hang in the air." << endl;
            throw runtime_error("");
        }
    }

    for(uint_64 i = 0; i < coreN; ++i)
    {
        if(allCorePtr[i] == nullptr)
        {
            cerr << "Core[" << i << "] hang in the air." << endl;
            throw runtime_error("");
        }
    }
    ifs.close();
}

void serialManager::run()
{
    //initialize
    uint_64 routerN = gHandle->getTotalRouterNum();
    uint_64 coreN = gHandle->getTotalCoreNum();
    uint_64 channelNum = gHandle->getTotalChannelNum();
    cout << "router initializing..." << endl;
    for(uint_64 i = 0; i < routerN; ++i)
    {
        allRouterPtr[i]->initialize();
    }

    cout << "core initializing..." << endl;
    for(uint_64 i = 0; i < coreN; ++i)
    {
        allCorePtr[i]->initialize();
    }
    //clock-cycle handleMessage
    uint_64 uTimeLimit = gHandle->getSimTimeLimit();
    while(curTime < uTimeLimit)
    {
        for(uint_64 i = 0; i < routerN; ++i)
        {
            (allRouterPtr[i])->readInput();
            (allRouterPtr[i])->evaluate();//reach the set time point.trigger event
        }

        for(uint_64 i = 0; i < coreN; ++i)
        {
            (allCorePtr[i])->readInput();
            (allCorePtr[i])->evaluate();//reach the set time point.trigger event.
        }

        for(uint_64 i = 0; i < channelNum; ++i)
        {
            outputChannelSpool[i].evaluate();
            inputChannelSpool[i].evaluate();
        }
        if(curTime%10 == 0)
            cout << "The Task Schedule : " << static_cast<double>(curTime)/uTimeLimit*100 << "%......" << endl;
        curTime += 2;
    }
}

void serialManager::finish()
{
    cout << "finish..." << endl;
    uint_64 routerN = gHandle->getTotalRouterNum();
    uint_64 coreN = gHandle->getTotalCoreNum();
    for(uint_64 i = 0; i < routerN; ++i)
        (allRouterPtr[i])->finish();
    for(uint_64 i = 0; i < coreN; ++i)
        (allCorePtr[i])->finish();
}

serialManager::~serialManager()
{
    delete [] routerSpool;
    delete [] coreSpool;
    delete [] outputChannelSpool;
    delete [] inputChannelSpool;
}
