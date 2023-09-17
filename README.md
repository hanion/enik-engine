# enik-engine

### a tiny game engine i am currently developing to learn

![cover09_17](https://github.com/hanion/enik-engine/assets/55713249/d56e812d-7972-4913-ba6d-3da2708bff38)

<br>

# Key Features
- **Native C++ Scripting**
  - Write game logic using C++ scripting for maximum performance and flexibility.

- **Dynamic Script Loading**
  - The engine automatically detects and reloads script module when you compile it.

- **Physics Engine**
  - In development, this engine provides 2D physics simulations.

- **Tracy Profiler**
  - Integrated Tracy profiler for profiling and optimizing performance.


<br>


# System Requirements

**Operating System**: 🐧 Linux

📣 **Note**: 
I have a strong preference for developing on Linux and currently do not have a Windows development environment.
But I'm open to contributions and may consider adding Windows support in the future.
Adding Windows support should not be difficult, as I have intentionally avoided using extensive platform-specific code.

<br>

# How to Build

### Clone the repository

```bash
git clone --recursive https://github.com/hanion/enik-engine.git
cd enik-engine
```

### Build with CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Run the editor

```bash
cd editor
./editor
```
