#pragma once
#include <vector>
#include <filesystem>


namespace Enik {
namespace Utils {

void SortDirectoryEntries(std::vector<std::filesystem::directory_entry>& entries);

}
}