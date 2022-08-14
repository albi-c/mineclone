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

CXX       = clang
CXXSTD    = c++20
CXXOPT   := $(OPTIM_FLAG) $(DEBUG_FLAG) -I./src -I./src/lib/imgui `pkg-config --cflags freetype2`
CXXFLAGS := $(CXXOPT) -std=$(CXXSTD)

CC       = clang
CCSTD    = c2x
CCOPT   := $(OPTIM_FLAG) $(DEBUG_FLAG) -I./src -I./src/lib/imgui `pkg-config --cflags freetype2`
CCFLAGS := $(CCOPT) -std=$(CCSTD)

LFLAGS = $(CXXOPT) -lGL -lglfw -ldl -lGLEW -pthread -lm -lstdc++ -lfreetype

SOURCES  := $(shell find src/ -type f -name '*.cpp')
OBJECTS  := $(patsubst src/%.cpp, $(OBJDIR)/src/%.o, $(SOURCES))
CSOURCES := $(shell find src/ -type f -name '*.c')
COBJECTS := $(patsubst src/%.c, $(OBJDIR)/src/%.o, $(CSOURCES))

all: $(EXEC)
	@echo $(RDIRS)

$(EXEC): $(OBJECTS) $(COBJECTS)
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
	echo $(DD)
	rm -f $(EXEC) $(OBJECTS) $(COBJECTS)
