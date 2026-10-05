#include "command_parser.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace
{
std::string trim(const std::string& value)
{
    const auto first =
        value.find_first_not_of(" \t\n\r");

    if (first == std::string::npos)
    {
        return "";
    }

    const auto last =
        value.find_last_not_of(" \t\n\r");

    return value.substr(
        first,
        last - first + 1
    );
}

bool contains(
    const std::string& text,
    const std::string& phrase
)
{
    return text.find(phrase) != std::string::npos;
}

void replaceAll(
    std::string& text,
    const std::string& from,
    const std::string& to
)
{
    if (from.empty())
    {
        return;
    }

    std::size_t pos = 0;

    while (
        (pos = text.find(from, pos))
        != std::string::npos
    )
    {
        text.replace(
            pos,
            from.length(),
            to
        );

        pos += to.length();
    }
}
}

std::string CommandParser::normalize(
    const std::string& input
) const
{
    std::string result;

    for (unsigned char c : input)
    {
        result += static_cast<char>(
            std::tolower(c)
        );
    }

    // Whisper punctuation cleanup.
    for (char& c : result)
    {
        if (
            c == ',' ||
            c == '!' ||
            c == '?' ||
            c == ':' ||
            c == ';'
        )
        {
            c = ' ';
        }
    }

    // Remove sentence-ending periods while preserving periods inside filenames and URLs.
    while (!result.empty() && result.back() == '.')
    {
        result.pop_back();
    }

    // Common Whisper recognition variations.
    replaceAll(result, "you tube", "youtube");
    replaceAll(result, "you-tube", "youtube");

    replaceAll(result, "git hub", "github");
    replaceAll(result, "get hub", "github");

    replaceAll(result, "g mail", "gmail");
    replaceAll(result, "g-mail", "gmail");

    replaceAll(result, "what's app", "whatsapp");
    replaceAll(result, "what s app", "whatsapp");
    replaceAll(result, "what-app", "whatsapp");

    replaceAll(result, "linked in", "linkedin");
    replaceAll(result, "chat gpt", "chatgpt");

    replaceAll(result, "stack overflow", "stackoverflow");

    std::string cleaned;

    bool previousSpace = false;

    for (char c : result)
    {
        if (
            std::isspace(
                static_cast<unsigned char>(c)
            )
        )
        {
            if (!previousSpace)
            {
                cleaned += ' ';
            }

            previousSpace = true;
        }
        else
        {
            cleaned += c;
            previousSpace = false;
        }
    }

    return trim(cleaned);
}

bool CommandParser::startsWith(
    const std::string& text,
    const std::string& prefix
) const
{
    return text.rfind(prefix, 0) == 0;
}

std::string CommandParser::removePrefix(
    const std::string& text,
    const std::string& prefix
) const
{
    if (!startsWith(text, prefix))
    {
        return text;
    }

    return trim(
        text.substr(prefix.length())
    );
}

