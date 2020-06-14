#include "socket.io-client-cpp/src/sio_client.h"

#include <iostream>

#define DEBUG

#ifdef DEBUG
bool __log = true;
#else
bool __log = false;
#endif

int plaeyrsCount = 0;

void ONsFailure(sio::event &ev) {
    if (__log) {
        std::cerr << ev->getMap()["msg"]->getString() << '\n';
    }
}

void ONsYouJoined(sio::event &ev) {
    if (__log) {
        std::cerr << ev->getMap()["msg"]->getString()
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Wrong number of argument: expected 3, got " << argc << ".\n";
        return 1;
    }

    std::string key, name;
    int playersCount;

    key = argv[1];
    name = argv[3];
    playersCount = atoi(argv[2]);

    sio::client h;
    h.connect("https://m20-sch57.site:3005");

    //

    return 0;
}
