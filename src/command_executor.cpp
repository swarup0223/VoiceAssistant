#include "command_executor.hpp"

#include "file_manager.hpp"
#include "process_manager.hpp"
#include "system_monitor.hpp"

#include <cstdlib>
#include <iostream>

CommandExecutor::CommandExecutor()
    : application_manager_(),
      browser_manager_(),
      driver_("/dev/voiceassistant")
{
}

void CommandExecutor::execute(
    const ParsedCommand& command
)
{
    switch (command.type)
    {
        case CommandType::OPEN_APPLICATION:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "open " + command.argument
                );
            }

            if (
                application_manager_.launch(
                    command.argument
                )
            )
            {
                std::cout
                    << "[VoiceAssistant] Opened: "
                    << command.argument
                    << '\n';
            }
            else
            {
                std::cout
                    << "[VoiceAssistant] Application not found: "
                    << command.argument
                    << '\n';
            }

            break;
        }

        case CommandType::OPEN_URL:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "open url " +
                    command.argument
                );
            }

            if (
                browser_manager_.open(
                    command.argument
                )
            )
            {
                std::cout
                    << "[VoiceAssistant] Opened URL: "
                    << command.argument
                    << '\n';
            }
            else
            {
                std::cout
                    << "[VoiceAssistant] Unable to open URL\n";
            }

            break;
        }

        case CommandType::SEARCH_WEB:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "search " +
                    command.argument
                );
            }

            if (
                browser_manager_.searchGoogle(
                    command.argument
                )
            )
            {
                std::cout
                    << "[VoiceAssistant] Searching Google for: "
                    << command.argument
                    << '\n';
            }
            else
            {
                std::cout
                    << "[VoiceAssistant] Search failed\n";
            }

            break;
        }

        case CommandType::CLOSE_APPLICATION:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "close " +
                    command.argument
                );
            }

            if (
                application_manager_.close(
                    command.argument
                )
            )
            {
                std::cout
                    << "[VoiceAssistant] Closed: "
                    << command.argument
                    << '\n';
            }
            else
            {
                std::cout
                    << "[VoiceAssistant] Could not close: "
                    << command.argument
                    << '\n';
            }

            break;
        }

        case CommandType::SYSTEM_INFO:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "system information"
                );
            }

            /*
             * Use the existing SystemMonitor API.
             *
             * Your current class does not have
             * showSystemInfo(), so use the
             * existing display method.
             */
            SystemMonitor monitor;
            monitor.displaySystemInformation();

            break;
        }

        case CommandType::SHOW_PROCESSES:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "show processes"
                );
            }

            ProcessManager manager;
            manager.listProcesses();

            break;
        }

        case CommandType::LIST_FILES:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "list files"
                );
            }

            FileManager manager;

            /*
             * Your existing FileManager requires
             * a path argument.
             *
             * "." means current directory.
             */
            manager.listFiles(".");

            break;
        }

        case CommandType::CREATE_FILE:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "create file " + command.argument
                );
            }

            FileManager manager;
            manager.createFile(command.argument);

            break;
        }

        case CommandType::DELETE_FILE:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "delete file " + command.argument
                );
            }

            FileManager manager;
            manager.deleteFile(command.argument);

            break;
        }


        case CommandType::DISK_INFO:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "disk information"
                );
            }

            std::cout
                << "[VoiceAssistant] Disk information:\n";

            std::system("df -h");

            break;
        }

        case CommandType::NETWORK_INFO:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "network information"
                );
            }

            std::cout
                << "[VoiceAssistant] Network information:\n";

            std::system("ip addr");

            break;
        }

        case CommandType::CPU_USAGE:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "cpu usage"
                );
            }

            SystemMonitor monitor;
            monitor.displayCPUUsage();

            break;
        }

        case CommandType::MEMORY_USAGE:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "memory usage"
                );
            }

            SystemMonitor monitor;
            monitor.displayMemoryUsage();

            break;
        }

        case CommandType::UPTIME:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "uptime"
                );
            }

            SystemMonitor monitor;
            monitor.displayUptime();

            break;
        }

        case CommandType::LOAD_AVERAGE:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "load average"
                );
            }

            SystemMonitor monitor;
            monitor.displayLoadAverage();

            break;
        }

        case CommandType::KERNEL_INFO:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand(
                    "kernel information"
                );
            }

            SystemMonitor monitor;
            monitor.displayKernelInformation();

            break;
        }


        case CommandType::HELP:
        {
            showHelp();
            break;
        }

        case CommandType::EXIT:
        {
            if (driver_.isOpen())
            {
                driver_.sendCommand("exit");
            }

            std::cout
                << "[VoiceAssistant] Exiting...\n";

            break;
        }

        default:
        {
            std::cout
                << "[VoiceAssistant] Unknown command: "
                << command.argument
                << '\n';

            break;
        }
    }
}

void CommandExecutor::showHelp()
{
    std::cout << R"(
================ VoiceAssistant Commands ================

APPLICATIONS

  open firefox
  open vscode
  open terminal
  open vlc
  open file manager
  close firefox

WEB

  open youtube
  open whatsapp
  open gmail
  open github
  open google
  open linkedin
  open reddit
  open chatgpt

URL

  open https://example.com

SEARCH

  search google for Linux device drivers
  search for C++ tutorials

SYSTEM

  show system information
  show processes
  list files
  show disk space
  show network information

FILES

  create file test.txt
  delete file test.txt

ASSISTANT

  help
  exit

=====================================================
)";
}
