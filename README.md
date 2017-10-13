# Charge Game

## Introduction

Charge game with SDL2.

This program create a particle that can be attracted or repulse from multiples charges. The player need to place others charges to make the moving particle arrive at a certain point. The player can retry multiple time. The goal is to suceed the level in the less try as possible. The 3 best scores are keep in a file and can be display in the highscore file. The level can also be created in a level editor, save and load.

## Build

```sh
sudo apt-get install libsdl2
sudo apt-get install libsdl2
sudo apt-get install libsdl2

mkdir build
cd build
cmake -DSDL2_DIR=./SDL2 ..
make
```
