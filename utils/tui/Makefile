# A generic build template for C/C++ programs
EXTERNAL=../../../

# executable name
EXE = tcedit

# C compiler
CC = gcc
# C++ compiler
CXX = g++
# linker
LD = g++
# C flags
CFLAGS = 
# C++ flags
CXXFLAGS =\
	-g\
	-I $(EXTERNAL)/json/include\
	-I $(EXTERNAL)/moderncpp.git/include\
	-I $(EXTERNAL)/json/include\
	-I $(EXTERNAL)/json/include/nlohmann/ \
	-I $(EXTERNAL)/json/include/\
	-I $(EXTERNAL)/tvision.git/include\
	-I $(EXTERNAL)/tvision.git/include/tvision/compat/borland\
	-I.

# C/C++ flags
CPPFLAGS = -Wall
# dependency-generation flags
DEPFLAGS = -MMD -MP
# linker flags
LDFLAGS =  -lrt -ldl -lmbedtls -lmbedx509 -lmbedcrypto -rdynamic  -pthread \
     $(EXTERNAL)/tvision.git/build/libtvision.a\
     $(shell ncurses5-config --cflags --libs)\
     -lzmq

# library flags
LDLIBS = 

# build directories
BIN = bin
OBJ = obj
SRC = src

SOURCES := $(wildcard $(SRC)/*.c $(SRC)/*.cc $(SRC)/*.cpp $(SRC)/*.cxx $(SRC)/plugin/*.cpp)

OBJECTS := \
	$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(wildcard $(SRC)/*.c)) \
	$(patsubst $(SRC)/%.cc, $(OBJ)/%.o, $(wildcard $(SRC)/*.cc)) \
	$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(wildcard $(SRC)/*.cpp)) \
	$(patsubst $(SRC)/%.cxx, $(OBJ)/%.o, $(wildcard $(SRC)/*.cxx)) \
	$(patsubst $(SRC)/plugin/%.cpp, $(OBJ)/plugin/%.o, $(wildcard $(SRC)/plugin/*.cpp))

# include compiler-generated dependency rules
DEPENDS := $(OBJECTS:.o=.d)

# compile C source
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
# compile C++ source
COMPILE.cxx = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@
# link objects
LINK.o = $(LD) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

.DEFAULT_GOAL = all

.PHONY: all
all: $(BIN)/$(EXE)

$(BIN)/$(EXE): $(SRC) $(OBJ) $(BIN) $(OBJECTS)
	$(LINK.o)

$(SRC):
	mkdir -p $(SRC)

$(OBJ):
	mkdir -p $(OBJ)
	mkdir -p $(OBJ)/plugin

$(BIN):
	mkdir -p $(BIN)

$(OBJ)/%.o:	$(SRC)/%.c
	$(COMPILE.c) $<

$(OBJ)/%.o:	$(SRC)/%.cc
	$(COMPILE.cxx) $<

$(OBJ)/%.o:	$(SRC)/%.cpp
	$(COMPILE.cxx) $<

$(OBJ)/%.o:$(SRC)/%.cxx
	$(COMPILE.cxx) $<

$(OBJ)/plugin/%.o:$(SRC)/plugin/%.cpp
	$(COMPILE.cxx) $<


# force rebuild
.PHONY: remake
remake:	clean $(BIN)/$(EXE)

# execute the program
.PHONY: run
run: $(BIN)/$(EXE)
	./$(BIN)/$(EXE)

# remove previous build and objects
.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) $(DEPENDS)
	$(RM) $(BIN)/$(EXE)

# remove everything except source
.PHONY: reset
reset:
	$(RM) -r $(OBJ)
	$(RM) -r $(BIN)


list:
	@echo $(OBJECTS)


-include $(DEPENDS)
