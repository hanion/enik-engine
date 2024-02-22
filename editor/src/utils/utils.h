#pragma once
#include <base.h>


namespace Enik {
namespace Utils {

void SortDirectoryEntries(std::vector<std::filesystem::directory_entry>& entries);

bool FolderContainsFilesWithExtensions(const std::filesystem::path& directory, const std::vector<std::string>& extensions, int depth = 10);

void FilterFiles(std::vector<std::filesystem::directory_entry>& entries, const std::vector<std::string>& filters, const bool remove_folders = true);

}
}