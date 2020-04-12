CFLAGS  = -Wall -fPIC -ggdb -O0  $(OPTS) $(INCLUDE) $(DEFINES)
LDFLAGS = -lpthread -lssl -lcrypto -ljansson -ldl -lm -lrt -L. -ggdb

OBJ					= obj
EXECUTABLE 			= rccast

BASE		= .

SRC			= ./
NANOPB		= $(BASE)/nanopb

DEFINES 	= -D_FILE_OFFSET_BITS=64 -DPB_FIELD_16BIT -DHAVE_STDINT_H

CFLAGS 		+= -fdata-sections -ffunction-sections 

LDFLAGS 	+= -Wl,--gc-sections

vpath %.c $(TOOLS):$(SRC):$(NANOPB)

INCLUDE = -I. \
		  -I$(SRC) -I$(SRC)/inc \
		  -I$(NANOPB) 
		  		  
DEPS	= 
				  
SOURCES =  cast_util.c util.c util_common.c log_util.c \
		   castcore.c cast_parse.c castmessage.pb.c \
		  pb_common.c pb_decode.c pb_encode.c \
		   main.c
		
SOURCES_LIBS = 
		
OBJECTS 		= $(patsubst %.c,$(OBJ)/%.o,$(SOURCES)) $(patsubst %.c,$(OBJ)/%.o,$(SOURCES_LIBS)) 

LIBRARY 		= 

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBRARY) $(LDFLAGS)  -o $@

$(OBJECTS): $(DEPS) | bin $(OBJ) 

$(OBJ):
	@mkdir -p $@
	
bin:	
	@mkdir -p bin

$(OBJ)/%.o : %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) $< -c -o $@

$(OBJ)/%-static.o : %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -DLINKALL $(INCLUDE) $< -c -o $(OBJ)/$*-static.o	
	
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(OBJECTS_STATIC) $(EXECUTABLE_STATIC) 

