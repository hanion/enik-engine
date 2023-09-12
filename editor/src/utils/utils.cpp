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

}
}