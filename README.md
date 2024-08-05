![Bowser's Fury Online](https://i.imgur.com/EhcOm9z.png)

Bowser's Fury Online is a mod for Bowser's Fury that allows you to play as the 5 characters from 3D World as opposed to only Mario, and also do that online with friends through a server with max. 5 participants (may be increased in the future, there are memory issues). Save files will be kept on the server and synced to all players.

## Prerequisites
- Modded Switch Console (with e.g. Atmosphère)
- Super Mario 3D World + Bowser's Fury (version 1.1.0/latest)
- If you will host a server: a Windows or Linux/Unix machine to host it on, as well as the ability for all participants to connect to that machine (usually using port forwarding)

This mod is safe to use on SysNAND.

## Install
There are pre-built binaries of the mod and the server (for Windows) in [the releases section](https://github.com/fruityloops1/bf-multiplayer/releases). If you compiled the mod yourself, use these instructions:
- Put contents of the `sd` folder onto the root of SD card
- Copy `build/subsdk9.npdm` into `exefs/main.npdm`
- Copy `build/subsdk9` into `exefs/subsdk9`
- Copy contents of `romfs` into LayeredFS RomFS folder

If you are using a pre-compiled build of the mod, simply extract SDCard.zip onto the root of your SD card.

## Usage
Run the server using the `peepa-server` binary.
The server will automatically open a connection on 0.0.0.0:7089 (UDP). The port can be changed in the `Config.json` file. Type `help` for a list of commands.
Connect to your server's IP using the "Connect to Server" menu in-game. If you intend on connecting to your server from outside your local network, you must port forward the UDP port.

If you wish to upload a custom save file to the server, turn it off, and replace the GameData.bin file in the same directory as the server with your desired save file. Make sure the save you intend to use is on save slot number 4 (the last one), as that is the one used by the mod for save syncing over network.

## Build
Mod: Install devkitPro and run `make`

Server: Build using CMake on a POSIX system:
- Make sure to cd into the `server` directory
- `mkdir build && cd build && cmake ..`
- `make`

Server: Build using CMake on a Windows system:
- Install Clang/LLVM >= 14
- Download Ninja
- Make sure to cd into the `server` directory
- `mkdir build && cd build`
- `cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="clang" -DCMAKE_CXX_COMPILER="clang++" -GNinja -D"CMAKE_MAKE_PROGRAM:PATH=C:/path/to/ninja.exe"` (replace C:/path/to/ninja.exe with your path)
- `cd .. && ninja -C build`

# Credits

- GRAnimated, for helping with and improving the mod's logo
- [CraftyBoss](https://github.com/CraftyBoss/)
- [KillzXGaming](https://github.com/KillzXGaming/Switch-Toolbox)
- [OatmealDome](https://github.com/oatmealdome/enet-nx)
- [exlaunch](https://github.com/shadowninja108/exlaunch/)
- [Sanae](https://github.com/Sanae6)
- [EngineLessCC](https://github.com/EngineLessCC/exlaunch-cmake)
- [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere)
- [oss-rtld](https://github.com/Thog/oss-rtld)
- chico88959, cutesleepyy, mjcox24, GLOSHSEP, Vadenimo for helping record the [Trailer](https://youtu.be/pj00pXI1PNA)
- Sorry if I forgot to credit you please dm me regardless I love you mwah
