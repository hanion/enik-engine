#pragma once
#include <filesystem>

namespace fs = std::filesystem;

namespace Enik {

struct DialogFileData {
	bool has_searched = false;

	// fs::path current_directory = fs::current_path();
	fs::path current_directory = fs::canonical("../../");
	std::filesystem::path selected_path;

	std::vector<fs::directory_entry> entries;
};

enum DialogType {
	OPEN_FILE = 1,
	SAVE_FILE = 2
};

enum DialogResult {
	NONE = 0,
	CANCEL = 1,
	ACCEPT = 2
};

class DialogFile {
public:
	static DialogResult Show(bool& is_open, DialogType type, const std::string& ext = ".escn");

	static const std::filesystem::path& GetSelectedPath();

private:
	static DialogResult ShowPopup(bool& is_open, DialogType type, const std::string& ext);
	static void ShowDirectoriesTable(char* file_path_buffer);
	static bool isValidSelection();
};

}