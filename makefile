############################################################
#               Makefile for GraphAnon Suite               #
#                                                          #
#             Copyright (c) 2015, Sean Chester             #
#               (sean.chester@idi.ntnu.no)                 #
############################################################

RM = rm -rf
MV = mv
CP = cp -rf
CC = g++

TARGET = $(OUT)/graphAnon

SRC = $(wildcard src/*.cpp) $(wildcard src/labelled_graph/*.cpp) \
			 $(wildcard src/unlabelled_graph/*.cpp)

OBJ = $(addprefix $(OUT)/,$(notdir $(SRC:.cpp=.o)))

OUT = bin

LIB_DIR = # used as -L$(LIB_DIR)
INCLUDES = -I ./src/:./src/labelled_graph/:./src/unlabelled_graph/

LIB = 

# Forces make to look these directories
VPATH = src:src/labelled_graph:src/unlabelled_graph

# By default compiling for performance (optimal)
CXXFLAGS = -O3 -m64 -march=native -mavx \
       -Wno-deprecated -Wno-write-strings -nostdlib -Wpointer-arith \
       -Wcast-qual -Wcast-align -std=c++0x -fopenmp 
           
LDFLAGS=-m64 -mavx -march=native -fopenmp #-lrt 

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
