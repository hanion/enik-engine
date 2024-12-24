# enik-engine

enik-engine is a lightweight C++ game engine that focuses on simplicity and speed. It’s built to help you quickly iterate on small game projects. With features like instant script hot reloading, a clean editor UI, and support for multiple editor tabs, it’s perfect for experimenting, learning, and getting things done fast. Unlike larger engines, which can feel slow to open or iterate on, enik-engine is designed to keep you moving quickly without the bloat.

![ss](https://github.com/user-attachments/assets/2c2662e7-f14d-4cee-89f7-502a5dd00b2e)


## Building the `enik-engine`

### Clone the Repository
```bash
git clone --recursive https://github.com/hanion/enik-engine.git
```

### Build and run with `build.sh`
```bash
cd enik-engine
./build.sh run
```

### Build with cmake (Alternative)
```bash
cd enik-engine
mkdir build && cd build
cmake ..
cmake --build .
```

<br><br>

## Building with `build.sh`

The `build.sh` script simplifies the build process for the enik-engine. It supports various commands that control the build process, including cleaning the build directory, setting the build configuration, running the executable, and specifying the build target.

You can use the `build.sh` script with the following options:

```bash
./build.sh [clean] [config] [run] [target] [static]
```

 - `clean`: Cleans the build directory.

 - config: Specifies the build configuration (`debug`|`release`|`min`).

 - `run`: Runs the built executable after the build process completes.

 - target: Specifies the build target (`editor`|`runtime`).

 - `static`: Links the script module statically.

### Example usage

```bash
./build.sh clean release run runtime
```

This command cleans the build directory, builds the project in release mode and then runs the runtime executable.

<br>

## Exporting with `export.sh`

The `export.sh` script simplifies the process of exporting a project by packaging it along with its assets. It takes the project title and the path to the project’s directory as arguments and prepares everything for distribution.

You can use the `export.sh` script with the following syntax:

```bash
./export.sh <project_title> <path_to_project>
```

 - `<project_title>`: The name of the project.

 - `<path_to_project>`: The path to the project's root directory.


### Example usage

```bash
./export.sh snake-game /home/user/projects/snake-game
```

This command will export the project with the title "snake-game" located at `/home/user/projects/snake-game`, placing the exported files in the `enik-engine/export/platform/snake-game` directory. This prepares the project for distribution and deployment.

<br>

