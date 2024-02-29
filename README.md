# enik-engine

### a tiny game engine i am currently developing to learn

![cover09_17](https://github.com/hanion/enik-engine/assets/55713249/d56e812d-7972-4913-ba6d-3da2708bff38)

<br>

# Key features

**Native C++ scripting:** Write game logic using C++ scripting for maximum performance and flexibility.

**Dynamic script loading:** The engine automatically detects and reloads script module when you compile it.

**Physics engine:** In development, this engine provides 2D physics simulations.

**Tracy profiler:** Integrated profiler for measuring and optimizing performance.

<br>

# How to build

### Clone the repository

```bash
git clone --recursive https://github.com/hanion/enik-engine.git
```

### Build with CMake

```bash
cd enik-engine
mkdir build & cd build
cmake ..
cmake --build .
```