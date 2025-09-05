#!/bin/bash

print_usage() {
	echo
	echo "Usage: $0 <platform> <project_title> <path_to_project>"
	echo "    <platform>        : linux | mingw | all"
	echo "    <project_title>   : Name of the project"
	echo "    <path_to_project> : Path to the project's assets directory"
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

if [ $# -lt 3 ]; then
	print_error "Missing arguments"
	print_usage
	exit 1
fi

PLATFORM="linux"
if [[ "$1" == "linux" || "$1" == "mingw" || "$1" == "all" ]]; then
	PLATFORM=$1
	shift
fi

PROJECT_NAME=$1
PROJECT_PATH=$2

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

export_project() {
	local platform=$1
	local export_dir="./export/$platform/${PROJECT_NAME}"
	print_job "Creating export directory for $platform"
	mkdir -p "$export_dir"

	print_job "Building for $platform"
	if [ "$platform" == "linux" ]; then
		BUILD_FLAGS="clean min static"
	elif [ "$platform" == "mingw" ]; then
		BUILD_FLAGS="clean min static mingw"
	else
		print_error "Invalid platform: $platform"
		print_usage
		exit 1
	fi

	print_job "    ./build.sh $BUILD_FLAGS"
	start_time_ms=$(date +%s%3N)
	PROJECT_TITLE=$PROJECT_NAME PROJECT_ROOT=$PROJECT_PATH ./build.sh $BUILD_FLAGS
	build_success=$?
	end_time_ms=$(date +%s%3N)

	if [ $build_success -ne 0 ]; then
		print_error "Build failed for $platform"
		exit 1
	fi

	print_job "Copying project assets for $platform"
	cp -rf "$PROJECT_PATH/assets" "$export_dir/"
	cp     "$PROJECT_PATH/project.enik" "$export_dir/"
	cp -r  "$PROJECT_PATH/asset.registry" "$export_dir/"
	cp -rf ./editor/assets/icons/. "$export_dir/assets/icons/"       # editor assets, optional
	cp -rf ./editor/assets/fonts/. "$export_dir/assets/icons/"       # editor assets, optional
	cp -rf ./editor/assets/shaders/. "$export_dir/assets/shaders/"

	if [ "$platform" == "linux" ]; then
		print_job "Copying binaries for $platform"
		cp ./build/runtime/runtime "$export_dir/$PROJECT_NAME"
		cp ./build/editor/editor "$export_dir/enik-engine-editor"

		print_job "Stripping binaries for $platform"
		strip "$export_dir/$PROJECT_NAME"
		strip "$export_dir/enik-engine-editor"
	elif [ "$platform" == "mingw" ]; then
		print_job "Copying binaries for $platform"
		cp ./build/runtime/runtime.exe "$export_dir/$PROJECT_NAME.exe"
		cp ./build/editor/editor.exe "$export_dir/enik-engine-editor.exe"

		print_job "Stripping binaries for $platform"
		x86_64-w64-mingw32-strip "$export_dir/$PROJECT_NAME.exe"
		x86_64-w64-mingw32-strip "$export_dir/enik-engine-editor.exe"

		print_job "Copying MinGW runtime libraries"
		cp /usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll "$export_dir/"
		cp /usr/x86_64-w64-mingw32/bin/libstdc++-6.dll "$export_dir/"
		cp /usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll "$export_dir/"
	fi

	build_time=$(elapsed_time_ms $start_time_ms $end_time_ms)
	print_success "Exporting for $platform done in ${build_time}"
}

if [ "$PLATFORM" == "all" ]; then
	export_project "linux"
	export_project "mingw"
else
	export_project "$PLATFORM"
fi

