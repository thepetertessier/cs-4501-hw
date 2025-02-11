CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -O2
TARGET = fenwick
SRC = fenwick.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	python3 fenwick.py

clean:
	rm -f $(TARGET)
