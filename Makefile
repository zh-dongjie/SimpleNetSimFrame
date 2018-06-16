CC            = gcc
CXX           = g++
#DEFINES       = -DQT_QML_DEBUG
CFLAGS        = -pipe -g -Wall -W -fPIC #$(DEFINES)
CXXFLAGS      = -pipe -g -std=gnu++11  -W -fPIC #$(DEFINES)
LINK          = g++
LFLAGS        = -Wl,-O1

OBJECTS  = main.o channel.o core.o global.o i_string.o manager.o router.o routing_func.o \
           serial_manager.o shr_mem_manager.o time_module.o

TARGET   = netSim
###link
$(TARGET) : $(OBJECTS)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) 
###complile
main.o:      main.cpp  i_shm.h \
		 msgbuffer.h \
		 flit.h \
		 time_module.h \
		 global.h \
		 manager.h
	$(CXX) -c $(CXXFLAGS)  -o main.o  main.cpp

channel.o:  channel.cpp  channel.h \
		 core.h \
		 i_string.h \
		 base_object.h \
		 time_module.h \
		 global.h \
		 router.h \
		 flit.h
	$(CXX) -c $(CXXFLAGS)  -o channel.o  channel.cpp

core.o:  core.cpp  i_shm.h \
		 channel.h \
		 core.h \
		 i_string.h \
		 base_object.h \
		 time_module.h \
		 global.h \
		 router.h \
		 flit.h \
		 msgbuffer.h
	$(CXX) -c $(CXXFLAGS)  -o core.o  core.cpp

global.o:  global.cpp  i_string.h \
		 manager.h \
		 time_module.h \
		 global.h \
		 router.h \
		 flit.h \
		 base_object.h \
		 routing_func.h
	$(CXX) -c $(CXXFLAGS)  -o global.o  global.cpp

manager.o:  manager.cpp  i_string.h \
		 manager.h \
		 time_module.h \
		 serial_manager.h \
		 channel.h \
		 core.h \
		 base_object.h \
		 global.h \
		 router.h \
		 flit.h \
		 shr_mem_manager.h
	$(CXX) -c $(CXXFLAGS)  -o manager.o  manager.cpp

router.o:  router.cpp  msgbuffer.h \
		 routing_func.h \
		 router.h \
		 flit.h \
		 time_module.h \
		 global.h \
		 base_object.h \
		 channel.h \
		 core.h \
		 i_string.h
	$(CXX) -c $(CXXFLAGS)  -o router.o  router.cpp

routing_func.o:  routing_func.cpp  routing_func.h \
		 router.h \
		 flit.h \
		 time_module.h \
		 global.h \
		 base_object.h
	$(CXX) -c $(CXXFLAGS)  -o routing_func.o  routing_func.cpp

serial_manager.o:  serial_manager.cpp  i_string.h \
		 channel.h \
		 core.h \
		 base_object.h \
		 time_module.h \
		 global.h \
		 router.h \
		 flit.h \
		 serial_manager.h \
		 manager.h
	$(CXX) -c $(CXXFLAGS)  -o serial_manager.o  serial_manager.cpp

shr_mem_manager.o:  shr_mem_manager.cpp  i_shm.h \
		 i_string.h \
		 msgbuffer.h \
		 channel.h \
		 core.h \
		 base_object.h \
		 time_module.h \
		 global.h \
		 router.h \
		 flit.h \
		 shr_mem_manager.h \
		 manager.h
	$(CXX) -c $(CXXFLAGS)  -o shr_mem_manager.o  shr_mem_manager.cpp

time_module.o:  time_module.cpp  time_module.h
	$(CXX) -c $(CXXFLAGS)  -o time_module.o  time_module.cpp

i_string.o:  i_string.cpp  i_string.h
	$(CXX) -c $(CXXFLAGS)  -o i_string.o  i_string.cpp

###clean
.PHONY : clean
clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
