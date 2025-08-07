#include "filemanager.h"

//----------------//
//  File Manager  // 
//----------------//

FileManager::FileManager()
{
    recursiveMode = 0;
    sortMode = 0;
}

FileManager::~FileManager()
{

}

void FileManager::OpenDirectory()
{
    if (recursiveMode == 1)
    {
        for (const auto& dir : std::filesystem::recursive_directory_iterator(directory))
        {
            if (!dir.is_directory())
            {
                if (!IsSupportedFile(dir.path())) continue;
                filePaths.push_back(dir.path());
            }
        }
        return;
    }

    for (const auto& dir : std::filesystem::directory_iterator(directory))
    {
        if (!dir.is_directory())
        {
            if (!IsSupportedFile(dir.path())) continue;
            filePaths.push_back(dir.path());
        }
    }

    return;
}

void FileManager::Sort()
{
    if (filePaths.empty()) return;

    if (sortMode == SORT_NAME)
    {
        std::sort(filePaths.begin(), filePaths.end());
        return;
    }
    else if (sortMode == SORT_DATE)
    {
        std::sort(filePaths.begin(), filePaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                auto ftime_a = std::filesystem::last_write_time(a);
                auto ftime_b = std::filesystem::last_write_time(b);
                return ftime_a < ftime_b;
            });
        return;
    }
    else if (sortMode == SORT_SIZE)
    {
        std::sort(filePaths.begin(), filePaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                return std::filesystem::file_size(a) < std::filesystem::file_size(b);
            });
        return;
    }
    else if (sortMode == SORT_TYPE)
    {
        std::sort(filePaths.begin(), filePaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                return std::filesystem::path(a).extension() < std::filesystem::path(b).extension();
            });
        return;
    }
    else {}

    return;
}

bool FileManager::IsSupportedFile(std::filesystem::path path)
{
    std::string ext = path.extension().string().c_str();

    // Video formats
    if (ext == ".mp4") return true;
    else if (ext == ".webm") return true;
    else if (ext == ".avi") return true;
    else if (ext == ".mov") return true;
    else if (ext == ".mkv") return true;
    else if (ext == ".m4v") return true;
    else if (ext == ".mpeg") return true;

    // Image formats
    else if (ext == ".gif") return true;
    else if (ext == ".webp") return true;
    else if (ext == ".png") return true;
    else if (ext == ".jpg") return true;
    else if (ext == ".jpeg") return true;
    else if (ext == ".bmp") return true;

    return false;
}

//-------------------------//
//  File Manager Extended  // 
//-------------------------//

FileManagerExtended::FileManagerExtended()
{
}

FileManagerExtended::~FileManagerExtended()
{
}

void FileManagerExtended::SortButtons(std::vector<wxButton*> arrButtons)
{
    if (sortMode == SORT_NAME)
    {
        std::sort(arrButtons.begin(), arrButtons.end(), [](wxButton* a, wxButton* b) {
            auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
            auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
            return *pathA < *pathB;
            });
    }
    else if (sortMode == SORT_DATE)
    {
        std::sort(arrButtons.begin(), arrButtons.end(), [](wxButton* a, wxButton* b)
            {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                auto ftime_a = std::filesystem::last_write_time(*pathA);
                auto ftime_b = std::filesystem::last_write_time(*pathB);
                return ftime_a < ftime_b;
            });
    }
    else if (sortMode == SORT_SIZE)
    {
        std::sort(arrButtons.begin(), arrButtons.end(), [](wxButton* a, wxButton* b) {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                return std::filesystem::file_size(*pathA) < std::filesystem::file_size(*pathB);
            });
    }
    else if (sortMode == SORT_TYPE)
    {
        std::sort(arrButtons.begin(), arrButtons.end(), [](wxButton* a, wxButton* b)
            {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                return std::filesystem::path(*pathA).extension() < std::filesystem::path(*pathB).extension();
            });
    }
    else {}
}
