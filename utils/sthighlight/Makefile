EXTERNAL=../external/
WX_CONFIG := wx-config
CFLAGS=-I$(EXTERNAL)/argh.git\
	-g \
	$(shell $(WX_CONFIG) --cxxflags)\
	$(shell xml2-config --cflags )\
	-std=c++17\
	-lstdc++fs\
	-DUNIX\
	   -g\
	-I .\
    -I src\
-I /usr/include/fxscintilla\
    -I ./include/\
    -Icontrols\
    -I $(EXTERNAL)/json/include

ifeq ( $(shell uname),Linux)
CFLAGS+=-DUNIX_BUILD
endif

EXTRALIBS=
#$(EXTERNAL)/open62541.git/build/bin/libopen62541.a  
LIBS=-lpthread $(shell $(WX_CONFIG) --libs all ) $(shell xml2-config --libs )
$(info "Unix build")
OUTPUT=stcat

LIBOBJS=

OBJS=\
src/main.o

$(LIBOBJS)


COBJS=


-include $(OBJS:.o=.d)

%.o:%.c
	@echo  "CC $<"
	@gcc -std=c99 -o $@ -I. -MT$@  -MD -MP -c $< 


%.o:%.cpp
	@echo  "CXX $<"
	@g++ $(CFLAGS)  -g -c -o $@ -MT$@ -MFaccesstest_accesstest.d -MD -MP $< 


all: $(OUTPUT)
	@echo make $(OUTPUT)


#############################################################
$(OUTPUT): $(OBJS) $(COBJS)
	@echo linking $@ $(COBJS) $(OBJS), link app with
	@g++ -g $(OBJS) $(COBJS) $(LIBS) $(EXTRALIBS) -o $(OUTPUT) 

$(OUTPUTQ): $(OBJS) $(COBJS)
	@echo linking $@
	@g++ $(CFLAGS) -g -DUNITTEST queue.cpp  $(LIBS) $(EXTRALIBS) -o $(OUTPUTQ) 


clean:
	rm -f $(OBJS) $(OUTPUT) *.d libmf.a


