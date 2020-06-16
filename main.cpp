#include <sio_client.h>

#include <iostream>
#include <mutex>
#include <condition_variable>

#define DEBUG

#ifdef DEBUG
bool _log = true;
#else
bool _log = false;
#endif

int plaeyrsCount = 0;
std::mutex _lock;
std::condition_variable_any _cond;
bool connect_finish = false;

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

void on_connected() {
    _lock.lock();
    _cond.notify_all();
    connect_finish = true;
    _lock.unlock();
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

    h.set_open_listener(&on_connected);

    h.connect("ws://localhost:3005");

    _lock.lock();
    if (!connect_finish) {
        _cond.wait(_lock);
    }
    _lock.unlock();

    h.socket()->on("sFailure", &ONsFailure);
    h.socket()->on("sYouJoined", &ONsYouJoined);

    sio::object_message::ptr pJoinRoom = sio::object_message::create();

    pJoinRoom->get_map()["key"] = sio::string_message::create(key);
    pJoinRoom->get_map()["username"] = sio::string_message::create(name);
    pJoinRoom->get_map()["time_zone_offset"] = sio::int_message::create(0);

    h.socket()->emit("cJoinRoom", pJoinRoom);

    int N;
    std::cin >> N;

    return 0;
}
