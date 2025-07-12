# enik-engine

**enik-engine** is a lightweight, fast C++ game engine with its own custom
editor. It’s built for rapid iteration and low friction. With features like
instant script hot reloading it’s perfect for experimenting, learning, and
getting things done fast. Unlike bigger engines, enik-engine opens instantly,
runs fast, and gets out of your way.


![ss](https://github.com/user-attachments/assets/2c2662e7-f14d-4cee-89f7-502a5dd00b2e)


## Key Features

### Engine
* Native C++ scripting with hot reload
* Physics (Jolt Physics)
* Scene management
* Prefab system
* Persistent entities
* Text rendering
* Audio system
* Global transforms
* Runtime environment

### Editor
* Tabbed interface
* Editor colors
* Animation editor

### Misc
* Cross-platform builds
* Used in real games (4 published titles) - one of them won a game jam prize:
    - [reborn](https://hanion.itch.io/reborn)
    - [square up](https://hanion.itch.io/square-up)
    - [gear gambit](https://hanion.itch.io/gear-gambit)
    - [snake game](https://hanion.itch.io/snake-game)


<br>


## Getting Started

### Clone the Repository
```bash
git clone --recursive https://github.com/hanion/enik-engine.git
```
### Building with `build.sh`

Simple wrapper script for building and running.

```bash
./build.sh [clean] [config] [run] [target] [static]
```

Arguments:

* `clean`: Removes the build directory
* `config`: `debug`, `release`, or `min`
* `run`: Runs the executable after build
* `target`: Choose `editor` or `runtime`
* `static`: Statically link the script module

#### Example

```bash
./build.sh clean run
```


### Building with CMake (Manual)

```bash
cd enik-engine
mkdir build && cd build
cmake ..
cmake --build .
```


<br>


### Exporting Projects

The `export.sh` script bundles a project and its assets for distribution.

```bash
./export.sh <project_title> <path_to_project>
```

Arguments:

* `<project_title>`: name of the exported folder
* `<path_to_project>`: root path to your enik-engine project

#### Example

```bash
./export.sh snake-game /home/user/projects/snake-game
```

This will create an export at: `enik-engine/export/platform/snake-game`


