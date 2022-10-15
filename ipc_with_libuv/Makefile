CXX := clang++
CXXFLAGS := -Wall -g -fdiagnostics-color=always -std=c++17
# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like -lmylib -lm link in libmylib.so and libm.so:
LDFLAGS := -luv
# define any directories containing header files other than /usr/include
#
INC_DIRS := -Ilibuv/include -Ilibuv/test
LIB_DIRS := -Llibuv/.libs

PROGS := $(shell find . -name '*.cpp' -maxdepth 1 | sed 's/\.\/\(.*\)\.cpp/\1/')
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
OBJS := $(PROGS:%=%.o)

# define the executable file
MAIN := main.out

all: $(MAIN)
	@echo Make programs: main

$(MAIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LIB_DIRS) $(LDFLAGS) $^ -o $@	

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

print:
	@echo PROGS=$(PROGS)
	@echo OBJS=$(OBJS)

.PHONY: clean
clean:
	@rm $(MAIN) $(OBJS)