ParsedCommand CommandParser::parse(
    const std::string& input
) const
{
    std::string command =
        normalize(input);

    // ------------------------------------------------------------
    // CONVERSATIONAL PREFIXES
    // ------------------------------------------------------------

    const std::vector<std::string> prefixes =
    {
        "hello voice assistant ",
        "hi voice assistant ",
        "hey voice assistant ",
        "would you please ",
        "can you please ",
        "could you please ",
        "i would like you to ",
        "i would like to ",
        "i want you to ",
        "i want to ",
        "please ",
        "hello ",
        "hi ",
        "hey ",
        "can you ",
        "could you ",
        "would you ",
        "will you ",
        "i will ",
        "ill ",
        "i'll "
    };

    bool removedPrefix = true;

    while (
        removedPrefix &&
        !command.empty()
    )
    {
        removedPrefix = false;

        for (const auto& prefix : prefixes)
        {
            if (startsWith(command, prefix))
            {
                command =
                    removePrefix(
                        command,
                        prefix
                    );

                removedPrefix = true;
                break;
            }
        }
    }

    // ------------------------------------------------------------
    // EXIT
    // ------------------------------------------------------------

    if (
        command == "exit" ||
        command == "quit" ||
        command == "close assistant" ||
        command == "exit assistant" ||
        command == "quit assistant"
    )
    {
        return {
            CommandType::EXIT,
            {}
        };
    }

    // ------------------------------------------------------------
    // HELP
    // ------------------------------------------------------------

    if (
        command == "help" ||
        command == "show help" ||
        command == "help me" ||
        contains(command, "what can you do")
    )
    {
        return {
            CommandType::HELP,
            {}
        };
    }

    // ------------------------------------------------------------
    // SYSTEM INFORMATION
    // ------------------------------------------------------------

    if (
        command == "system information" ||
        command == "system info" ||
        command == "show system information" ||
        command == "show system status" ||
        command == "show system info" ||
        command == "show system details" ||
        command == "system details" ||
        command == "system specifications" ||
        command == "show system specifications" ||
        contains(command, "tell me about my system") ||
        contains(command, "show me my system")
    )
    {
        return {
            CommandType::SYSTEM_INFO,
            {}
        };
    }

    // ------------------------------------------------------------
    // PROCESSES
    // ------------------------------------------------------------

    if (
        command == "show processes" ||
        command == "show running processes" ||
        command == "list processes" ||
        command == "show all processes" ||
        command == "show running programs" ||
        command == "list running programs" ||
        command == "what programs are running"
    )
    {
        return {
            CommandType::SHOW_PROCESSES,
            {}
        };
    }

    // ------------------------------------------------------------
    // FILE LISTING
    // ------------------------------------------------------------

    if (
        command == "list files" ||
        command == "show files" ||
        command == "show all files" ||
        command == "list all files" ||
        command == "display files" ||
        command == "display all files" ||
        command == "show me files" ||
        command == "show me all files" ||
        command == "list the files" ||
        command == "show the files" ||
        command == "show everything in this folder" ||
        command == "show files in this folder"
    )
    {
        return {
            CommandType::LIST_FILES,
            "."
        };
    }

    // ------------------------------------------------------------
    // DISK
    // ------------------------------------------------------------

    if (
        command == "disk information" ||
        command == "disk info" ||
        command == "show disk space" ||
        command == "show disk information" ||
        command == "show disk info" ||
        command == "disk usage" ||
        command == "show disk usage" ||
        command == "how much disk space do i have"
    )
    {
        return {
            CommandType::DISK_INFO,
            {}
        };
    }

    // ------------------------------------------------------------
    // NETWORK
    // ------------------------------------------------------------

    if (
        command == "network information" ||
        command == "network info" ||
        command == "show network information" ||
        command == "show network info" ||
        command == "show network" ||
        command == "network status" ||
        command == "show network status"
    )
    {
        return {
            CommandType::NETWORK_INFO,
            {}
        };
    }

    // ------------------------------------------------------------

    // ------------------------------------------------------------
    // CPU USAGE
    // ------------------------------------------------------------

    if (
        command == "cpu usage" ||
        command == "show cpu usage" ||
        command == "cpu information" ||
        command == "cpu info" ||
        command == "show cpu information" ||
        command == "what is my cpu usage" ||
        command == "how much cpu am i using"
    )
    {
        return {
            CommandType::CPU_USAGE,
            {}
        };
    }

    // ------------------------------------------------------------
    // MEMORY USAGE
    // ------------------------------------------------------------

    if (
        command == "memory usage" ||
        command == "show memory usage" ||
        command == "memory information" ||
        command == "memory info" ||
        command == "ram usage" ||
        command == "show ram usage" ||
        command == "show memory information" ||
        command == "how much ram do i have" ||
        command == "how much ram am i using"
    )
    {
        return {
            CommandType::MEMORY_USAGE,
            {}
        };
    }

    // ------------------------------------------------------------
    // UPTIME
    // ------------------------------------------------------------

    if (
        command == "uptime" ||
        command == "show uptime" ||
        command == "system uptime" ||
        command == "show system uptime" ||
        command == "how long has the system been running"
    )
    {
        return {
            CommandType::UPTIME,
            {}
        };
    }

    // ------------------------------------------------------------
    // LOAD AVERAGE
    // ------------------------------------------------------------

    if (
        command == "load average" ||
        command == "show load average" ||
        command == "system load" ||
        command == "show system load"
    )
    {
        return {
            CommandType::LOAD_AVERAGE,
            {}
        };
    }

    // ------------------------------------------------------------
    // KERNEL INFORMATION
    // ------------------------------------------------------------

    if (
        command == "kernel information" ||
        command == "kernel info" ||
        command == "show kernel information" ||
        command == "show kernel info" ||
        command == "kernel version" ||
        command == "show kernel version"
    )
    {
        return {
            CommandType::KERNEL_INFO,
            {}
        };
    }

    // ------------------------------------------------------------
    // YOUTUBE SEARCH
    // ------------------------------------------------------------

    const std::vector<std::string> youtubeSearchPrefixes =
    {
        "search youtube for ",
        "search youtube ",
        "youtube search ",
        "find on youtube ",
        "find on youtube for ",
        "play on youtube "
    };

    for (const auto& prefix : youtubeSearchPrefixes)
    {
        if (startsWith(command, prefix))
        {
            const std::string query =
                removePrefix(
                    command,
                    prefix
                );

            if (!query.empty())
            {
                return {
                    CommandType::OPEN_URL,
                    "youtube search:" + query
                };
            }
        }
    }

    // SEARCH
    // ------------------------------------------------------------

    const std::vector<std::string> searchPrefixes =
    {
        "search google for ",
        "search google ",
        "search for ",
        "search "
    };

    for (const auto& prefix : searchPrefixes)
    {
        if (startsWith(command, prefix))
        {
            const std::string query =
                removePrefix(
                    command,
                    prefix
                );

            if (!query.empty())
            {
                return {
                    CommandType::SEARCH_WEB,
                    query
                };
            }
        }
    }

    // ------------------------------------------------------------
    // OPEN / LAUNCH / START
    // ------------------------------------------------------------

    const std::vector<std::string> openPrefixes =
    {
        "open ",
        "launch ",
        "start ",
        "run "
    };

    for (const auto& prefix : openPrefixes)
    {
        if (!startsWith(command, prefix))
        {
            continue;
        }

        std::string target =
            removePrefix(
                command,
                prefix
            );

        // Remove articles.
        if (startsWith(target, "the "))
        {
            target =
                removePrefix(
                    target,
                    "the "
                );
        }

        // Common browser wording.
        replaceAll(
            target,
            "youtube website",
            "youtube"
        );

        replaceAll(
            target,
            "youtube browser",
            "youtube"
        );

        replaceAll(
            target,
            "whatsapp website",
            "whatsapp"
        );

        replaceAll(
            target,
            "github website",
            "github"
        );

        replaceAll(
            target,
            "github browser",
            "github"
        );

        // --------------------------------------------------------
        // KNOWN WEBSITES
        // --------------------------------------------------------

        if (
            target == "youtube" ||
            target == "youtube com"
        )
        {
            return {
                CommandType::OPEN_URL,
                "youtube"
            };
        }

        if (
            target == "whatsapp" ||
            target == "whatsapp web" ||
            target == "web whatsapp"
        )
        {
            return {
                CommandType::OPEN_URL,
                "whatsapp"
            };
        }

        if (target == "gmail")
        {
            return {
                CommandType::OPEN_URL,
                "gmail"
            };
        }

        if (target == "google")
        {
            return {
                CommandType::OPEN_URL,
                "google"
            };
        }

        if (target == "github")
        {
            return {
                CommandType::OPEN_URL,
                "github"
            };
        }

        if (target == "linkedin")
        {
            return {
                CommandType::OPEN_URL,
                "linkedin"
            };
        }

        if (target == "reddit")
        {
            return {
                CommandType::OPEN_URL,
                "reddit"
            };
        }

        if (target == "chatgpt")
        {
            return {
                CommandType::OPEN_URL,
                "chatgpt"
            };
        }

        if (target == "stackoverflow")
        {
            return {
                CommandType::OPEN_URL,
                "stackoverflow"
            };
        }

        // --------------------------------------------------------
        // DIRECT URL
        // --------------------------------------------------------

        if (
            startsWith(target, "http://") ||
            startsWith(target, "https://") ||
            target.find('.') != std::string::npos
        )
        {
            return {
                CommandType::OPEN_URL,
                target
            };
        }

        // --------------------------------------------------------
        // APPLICATION
        // --------------------------------------------------------

        return {
            CommandType::OPEN_APPLICATION,
            target
        };
    }

    // ------------------------------------------------------------
    // CLOSE APPLICATION
    // ------------------------------------------------------------

    const std::vector<std::string> closePrefixes =
    {
        "close ",
        "stop ",
        "terminate ",
        "quit "
    };

    for (const auto& prefix : closePrefixes)
    {
        if (startsWith(command, prefix))
        {
            std::string target =
                removePrefix(
                    command,
                    prefix
                );

            if (startsWith(target, "the "))
            {
                target =
                    removePrefix(
                        target,
                        "the "
                    );
            }

            return {
                CommandType::CLOSE_APPLICATION,
                target
            };
        }
    }

    // ------------------------------------------------------------
    // CREATE FILE
    // ------------------------------------------------------------

    const std::vector<std::string> createFilePrefixes =
    {
        "create file ",
        "make file ",
        "new file "
    };

    for (const auto& prefix : createFilePrefixes)
    {
        if (startsWith(command, prefix))
        {
            return {
                CommandType::CREATE_FILE,
                removePrefix(
                    command,
                    prefix
                )
            };
        }
    }

    // ------------------------------------------------------------
    // DELETE FILE
    // ------------------------------------------------------------

    const std::vector<std::string> deleteFilePrefixes =
    {
        "delete file ",
        "remove file "
    };

    for (const auto& prefix : deleteFilePrefixes)
    {
        if (startsWith(command, prefix))
        {
            return {
                CommandType::DELETE_FILE,
                removePrefix(
                    command,
                    prefix
                )
            };
        }
    }

    // ------------------------------------------------------------
    // CREATE FOLDER
    // ------------------------------------------------------------

    const std::vector<std::string> createFolderPrefixes =
    {
        "create folder ",
        "make folder ",
        "new folder ",
        "create directory ",
        "make directory "
    };

    for (const auto& prefix : createFolderPrefixes)
    {
        if (startsWith(command, prefix))
        {
            return {
                CommandType::CREATE_FOLDER,
                removePrefix(
                    command,
                    prefix
                )
            };
        }
    }

    // ------------------------------------------------------------
    // DELETE FOLDER
    // ------------------------------------------------------------

    const std::vector<std::string> deleteFolderPrefixes =
    {
        "delete folder ",
        "remove folder ",
        "delete directory ",
        "remove directory "
    };

    for (const auto& prefix : deleteFolderPrefixes)
    {
        if (startsWith(command, prefix))
        {
            return {
                CommandType::DELETE_FOLDER,
                removePrefix(
                    command,
                    prefix
                )
            };
        }
    }

    // ------------------------------------------------------------
    // UNKNOWN
    // ------------------------------------------------------------

    return {
        CommandType::UNKNOWN,
        input
    };
}
