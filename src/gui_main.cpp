#include <QApplication>
#include <QMessageBox>

#include "command_executor.hpp"
#include "command_parser.hpp"
#include "gui/main_window.hpp"
#include "voice_engine.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    CommandParser parser;
    CommandExecutor executor;

    VoiceEngine voiceEngine(
        "third_party/whisper.cpp/models/ggml-base.en.bin"
    );

    // IMPORTANT:
    // Load the Whisper model before any voice command is attempted.
    if (!voiceEngine.initialize())
    {
        QMessageBox::critical(
            nullptr,
            "VoiceAssistant - Voice Engine Error",
            "Failed to initialize the Whisper voice engine.\n\n"
            "Check that the Whisper model exists at:\n"
            "third_party/whisper.cpp/models/ggml-base.en.bin"
        );

        return 1;
    }

    MainWindow window(
        &parser,
        &executor,
        &voiceEngine
    );

    window.show();

    return app.exec();
}
