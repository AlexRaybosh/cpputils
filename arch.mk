
$(info COMPILING FOR $(ARCH))
BUILD:=$(ROOT)/build$(SUFFIX)/$(ARCH)

$(shell mkdir -p $(BUILD))

LIB_STATIC:=$(BUILD)/libcpputils.a
LIB_DYNAMIC:=$(BUILD)/libcpputils.so
SONAME:=libcpputils.so.1

SRC:=$(wildcard src/*.cc)
OBJS:=$(patsubst %.cc, $(BUILD)/%.o, $(SRC))
TEST_SRC:=$(wildcard tests/*.cc)
TEST_OBJS:=$(patsubst %.cc, $(BUILD)/%.o, $(TEST_SRC))
TEST_EXE:=$(patsubst %.cc, $(BUILD)/%, $(TEST_SRC))


H:=$(wildcard src/*.h)
$(info Sources: $(SRC))
$(info Headers: $(H))
$(info Objects: $(OBJS))
$(info Executable: $(LIB_STATIC))

$(info Test Sources: $(TEST_SRC))
$(info Test Objects: $(TEST_OBJS))
$(info Test Exec: $(TEST_EXE))
$(info Test Extra: $(TEST_EXTRA))


CXXFLAGS:=$(OPT) \
	-I$(ROOT)/src \
	-Wall -fmessage-length=0 -Wno-pessimizing-move -Wno-unused-result \
	-D_REENTRANT \
	-D_POSIX_C_SOURCE=202001L -D_XOPEN_SOURCE=600 -fPIC -pthread \
	-std=c++17

ifeq "$(ARCH)" "armhf"
	SYSROOT:=$(ROOT)/sysroot/$(ARCH)
	CXX:=arm-linux-gnueabihf-g++
	CXXFLAGS:=$(CXXFLAGS) --sysroot=$(SYSROOT)  -I$(SYSROOT)/usr/include
	LDFLAGS:=--sysroot=$(SYSROOT) -L$(SYSROOT)/lib/arm-linux-gnueabihf \
		-L$(SYSROOT)/usr/lib/arm-linux-gnueabihf \
		-static-libstdc++ -static-libgcc -Wl,-Bstatic -ljansson -Wl,-Bdynamic \
		-pthread -lrt
	AR:=arm-linux-gnueabihf-ar
	TESTER:= qemu-arm
	STRIP:=arm-linux-gnueabihf-strip
	OBJDUMP:=objdump
else ifeq "$(ARCH)" "x86_64"
	CXX:=g++
	LDFLAGS:= -L/usr/lib/x86_64-linux-gnu -ljansson -pthread -lrt
	AR:=ar
	TESTER:=
	STRIP:=strip
	OBJDUMP:=objdump
endif



$(info CXX $(CXX))
$(info LDFLAGS $(LDFLAGS))
$(info CXXFLAGS $(CXXFLAGS))

$(BUILD)/%.o : %.cc
	echo "Processing " $< " into " $@
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/tests/%.o : tests/%.cc
	echo "Processing " $< " into " $@
	mkdir -p $(BUILD)/tests
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/tests/% : $(BUILD)/tests/%.o
	echo "Processing " $< " into " $@
	$(CXX) -fPIC -o $@ $< -L$(BUILD) -Wl,-Bstatic -lcpputils -Wl,-Bdynamic $(LDFLAGS)
	@echo "Built: " $@
	@$(TESTER) $@ || { echo Test $@ Failed!!! ; exit 1; }

#mkdir -p $(@D)
	

$(BUILD)/tests/% : $(OBJ) 

$(OBJS) : $(SRC) $(H) Makefile arch.mk

$(TEST_OBJS) : $(TEST_SRC) $(SRC) $(H) Makefile arch.mk

$(LIB_DYNAMIC): $(OBJS)
	$(CXX) -shared -fPIC -o $@ -Wl,-soname,$(SONAME) $(OBJS) $(LDFLAGS)
ifeq "$(SUFFIX)" ""
	$(STRIP) $@
endif
	$(OBJDUMP) -p $@ | grep SONAME

$(LIB_STATIC): $(OBJS)
	$(AR) rvs $@ $(OBJS)
ifeq "$(SUFFIX)" ""
	$(STRIP) $@
endif	

$(TEST_EXE) : $(LIB_STATIC) $(LIB_DYNAMIC)

all: $(LIB_STATIC) $(LIB_DYNAMIC) $(TEST_EXE) 

clean:
	rm -rf $(BUILD)/*
	
.PHONY: clean
