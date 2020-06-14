Client: main.cpp
	g++ -std=c++17 -Wall -o client main.cpp

.PHONY: test, clean

clean: Client
	rm -f client
