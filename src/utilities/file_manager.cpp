#include "file_manager.h"

//----------------//
//  File Manager  // 
//----------------//

file_manager::file_manager()
{
}

file_manager::~file_manager()
{

}

int file_manager::set_directory(std::filesystem::path path)
{
	m_directory = path;
    if (!std::filesystem::exists(m_directory) || !std::filesystem::is_directory(m_directory))
    {
        return -1;
	}
    return 0;
}

int file_manager::open_directory(bool recursive)
{
    if (recursive)
    {
        for (const auto& dir : std::filesystem::recursive_directory_iterator(m_directory))
        {
            if (!dir.is_directory())
            {
                if (file_type(dir.path()) < 0) continue;
                m_files.push_back(dir.path());
            }
        }
        return 0;
    }
    else
    {
        for (const auto& dir : std::filesystem::directory_iterator(m_directory))
        {
            if (!dir.is_directory())
            {
                if (file_type(dir.path()) < 0) continue;
                m_files.push_back(dir.path());
            }
        }
    }

    return 0;
}

int file_manager::sort_files(int sort_mode)
{
    if (m_files.empty()) return -1;

    if (sort_mode == FM_SORT_NAME)
    {
        std::sort(m_files.begin(), m_files.end());
    }
    else if (sort_mode == FM_SORT_DATE)
    {
        std::sort(m_files.begin(), m_files.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                auto ftime_a = std::filesystem::last_write_time(a);
                auto ftime_b = std::filesystem::last_write_time(b);
                return ftime_a < ftime_b;
            });
    }
    else if (sort_mode == FM_SORT_SIZE)
    {
        std::sort(m_files.begin(), m_files.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                return std::filesystem::file_size(a) < std::filesystem::file_size(b);
            });
    }
    else if (sort_mode == FM_SORT_TYPE)
    {
        std::sort(m_files.begin(), m_files.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                return std::filesystem::path(a).extension() < std::filesystem::path(b).extension();
            });
    }
    else {}

    return 0;
}

int file_manager::clear_files()
{
    m_files.clear();
    return 0;
}

int file_manager::file_type(std::filesystem::path path)
{
    std::string ext = path.extension().string().c_str();

    // Video formats
    if (ext == ".mp4") return FM_FILE_TYPE_VIDEO;
    else if (ext == ".webm") return FM_FILE_TYPE_VIDEO;
    else if (ext == ".avi") return FM_FILE_TYPE_VIDEO;
    else if (ext == ".mov") return FM_FILE_TYPE_VIDEO;
    else if (ext == ".mkv") return FM_FILE_TYPE_VIDEO;
    else if (ext == ".m4v") return FM_FILE_TYPE_VIDEO;
    else if (ext == ".mpeg") return FM_FILE_TYPE_VIDEO;
    else if (ext == ".gif") return FM_FILE_TYPE_VIDEO;
    else if (ext == ".webp") return FM_FILE_TYPE_VIDEO;

    // WEBP format
	else if (ext == ".webp") return FM_FILE_TYPE_WEBP;

    // Image formats
    else if (ext == ".png") return FM_FILE_TYPE_IMAGE;
    else if (ext == ".jpg") return FM_FILE_TYPE_IMAGE;
    else if (ext == ".jpeg") return FM_FILE_TYPE_IMAGE;
    else if (ext == ".bmp") return FM_FILE_TYPE_IMAGE;

	// Not supported
	return FM_FILE_TYPE_UNKNOWN;
}


//-------------------------//
//  File Manager Extended  // 
//-------------------------//

file_manager_extended::file_manager_extended()
{
}

file_manager_extended::~file_manager_extended()
{
}

int file_manager_extended::sort_buttons(std::vector<wxButton*> arr_buttons, int sort_mode)
{
    if (sort_mode == FM_SORT_NAME)
    {
        std::sort(arr_buttons.begin(), arr_buttons.end(), [](wxButton* a, wxButton* b) {
            auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
            auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
            return *pathA < *pathB;
            });
    }
    else if (sort_mode == FM_SORT_DATE)
    {
        std::sort(arr_buttons.begin(), arr_buttons.end(), [](wxButton* a, wxButton* b)
            {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                auto ftime_a = std::filesystem::last_write_time(*pathA);
                auto ftime_b = std::filesystem::last_write_time(*pathB);
                return ftime_a < ftime_b;
            });
    }
    else if (sort_mode == FM_SORT_SIZE)
    {
        std::sort(arr_buttons.begin(), arr_buttons.end(), [](wxButton* a, wxButton* b) {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                return std::filesystem::file_size(*pathA) < std::filesystem::file_size(*pathB);
            });
    }
    else if (sort_mode == FM_SORT_TYPE)
    {
        std::sort(arr_buttons.begin(), arr_buttons.end(), [](wxButton* a, wxButton* b)
            {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                return std::filesystem::path(*pathA).extension() < std::filesystem::path(*pathB).extension();
            });
    }
    else {}

    return 0;
}
