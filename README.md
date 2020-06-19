# TheTrueHat-client-cpp
Lightweight client for testing capacity of TheTrueHat server.

## Build
Prepare directory `~/lib`, lib will be copied there.
Run `make build`. (Make sure that you have installed all dependencies)
Add path `~/lib` to environment variable `LD_LIBRARY_PATH`.
Run `make client`.

## Run
Adjust `config.sh` to your needs.
Adjust config constants in `main.cpp` to your needs.
Find and set up proper url in `main.cpp`.
Run `make client` to compile with updated constants.
Run `./runAll.sh` and have fun.
