#!/bin/bash

print_usage() {
	echo
	echo "Usage: $0 <project_title> <engine_source_dir> <platform>"
	echo "    <project_title>    : Name of the project"
	echo "    <engine_source_dir>: Path to the engine source directory"
	echo "    <platform>         : linux | mingw | all"
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

PROJECT_NAME=$1
ENGINE_SOURCE_DIR=$2
PLATFORM=$3
PROJECT_PATH="$(pwd)"

if [ ! -f "$PROJECT_PATH/project.enik" ]; then
	print_error "project.enik file not found in current directory ($PROJECT_PATH)"
	exit 1
fi
if [ ! -d "$PROJECT_PATH/assets" ]; then
	print_error "assets directory not found in $PROJECT_PATH"
	exit 1
fi
if [ ! -d "$ENGINE_SOURCE_DIR" ]; then
	print_error "Engine source directory not found: $ENGINE_SOURCE_DIR"
	exit 1
fi
if [[ "$PLATFORM" != "linux" && "$PLATFORM" != "mingw" && "$PLATFORM" != "all" ]]; then
	print_error "Invalid platform: $PLATFORM"
	print_usage
	exit 1
fi

export_project() {
	local platform=$1
	local export_dir="$PROJECT_PATH/export/$platform/${PROJECT_NAME}"
	print_job "Creating export directory for $platform"
	mkdir -p "$export_dir"

	print_job "Building for $platform"
	if [ "$platform" == "linux" ]; then
		BUILD_FLAGS="clean min static"
	elif [ "$platform" == "mingw" ]; then
		BUILD_FLAGS="clean min static mingw"
	fi

	print_job "    build.sh $BUILD_FLAGS"
	start_time_ms=$(date +%s%3N)
	pushd "$ENGINE_SOURCE_DIR" > /dev/null
	PROJECT_TITLE=$PROJECT_NAME PROJECT_ROOT=$PROJECT_PATH ./build.sh $BUILD_FLAGS
	build_success=$?
	popd > /dev/null
	end_time_ms=$(date +%s%3N)

	if [ $build_success -ne 0 ]; then
		print_error "Build failed for $platform"
		exit 1
	fi

	print_job "Copying project assets for $platform"
	cp -rf "$PROJECT_PATH/assets" "$export_dir/"
	cp     "$PROJECT_PATH/project.enik" "$export_dir/"
	cp -r  "$PROJECT_PATH/asset.registry" "$export_dir/" 2>/dev/null || true
	cp -rf "$ENGINE_SOURCE_DIR/editor/assets/icons/."   "$export_dir/assets/icons/"    || true
	cp -rf "$ENGINE_SOURCE_DIR/editor/assets/fonts/."   "$export_dir/assets/fonts/"    || true
	cp -rf "$ENGINE_SOURCE_DIR/editor/assets/shaders/." "$export_dir/assets/shaders/"  || true

	if [ "$platform" == "linux" ]; then
		print_job "Copying binaries for $platform"
		cp "$ENGINE_SOURCE_DIR/build/runtime/runtime" "$export_dir/$PROJECT_NAME"
		cp "$ENGINE_SOURCE_DIR/build/editor/editor" "$export_dir/enik-engine-editor"

		print_job "Stripping binaries for $platform"
		strip "$export_dir/$PROJECT_NAME"
		strip "$export_dir/enik-engine-editor"
	elif [ "$platform" == "mingw" ]; then
		print_job "Copying binaries for $platform"
		cp "$ENGINE_SOURCE_DIR/build/runtime/runtime.exe" "$export_dir/$PROJECT_NAME.exe"
		cp "$ENGINE_SOURCE_DIR/build/editor/editor.exe" "$export_dir/enik-engine-editor.exe"

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

