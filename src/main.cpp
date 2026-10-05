#include "command_executor.hpp"
#include "command_parser.hpp"
#include "safety_engine.hpp"
#include "voice_engine.hpp"

#include <iostream>
#include <string>

namespace
{
bool requiresConfirmation(
    const ParsedCommand& command
)
{
    return
        command.type == CommandType::DELETE_FILE ||
        command.type == CommandType::DELETE_FOLDER;
}

bool confirmCommand(
    const ParsedCommand& command
)
{
    std::cout
        << "\nVoiceAssistant Confirmation Required\n"
        << "---------------------------------\n"
        << "Action: ";

    if (command.type == CommandType::DELETE_FILE)
    {
        std::cout << "Delete file";
    }
    else if (command.type == CommandType::DELETE_FOLDER)
    {
        std::cout << "Delete folder";
    }

    std::cout
        << "\nTarget: "
        << command.argument
        << "\n\n"
        << "Are you sure? (y/n): ";

    std::string response;
    std::getline(
        std::cin,
        response
    );

    if (
        response == "y" ||
        response == "Y" ||
        response == "yes" ||
        response == "YES"
    )
    {
        return true;
    }

    std::cout
        << "VoiceAssistant: Action cancelled.\n";

    return false;
}

int processCommand(
    const std::string& input,
    CommandParser& parser,
    CommandExecutor& executor,
    SafetyEngine& safety
)
{
    if (input.empty())
    {
        std::cerr
            << "VoiceAssistant: Empty command.\n";

        return 1;
    }

    std::cout
        << "You said: "
        << input
        << "\n";

    if (!safety.isSafe(input))
    {
        std::cout
            << "VoiceAssistant Safety Engine: "
            << "This command has been blocked "
            << "for security reasons.\n";

        return 1;
    }

    const ParsedCommand command =
        parser.parse(input);

    if (command.type == CommandType::UNKNOWN)
    {
        std::cout
            << "VoiceAssistant: Unknown command.\n";

        return 1;
    }

    if (command.type == CommandType::EXIT)
    {
        std::cout
            << "VoiceAssistant: Goodbye.\n";

        return 0;
    }

    if (requiresConfirmation(command))
    {
        if (!confirmCommand(command))
        {
            return 0;
        }
    }

    executor.execute(command);

    return 0;
}
}

int main()
{
    std::cout << "\n";
    std::cout << "============================================\n";
    std::cout << "              VOICEASSISTANT\n";
    std::cout << "       Linux Desktop Assistant\n";
    std::cout << "============================================\n\n";

    CommandParser parser;
    CommandExecutor executor;
    SafetyEngine safety;

    std::cout
        << "Select command input mode:\n"
        << "  1. Type a command\n"
        << "  2. Speak a command\n\n"
        << "Enter choice: ";

    std::string choice;
    std::getline(
        std::cin,
        choice
    );

    if (choice == "1")
    {
        std::cout
            << "\nType your VoiceAssistant command:\n> ";

        std::string typedCommand;
        std::getline(
            std::cin,
            typedCommand
        );

        return processCommand(
            typedCommand,
            parser,
            executor,
            safety
        );
    }

    if (choice == "2")
    {
        const std::string modelPath =
            "third_party/whisper.cpp/models/ggml-small.en.bin";

        VoiceEngine voiceEngine(modelPath);

        if (!voiceEngine.initialize())
        {
            std::cerr
                << "VoiceAssistant: "
                << "Voice subsystem initialization failed.\n";

            return 1;
        }

        std::cout
            << "\nVoiceAssistant: Voice subsystem ready.\n\n"
            << "Speak your command now.\n";

        const std::string recognizedText =
            voiceEngine.listen();

        if (recognizedText.empty())
        {
            std::cerr
                << "VoiceAssistant: No speech recognized.\n";

            return 1;
        }

        return processCommand(
            recognizedText,
            parser,
            executor,
            safety
        );
    }

    std::cout
        << "VoiceAssistant: Invalid input mode.\n";

    return 1;
}
