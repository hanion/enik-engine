#pragma once
#include <filesystem>

namespace fs = std::filesystem;

namespace Enik {

enum DialogType {
	OPEN_FILE = 1,
	SAVE_FILE = 2
};

struct DialogFileData {
	bool has_searched = false;

	// fs::path current_directory = fs::current_path();
	fs::path current_directory = fs::canonical("../../");
	std::filesystem::path selected_path;

	std::vector<fs::directory_entry> entries;

	bool is_open = false;
	DialogType type = DialogType::OPEN_FILE;
	std::string ext = ".escn";
};

enum DialogFileResult {
	NONE = 0,
	CANCEL,
	ACCEPT_SAVE,
	ACCEPT_OPEN
};

class DialogFile {
public:
	static void OpenDialog(DialogType type, const std::string& ext = ".escn");

	static DialogFileResult Show();

	static const std::filesystem::path& GetSelectedPath();

private:
	static DialogFileResult ShowPopup();
	static void ShowDirectoriesTable(char* file_path_buffer);
	static bool isValidSelection();
};

}