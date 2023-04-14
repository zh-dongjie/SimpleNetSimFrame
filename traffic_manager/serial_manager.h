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
        serialManager() = delete;
        serialManager(const serialManager&) = delete;
        serialManager(serialManager &&) = delete;
        serialManager& operator=(const serialManager&) = delete;

        virtual void initialize() override;
        virtual void buildNetwork() override;
        virtual void run() override;
        virtual void finish() override;

        virtual ~serialManager();
    private:

};

#endif
