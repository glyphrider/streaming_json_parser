EXE=json_parser_tests
OBJS=json_parser_tests.o json_parser.o gmock-all.o gtest-all.o

GMOCK_ROOT ?= gmock-1.7.0
GTEST_ROOT ?= $(GMOCK_ROOT)/gtest
GTEST_CXXFLAGS = -I$(GTEST_ROOT) -I$(GTEST_ROOT)/include
GTEST_LDFLAGS = -pthread
GMOCK_CXXFLAGS = -I$(GMOCK_ROOT) -I$(GMOCK_ROOT)/include $(GTEST_CXXFLAGS)
GMOCK_LDFLAGS = $(GTEST_LDFLAGS)

.PHONY : tests

tests : $(EXE)
	./$(EXE)

$(EXE) : $(OBJS)
	$(CXX) $(GMOCK_LDFLAGS) -o $@ $^

gmock-all.o : $(GMOCK_ROOT)/src/gmock-all.cc
	$(CXX) $(GMOCK_CXXFLAGS) -o $@ -c $^

gtest-all.o : $(GTEST_ROOT)/src/gtest-all.cc
	$(CXX) $(GTEST_CXXFLAGS) -o $@ -c $^

%.o : %.cc
	$(CXX) $(GMOCK_CXXFLAGS) -o $@ -c $^

clean :
	rm -f $(EXE) $(OBJS)
