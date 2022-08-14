EXEC     = mineclone
EXECOPTS = 

DEBUG = true

ifeq ($(DEBUG), true)
	OPTIM_FLAG =
	DEBUG_FLAG = -g
	OBJDIR = build/debug
endif
ifneq ($(DEBUG), true)
	OPTIM_FLAG = -O3
	DEBUG_FLAG =
	OBJDIR = build/release
endif

CXX       = g++
CXXSTD    = c++20
CXXOPT   := $(OPTIM_FLAG) $(DEBUG_FLAG) -I./src -I./lib/imgui `pkg-config --cflags freetype2`
CXXFLAGS := $(CXXOPT) -std=$(CXXSTD)

CC       = gcc
CCSTD    = c2x
CCOPT   := $(OPTIM_FLAG) $(DEBUG_FLAG) -I./src -I./lib/imgui `pkg-config --cflags freetype2`
CCFLAGS := $(CCOPT) -std=$(CCSTD)

LFLAGS = $(CXXOPT) -lGL -lglfw -ldl -lGLEW -pthread -lm -lstdc++ -lfreetype

SOURCES  := $(shell find src/ -type f -name '*.cpp')
OBJECTS  := $(patsubst src/%.cpp, $(OBJDIR)/src/%.o, $(SOURCES))
CSOURCES := $(shell find src/ -type f -name '*.c')
COBJECTS := $(patsubst src/%.c, $(OBJDIR)/src/%.o, $(CSOURCES))

LIB_SOURCES  := $(shell find lib/ -type f -name '*.cpp')
LIB_OBJECTS  := $(patsubst lib/%.cpp, $(OBJDIR)/lib/%.o, $(LIB_SOURCES))
LIB_CSOURCES := $(shell find lib/ -type f -name '*.c')
LIB_COBJECTS := $(patsubst lib/%.c, $(OBJDIR)/lib/%.o, $(LIB_CSOURCES))

all: $(EXEC)
	@echo $(RDIRS)

$(EXEC): $(OBJECTS) $(COBJECTS) $(LIB_OBJECTS) $(LIB_COBJECTS)
	$(CXX) $(LFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c -o $@ $<

run: $(EXEC)
	./$< $(EXECOPTS)

debug: $(EXEC)
	gdb ./$<

crun:
	@$(MAKE) clean
	@$(MAKE) run
cdebug:
	@$(MAKE) clean
	@$(MAKE) debug

clean:
	rm -f $(EXEC) $(OBJECTS) $(COBJECTS)

clean_libs:
	rm -f $(LIB_OBJECTS) $(LIB_COBJECTS)
