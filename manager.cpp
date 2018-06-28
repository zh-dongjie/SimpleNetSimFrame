#include <iostream>
#include "i_string.h"
#include <stdexcept>
#include <regex>
#include "manager.h"
#include "serial_manager.h"
#include "shr_mem_manager.h"

using namespace std;

trafficManager*trafficManager::_tf = nullptr;

trafficManager* trafficManager::getConcretManager(string& _ts, globalVar* g)
{
    if(_tf == nullptr)
	{
		if(_ts == "serial")
            _tf = new serialManager(g);
		else if(_ts == "single-machine")
            _tf = new shmManager(g);
		else
			throw runtime_error("please select right trafficManager style.");
		return _tf;
	}
	else
	{
	    return _tf;
	}
}

bool trafficManager::nedCheck(string &target)
{
    return 1;
    smatch sm;
    //size_t tmp = target.find(";");
    //target = target.substr(0, tmp);
    string str = "(([ ]*(Core|Router)\\[\\d+\\]\\.Channel\\[\\d+\\] +<--> +(Core|Router)\\[\\d+\\]\\.Channel\\[\\d+\\][ ]*;[ ]*)|(^[ ]*))$";
    regex reNed(str);
    return regex_match(target, sm, reNed);
}

