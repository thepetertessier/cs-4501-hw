CXX = g++
CXXFLAGS = -O3 -march=native -mtune=native -ffast-math -flto -std=c++20

SRC = main.cpp
OBJ = $(SRC:.cpp=.o)
EXE = $(SRC:.cpp=)

TSRC = test.cpp
TOBJ = $(TSRC:.cpp=.o)
TEXE = $(TSRC:.cpp=)


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


ctest: $(TOBJ)
	$(CXX) $(CXXFLAGS) -o $(TEXE) $(TOBJ)

rtest: ctest
	./$(TEXE)
