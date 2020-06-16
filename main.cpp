#include <sio_client.h>

#include <unistd.h>
#include <iostream>
#include <mutex>
#include <condition_variable>

#define DEBUG

#ifdef DEBUG
bool _log = true;
#else
bool _log = false;
#endif

size_t playersCount = 0;
std::mutex _lock;
std::condition_variable_any _cond;
bool connect_finish = false;
bool game_finished = false;

sio::client h;
std::string name = "";

bool should_start(const sio::message::ptr &data) {
    return data->get_map()["playerList"]->get_vector().size() == playersCount && name == data->get_map()["host"]->get_string();
}

void ONsFailure(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (_log) {
        std::cerr << data->get_map()["msg"]->get_string() << '\n';
    }
}

void ONsYouJoined(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (should_start(data)) {
        h.socket()->emit("cStartGame");
    }
}

void ONsPlayerJoined(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (should_start(data)) {
        h.socket()->emit("cStartGame");
    }
}

void ONsGameStarted(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (name == data->get_map()["speaker"]->get_string()) {
        h.socket()->emit("cSpeakerReady");
    }
    if (name == data->get_map()["listener"]->get_string()) {
        h.socket()->emit("cListenerReady");
    }
}

void ONsNewWord(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    sleep(1);
    sio::object_message::ptr pCauseMsg = sio::object_message::create();
    pCauseMsg->get_map()["cause"] = sio::string_message::create("explained");
    sleep(3);
    h.socket()->emit("cEndWordExplanation", pCauseMsg);
}

void ONsWordsToEdit(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    h.socket()->emit("cWordsEdited", data);
}

void ONsGameEnded(sio::event &ev) {
    _lock.lock();
    _cond.notify_all();
    game_finished = false;
    _lock.unlock();
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

    std::string key;

    key = argv[1];
    name = argv[3];
    playersCount = atoi(argv[2]);

    h.set_open_listener(&on_connected);

    h.connect("ws://localhost:3005");

    _lock.lock();
    if (!connect_finish) {
        _cond.wait(_lock);
    }
    _lock.unlock();

    h.socket()->on("sFailure", &ONsFailure);
    h.socket()->on("sYouJoined", &ONsYouJoined);
    h.socket()->on("sPlayerJoined", &ONsPlayerJoined);
    h.socket()->on("sGameStarted", &ONsGameStarted);
    h.socket()->on("sNewWord", &ONsNewWord);
    h.socket()->on("sWordsToEdit", &ONsWordsToEdit);
    h.socket()->on("sGameEnded", &ONsGameEnded);

    sio::object_message::ptr pJoinRoom = sio::object_message::create();

    pJoinRoom->get_map()["key"] = sio::string_message::create(key);
    pJoinRoom->get_map()["username"] = sio::string_message::create(name);
    pJoinRoom->get_map()["time_zone_offset"] = sio::int_message::create(0);

    h.socket()->emit("cJoinRoom", pJoinRoom);

    _lock.lock();
    if (!game_finished) {
        _cond.wait(_lock);
    }
    _lock.unlock();

    return 0;
}
