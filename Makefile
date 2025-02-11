CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

SRC = fenwick.cpp
OBJ = $(SRC:.cpp=.o)
EXE = fenwick

# Targets
all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(EXE)
	./$(EXE)

clean:
	rm -f $(OBJ) $(EXE)
