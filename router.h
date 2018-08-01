#ifndef _ROUTER_H
#define _ROUTER_H

#include <iostream>
#include <deque>
#include <vector>
#include <functional>
#include <unordered_map>
#include <map>
#include "flit.h"
#include "base_object.h"
#include "global.h"
#define _OPEN 1
#define _CLOSE 0
using namespace std;

class Channel;
class routingFunc;
typedef unsigned long long int uint_64;
class Router : public base_object
{
    public:
        Router(){}
        Router(const Router&) = delete;
        Router(Router &&) = delete;
        Router& operator=(const Router&) = delete;

        virtual void initialize() override;
        virtual void handleMessage(Flit* &f) override;
		virtual void finish() override;
		uint_64 getRouterId() const;
        int getGatesNum() const{return gatesNum;}
		void putFlitIntoStack(Flit* f);
        Flit* popFlitFromStack();
        void generateFlits(vector<Flit*>& srcFlit);

        void send(Flit* f, int gateId);
        void sendDirect(Flit* f, uint_64 _routerId);

        Channel *getInputChannelPtr(int inputGateId){return cxtInputChannel[inputGateId];}
        Channel *getOutputChannelPtr(int outputGateId){return cxtOutputChannel[outputGateId];}
        virtual void readInput() override;

        friend class serialManager;
        friend class shmManager;
        //friend class sckManager;
        virtual ~Router();

	private:

        virtual void evaluate() override;
        void killFlit(Flit*);


        int gatesNum = 0;
        bool dam = _OPEN;
        uint_64 routerId = 0;
        uint_64 totalRouters = 0;
		int packetFlits = 1;


        routingFunc* _rfPtr = nullptr;
        string _rfName = "flood";

		vector<int> linkNodeId;
        deque<Flit*> sourceFlits;//message received from outside are temporarily stored here
        globalVar* gHandle = nullptr;

        vector<int> routingTable; //added for routingTable algorithm

        unordered_map<uint_64, int> gateIdHead; //record which gate the head flit go to. pid->gate

        vector<Channel*> cxtOutputChannel;//gateId->channelAddr
        vector<Channel*> cxtInputChannel;//gateId->channelAddr

        vector<Flit*> curFlit;//the container store the message to be sent at the moment
};
#endif
