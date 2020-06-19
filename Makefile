SOCKETIO_PATH=socket.io-client-cpp/src
WEBSOCKET_PATH=websocketpp
RAPIDJSON_PATH=rapidjson/include

COMMON_FLAGS=-std=c++17 -Wall -Llib -lsio_client -lpthread -lssl -lcrypto
CLIENT_FLAGS=-I$(SOCKETIO_PATH)

client: main.cpp
	g++ $(COMMON_FLAGS) $(CLIENT_FLAGS) -o client main.cpp

build: sio_client.o sio_client_impl.o sio_packet.o sio_socket.o
	g++ -shared -o lib/libsio_client.so lib/sio_client.o lib/sio_client_impl.o lib/sio_packet.o lib/sio_socket.o
	cp -v lib/libsio_client.so ~/lib

sio_client.o:
	g++ -std=c++17 -c -shared -fpic -fPIC -o lib/sio_client.o -I$(WEBSOCKET_PATH) $(SOCKETIO_PATH)/sio_client.cpp -DSIO_TLS

sio_socket.o:
	g++ -std=c++17 -c -shared -fpic -fPIC -o lib/sio_socket.o -I$(WEBSOCKET_PATH) $(SOCKETIO_PATH)/sio_socket.cpp -DSIO_TLS

sio_packet.o: $(SOCKETIO_PATH)/internal/sio_packet.cpp
	g++ -std=c++17 -c -shared -fpic -fPIC -o lib/sio_packet.o -I$(WEBSOCKET_PATH) -I$(RAPIDJSON_PATH) $(SOCKETIO_PATH)/internal/sio_packet.cpp -DSIO_TLS

sio_client_impl.o: $(SOCKETIO_PATH)/internal/sio_client_impl.cpp
	g++ -std=c++17 -c -shared -fpic -fPIC -o lib/sio_client_impl.o -I$(WEBSOCKET_PATH) $(SOCKETIO_PATH)/internal/sio_client_impl.cpp -DSIO_TLS

.PHONY: clean

clean: client
	rm -f client
