#ifndef _CHANNEL_H
#define _CHANNEL_H
#include <iostream>
#include <deque>
#include <vector>
#include "core.h"
#include "router.h"
#include "channel.h"
#include "global.h"

class Flit;
#define _OPEN 1
#define _CLOSE 0
struct lastStepState
{
    deque<Flit*> flitsStack;
};
class Channel:public base_object
{
    public:
        Channel();
        void pushFlitIntoStack(Flit* f){pre.flitsStack.push_back(f);}
        Flit* popFlitFromStack();

        virtual void initialize() override;
        virtual void evaluate() override;
        friend void Router::readInput();
        friend void Core::readInput();
        bool empty();
        void setBoundary(){boundary = 1;}
        bool isBoundary(){return boundary;}
        void setChannelId(uint_64 _channelId){channelId = _channelId;}
        uint_64 getChannelId(){return channelId;}
        virtual ~Channel(){}


	private:
        uint16_t endProc;
        bool boundary = 0;
        bool dam = _OPEN;
        uint_64 channelId;
		int startNodeId;
		int endNodeId;
        deque<Flit*> flitsStack;
        struct lastStepState pre;
        globalVar * gHandle;
};

#endif
