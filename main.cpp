#include <sio_client.h>

#include <sys/time.h>
#include <stdlib.h>
#include <iostream>
#include <mutex>
#include <condition_variable>

//#define DEBUG

#ifdef DEBUG
bool _log = true;
#else
bool _log = false;
#endif

// start of config constants

// client config
const time_t delay = 150;
const time_t minExplTime = 100;
const time_t maxExplTime = 100;

// room config
const int64_t delayTime = 0;
const int64_t explanationTime = 5000;
const int64_t aftermathTime = 100;
const int64_t wordNumber = 100;

// end of config constants

size_t playersCount = 0;
std::mutex _lock;
std::condition_variable_any _cond;
bool connect_finish = false;
bool game_finished = false;

time_t start_time;

sio::client h;
std::string name = "";

void msleep(uint64_t ms) {
    if (_log) {
        std::cerr << "Waiting " << ms << "ms\n";
    }
    struct timespec ts;
    int res;

    if (ms < 0) {
        return;
    }

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res);
}

bool is_host(const sio::message::ptr &data) {
   std::string host = data->get_map()["host"]->get_string();
   return name == host;
}

bool should_start(const sio::message::ptr &data) {
    size_t plCount = data->get_map()["playerList"]->get_vector().size();
    std::string host = data->get_map()["host"]->get_string();
    return is_host(data) && playersCount == plCount;
}

void emit(const std::string &signal) {
    msleep(delay);
    h.socket()->emit(signal);
}

void emit(const std::string &signal, const sio::object_message::ptr &pMsg) {
    msleep(delay);
    h.socket()->emit(signal, pMsg);
}

void ONsFailure(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (_log) {
        std::cerr << data->get_map()["msg"]->get_string() << '\n';
    }
}

void ONsYouJoined(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (is_host(data)) {
        //changing settings
        sio::object_message::ptr pSettings = sio::object_message::create();
        pSettings->get_map()["delayTime"] = sio::int_message::create(delayTime);
        pSettings->get_map()["explanationTime"] = sio::int_message::create(explanationTime);
        pSettings->get_map()["aftermathTime"] = sio::int_message::create(aftermathTime);
        pSettings->get_map()["wordNumber"] = sio::int_message::create(wordNumber);
        sio::object_message::ptr pSettingsMsg = sio::object_message::create();
        pSettingsMsg->get_map()["settings"] = pSettings;
        emit("cApplySettings", pSettingsMsg);

        if (should_start(data)) {
            emit("cStartGame");
        }
    }
}

void ONsPlayerJoined(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (should_start(data)) {
        emit("cStartGame");
    }
}

void ONsPlayerLeft(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (should_start(data)) {
        emit("cStartGame");
    }
}

void ONsGameStarted(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    if (name == data->get_map()["speaker"]->get_string()) {
        emit("cSpeakerReady");
    }
    if (name == data->get_map()["listener"]->get_string()) {
        emit("cListenerReady");
    }
}

void ONsExplanationStarted(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    start_time = data->get_map()["startTime"]->get_int();
}

void ONsNewWord(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    sio::object_message::ptr pCauseMsg = sio::object_message::create();
    pCauseMsg->get_map()["cause"] = sio::string_message::create("explained");

    // waiting for start of the explanation
    struct timeval tp;
    gettimeofday(&tp, NULL);
    time_t timems = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    msleep((start_time > timems) ? start_time - timems : 0);

    // explaining
    msleep(minExplTime + random() * (maxExplTime - minExplTime) / RAND_MAX);

    emit("cEndWordExplanation", pCauseMsg);
}

void ONsWordsToEdit(sio::event &ev) {
    sio::message::ptr data = ev.get_message();
    emit("cWordsEdited", data);
}

void ONsGameEnded(sio::event &ev) {
    _lock.lock();
    _cond.notify_all();
    game_finished = true;
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
        std::cout << "Wrong number of argument: expected 3, got " << argc - 1 << ".\n";
        std::cerr << argv[0] << ' ' << argv[1] << ' ' << argv[2] << ' ' << argv[3] << '\n';
        return 1;
    }

    std::string key;

    key = argv[1];
    name = argv[3];
    playersCount = atoi(argv[2]);

    h.set_open_listener(&on_connected);
    h.set_logs_quiet();

    h.connect("wss://m20-sch57.site:3005");

    _lock.lock();
    if (!connect_finish) {
        _cond.wait(_lock);
    }
    _lock.unlock();

    h.socket()->on("sFailure", &ONsFailure);
    h.socket()->on("sYouJoined", &ONsYouJoined);
    h.socket()->on("sPlayerJoined", &ONsPlayerJoined);
    h.socket()->on("sPlayerLeft", &ONsPlayerLeft);
    h.socket()->on("sGameStarted", &ONsGameStarted);
    h.socket()->on("sNextTurn", ONsGameStarted); // this is not a typo
    h.socket()->on("sExplanationStarted", &ONsExplanationStarted);
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
