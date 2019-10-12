#ifndef _SOCKET_MANAGER
#define _SOCKET_MANAGER

#include <string>
#include <vector>
#include <unordered_map>
#include "manager.h"
class sckManager: public trafficManager
{
    public:

        sckManager(globalVar*g):trafficManager(g){}

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
        std::map<int, std::vector<int>> partitionInHost;
        std::map<int, std::vector<pid_t>> procInPartition;
		std::map<std::string, int> portNoMap;
        std::vector<int> otherProcID;
        std::string hostIP;
        size_t hostNum = -1;
        int shmID = -1;
        const char *gShmPath = "/tmp/gShm";
        unsigned char *gPtr = nullptr;
        int gShmID = -1;
		int gProcID = 0;
		int gHostID = -1;
		unsigned long int partitionID = 0;
        
};

#endif
