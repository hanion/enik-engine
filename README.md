# enik-engine

**enik-engine** is a lightweight, fast C++ game engine with a custom editor.
It’s built for rapid iteration and low friction.
With features like instant script hot reloading it’s perfect for experimenting, learning, and getting things done fast.
Unlike bigger engines, enik-engine opens instantly, runs fast, and gets out of your way.


Learn more in my full post: [hanion.dev/post/enik-engine](https://hanion.dev/post/enik-engine)

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
* Cross-platform builds

### Editor
* Tabbed interface
* Editor colors
* Animation editor

### Games Made with enik-engine
- [square up](https://hanion.itch.io/square-up)
<img width="1732" height="959" alt="square up in editor" src="https://github.com/user-attachments/assets/25b01178-33ed-46db-8e45-e70e15cf940f" />

- [reborn](https://hanion.itch.io/reborn)
<img width="1730" height="958" alt="reborn in editor" src="https://github.com/user-attachments/assets/1cd99834-2620-41ea-9297-0af9e2d6a77d" />

- [snake game](https://hanion.itch.io/snake-game)
<img width="1732" height="959" alt="snake game in editor" src="https://github.com/user-attachments/assets/eca50ace-49b5-44c0-9cf0-112341631fd1" />

- [gear gambit](https://hanion.itch.io/gear-gambit)
<img width="1280" height="626" alt="gear-gambit-in-editor" src="https://github.com/user-attachments/assets/d7189fb3-44a4-47cf-91cd-cc4e0823c993" />



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

