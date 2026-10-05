#include "gui/main_window.hpp"

#include "command_executor.hpp"
#include "command_parser.hpp"
#include "device_driver_client.hpp"
#include "safety_engine.hpp"
#include "voice_engine.hpp"

#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMetaObject>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollBar>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <future>
#include <sstream>
#include <string>
#include <thread>

namespace
{

QFrame* createCard()
{
    auto* card = new QFrame();

    card->setStyleSheet(
        "QFrame {"
        " background-color: #151a21;"
        " border: 1px solid #2a313b;"
        " border-radius: 14px;"
        "}"
    );

    return card;
}

QLabel* createSectionTitle(const QString& text)
{
    auto* label = new QLabel(text);

    label->setStyleSheet(
        "QLabel {"
        " color: #ffffff;"
        " font-size: 15px;"
        " font-weight: 700;"
        " padding-top: 4px;"
        " padding-bottom: 4px;"
        "}"
    );

    return label;
}

}

MainWindow::MainWindow(
    CommandParser* parser,
    CommandExecutor* executor,
    VoiceEngine* voiceEngine,
    QWidget* parent
)
    : QMainWindow(parent),
      statusLabel_(nullptr),
      driverStatusLabel_(nullptr),
      whisperStatusLabel_(nullptr),
      cpuValueLabel_(nullptr),
      memoryValueLabel_(nullptr),
      diskValueLabel_(nullptr),
      cpuProgress_(nullptr),
      memoryProgress_(nullptr),
      diskProgress_(nullptr),
      commandInput_(nullptr),
      executeButton_(nullptr),
      voiceButton_(nullptr),
      historyView_(nullptr),
      systemTimer_(nullptr),
      parser_(parser),
      executor_(executor),
      voiceEngine_(voiceEngine),
      listening_(false)
{
    setupUi();
    setupConnections();

    updateSystemStatus();

    systemTimer_ = new QTimer(this);

    connect(
        systemTimer_,
        &QTimer::timeout,
        this,
        &MainWindow::updateSystemStatus
    );

    systemTimer_->start(3000);
}

