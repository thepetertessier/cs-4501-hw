CXX = g++
CXXFLAGS = -g -std=c++20 -Wall -Wextra -O2

SRC = main.cpp
OBJ = $(SRC:.cpp=.o)
EXE = exe

# Targets
all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJ)

main.o: main.cpp main.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(EXE)
	./$(EXE)

clean:
	rm -f $(OBJ) $(EXE)
