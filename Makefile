SOCKETIO_PATH=socket.io-client-cpp
WEBSOCKET_PATH=$(SOCKETIO_PATH)/lib/websocketpp

COMMON_FLAGS=-std=c++17 -Wall
WS_FLAGS=-I$(WEBSOCKET_PATH) -lboost_system -lpthread
CLIENT_FLAGS=-I$(SOCKETIO_PATH)/src -I$(SOCKETIO_PATH)/lib/rapidjson/include

Client: main.cpp
	echo $(WEBSOCKET_PATH)
	g++ $(COMMON_FLAGS) $(WS_FLAGS) $(CLIENT_FLAGS) -o client main.cpp

TestWS: test.cpp
	g++ $(COMMON_FLAGS) $(WS_FLAGS) -o test_ws test.cpp

.PHONY: clean, cleanWS

clean: Client
	rm -f client

cleanWS: TestWS
	rm -f test_ws
