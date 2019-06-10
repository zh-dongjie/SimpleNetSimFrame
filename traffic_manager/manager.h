#ifndef MANAGE_H_
#define MANAGE_H_
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <regex>
#include <stdexcept>

#include "../global/time_module.h"
class Router;
class Flit;
class Core;
class Channel;
class globalVar;
using namespace std;

class trafficManager:public timeModule
{
    public:
        trafficManager(const trafficManager&) = delete;
        trafficManager(trafficManager &&) = delete;
        trafficManager & operator=(trafficManager&) = delete;
        static trafficManager* getConcretManager(string &, globalVar* );
        virtual void initialize() = 0;
        //void loadParamater(string network, string nedFile);

        //friend class globalVar;
        virtual void sendMsgToOtherProc(Flit*f){}
        virtual void recvMsgFromOtherProc(){}
        virtual void buildNetwork() = 0;
        virtual void run() = 0;
        virtual void finish() = 0;
        Core* getCorePtr(uint_64 coreId){return allCorePtr[coreId];}
        Router* getRouterPtr(uint_64 routerId){return allRouterPtr[routerId];}
        virtual ~trafficManager(){}
	private:
        static trafficManager* _tf;

    protected:
        trafficManager(globalVar*g):gHandle(g){}
        bool nedCheck(string &target);
		globalVar* gHandle;
	    string runStyle;
	   // string cfgDefault;
        map<uint_64, Router*> allRouterPtr;
        map<uint_64, Core*> allCorePtr;
        map<uint_64, Channel*> allChannelPtr;
		map<string, string> netConfig;
        Router* routerSpool = nullptr;
        Core* coreSpool = nullptr;
        Channel* outputChannelSpool = nullptr;
        Channel* inputChannelSpool = nullptr;
        bool nedReady = 1;
};

#endif
