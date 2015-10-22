############################################################
#            Makefile for AlphaProximity Suite             #
#                                                          #
#             Copyright (c) 2015, Sean Chester             #
#                    (schester@cs.au.dk)                   #
############################################################

RM = rm -rf
MV = mv
CP = cp -rf
CC = g++

TARGET = $(OUT)/alpha_proximity

SRC = $(wildcard src/*.cpp)

OBJ = $(addprefix $(OUT)/,$(notdir $(SRC:.cpp=.o)))

OUT = bin

LIB_DIR = # used as -L$(LIB_DIR)
INCLUDES = -I ./src/

LIB = 

# Forces make to look these directories
VPATH = src:

# By default compiling for performance (optimal)
CXXFLAGS = -O3 -m64 -march=native -mavx \
       -Wno-deprecated -Wno-write-strings -nostdlib -Wpointer-arith \
       -Wcast-qual -Wcast-align -std=c++0x 
           
LDFLAGS=-m64 #-lrt 

# All Target
all: $(TARGET)

# Tool invocations
$(TARGET): $(OBJ) $(LIB_DIR)$(LIB)
	@echo 'Building target: $@ (GCC C++ Linker)'
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)
	@echo 'Finished building target: $@'
	@echo ' '

$(OUT)/%.o: %.cpp
	@echo 'Building file: $< (GCC C++ Compiler)'
	$(CC) $(CXXFLAGS) $(INCLUDES) -c -o"$@" "$<" 
	@echo 'Finished building: $<'
	@echo ' '

clean:
	-$(RM) $(OBJ) $(TARGET) $(addprefix $(OUT)/,$(notdir $(SRC:.cpp=.d)))
	-@echo ' '

deepclean:
	-$(RM) bin/*
	-@echo ' '


.PHONY: all clean deepclean dbg tests
