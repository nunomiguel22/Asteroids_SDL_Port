CXX = g++
CXXFLAGS = -Wall -Wextra -Wfloat-equal -pedantic -Wundef -Wswitch-default -Wunreachable-code
LDFLAGS = -g -mwindows
LBLIBS = -Iinclude -lpthread -Llib -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_net -lSDL2_mixer

# Source code folders
SRC = $(wildcard Source/*.cpp) $(wildcard Source/entities/*.cpp) $(wildcard Source/game/*.cpp) $(wildcard Source/general/*.cpp) $(wildcard Source/renderer/*.cpp) $(wildcard Source/ui/*.cpp) $(wildcard icon.o)
OBJ = $(SRC:.cc=.o)
#Executable Name
EXEC = asteroids 

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LBLIBS) $(CXXFLAGS)

clean:
	rm -rf $(OBJ) $(EXEC)


#Useful CXXFLAGS flags
#-g - Generate executable with debug information
#-lws2_32 - For libraries like winsock
#-fsanitize=address - Analyze memory leaks
#-Wall -Wextra - Show compiler warnings
#-Werror - Treat warnings as errors
#-Wfloat-equal - Warning when comparing floats with 0
#-Wundef - Warning if an undefined identifier is evaluated
#-Wswitch-default - Warning if a switch stamentent does not have a default case
#-Wunreachable-code - Warn if the compiler detects code that cannot be reached
#-Wconversion - Warn if conversion may alter value

#Optimization flags
#-O0 Default: No optimization, fastest compile time, recommended for debugging
#-O1 Moderate optimization
#-O2 Performs nearly all supported optimizations
#-O3 Full optimization
#-Os Optimize for size










