EXEC     = mineclone
EXECOPTS = 

DEBUG_FLAG = -g

CXX       = g++
CXXSTD    = c++20
CXXOPT   := -O3 $(DEBUG_FLAG) -I./src
CXXFLAGS := $(CXXOPT) -std=$(CXXSTD)

CC       = gcc
CCSTD    = c2x
CCOPT   := -O3 $(DEBUG_FLAG) -I./src
CCFLAGS := $(CCOPT) -std=$(CCSTD)

LFLAGS = $(CXXOPT)

OBJDIR = build

SOURCES  := $(shell find src/ -type f -name '*.cpp')
OBJECTS  := $(patsubst src/%.cpp, $(OBJDIR)/src/%.o, $(SOURCES))
CSOURCES := $(shell find src/ -type f -name '*.c')
COBJECTS := $(patsubst src/%.c, $(OBJDIR)/src/%.o, $(CSOURCES))

all: $(EXEC)
	@echo $(RDIRS)

$(EXEC): $(OBJECTS) $(COBJECTS)
	$(CXX) $(LFLAGS) -o $@ $^ -lGL -lglfw -ldl -lGLEW -pthread

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
