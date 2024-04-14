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

	fs::path current_directory = fs::canonical("../../");
	fs::path selected_path;

	std::vector<fs::directory_entry> entries;

	bool is_open = false;
	DialogType type = DialogType::OPEN_FILE;
	std::string ext = ".escn";

	std::function<void()> call_function;
};

enum DialogFileResult {
	NONE = 0,
	CANCEL,
	ACCEPT_SAVE,
	ACCEPT_OPEN
};

class DialogFile {
public:
	static void OpenDialog(DialogType type, const std::function<void()>& call_when_confirmed, const std::string& ext = "");

	static DialogFileResult Show();

	static const std::filesystem::path& GetSelectedPath();
	static void SetCurrentDir(const std::filesystem::path& directory);

private:
	static DialogFileResult ShowPopup();
	static void ShowDirectoriesTable(char* file_path_buffer);
	static bool IsValidSelection();
};

}