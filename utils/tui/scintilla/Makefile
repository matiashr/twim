# A generic build template for C/C++ programs
EXTERNAL=../../../../

# executable name
LIB= lscintilla.a

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
	-I $(EXTERNAL)/tvision.git/include\
	-I include\
	-I scintilla/include\
	-I scintilla/src/\
	-I scintilla/lexlib/\
	-I src/include/scintilla/include\
	-I src/include/scintilla/src\
	-I src/include/scintilla/lexlib\
	-I.
	#-I $(EXTERNAL)/tvision.git/include/tvision/compat/borland\

# C/C++ flags
CPPFLAGS = -Wall
# dependency-generation flags
DEPFLAGS = -MMD -MP
# linker flags
LDFLAGS =  -lrt -ldl -rdynamic  -pthread 

# library flags
LDLIBS = 

# build directories
BIN = bin
OBJ = obj
SRC = src


SOURCES := $(wildcard  $(SRC)/*.cxx  scintilla/lexers/*.cxx )

OBJECTS := \
	$(patsubst $(SRC)/%.cxx, $(OBJ)/%.o, $(wildcard $(SRC)/*.cxx)) \
	$(patsubst scintilla/lexers/%.cxx, $(OBJ)/scintilla/src/%.o, $(wildcard src/scintilla/lexers/*.cxx)) 

# include compiler-generated dependency rules
DEPENDS := $(OBJECTS:.o=.d)

# compile C source
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
# compile C++ source
COMPILE.cxx = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@
# link objects
LINK.o = ar -rcs $@ $(OBJECTS) 

.DEFAULT_GOAL = all

.PHONY: all
all: $(BIN)/$(LIB)

$(BIN)/$(LIB): $(SRC) $(OBJ) $(BIN) $(OBJECTS)
	echo link
	$(LINK.o)

$(SRC):
	mkdir -p $(SRC)

dirs:
	mkdir -p $(OBJ)
	mkdir -p $(OBJ)/scintilla/src/

$(BIN):
	mkdir -p $(BIN)

$(OBJ)/%.o:$(SRC)/%.c
	$(COMPILE.c) $<

$(OBJ)/%.o:$(SRC)/%.cc
	$(COMPILE.cxx) $<

$(OBJ)/%.o:$(SRC)/%.cpp
	$(COMPILE.cxx) $<

$(OBJ)/%.o:$(SRC)/%.cxx
	$(COMPILE.cxx) $<

$(OBJ)/src/scintilla/src/%.o:$(SCINTILLA)/%.cxx
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
	@echo $(SOURCES)
	@echo $(OBJECTS)


-include $(DEPENDS)
