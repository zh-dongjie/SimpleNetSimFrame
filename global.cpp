#include <iostream>
#include <thread>
#include <fstream>
#include "i_string.h"
#include <regex>
#include <stdexcept>
#include <ctype.h>
#include <random>
#include <map>
#include <functional>
#include <chrono>
#include <unistd.h>
#include "manager.h"
#include "global.h"
#include "router.h"
#include "routing_func.h"
using namespace std;

int globalVar::singleFlag = 0;
uint_64 globalVar::packetId = 0;
uint_64 globalVar::flitId = 0;
enum paramHash
{
    __runType = 5045028521718112676UL,
    __channelDelay = 9172537090517324817UL,
    __channelType = 4315693204088466895UL,
    __clockCycle = 5300761604345608817UL,
    __routingFunc = 16365563138562764850UL,
    __totalCoreNum = 11880624436780862395UL,
    __totalRouterNum = 11066386352999594203UL,
    __routerGateNum = 3019785267401912625UL,
    __coreGateNum = 18216319699104575549UL,
    __readNedStyle = 7724702862302528637UL,
    __packetFlits = 6938426755752821536UL,
    __totalChannelNum = 8563669837341159332UL,
    __simTimeLimit = 1093547823923524922UL
};
globalVar::globalVar(string &iniFile)
{
	if(singleFlag)
	    throw runtime_error("class glovalVar should have only one instance.");
    loadParameter(iniFile);
    _rfPtr = new routingFunc(this);
    ++singleFlag;
}

void globalVar::loadParameter(string &iniFile)
{
    cout << "loadParameter..." << endl;
    int left = 0,right = 0,line = 0,flag = 0;
    //string fileName = "config.ini";
    string str,tmpStr,_tmpStr;
	ifstream ifs;
	ifs.open(iniFile);
    while(getline(ifs, str))
	{
	    ++line;
        if((left = str.find("[") != string::npos) && str.find("config") != string::npos)
		{
            ++flag;
            if( flag == 2) break;
			right = str.find("]");
            str = str.substr(left + 7, right - left - 7);
            network = str;
            getline(ifs, str);
		}
        cout << "load " << network << "..." << endl;

        size_t tmp = str.find_first_of('=');
        left = 0;
        right = tmp - 1;
        tmpStr = str.substr(left, right + 1);

        left = tmp + 1;
        right = str.find_first_of(';') - 1;
        _tmpStr = str.substr(left, right - left + 1);

        if(tmpStr == "runType")
            runType = _tmpStr;
        else if(tmpStr == "channelDelay")
            channelDelay = _tmpStr;
        else if(tmpStr == "channelType")
            channelType = _tmpStr;
        else if(tmpStr == "clockCycle")
            clockCycle = _tmpStr;
        else if(tmpStr == "routingFunc")
            _rfName = _tmpStr;
        else if(tmpStr == "totalCoreNum")
            totalCoreNum = stoull(_tmpStr);
        else if(tmpStr == "totalRouterNum")
            totalRouterNum = stoull(_tmpStr);
        else if(tmpStr == "totalChannelNum")
            totalChannelNum = stoull(_tmpStr);
        else if(tmpStr == "sim-time-limit")
            setSimTimeLimit(_tmpStr);
        else if(tmpStr == "packetFlits")
            packetFlits = stoi(_tmpStr);
        else if(tmpStr == "procNum")
            procNum = stoi(_tmpStr);
        else if(tmpStr == "totalRouterNum")
            totalRouterNum = stoull(_tmpStr);
        else if(tmpStr == "totalCoreNum")
            totalCoreNum = stoull(_tmpStr);
        else if(tmpStr == "routerGateNum")
            routerGateNum = stoi(_tmpStr);
        else if(tmpStr == "coreGateNum")
            coreGateNum = stoi(_tmpStr);
        else if(tmpStr == "readNedStyle")
            readNedStyle = _tmpStr;
        else if(tmpStr == "partitionNum")
        {
            partitionNum = stoul(_tmpStr);
            coresNumEachPartition.resize(partitionNum);
            routersNumEachPartition.resize(partitionNum);
        }
        else if(tmpStr.find("partition-id") != string::npos)
            parsePartition(str, tmpStr, _tmpStr);
        else
        {
            cerr << " Unknown Parameter : " << tmpStr << endl;
            throw runtime_error("");
        }
    }
}

