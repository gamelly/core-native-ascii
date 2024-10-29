# Core Native ASCII
> create your own game-engine with just lua for terminal. (TUI based games in ascii 256 colors) 

 * supported in 20 plataforms
 * big texts fonts
 * non-curses tui
 * RGB 32bits
 * https
 * json

## How to build

### Clone repo
 
```
git clone https://github.com/gamelly/core-native-ascii
```

### Configure project

 * **GAME:** path to your `game.lua`
 * **TARGET:** building static executable portable and cross compiled. <sup><sub>([see all plataform supported here](#configure-project))</sub></sup>
    - x86_64-linux, x86_64-macos, x86_64-windows...
 * **ENGINE:** custom engine instead gly-engine.

### Build

```
cmake -Bbuild -H. -DGAME=vendor/engine/examples/pong/game.lua
```

## Run the game

```
./build/bin/game
```
