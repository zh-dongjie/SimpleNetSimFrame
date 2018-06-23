#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <sys/shm.h>
#include <stdexcept>
#include <map>
#include <thread>
#include <signal.h>
#include <cassert>
#include "i_shm.h"
#include "msgbuffer.h"
#include "flit.h"
#include "global.h"
#include "manager.h"
using namespace std;

#define SVMSG_MODE 111000000
void gInitializeMemory(const size_t gLenthInByte, unsigned char *&, globalVar*const);
void gInitializeMQ(const char *const mqPath, globalVar *const gHanle);
void gSend(unsigned char *const, const size_t procNum);
bool gReceive(unsigned char *const, const size_t procNum);
void readStatProcMem(const int statsMsgQueueId);
void destroyMQ(globalVar * const gHanle);
void destroyShmMem(globalVar *const gHandle);
//static void sig_child(const int signo);

void usage()
{
    cerr << "[./netsim] [INIFILE]\n--help    list this table" << endl;
}

int main(int argc,char**argv)
{
    pid_t gProcId = getpid();
    unsigned char* gPtr = nullptr;

    if(argc != 2)
	{
	    usage();
	    throw runtime_error("");
	}
    string iniFile = argv[1];
    cout << "inifile:" << iniFile << endl;

    trafficManager *_TF;
    globalVar *gHandle = new globalVar(iniFile);
	gHandle->printConfig();
    gHandle->setGlobalProcId(gProcId);
    cout << "Process Manager Id Is " << gProcId << endl;            // set manager process to statistics

    size_t procNum = gHandle->getProcNum();
    size_t gLengthInByte = procNum;

    uint_64 simTimeLimit = gHandle->getSimTimeLimit();
    string runType = gHandle->getRunType();
    _TF = trafficManager::getConcretManager(runType, gHandle);
    gHandle->setTrafficManager(_TF);
    if(runType == "serial")
    {
        _TF->initialize();
        _TF->buildNetwork();
        _TF->run();
        _TF->finish();
    }
    else if(runType == "single-machine")
    {
        pid_t pid;
        //const char *const gShmPath = "/tmp/gShm";
        const char *const mqPath = "/tmp/msgQueue";
        //signal(SIGCHLD, sig_child);
        gInitializeMQ(mqPath, gHandle);
        gInitializeMemory(gLengthInByte, gPtr, gHandle);
        vector<pid_t> procIdStack;
        size_t procNum = gHandle->getProcNum();
        for(size_t order = 0; order < procNum; ++order)                    //procNum is number of child process.
        {
            pid = fork();
            if(pid == 0)
            {
                cout << "create process" << order << "   process Id = " << getpid() << endl;
                gHandle->setPartitionId(order);
                break;
            }
            else if(pid == -1)
                throw runtime_error("fork() failed.");
            else
                procIdStack.push_back(pid);
        }
        if( pid == 0 )                             //child process
        {
            _TF->initialize();
            _TF->buildNetwork();
            _TF->run();
            _TF->finish();
        }
        else                                                    //parent process
        {
            uint_64 curTime = 0;                                //default clock_cycle is 2ns
            int readyFlag = 0;
            while(!readyFlag)
            {
                readyFlag = 1;
                unsigned char *_tmp = gPtr;
                //sleep(30);
                //wait();
                for(uint i = 0; i < procNum; ++i)
                {
                    if(*_tmp++ != 66)
                        readyFlag = 0;
                }
            }
            while(curTime < simTimeLimit)
            {
                //sleep(1);
                while( gReceive(gPtr, procNum) );              //gReceive() is for receive msg from other proc
                gSend(gPtr, procIdStack.size());               //gSend() send info to child process to start next cycle
                curTime += 2;
            }
            int _statsMqId = gHandle->getStatsMqId();
            readStatProcMem(_statsMqId);
            destroyMQ(gHandle);
            destroyShmMem(gHandle);
        }
    }
    else if(runType == "multi-machine")
    {
        cerr << "This function is being added." << endl;
    }
    else
        throw runtime_error("Unknown Runtype.");
    return 0;
}

void gInitializeMemory(size_t gLengthInByte, unsigned char *&gPtr, globalVar*const gHandle)
{
    int shmId = shmget(IPC_PRIVATE, gLengthInByte, SVSHM_MODE | IPC_CREAT);
    if(shmId == -1)
        throw runtime_error("shmget() failed in gInitializeMemory() function.");
    gHandle->setgShmId(shmId);
    gPtr = (unsigned char*)(shmat(shmId, NULL, 0));
    if(gPtr == nullptr)
        throw runtime_error("shmat() failed in gInitializeMemory() function.");

    unsigned char* tmp = gPtr;
    for(size_t i = 0; i < gLengthInByte; ++i)
        *tmp++ = static_cast<unsigned char>(65);
}

