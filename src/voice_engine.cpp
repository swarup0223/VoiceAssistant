#include "voice_engine.hpp"

#include "whisper.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

VoiceEngine::VoiceEngine(const std::string& modelPath)
    : modelPath(modelPath),
      context(nullptr)
{
}

VoiceEngine::~VoiceEngine()
{
    if (context != nullptr)
    {
        whisper_free(context);
        context = nullptr;
    }
}

bool VoiceEngine::initialize()
{
    std::cout
        << "VoiceEngine: Loading Whisper model...\n";

    context =
        whisper_init_from_file(modelPath.c_str());

    if (context == nullptr)
    {
        std::cerr
            << "VoiceEngine: Failed to load Whisper model.\n";

        return false;
    }

    std::cout
        << "VoiceEngine: Whisper model loaded successfully.\n";

    return true;
}

bool VoiceEngine::recordAudio(
    const std::string& filename
) const
{
    std::cout
        << "\nListening for 5 seconds...\n"
        << "Speak a VoiceAssistant command now.\n";

    std::remove(filename.c_str());

    const std::string recordCommand =
        "timeout --signal=INT --kill-after=1s 8s "
        "parecord "
        "--device=RDPSource "
        "--format=s16le "
        "--rate=16000 "
        "--channels=1 "
        "--file-format=wav "
        "\"" + filename + "\" "
        "|| test -s \"" + filename + "\"";

    const int recordResult =
        std::system(recordCommand.c_str());

    std::ifstream audioFile(filename);

    if (recordResult != 0 || !audioFile.good())
    {
        std::cerr
            << "VoiceEngine: Microphone recording failed.\n";

        audioFile.close();
        std::remove(filename.c_str());

        return false;
    }

    audioFile.close();

    const std::string convertedFile =
        filename + ".converted.wav";

    std::remove(convertedFile.c_str());

    const std::string convertCommand =
        "ffmpeg -y -loglevel error "
        "-i \"" + filename + "\" "
        "-ar 16000 "
        "-ac 1 "
        "-c:a pcm_s16le "
        "\"" + convertedFile + "\"";

    const int convertResult =
        std::system(convertCommand.c_str());

    if (convertResult != 0)
    {
        std::cerr
            << "VoiceEngine: Audio conversion failed.\n";

        std::remove(filename.c_str());
        std::remove(convertedFile.c_str());

        return false;
    }

    std::remove(filename.c_str());

    if (std::rename(
            convertedFile.c_str(),
            filename.c_str()) != 0)
    {
        std::cerr
            << "VoiceEngine: Could not replace "
            << "the audio file.\n";

        std::remove(convertedFile.c_str());

        return false;
    }

    return true;
}

