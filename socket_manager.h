#ifndef _SOCKET_MANAGER
#define _SOCKET_MANAGER

#include <string>
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
};

#endif
