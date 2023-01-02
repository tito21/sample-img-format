
#OBJS specifies which files to compile as part of the project
OBJS = img-compressor.cpp Image.cpp CompImage.cpp

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -Wall

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = $(shell pkg-config --cflags --libs opencv4)

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = img-compressor

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

debug : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) -g $(LINKER_FLAGS) -o $(OBJ_NAME)