void gInitializeMQ(const char *const mqPath, globalVar *const gHandle)
{
    int msgQId = -1, oFlag = SVMSG_MODE | IPC_CREAT;
    int procN = gHandle->getProcNum();
    for(int i = 0; i < procN + 1; ++i)
    {
        string path = mqPath + to_string(i);
        cout << "path:" << path << endl;
        msgQId = msgget(IPC_PRIVATE, oFlag);
        if(msgQId == -1)
            throw runtime_error("msgget() failed in gInitializeMQ() function.");
        cout << "create message deque " << i << " message deque id:" << msgQId << endl;
        if(i == procN)
        {
            gHandle->setStatsMqId(msgQId);
            break;
        }
        gHandle->setMqId(i, msgQId);
    }
}

void gSend(unsigned char *const gPtr, const size_t procNum)
{
    unsigned char *tmp = gPtr;
    for(size_t i = 0; i < procNum; ++i)
    {
        *tmp++ = static_cast<unsigned char>(67);                       //67:flag of beginning a new cycle
    }
}

bool gReceive(unsigned char *const gPtr, const size_t procNum)
{
    uint recvFlag = 0;
    unsigned char *_ptr = gPtr;
    while(!recvFlag)
    {
        recvFlag = 0;
        for(uint i =0; i < procNum; ++i)
        {
            if( *(_ptr + i) == static_cast<unsigned char>(66) )      //66:flag of one cycle finished
            {
                ++recvFlag;
            }
        }
        cout << "recvFlag:" << recvFlag << endl;
        cout << "process Num:" << procNum << endl;
        if(recvFlag == procNum)
        {
            recvFlag = 0;
            break;
        }
        //_ptr = _ptr - procNum;
        //sleep(30);
    }
    return recvFlag;
}

void readStatProcMem(const int statsMsgQueueId)
{
    using namespace chrono_literals;
    uint_64 sendFlitsNum = 0, recvFlitsNum = 0, flitsLifeTimeSum = 0;
    double averageDelay;
    uint_64 tmp;
    int __tmp;
    cout << "To keep the time sequence.Wait for 2 seconds." << endl;
    this_thread::sleep_for(2s);
    while(1)
    {
        statsMsgBuffer *_ptr = new statsMsgBuffer;
        __tmp = msgrcv(statsMsgQueueId, _ptr, sizeof(statsMsgBuffer), 0, O_NONBLOCK | MSG_NOERROR);
        if(__tmp == -1)
            break;
        assert(_ptr->type == 2);
        i_shm_read(_ptr->sendFlitsNum, tmp);
        sendFlitsNum += tmp;
        i_shm_read(_ptr->recvFlitsNum, tmp);
        recvFlitsNum += tmp;
        i_shm_read(_ptr->sumLifeTime, tmp);
        flitsLifeTimeSum += tmp;
    }
    using _sclk = chrono::system_clock;
    ofstream ofs;
    _sclk::time_point now_t = _sclk::now();
    auto _t = now_t.time_since_epoch().count();
    string str = to_string(_t);
    string randomStr = str.substr(str.size() - 15, str.size() - 1);
    string resultFile = "parallel_" + randomStr + ".log";

    ofs.open(resultFile, ios::app);

    ofs << "sendFlitsNum:  " << sendFlitsNum << endl;
    ofs << "recvFlitsNum:  " << recvFlitsNum << endl;

    averageDelay = static_cast<double>(flitsLifeTimeSum) / recvFlitsNum;
    ofs << "averageDelay:  " << averageDelay << endl;
    ofs.close();
}

void destroyMQ( globalVar *const gHanle)
{
    int _mqid, tmp;
    int procN = gHanle->getProcNum();
    for(int i = 0; i < procN; ++i)
    {
        _mqid = gHanle->getMqId(i);
        tmp = msgctl(_mqid, IPC_RMID, NULL);
        if(tmp == -1)
            throw runtime_error("msgctl() failed in destroy() function.");
        cout << "destroyed message queue " << _mqid << endl;
    }
    _mqid = gHanle->getStatsMqId();
    tmp = msgctl(_mqid, IPC_RMID, NULL);
    if(tmp == -1)
        throw runtime_error("msgctl() failed in destroy() function.");
    cout << "destroyed statistics message queue " << _mqid << endl;
}

void destroyShmMem(globalVar *gHandle)
{
    int gShmId = gHandle->getgShmId();
    int tmp = shmctl(gShmId, IPC_RMID, NULL);
    if(tmp == -1)
        throw("shmctl() function failed.");
}
