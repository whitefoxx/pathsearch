CXX := clang++
CXXFLAGS := -Wall -g -fdiagnostics-color=always -std=c++17
LDFLAGS := -luv
INC_DIRS := -Ilibuv/include -Ilibuv/test
LIB_DIRS := -Llibuv/.libs

PROGS := $(shell find . -name '*.cpp' -maxdepth 1 | sed 's/\.\/\(.*\)\.cpp/\1/')
OBJS := $(PROGS:%=%.o)

all: main.out
	@echo Make programs: main

main.out: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LIB_DIRS) $(LDFLAGS) $^ -o $@	

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

print:
	@echo PROGS=$(PROGS)
	@echo OBJS=$(OBJS)

.PHONY: clean
clean:
	@rm main.out $(OBJS)