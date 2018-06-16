#ifndef _TIME_MODULE_H
#define _TIME_MODULE_H
using uint_64 = unsigned long long int;
class timeModule
{
    public:
        friend class serialManager;
        friend class shmManager;
        //friend class sckManager;

    protected:
        static double getSimTime(){return curTime;}
    private:
        static uint_64 curTime; //ns
};



#endif