std::string VoiceEngine::transcribe(
    const std::string& filename
)
{
    std::cout
        << "VoiceEngine: Transcribing...\n";

    std::ifstream file(
        filename,
        std::ios::binary
    );

    if (!file)
    {
        std::cerr
            << "VoiceEngine: Could not open audio file.\n";

        return "";
    }

    /*
     * Verify RIFF/WAV header.
     */
    char riff[4];

    file.read(
        riff,
        4
    );

    if (file.gcount() != 4 ||
        riff[0] != 'R' ||
        riff[1] != 'I' ||
        riff[2] != 'F' ||
        riff[3] != 'F')
    {
        std::cerr
            << "VoiceEngine: Invalid WAV file.\n";

        file.close();

        return "";
    }

    /*
     * Read number of channels.
     */
    file.seekg(
        22,
        std::ios::beg
    );

    std::uint16_t channels = 0;

    file.read(
        reinterpret_cast<char*>(&channels),
        sizeof(channels)
    );

    /*
     * Read sample rate.
     */
    file.seekg(
        24,
        std::ios::beg
    );

    std::uint32_t sampleRate = 0;

    file.read(
        reinterpret_cast<char*>(&sampleRate),
        sizeof(sampleRate)
    );

    /*
     * Read bits per sample.
     */
    file.seekg(
        34,
        std::ios::beg
    );

    std::uint16_t bitsPerSample = 0;

    file.read(
        reinterpret_cast<char*>(&bitsPerSample),
        sizeof(bitsPerSample)
    );

    if (channels != 1 ||
        sampleRate != 16000 ||
        bitsPerSample != 16)
    {
        std::cerr
            << "VoiceEngine: WAV format is not "
            << "16-bit mono 16 kHz.\n";

        std::cerr
            << "Channels: "
            << channels
            << ", Rate: "
            << sampleRate
            << ", Bits: "
            << bitsPerSample
            << "\n";

        file.close();

        return "";
    }

    /*
     * Search for the WAV data chunk.
     */
    file.seekg(
        12,
        std::ios::beg
    );

    char chunkId[4];

    std::uint32_t chunkSize = 0;

    bool foundData = false;

    std::uint32_t dataSize = 0;

    while (file.good())
    {
        file.read(
            chunkId,
            4
        );

        if (file.gcount() != 4)
        {
            break;
        }

        file.read(
            reinterpret_cast<char*>(&chunkSize),
            sizeof(chunkSize)
        );

        if (file.gcount() != 4)
        {
            break;
        }

        if (chunkId[0] == 'd' &&
            chunkId[1] == 'a' &&
            chunkId[2] == 't' &&
            chunkId[3] == 'a')
        {
            dataSize = chunkSize;
            foundData = true;

            break;
        }

        /*
         * Skip unknown WAV chunk.
         */
        file.seekg(
            chunkSize,
            std::ios::cur
        );
    }

    if (!foundData ||
        dataSize == 0)
    {
        std::cerr
            << "VoiceEngine: WAV data chunk not found.\n";

        file.close();

        return "";
    }

    /*
     * Convert 16-bit PCM samples to float.
     */
    const std::size_t sampleCount =
        dataSize / sizeof(std::int16_t);

    std::vector<std::int16_t> samples(
        sampleCount
    );

    file.read(
        reinterpret_cast<char*>(
            samples.data()
        ),
        static_cast<std::streamsize>(
            dataSize
        )
    );

    file.close();

    if (samples.empty())
    {
        std::cerr
            << "VoiceEngine: No audio samples found.\n";

        return "";
    }

    std::vector<float> pcmf32(
        sampleCount
    );

    for (std::size_t i = 0;
         i < sampleCount;
         ++i)
    {
        pcmf32[i] =
            static_cast<float>(
                samples[i]
            ) / 32768.0f;
    }

    /*
     * Whisper configuration.
     */
    whisper_full_params params =
        whisper_full_default_params(
            WHISPER_SAMPLING_GREEDY
        );

    params.print_progress = false;
    params.print_special = false;
    params.print_realtime = false;
    params.print_timestamps = false;

    params.translate = false;
    params.language = "en";

    params.n_threads = 4;

    params.no_context = true;
    params.single_segment = false;

    params.temperature = 0.0f;

    /*
     * Run Whisper.
     */
    const int result =
        whisper_full(
            context,
            params,
            pcmf32.data(),
            static_cast<int>(
                pcmf32.size()
            )
        );

    if (result != 0)
    {
        std::cerr
            << "VoiceEngine: Whisper transcription failed.\n";

        return "";
    }

    /*
     * Collect Whisper segments.
     */
    std::string text;

    const int segmentCount =
        whisper_full_n_segments(
            context
        );

    for (int i = 0;
         i < segmentCount;
         ++i)
    {
        const char* segmentText =
            whisper_full_get_segment_text(
                context,
                i
            );

        if (segmentText != nullptr)
        {
            text += segmentText;
        }
    }

    /*
     * Trim whitespace.
     */
    const std::size_t first =
        text.find_first_not_of(
            " \t\n\r"
        );

    const std::size_t last =
        text.find_last_not_of(
            " \t\n\r"
        );

    if (first == std::string::npos)
    {
        return "";
    }

    return text.substr(
        first,
        last - first + 1
    );
}

std::string VoiceEngine::listen()
{
    const std::string filename =
        "/tmp/voiceassistant_command.wav";

    if (!recordAudio(filename))
    {
        return "";
    }

    const std::string result =
        transcribe(filename);

    std::remove(
        filename.c_str()
    );

    return result;
}