void MainWindow::setupUi()
{
    setWindowTitle("VoiceAssistant - Linux Desktop Assistant");

    resize(1050, 760);
    setMinimumSize(900, 650);

    setStyleSheet(
        "QMainWindow {"
        " background-color: #0b0f14;"
        " color: #ffffff;"
        "}"

        "QLabel {"
        " color: #e8edf3;"
        "}"

        "QLineEdit {"
        " background-color: #11161d;"
        " border: 1px solid #303844;"
        " border-radius: 8px;"
        " color: #ffffff;"
        " padding: 11px;"
        " font-size: 14px;"
        "}"

        "QLineEdit:focus {"
        " border: 1px solid #29c94f;"
        "}"

        "QPushButton {"
        " background-color: #202732;"
        " border: 1px solid #343d49;"
        " border-radius: 8px;"
        " color: #ffffff;"
        " padding: 10px 18px;"
        " font-weight: 600;"
        "}"

        "QPushButton:hover {"
        " background-color: #29323e;"
        "}"

        "QPushButton:disabled {"
        " color: #69727e;"
        " background-color: #171c23;"
        "}"

        "QTextEdit {"
        " background-color: #11161d;"
        " border: 1px solid #2c3440;"
        " border-radius: 10px;"
        " color: #dce4ed;"
        " padding: 10px;"
        " font-size: 13px;"
        "}"
    );

    auto* central = new QWidget();
    auto* mainLayout = new QVBoxLayout(central);

    mainLayout->setContentsMargins(28, 22, 28, 22);
    mainLayout->setSpacing(16);

    setCentralWidget(central);

    // ------------------------------------------------------------
    // HEADER
    // ------------------------------------------------------------

    auto* headerLayout = new QHBoxLayout();

    auto* titleLayout = new QVBoxLayout();

    auto* title = new QLabel("VoiceAssistant");

    title->setStyleSheet(
        "QLabel {"
        " color: #ffffff;"
        " font-size: 31px;"
        " font-weight: 800;"
        "}"
    );

    auto* subtitle = new QLabel(
        "Linux Voice-Controlled Desktop Assistant"
    );

    subtitle->setStyleSheet(
        "QLabel {"
        " color: #758397;"
        " font-size: 13px;"
        "}"
    );

    titleLayout->addWidget(title);
    titleLayout->addWidget(subtitle);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    statusLabel_ = new QLabel("● READY");

    statusLabel_->setStyleSheet(
        "QLabel {"
        " color: #6ee787;"
        " font-size: 13px;"
        " font-weight: 700;"
        "}"
    );

    headerLayout->addWidget(statusLabel_);

    mainLayout->addLayout(headerLayout);

    // ------------------------------------------------------------
    // VOICE COMMAND CARD
    // ------------------------------------------------------------

    auto* voiceCard = createCard();
    auto* voiceLayout = new QVBoxLayout(voiceCard);

    voiceLayout->setContentsMargins(18, 18, 18, 18);
    voiceLayout->setSpacing(10);

    auto* question = new QLabel(
        "What would you like me to do?"
    );

    question->setAlignment(Qt::AlignCenter);

    question->setStyleSheet(
        "QLabel {"
        " color: #f0f4f8;"
        " font-size: 18px;"
        " font-weight: 700;"
        " padding: 8px;"
        "}"
    );

    voiceLayout->addWidget(question);

    voiceButton_ = new QPushButton("🎙  LISTEN");

    voiceButton_->setMinimumHeight(58);

    voiceButton_->setStyleSheet(
        "QPushButton {"
        " background-color: #1f8f3a;"
        " border: none;"
        " border-radius: 10px;"
        " color: white;"
        " font-size: 17px;"
        " font-weight: 800;"
        "}"

        "QPushButton:hover {"
        " background-color: #27a844;"
        "}"

        "QPushButton:disabled {"
        " background-color: #35423a;"
        " color: #aeb9b1;"
        "}"
    );

    voiceLayout->addWidget(voiceButton_);

    auto* commandRow = new QHBoxLayout();

    commandInput_ = new QLineEdit();

    commandInput_->setPlaceholderText(
        "Type a command or use the LISTEN button..."
    );

    commandInput_->setMinimumHeight(42);

    executeButton_ = new QPushButton("EXECUTE");

    executeButton_->setMinimumHeight(42);

    commandRow->addWidget(commandInput_, 1);
    commandRow->addWidget(executeButton_);

    voiceLayout->addLayout(commandRow);

    mainLayout->addWidget(voiceCard);

    // ------------------------------------------------------------
    // SYSTEM STATUS
    // ------------------------------------------------------------

    mainLayout->addWidget(
        createSectionTitle("SYSTEM STATUS")
    );

    auto* statusCard = createCard();
    auto* statusLayout = new QVBoxLayout(statusCard);

    statusLayout->setContentsMargins(18, 12, 18, 12);
    statusLayout->setSpacing(8);

    auto addProgress =
        [&](const QString& name,
            QProgressBar*& bar,
            QLabel*& value)
    {
        auto* row = new QHBoxLayout();

        auto* label = new QLabel(name);

        label->setFixedWidth(52);

        bar = new QProgressBar();

        bar->setRange(0, 100);
        bar->setValue(0);
        bar->setTextVisible(false);
        bar->setFixedHeight(13);

        bar->setStyleSheet(
            "QProgressBar {"
            " background-color: #252c35;"
            " border: none;"
            " border-radius: 6px;"
            "}"

            "QProgressBar::chunk {"
            " background-color: #23a744;"
            " border-radius: 6px;"
            "}"
        );

        value = new QLabel("0%");
        value->setFixedWidth(35);

        row->addWidget(label);
        row->addWidget(bar, 1);
        row->addWidget(value);

        statusLayout->addLayout(row);
    };

    addProgress(
        "CPU",
        cpuProgress_,
        cpuValueLabel_
    );

    addProgress(
        "Memory",
        memoryProgress_,
        memoryValueLabel_
    );

    addProgress(
        "Disk",
        diskProgress_,
        diskValueLabel_
    );

    auto* serviceRow = new QHBoxLayout();

    driverStatusLabel_ = new QLabel("Driver: ● CHECKING");

    whisperStatusLabel_ = new QLabel("Whisper: READY");

    driverStatusLabel_->setStyleSheet(
        "QLabel { color: #6ee787; font-weight: 600; }"
    );

    whisperStatusLabel_->setStyleSheet(
        "QLabel { color: #6ee787; font-weight: 600; }"
    );

    serviceRow->addWidget(driverStatusLabel_);
    serviceRow->addSpacing(15);
    serviceRow->addWidget(whisperStatusLabel_);
    serviceRow->addStretch();

    statusLayout->addLayout(serviceRow);

    mainLayout->addWidget(statusCard);

    // ------------------------------------------------------------
    // HISTORY
    // ------------------------------------------------------------

    mainLayout->addWidget(
        createSectionTitle("RECENT COMMANDS")
    );

    historyView_ = new QTextEdit();

    historyView_->setReadOnly(true);
    historyView_->setMinimumHeight(170);

    historyView_->setPlaceholderText(
        "VoiceAssistant command history will appear here..."
    );

    mainLayout->addWidget(historyView_, 1);
}

