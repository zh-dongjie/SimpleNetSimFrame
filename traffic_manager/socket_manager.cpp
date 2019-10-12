#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <unordered_map>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string>
#include "global.h"
#include "socket_manager.h"
#include "core.h"
#include "router.h"
#include "i_shm.h"
#include "../channel.h"

using namespace std;

int setUpSocket(const std::string &ipStr, int portNo)
{
    int sockfd;
    struct sockaddr_in serverAddr;

    /*Create UDP socket*/
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    /*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNo);
    serverAddr.sin_addr.s_addr = inet_addr(ipStr.c_str());
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    return sockfd;
}

void sckManager::initialize()
{
    // 初始化网络，使得每个Node得知整个网络的其他节点所处的位置（是否同进程）
    // 机器之间建立UDP连接，获取机器IP与Socket File Descriptor之间的关系
    // 获取管理进程与计算进程间共享内存区的ID，获取同一机器不同进程间建立的消息队列的ID

    cout << "initializing......" << endl;
    partitionID = gHandle->getPartitionId();
    gProcID = gHandle->getGlobalProcId();
    //each process has unique message queue ID
    hostNum = gHandle->getHostNum();
    hostIP = gHandle->getHostIp();
    portNoMap = gHandle->getHostPortNoMap();
    for(auto iter : portNoMap)
    {
        if(iter.first == hostIP)
            continue;
        int sockfd = setUpSocket(iter.first, iter.second);
        ipToSockfd.insert(std::make_pair(iter.first, sockfd));
    }
    uint_64 routerNumInProc = gHandle->getRoutersNumInPartion(partitionID);
    routerSpool = new Router[routerNumInProc];

    cout << "routerNumInProc" << routerNumInProc << endl;
    uint_64 coreNumInProc = gHandle->getCoresNumInPartition(partitionID);
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
    gShmID = gHandle->getgShmId();
    gPtr = (unsigned char*)( shmat(gShmID, NULL, 0) );
    if(gPtr == nullptr)
        throw runtime_error("shmat() failed.");
}

void sckManager::buildNetwork()
{

}

void sckManager::sendMsgToOtherProc(Flit* f)
{

}

void sckManager::sendMsgToOtherMachine(Flit*f)
{

}

void sckManager::recvMsgFromOtherProc()
{

}

void sckManager::recvMsgFromOtherMachine()
{

}

void sckManager::run()
{

}

void sckManager::finish()
{

}

sckManager::~sckManager()
{

}
