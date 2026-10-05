#ifndef VOICEASSISTANT_MAIN_WINDOW_HPP
#define VOICEASSISTANT_MAIN_WINDOW_HPP

#include <QMainWindow>

#include <string>

class QLabel;
class QPushButton;
class QLineEdit;
class QTextEdit;
class QProgressBar;
class QTimer;

class CommandParser;
class CommandExecutor;
class VoiceEngine;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(
        CommandParser* parser,
        CommandExecutor* executor,
        VoiceEngine* voiceEngine,
        QWidget* parent = nullptr
    );

private:
    void setupUi();
    void setupConnections();

    void executeTextCommand();
    void startVoiceCommand();

    void processCommand(const std::string& input);
    void updateSystemStatus();

    void addHistory(
        const QString& command,
        const QString& result
    );

    void setReadyStatus();
    void setListeningStatus();
    void setProcessingStatus();

    QLabel* statusLabel_;
    QLabel* driverStatusLabel_;
    QLabel* whisperStatusLabel_;

    QLabel* cpuValueLabel_;
    QLabel* memoryValueLabel_;
    QLabel* diskValueLabel_;

    QProgressBar* cpuProgress_;
    QProgressBar* memoryProgress_;
    QProgressBar* diskProgress_;

    QLineEdit* commandInput_;

    QPushButton* executeButton_;
    QPushButton* voiceButton_;

    QTextEdit* historyView_;

    QTimer* systemTimer_;

    CommandParser* parser_;
    CommandExecutor* executor_;
    VoiceEngine* voiceEngine_;

    bool listening_;
};

#endif