void MainWindow::setupConnections()
{
    connect(
        executeButton_,
        &QPushButton::clicked,
        this,
        &MainWindow::executeTextCommand
    );

    connect(
        commandInput_,
        &QLineEdit::returnPressed,
        this,
        &MainWindow::executeTextCommand
    );

    connect(
        voiceButton_,
        &QPushButton::clicked,
        this,
        &MainWindow::startVoiceCommand
    );
}

void MainWindow::executeTextCommand()
{
    const QString text = commandInput_->text().trimmed();

    if (text.isEmpty())
    {
        return;
    }

    commandInput_->clear();

    processCommand(
        text.toStdString()
    );
}

void MainWindow::startVoiceCommand()
{
    if (listening_)
    {
        return;
    }

    if (voiceEngine_ == nullptr)
    {
        QMessageBox::warning(
            this,
            "VoiceAssistant",
            "Voice engine is not available."
        );

        return;
    }

    listening_ = true;

    voiceButton_->setEnabled(false);
    executeButton_->setEnabled(false);

    setListeningStatus();

    historyView_->append(
        "<span style='color:#8b98a9;'>"
        "🎙 Listening for a command..."
        "</span>"
    );

    /*
     * VoiceEngine::listen() performs the existing Whisper workflow:
     *
     * Microphone
     *     ↓
     * parecord
     *     ↓
     * WAV / PCM
     *     ↓
     * whisper.cpp
     *     ↓
     * recognized text
     *
     * It is blocking, so it must NOT run on the Qt GUI thread.
     */

    std::thread(
        [this]()
        {
            std::string recognized;

            try
            {
                recognized = voiceEngine_->listen();
            }
            catch (...)
            {
                recognized.clear();
            }

            QMetaObject::invokeMethod(
                this,
                [this, recognized]()
                {
                    listening_ = false;

                    voiceButton_->setEnabled(true);
                    executeButton_->setEnabled(true);

                    if (recognized.empty())
                    {
                        setReadyStatus();

                        historyView_->append(
                            "<span style='color:#f2cc60;'>"
                            "⚠ No command recognized."
                            "</span>"
                        );

                        return;
                    }

                    commandInput_->setText(
                        QString::fromStdString(recognized)
                    );

                    historyView_->append(
                        QString(
                            "<span style='color:#8ab4f8;'>"
                            "🎙 Recognized: <b>%1</b>"
                            "</span>"
                        )
                        .arg(
                            QString::fromStdString(recognized)
                                .toHtmlEscaped()
                        )
                    );

                    processCommand(recognized);
                },
                Qt::QueuedConnection
            );
        }
    ).detach();
}

