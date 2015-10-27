EXEC_NAME = "lc3sticher"
all:
	g++ -o $(EXEC_NAME) -O3 -std=c++11 Parser.cpp main.cpp
release:
	g++ -o $(EXEC_NAME) -O3 -std=c++11 Parser.cpp main.cpp
debug:
	g++ -o $(EXEC_NAME)d -g -std=c++11 Parser.cpp main.cpp
clean:
	rm $(EXEC_NAME) $(EXEC_NAME)d *.o
