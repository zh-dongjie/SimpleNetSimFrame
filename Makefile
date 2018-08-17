CC            = gcc
CXX           = g++
DEFINES       = -D_SERIAL_STATISTICS #-D_PARALLEL_STATISTICS or -D_SERIAL_STATISTICS
INCPATH       = -I. -I.. -I../global -I../router -I../routing_func -I../terminal -I../traffic_manager -I../utilities \
                -Iglobal -Irouter -Irouting_func -Iterminal -Itraffic_manager -Iutilities
CFLAGS        = -pipe $(INCPATH) -g -Wall -W -fPIC $(DEFINES)
CXXFLAGS      = -pipe $(INCPATH) -g -Wall -W -fPIC $(DEFINES)
LINK          = g++
LFLAGS        = -Wl,-O1
OBJECTS  = main.o channel.o core.o global.o i_string.o manager.o router.o routing_func.o \
           serial_manager.o shr_mem_manager.o time_module.o randomNumGen.o 
CPP_SRCS = $(wildcard *.cpp) $(wildcard */*.cpp)
CPP_HDRS = $(wildcard *.h) $(wildcard */*.h)

TARGET   = netSim

###link
$(TARGET) : $(OBJECTS)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) 

###complile
%.o : %.cpp 
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@
%.o : */%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

###clean
.PHONY : clean
clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
	rm -f *\.d
