# Charge Game

[TOC]

## Introduction

The program shoots a particle of positive charge in a given direction. There are a few other charged particles for the player to place them on the field. The goal is to place the particles in a way to make the moving particle hit a predefined target. Each scenario can have walls, fixed charges, etc.

The program should also score the player (e.g. the number of trials to hit the target), and administer a persistent hall of fame.

## Build

```sh
mkdir build
cd build
cmake -DSDL2_DIR=./SDL2 ..
make
```