void MainWindow::processCommand(
    const std::string& input
)
{
    if (input.empty())
    {
        return;
    }

    setProcessingStatus();

    /*
     * IMPORTANT:
     *
     * Keep exactly the same safety boundary used by the CLI.
     *
     * Voice commands are NOT allowed to bypass SafetyEngine.
     */

    SafetyEngine safety;

    if (!safety.isSafe(input))
    {
        addHistory(
            QString::fromStdString(input),
            "BLOCKED — unsafe command"
        );

        QMessageBox::warning(
            this,
            "VoiceAssistant Safety",
            "This command was blocked by the safety engine."
        );

        setReadyStatus();

        return;
    }

    if (parser_ == nullptr || executor_ == nullptr)
    {
        addHistory(
            QString::fromStdString(input),
            "ERROR — command subsystem unavailable"
        );

        setReadyStatus();

        return;
    }

    ParsedCommand command =
        parser_->parse(input);

    executor_->execute(command);

    addHistory(
        QString::fromStdString(input),
        "Command sent to executor"
    );

    setReadyStatus();

    /*
     * Exit is handled by the GUI itself after the executor
     * receives the command.
     */
    if (command.type == CommandType::EXIT)
    {
        QTimer::singleShot(
            250,
            qApp,
            &QApplication::quit
        );
    }
}

void MainWindow::addHistory(
    const QString& command,
    const QString& result
)
{
    const QString escapedCommand =
        command.toHtmlEscaped();

    const QString escapedResult =
        result.toHtmlEscaped();

    historyView_->append(
        QString(
            "<div style='margin-bottom:8px;'>"
            "<span style='color:#6ee787;'>"
            "▶ "
            "</span>"
            "<b>%1</b>"
            "<br>"
            "<span style='color:#7f8b99;'>"
            "%2"
            "</span>"
            "</div>"
        )
        .arg(
            escapedCommand,
            escapedResult
        )
    );

    historyView_->verticalScrollBar()->setValue(
        historyView_->verticalScrollBar()->maximum()
    );
}

void MainWindow::setReadyStatus()
{
    statusLabel_->setText("● READY");

    statusLabel_->setStyleSheet(
        "QLabel {"
        " color: #6ee787;"
        " font-size: 13px;"
        " font-weight: 700;"
        "}"
    );

    voiceButton_->setText("🎙  LISTEN");
}

void MainWindow::setListeningStatus()
{
    statusLabel_->setText("● LISTENING...");

    statusLabel_->setStyleSheet(
        "QLabel {"
        " color: #f2cc60;"
        " font-size: 13px;"
        " font-weight: 700;"
        "}"
    );

    voiceButton_->setText(
        "🎙  LISTENING..."
    );
}

void MainWindow::setProcessingStatus()
{
    statusLabel_->setText("● PROCESSING...");

    statusLabel_->setStyleSheet(
        "QLabel {"
        " color: #8ab4f8;"
        " font-size: 13px;"
        " font-weight: 700;"
        "}"
    );

    voiceButton_->setText(
        "⚙  PROCESSING..."
    );
}

