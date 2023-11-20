TITLE:=laud
OUT:=$(TITLE)

CC:=gcc
CFLAGS:=-c -std=c17 -fPIC
CLDFLAGS:=-L$$HOME -lUbject
CBUILD:=./build/
CSRC:= $(wildcard ./c/*.c) $(wildcard ./c/ds/*.c) $(wildcard ./c/operators/*.c)
CHDR:= $(wildcard ./c/*.h) $(wildcard ./c/ds/*.h) $(wildcard ./c/operators/*.h)
COBJ:=$(CSRC:%.c=$(CBUILD)%.o)
LAUD_C:=$$HOME/lib$(OUT)-1.so

CXX:=g++
CXXFLAGS:=-c -fPIC
CXXLDFLAGS:=-L$$HOME -l$(OUT)-1
CXXBUILD:=./build/
CXXSRC:=$(wildcard ./cpp/*.cpp)
CXXHPP:=$(wildcard ./cpp/*.hpp)
CXXOBJ:=$(CXXSRC:%.cpp=$(CXXBUILD)%.o)
LAUD_CPP:=$$HOME/lib$(OUT)-2.so

IFLAGS:=-I../Ubject
GFLAGS:= -g -fno-omit-frame-pointer -fsanitize=address 



ALL: $$HOME/libUbject.so PREBUILD $(LAUD_C) $(LAUD_CPP) APP POSTBUILD
	ASAN_OPTIONS=symbolize=1 ASAN_SYMBOLIZER_PATH=$(shell which llvm-symbolizer) $$HOME/$(OUT)

$$HOME/libUbject.so:
	cd ../Ubject && make host=termux target=termux

PREBUILD: 
	mkdir -p $(CBUILD)c $(CBUILD)c/ds $(CBUILD)c/operators $(CXXBUILD)cpp

$(LAUD_C): $(COBJ)  
	$(CC) $(CLDFLAGS) $(GFLAGS) $^ -o $@ -shared

$(CBUILD)%.o:%.c
	$(CC) $(CFLAGS) $(GFLAGS) $^ -o $@ 

%.c: $(CHDR)

$(LAUD_CPP): $(CXXOBJ)  
	$(CXX) $(CXXLDFLAGS) $(GFLAGS) $^ -o $@ -shared

$(CXXBUILD)%.o:%.cpp 
	$(CXX)  $(CXXFLAGS) $(GFLAGS) $^ -o $@ 

%.cpp: $(CXXHPP)

APP: build/main.o
	$(CXX) $(CXXLDFLAGS) $(GFLAGS) -l$(OUT)-2 $^ -o $$HOME/$(OUT)

build/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(GFLAGS) -g main.cpp -o $@

POSTBUILD:
	@echo Finished building!

clean: remove_all ALL
	$$HOME/$(OUT)

remove_all:
	@rm -r ./build
	@rm $$HOME/liblaud* $$HOME/laud $$HOME/libUbject.so
	@echo ./build cleaned
	@echo
