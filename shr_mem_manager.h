#ifndef _SHR_MEM_MANAGER_H
#define _SHR_MEM_MANAGER_H
#include <unistd.h>
#include <sys/shm.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>
#include "manager.h"

using namespace std;

class shmManager:public trafficManager
{
    public:
        shmManager(globalVar*g):trafficManager(g){}
        virtual void initialize() override;
        virtual void sendMsgToOtherProc(Flit*) override;
        virtual void recvMsgFromOtherProc() override;
		virtual void buildNetwork() override;
		virtual void run() override;
        virtual void finish() override;

		virtual ~shmManager();
	private:

        //vector<uint_64> routerIdInProc;
        //vector<uint_64> coreIdInProc;
	    vector<int> otherProcId;
		unordered_map<int,unsigned char*> procShmAddr;

	    int shmId;
        int openFlag = 0;
        const char* shmPath = "/tmp/shm"; //need to verify
		unsigned char *ptr = nullptr;
	    struct shmid_ds buff;

        int gShmId;
        const char* gShmPath = "/tmp/gShm";
        unsigned char* gPtr;
        int partitionId;
        pid_t gProcId;

		void packMessage(Flit* f);
		void unpackMessage();

		template<typename T>
	    void pack(T para);

		template<typename T>
		void unpack(T & para);
};

#endif
