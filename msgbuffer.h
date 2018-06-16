#ifndef MSGBUFFER_H
#define MSGBUFFER_H
using uchar = unsigned char;
enum message{generalMsg = 1L, statsMsg = 2L};
struct msgBuffer
{
    long type = generalMsg;
    uchar head[1];
    uchar tail[1];
    uchar state[1];
    uchar arrivalGate[4];
    uchar timer[4];
    uchar hopCnt[4];
    uchar srcId[8];
    uchar destId[8];
    uchar id[8];
    uchar pid[8];
    uchar lastRouterId[8];
    uchar currentRouterId[8];
    uchar nextRouterId[8];
    uchar actSimTime[8];
    uchar currentChannelId[8];
};

struct statsMsgBuffer
{
    long type = statsMsg;
    uchar sendFlitsNum[8];
    uchar recvFlitsNum[8];
    uchar sumLifeTime[8];
};
#endif // MSGBUFFER_H

