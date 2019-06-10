#ifndef _SOCKET_MANAGER
#define _SOCKET_MANAGER

#include <string>
#include <vector>
#include <unordered_map>
#include "manager.h"
class SckManager: public trafficManager
{
    public:

        SckManager(globalVar*g):trafficManager(g){}

        virtual void initialize() override;
	    virtual void buildNetwork() override;
        virtual void sendMsgToOtherProc(Flit*f) override;
        virtual void recvMsgFromOtherProc() override;
        virtual void run() override;
        virtual void finish() override;

        virtual ~sckManager();
	private:
        void recvMsgFromOtherMachine();
        void sendMsgToOtherMachine(Flit*f);
		std::unordered_map<std::string, int> ipToSockfd;
        std::vector<int, std::vector<int>> partitionInHost;
        std::vector<int, std::vector<pid_t>> procInPartition;
		std::vector<int> otherProcID;
        int hostID = -1;
        int shmID = -1;
        const char* gShmPath = "/tmp/gShm";
		pid_t gProcID = 0;
		int gHostID = -1;
        
};

#endif
