#include "utils.h"
#include <vector>
#include <filesystem>
#include <algorithm>

namespace Enik {
namespace Utils {


void SortDirectoryEntries(std::vector<std::filesystem::directory_entry>& entries) {
	std::sort(entries.begin(), entries.end(),
		[](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
			bool is_a_directory = std::filesystem::is_directory(a.path());
			bool is_b_directory = std::filesystem::is_directory(b.path());

			// directories come first
			if (is_a_directory && !is_b_directory) {
				return true;
			}
			if (!is_a_directory && is_b_directory) {
				return false;
			}

			return a.path().filename().string() < b.path().filename().string();
		});
}

bool FolderContainsFilesWithExtensions(const std::filesystem::path& directory, const std::vector<std::string>& extensions, int depth) {
	if (depth == 0) {
		return true;
	}

	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.is_regular_file()) {
			const std::string extension = entry.path().extension().string();
			if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
				return true;
			}
		}
		else if (entry.is_directory()) {
			// recursively check subdirectories
			if (FolderContainsFilesWithExtensions(entry.path(), extensions, --depth)) {
				return true;
			}
		}
	}
	return false;
}

void FilterFiles(std::vector<std::filesystem::directory_entry>& entries, const std::vector<std::string>& filters, const bool remove_folders) {
	entries.erase(
		std::remove_if(entries.begin(), entries.end(), [&](const std::filesystem::directory_entry& entry) {
			if (entry.is_directory()) {
				if (remove_folders) {
					return not FolderContainsFilesWithExtensions(entry.path(), filters);
				}
				return false;
			}
			return std::find(filters.begin(), filters.end(), entry.path().extension().string()) == filters.end();
		}),
		entries.end()
	);
}


}
}