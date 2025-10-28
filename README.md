# 🧙‍♂️ Potion Collector

Small code together example made in class.

Feel free to share your improvements.

# Compile

### Using GCC

`gcc -Wall -Wextra -Werror -std=c17 game.c map.c player.c -o game`

### Using MakeFile

- `make` builds debug (default).
- `make debug` same as make. Builds debug.
- `make release` optimized build.
- `make run` builds debug then runs app/exe
- `make clean` deletes all .o, .d and builds

Don't forget to install the required packages to use make:

Installing Make, CMake and pkg-config

- Windows
    ```bash
    pacman -S make cmake mingw-w64-x86_64-pkg-config
    ```

- MacOS
    ```bash
    brew install make cmake pkg-config
    ```