void MainWindow::updateSystemStatus()
{
    // ------------------------------------------------------------
    // MEMORY
    // ------------------------------------------------------------

    std::ifstream meminfo(
        "/proc/meminfo"
    );

    long long totalKB = 0;
    long long availableKB = 0;

    std::string line;

    while (std::getline(meminfo, line))
    {
        if (line.rfind("MemTotal:", 0) == 0)
        {
            std::istringstream iss(line);
            std::string key;
            iss >> key >> totalKB;
        }

        if (line.rfind("MemAvailable:", 0) == 0)
        {
            std::istringstream iss(line);
            std::string key;
            iss >> key >> availableKB;
        }
    }

    if (totalKB > 0)
    {
        const int memory =
            static_cast<int>(
                100.0 *
                (1.0 -
                 static_cast<double>(availableKB) /
                 static_cast<double>(totalKB))
            );

        const int bounded =
            std::clamp(memory, 0, 100);

        memoryProgress_->setValue(bounded);

        memoryValueLabel_->setText(
            QString("%1%").arg(bounded)
        );
    }

    // ------------------------------------------------------------
    // CPU LOAD
    // ------------------------------------------------------------

    std::ifstream loadavg(
        "/proc/loadavg"
    );

    double load = 0.0;

    if (loadavg >> load)
    {
        const unsigned int cpuCount =
            std::max(
                1u,
                std::thread::hardware_concurrency()
            );

        const int cpu =
            std::clamp(
                static_cast<int>(
                    (load /
                     static_cast<double>(cpuCount))
                    * 100.0
                ),
                0,
                100
            );

        cpuProgress_->setValue(cpu);

        cpuValueLabel_->setText(
            QString("%1%").arg(cpu)
        );
    }

    // ------------------------------------------------------------
    // DISK
    // ------------------------------------------------------------

    FILE* pipe = popen(
        "df -P / 2>/dev/null",
        "r"
    );

    if (pipe != nullptr)
    {
        char buffer[512];

        std::string output;

        while (
            fgets(
                buffer,
                sizeof(buffer),
                pipe
            )
        )
        {
            output += buffer;
        }

        pclose(pipe);

        std::istringstream iss(output);

        std::string line1;
        std::string line2;

        std::getline(iss, line1);
        std::getline(iss, line2);

        if (!line2.empty())
        {
            std::istringstream disk(line2);

            std::string filesystem;
            std::string blocks;
            std::string used;
            std::string available;
            std::string percentage;

            disk >>
                filesystem >>
                blocks >>
                used >>
                available >>
                percentage;

            if (!percentage.empty() &&
                percentage.back() == '%')
            {
                percentage.pop_back();

                try
                {
                    const int value =
                        std::clamp(
                            std::stoi(percentage),
                            0,
                            100
                        );

                    diskProgress_->setValue(value);

                    diskValueLabel_->setText(
                        QString("%1%").arg(value)
                    );
                }
                catch (...)
                {
                }
            }
        }
    }

    // ------------------------------------------------------------
    // DRIVER
    // ------------------------------------------------------------

    DeviceDriverClient driver;

    if (driver.isOpen())
    {
        driverStatusLabel_->setText(
            "Driver: ● ONLINE"
        );

        driverStatusLabel_->setStyleSheet(
            "QLabel {"
            " color: #6ee787;"
            " font-weight: 600;"
            "}"
        );
    }
    else
    {
        driverStatusLabel_->setText(
            "Driver: ● OFFLINE"
        );

        driverStatusLabel_->setStyleSheet(
            "QLabel {"
            " color: #ff7b72;"
            " font-weight: 600;"
            "}"
        );
    }

    // ------------------------------------------------------------
    // WHISPER
    // ------------------------------------------------------------

    if (voiceEngine_ != nullptr)
    {
        whisperStatusLabel_->setText(
            "Whisper: READY"
        );

        whisperStatusLabel_->setStyleSheet(
            "QLabel {"
            " color: #6ee787;"
            " font-weight: 600;"
            "}"
        );
    }
    else
    {
        whisperStatusLabel_->setText(
            "Whisper: OFFLINE"
        );

        whisperStatusLabel_->setStyleSheet(
            "QLabel {"
            " color: #ff7b72;"
            " font-weight: 600;"
            "}"
        );
    }
}