void globalVar::printConfig()
{
    cout << "network:" << network << endl;
    cout << "runType:" << runType << endl;
    cout << "channelType:" << channelType << endl;
    cout << "channelDelay:" << channelDelay << endl;
    cout << "clockCyle:" << clockCycle << endl;
    cout << "routingFunc:" << _rfName << endl;
    cout << "totalCoreNum:" << totalCoreNum << endl;
    cout << "totalRouterNum:" << totalRouterNum << endl;
    cout << "totalChannelNum:" << totalChannelNum << endl;
    cout << "packetFlits:" << packetFlits << endl;
    cout << "simTimelimit:" << simTimeLimit << endl;
    cout << "procNum:" << procNum << endl;
    int i = 0;
    for(auto iter : coresNumEachPartition)
    {
        cout << "partition" << i << "  coresNum:" << iter << endl;
        ++i;
    }
    i = 0;
    for(auto iter : routersNumEachPartition)
    {
        cout << "partition" << i << "  routersNum:" << iter << endl;
        ++i;
    }
}

uint_64 globalVar::getUniqId(string objType)
{
    int tmpId;
	if(objType == "message")
	{
	    tmpId = flitId;
		++flitId;
	}
	else if(objType == "packet")
	{
	    tmpId = packetId;
		++packetId;
	}
	else
	    throw runtime_error("wrong objType.can't give unique Id");
    return tmpId;
}

void globalVar::setSimTimeLimit(string& str)
{
    int cnt = 0;
    while( isdigit(str[cnt]) )
        ++cnt;
    string tmpStr = str.substr(cnt, str.size() - 1);
    uint_64 uTimeLimit = stoull( str.substr(0,cnt) );
    if(tmpStr == "us")// convert us/ms/s to ns
        uTimeLimit *= 1000;
    else if(tmpStr == "ms")
        uTimeLimit *= 1000000;
    else if(tmpStr == "s")
        uTimeLimit *= 1000000000;
    simTimeLimit = uTimeLimit;
}

void globalVar::parsePartition(string& str, string& nodeStr, string& __partitionId)
{
    int left = nodeStr.find("[");
    int right = nodeStr.find("]");
    int pos = str.find_first_of("..");
    string moduleType = nodeStr.substr(0, left);
    string startStr = str.substr(left + 1, pos - left - 1);
    string endStr = str.substr(pos + 2, right - pos - 2);
    uint_64 startNodeId = stoull(startStr);
    uint_64 endNodeId = stoull(endStr);
    int partId = stoi(__partitionId);
    if(moduleType == "Router")
    {
        for(uint_64 nodeId = startNodeId; nodeId < endNodeId + 1; ++nodeId)
        {
            routerPartition[nodeId] = partId;
            cout << "nodeId:" << nodeId << "--> partId:" << partId << endl;
            ++routersNumEachPartition[partId];
        }
    }
    else if(moduleType == "Core")
    {
        for(uint_64 nodeId = startNodeId; nodeId < endNodeId + 1; ++nodeId)
        {
            corePartition[nodeId] = partId;
            ++coresNumEachPartition[partId];
        }
    }
    else
    {
        cerr << " Unknown Module Type : " << moduleType << endl;
        throw runtime_error("");
    }
}

bool globalVar::formatChk(string& target)
{
    regex re("");
    smatch sm;
    return regex_match(target, sm, re);
}

uint_64 globalVar::getRandomUint(uint_64 left, uint_64 right)
{
    random_device r;
    default_random_engine gen(r());
    uniform_int_distribution<int> dist(left, right);
    return dist(gen);
}

Router* globalVar::getRouterPtr(uint_64 routerId)
{
    return _tf->getRouterPtr(routerId);
}

Core* globalVar::getCorePtr(uint_64 coreId)
{
    return _tf->getCorePtr(coreId);
}

int globalVar::getRouterPartitionId(uint_64 routerId)
{
    if(routerPartition.find(routerId) != routerPartition.end())
        return routerPartition[routerId];
    else
        return -1;
}

int globalVar::getCorePartitionId(uint_64 coreId)
{
    if(corePartition.find(coreId) != corePartition.end())
        return corePartition[coreId];
    else
        return -1;
}
