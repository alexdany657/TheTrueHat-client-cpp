#include <sio_client.cpp>

#include <iostream>

#define DEBUG

#ifdef DEBUG
bool _log = true;
#else
bool _log = false;
#endif

int plaeyrsCount = 0;

void ONsFailure(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (_log) {
        std::cerr << data->get_map()["msg"]->get_string() << '\n';
    }
}

void ONsYouJoined(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (_log) {
        std::cerr << data->get_map()["key"]->get_string() << '\n';
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

    h.socket()->on("sFailure", &ONsFailure);
    h.socket()->on("sYouJoined", &ONsYouJoined);

    h.socket()->emit("cJoinRoom", "{\"key\": \"" + key + "\", \"username\": \"" + name + "\", \"time_zone_offset\": 0}");

    return 0;
}
