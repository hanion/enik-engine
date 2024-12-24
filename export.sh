#!/bin/bash

print_usage() {
	echo
	echo "Usage: $0 <project_title> <path_to_project>"
	echo "    <project_title>  : Name of the project"
	echo "    <path_to_project>: Path to the project's assets directory"
}

supports_color() {
	[[ -t 1 && $(command -v tput) && $(tput colors) -ge 8 ]]
}

print_colored() {
	local color_code=$1
	local message=$2
	if supports_color; then
		echo -e "\033[${color_code}m${message}\033[0m"
	else
		echo "$message"
	fi
}

print_success() { print_colored "32" "$1"; }
print_error() { print_colored "91" "$1"; }
print_job() { print_colored "96" "$1"; }

elapsed_time_ms() {
	local start=$1
	local end=$2
	local elapsed=$(( end - start ))
	echo "$((elapsed / 1000))s $((elapsed % 1000))ms"
}

if [ $# -lt 2 ]; then
	print_error "Missing arguments"
	print_usage
	exit 1
fi

PROJECT_NAME=$1
PROJECT_PATH=$2

EXPORT_DIR="./export"
EXPORT_LINUX_DIR="$EXPORT_DIR/linux/${PROJECT_NAME}"

if [ ! -d "$PROJECT_PATH" ]; then
	print_error "Project directory not found: $PROJECT_PATH"
	exit 1
fi
if [ ! -d "$PROJECT_PATH/assets" ]; then
	print_error "assets directory not found in $PROJECT_PATH"
	exit 1
fi
if [ ! -f "$PROJECT_PATH/project.enik" ]; then
	print_error "project.enik file not found in $PROJECT_PATH"
	exit 1
fi

print_job "Creating export directory"
mkdir -p "$EXPORT_LINUX_DIR"

print_job "Copying project assets"
cp -rf "$PROJECT_PATH/assets" "$EXPORT_LINUX_DIR/"
cp "$PROJECT_PATH/project.enik" "$EXPORT_LINUX_DIR/"
cp -rf ./editor/assets/icons/ "$EXPORT_LINUX_DIR/assets/icons/"
cp -rf ./editor/assets/shaders/ "$EXPORT_LINUX_DIR/assets/shaders/"

print_job "Building Release version:"
print_job "    ./build.sh clean min static"

start_time_ms=$(date +%s%3N)
PROJECT_TITLE=$PROJECT_NAME PROJECT_ROOT=$PROJECT_PATH ./build.sh clean min static
release_build_success=$?
end_time_ms=$(date +%s%3N)

if [ $release_build_success -ne 0 ]; then
	print_error "Release build failed"
	exit 1
fi

print_job "Copying Release binaries"
cp ./build/runtime/runtime "$EXPORT_LINUX_DIR/$PROJECT_NAME"
cp -f ./build/engine/libenik-engine.so "$EXPORT_LINUX_DIR"
cp ./build/editor/editor "$EXPORT_LINUX_DIR/enik-engine-editor"

print_job "Stripping Release binaries"
strip "$EXPORT_LINUX_DIR/$PROJECT_NAME"
strip "$EXPORT_LINUX_DIR/libenik-engine.so"
strip "$EXPORT_LINUX_DIR/enik-engine-editor"

release_build_time=$(elapsed_time_ms $start_time_ms $end_time_ms)
print_success "Exporting done in ${release_build_time}"

