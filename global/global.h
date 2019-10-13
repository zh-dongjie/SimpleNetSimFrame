#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <vector>
#include <map>
#include <limits>
#include <unordered_map>
using namespace std;
//#define _PRINT_DATA_STREAM
//#define _SERIAL_STATISTICS
#ifndef _SERIAL_STATISTICS
    #define _PARALLEL_STATISTICS
#endif
typedef unsigned long long int uint_64;
#define SVSHM_MODE 0600
#define ASSERT(exp) assert(exp)

class randomNumGenerator;
class trafficManager;
class Router;
class Core;
class routingFunc;

class globalVar
{
    public:
        globalVar(string& iniFile);
	    globalVar(globalVar const &) = delete;
	    globalVar(globalVar &&) = delete;
		void operator = (globalVar const&) = delete;

		void loadParameter(string& iniFile);
        void printConfig();

        uint_64 getTotalChannelNum(){return totalChannelNum;}
		string getNetwork(){return network;}
		string getRunType(){return runType;}
		string getChannelType(){ return channelType;}
		string getClockCycle(){return clockCycle;}
        string getChannelDelay(){return channelDelay;}
        string getRoutingFunc(){return _rfName;}
        string getReadNedStyle(){return readNedStyle;}
        routingFunc* getRfPtr(){return _rfPtr;}

        uint_64 getSimTimeLimit(){return simTimeLimit;}
        uint_64 getTotalCoreNum(){return totalCoreNum;}

        size_t getFlitSize(){return flitSizeInBytes;}

        std::string getHostIp() const{return ipStr;}
        std::vector<std::string> getHostVec(){
            return hostVec;
        }
        int getPortNo() const{return portNo;}
        std::map<std::string, int> getHostPortNoMap(){
            return portNoMap;
        }


		uint_64 getUniqId(string objType);
        uint_64 getTotalRouterNum(){return totalRouterNum;}
        size_t getProcNum(){return procNum;}
        size_t getHostNum(){return hostNum;}
        int getRouterGateNum(){return routerGateNum;}
        int getCoreGateNum(){return coreGateNum;}
        int getPacketFlits(){return packetFlits;}
        uint_64 getRandomUint(uint_64 left, uint_64 right);

        trafficManager* getTrafficManager(){return _tf;}
        void setTrafficManager( trafficManager* tf){_tf = tf;}
        Router* getRouterPtr(uint_64 routerId); //由节点Id直接获取模块指针
        Core* getCorePtr(uint_64 coreId);
        //以下成员函数用于多进程运行
        void setMqId(int _partitionId, int msgQId){mqId[_partitionId] = msgQId;}
        int getMqId(int _partitionId){return mqId[_partitionId];}
        void setStatsMqId(int _statsMqId){statsMqId = _statsMqId;}
        int getStatsMqId(){return statsMqId;}
        void setgShmId(int _gShmId){gShmId = _gShmId;}
        int getgShmId(){return gShmId;}
        void setPartitionId(size_t i){partitionId = i;}
        size_t getPartitionId(){return partitionId;}
        void setGlobalProcId(pid_t p){gProcId = p;}
        pid_t getGlobalProcId(){return gProcId;}
        void setManagerProcMem(unsigned char *g){gMemPtr = g;}
        unsigned char* getManagerProcMem(){return gMemPtr;}
        int getRouterPartitionId(uint_64 routerId);
        int getCorePartitionId(uint_64 coreId);

        uint_64 getChannelsNumInPartition(int partId){return channelsNumEachPartition[partId];}
        uint_64 getCoresNumInPartition(int partId){return coresNumEachPartition[partId];}
        uint_64 getRoutersNumInPartion(int partId){return routersNumEachPartition[partId];}
 		~globalVar();
    private:
        bool formatChk(string& target);
        void initHash();
        void parsePartition(string& str,string& nodeStr, string& partId);
        vector<uint_64> routingTable;
        string network = "loop_net";
        string runType = "serial";
		string channelType = "normal";
		string clockCycle = "2ns";
		string channelDelay = "100ns";
        string _rfName = "flood";
        string readNedStyle = "line_by_line";
        routingFunc* _rfPtr;

        size_t hostNum = 0;
        uint_64 totalChannelNum = 0;
        uint_64 totalCoreNum = 0;
        uint_64 totalRouterNum = 0;
		int packetFlits = 1;

        uint_64 simTimeLimit = std::numeric_limits<uint_64>::max();//ns
        void setSimTimeLimit(string&);

        static uint_64 flitId;
        static uint_64 packetId;
        static int singleFlag;
        size_t flitSizeInBytes = 5;
        size_t procNum = 1;

        int routerGateNum;
        int coreGateNum;

        trafficManager * _tf;
        size_t partitionId = -1;            //store own partitionId
        size_t partitionNum = 1;
        pid_t gProcId;
        map<uint_64, int> routerPartition;    //mapping nodeId to partitionId
        map<uint_64, int> corePartition;
        map<int, int> mqId;                 //mapping partitionId to message queue id
        int statsMqId = 0;
        vector<uint_64> coresNumEachPartition;
        vector<uint_64> routersNumEachPartition;
        vector<uint_64> channelsNumEachPartition;
        vector<pid_t> childProcId; //按顺序存放进程Id（不包括用于统计的进程）
        std::string ipStr; //本机器IP
        std::vector<std::string> hostVec;//所有机器的IP
        int portNo = -1; //本机器开放的用于UDP服务的端口号
        std::map<std::string, int> portNoMap; //ip地址对应的端口号
        unsigned char* gMemPtr = nullptr;
        int gShmId = -1;
        unordered_map<string, int> configHash;
        randomNumGenerator *rng;
};
#endif
