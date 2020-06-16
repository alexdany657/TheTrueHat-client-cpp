SOCKETIO_PATH=socket.io-client-cpp/src
WEBSOCKET_PATH=websocketpp
RAPIDJSON_PATH=rapidjson/include

COMMON_FLAGS=-std=c++17 -Wall -Llib -lsio_client -lpthread
WS_FLAGS=-I$(WEBSOCKET_PATH) -lboost_system
CLIENT_FLAGS=-I$(SOCKETIO_PATH)

Client: main.cpp
	g++ $(COMMON_FLAGS) $(CLIENT_FLAGS) -o client main.cpp

TestWS: test.cpp
	g++ $(COMMON_FLAGS) $(WS_FLAGS) -o test_ws test.cpp

.PHONY: clean, cleanWS

clean: Client
	rm -f client

cleanWS: TestWS
	rm -f test_ws
