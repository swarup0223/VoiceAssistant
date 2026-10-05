#include "file_manager.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

void FileManager::listFiles(const std::string& path) const
{
    try
    {
        std::cout << "\nFiles in " << path << ":\n";

        for (const auto& entry : fs::directory_iterator(path))
        {
            std::cout << "  "
                      << entry.path().filename().string();

            if (entry.is_directory())
                std::cout << "/";

            std::cout << "\n";
        }

        std::cout << "\n";
    }
    catch (const fs::filesystem_error& error)
    {
        std::cerr << "Filesystem error: "
                  << error.what() << "\n";
    }
}

bool FileManager::createFile(const std::string& filename) const
{
    std::ofstream file(filename);

    if (!file)
    {
        std::cerr << "Unable to create file: "
                  << filename << "\n";

        return false;
    }

    file << "Created by VoiceAssistant\n";

    file.close();

    std::cout << "File created: "
              << filename << "\n";

    return true;
}

bool FileManager::deleteFile(const std::string& filename) const
{
    try
    {
        if (!fs::exists(filename))
        {
            std::cout << "File does not exist: "
                      << filename << "\n";

            return false;
        }

        fs::remove(filename);

        std::cout << "File deleted: "
                  << filename << "\n";

        return true;
    }
    catch (const fs::filesystem_error& error)
    {
        std::cerr << "Unable to delete file: "
                  << error.what() << "\n";

        return false;
    }
}
