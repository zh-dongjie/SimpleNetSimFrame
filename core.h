#ifndef _CORE_H
#define _CORE_H

#include <iostream>
#include "i_string.h"
#include <vector>
#include <unordered_map>
#include "base_object.h"
#include "global.h"
typedef unsigned long long int uint_64;
class Flit;
class globalVar;
class Channel;
class trafficManager;
class routingFunc;

//#define _SERIAL_STATISTICS
class Core :public base_object
{
    public:
        Core(){}
        virtual void initialize() override;
        virtual void readInput() override;
        virtual void evaluate() override;
		virtual void handleMessage(Flit* &f) override;
		virtual void finish() override;

        void send(Flit* f,int gateId);
        void sendDirect(Flit* f, string destModule, uint_64 moduleId, string gateType, int gateId, uint_64 delay);

        void killFlit(Flit* &f);
		uint_64 getCoreId();
        Channel *getInputChannelPtr(int inputGateId){return cxtInputChannel[inputGateId];}
        Channel *getOutputChannelPtr(int outputGateId){return cxtOutputChannel[outputGateId];}
        friend class serialManager;
        friend class shmManager;
        //friend class sckManager;

        virtual ~Core();
	private:
	    uint_64 coreId = 0;
        int packetFlits;
        int gateNum;
        void generateFlits();
        bool isGenFlits();

        vector<Flit*> sourceFlits;//the core itself generate flits to store in this container
        vector<Flit*> sinkFlits;

		uint_64 getDestId();
		globalVar* gHandle;
        routingFunc* _rf;
        string _rfName;

        unordered_map<uint_64, int> gateIdHead;//record which gate the head flit go to.      pid->gate
        vector<Channel*> cxtOutputChannel;//gateId -> channelAddr
        vector<Channel*> cxtInputChannel;//gateId -> channelAddr

        uint_64 networkTotalCores;

#ifdef _SERIAL_STATISTICS
        static uint_64 recvFlitsNum;
        static uint_64 sendFlitsNum;
        static uint_64 flitIdBase;
        static uint_64 packetIdBase;
        static uint_64 flitsLifeTimeSum;
#endif
#ifdef _PARALLEL_STATISTICS
        uint_64 recvFlitsNum = 0;
        uint_64 sendFlitsNum = 0;
        uint_64 flitId = 0;
        uint_64 packetId = 0;
        uint_64 flitIdBase;
        uint_64 packetIdBase;
        unsigned char* statProcMem;
        uint_64 flitsLifeTimeSum = 0;
        int partitionId;
        int procNum;
        int statsMqId;
#endif

};

#endif
