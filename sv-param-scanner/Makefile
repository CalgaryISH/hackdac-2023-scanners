CXX = g++
CXXFLAGS = -std=c++17
LIBS = -lslang -lfmt

scanner: scanner.cpp scanner.h
	$(CXX) scanner.cpp -o scanner $(CXXFLAGS) $(LIBS)

.PHONY: clean
clean:
	-rm scanner
