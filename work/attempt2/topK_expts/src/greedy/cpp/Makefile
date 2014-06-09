CXX += -std=c++0x
CXXFLAGS += -O2
INC += -I${BOOST_ROOT}
LIBS += -L/usr/local/lib
LDFLAGS=-lboost_program_options -lboost_filesystem -lboost_system

SRCS=selectGreedy.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
EXECS=$(patsubst %.cpp,%,$(SRCS))

all: selectGreedy

depend: .depend

.depend: $(SRCS)
	$(CXX) $(CXXFLAGS) -MM $^ -MF ./.depend

include .depend

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<  $(LDFLAGS)

%: %.o
	$(CXX) $(CXXFLAGS) $(INC) -o $@ $< $(LIBS) $(LDFLAGS)

clean:
	@- $(RM) $(OBJS) $(EXECS)
