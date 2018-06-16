#ifndef _SERIAL_MANAGER_H
#define _SERIAL_MANAGER_H
#include <iostream>
#include <vector>
#include "channel.h"
#include "router.h"
#include "core.h"
#include "manager.h"

using namespace std;

class serialManager:public trafficManager
{
    public:
        serialManager(globalVar*g):trafficManager(g){}
        virtual void initialize() override;
		virtual void buildNetwork() override;
		virtual void run() override;
        virtual void finish() override;

        virtual ~serialManager();
	private:	    

};

#endif
