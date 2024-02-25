OUT         := laud
SRCDIR		:= ./ $(filter-out build/%, $(wildcard */ */*/ */*/*/ */*/*/*/ */*/*/*/*/))
OBJDIR		:= $(addprefix build/, $(SRCDIR))
CSRC		:= $(wildcard $(addsuffix *.c, $(SRCDIR)))
CXXSRC		:= $(filter-out %/main.cpp, $(wildcard $(addsuffix *.cpp, $(SRCDIR))))
COBJS		:= $(patsubst %.c, %.o, $(addprefix build/,$(CSRC)))
CXXOBJS		:= $(patsubst %.cpp, %.o, $(addprefix build/,$(CXXSRC)))
DEPS		:= $(patsubst %.o, %.o.d, $(COBJS))
BIN			:= $$HOME/

CFLAGS		:= -fPIC -ggdb3 -fno-omit-frame-pointer -fsanitize=address 
CPPFLAGS	:= -I../Ubject -I../laud
LDFLAGS		:= -L$$HOME
LDLIBS		:= -lUbject -l$(OUT)-0 -l$(OUT)-1 -lm

vpath $(OUT) build/
vpath %.so build/:../Ubject/build/

.PHONY: all run clean

all : libUbject.so lib$(OUT)-0.so lib$(OUT)-1.so $(OUT);

libUbject.so : $(wildcard $(addprefix ../Ubject, /*.c /*/*.c))
	@cd ../Ubject && $(MAKE)

build/%.o : %.c
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $< 
	@echo $@ built

lib$(OUT)-0.so : $(COBJS)
	@echo building $@
	@$(CC) -shared -o $$HOME/$@ $^
	@cp  $$HOME/$@ build/
	@echo $@ built

build/%.o : %.cpp
	@$(CXX) $(CFLAGS) $(CPPFLAGS) -c -o $@ $< 
	@echo $@ built

lib$(OUT)-1.so : $(CXXOBJS)
	@echo building $@
	@$(CXX) -shared -o $$HOME/$@ $^
	@cp  $$HOME/$@ build/
	@echo $@ built

$(OUT): main.cpp 
	@echo building $@
	@$(CXX) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -o $$HOME/$@ $<
	@cp  $$HOME/$@ build/
	@echo $@ built

build/%.o.d : %.c | build
	@set -e;\
	rm -f $@;\
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$;\
	sed 's,\(.*\.o\)[:]*,build/$*.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

build/%.o.d : %.cpp | build
	@set -e;\
	rm -f $@;\
	$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$;\
	sed 's,\(.*\.o\)[:]*,build/$*.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

build:
	@mkdir -p build $(OBJDIR)

run : all
	ASAN_OPTIONS=symbolize=1 ASAN_SYMBOLIZER_PATH=$(shell which llvm-symbolizer) $$HOME/$(OUT)

clean:
	@cd ../Ubject && $(MAKE) clean
	@$(RM) -r build
	@echo build removed

include $(DEPS)
