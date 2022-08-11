CXX       = g++
CXXSTD    = c++20
CXXOPT    = -O3
CXXFLAGS  = $(CXXOPT) -std=$(CXXSTD)

CC       = gcc
CCSTD    = c2x
CCOPT    = -O3
CCFLAGS  = $(CCOPT) -std=$(CCSTD)

LFLAGS = $(CXXOPT)

SOURCES = $(shell find src/ -type f -name '*.cpp')
OBJECTS = $(patsubst src/%.cpp, src/%.o, $(SOURCES))
CSOURCES = $(shell find src/ -type f -name '*.c')
COBJECTS = $(patsubst src/%.c, src/%.o, $(CSOURCES))

EXEC     = mineclone
EXECOPTS = 

all: $(EXEC)

$(EXEC): $(OBJECTS) $(COBJECTS)
	$(CXX) $(LFLAGS) -o $@ $^ -lGL -lglfw -ldl -lGLEW

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

run: $(EXEC)
	./$< $(EXECOPTS)

debug: $(EXEC)
	gdb ./$<

clean:
	rm -f $(OBJECTS) $(COBJECTS) $(EXEC)
