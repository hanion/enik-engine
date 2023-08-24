#pragma once
#include <filesystem>

namespace fs = std::filesystem;

namespace Enik {

struct DialogFileData {
	bool hasSearched = false;

	// fs::path currentDirectory = fs::current_path();
	fs::path currentDirectory = fs::canonical("../../");
	std::string selectedPath;

	std::vector<fs::directory_entry> entries;
};

enum DialogType {
	OPEN_FILE=1, SAVE_FILE=2
};

enum DialogResult {
	NONE=0, CANCEL=1, ACCEPT=2
};



class DialogFile {
public:
	static DialogResult Show(bool& isOpen, DialogType type, const std::string& ext = ".escn");

	static const std::string& GetSelectedPath();

private:
	static DialogResult ShowPopup(bool& isOpen, DialogType type, const std::string& ext);
	static void ShowDirectoriesTable(char* filePathBuffer);
	static bool isValidSelection();

};

}