#include "SoundManager.h"

#include "../Utils/SoundUtilities.h"

#include <functional>
#include <ios>
#include <istream>
#include <spdlog/spdlog.h>
#include <vector>

namespace ad {
namespace grapito {

constexpr int StreamingBufferSize = 32768;

static size_t readOggInputStreamCallback(void * destination, size_t elementSize, size_t elementCount, void * aDatasource)
{
    ALsizei lengthRead = elementSize * elementCount;
    std::istream & stream = *static_cast<std::istream*>(aDatasource);
    stream.read(static_cast<char*>(destination), lengthRead);
    const std::streamsize bytesRead = stream.gcount();

    if (stream.fail() && !stream.eof())
    {
        spdlog::get("grapito")->error("ERROR: Ogg stream has fail bit set");
    }
    else if (stream.bad())
    {
        spdlog::get("grapito")->error("ERROR: Ogg stream has bad bit set");
    }

    return static_cast<size_t>(bytesRead);
};

static int seekOggInputStreamCallback(void * aDatasource, ogg_int64_t offset, int origin)
{
    //Ok so the vorbis is shit honestly
    //From what I've gathered the the origin parameters in the callback tells us
    //from where we should seek into the stream
    //Unfortunately I cannot confirm that...
    static const std::vector<std::ios_base::seekdir> seekDirections{
        std::ios_base::beg,
        std::ios_base::cur,
        std::ios_base::end
    };
    std::istream & stream = *static_cast<std::istream*>(aDatasource);
    stream.seekg(offset, seekDirections.at(origin));

    if (stream.fail() && !stream.eof())
    {
        spdlog::get("grapito")->error("ERROR: Ogg stream has fail bit set");
    }
    else if (stream.bad())
    {
        spdlog::get("grapito")->error("ERROR: Ogg stream has bad bit set");
    }

    return 0;
};

static long tellOggInputStreamCallback(void * aDatasource)
{
    std::istream & stream = *static_cast<std::istream*>(aDatasource);
    const std::istream::pos_type position = stream.tellg();
    assert(position >= 0); //Position cannot be 0 or less because it should point to the next data to be read
    return static_cast<long>(position);
};

static ov_callbacks OggVorbisCallbacks{
    readOggInputStreamCallback,
    seekOggInputStreamCallback,
    nullptr, //We do not close istreams
    tellOggInputStreamCallback
};

OggSoundData SoundManager::loadOggFile(std::istream & aInputStream, bool streamed)
{
    OggVorbis_File oggFile;
    OggSoundData resultSoundData;

    //Get ogg vorbis file format from the file
    int result = ov_open_callbacks(&aInputStream, &oggFile, nullptr, 0, OggVorbisCallbacks);
    if (result < 0)
    {
        spdlog::get("grapito")->error("Error opening ogg file: {}", result);
    }
    //Get the info to fill out the format and frequency of the audio file
    vorbis_info * fileInfo = ov_info(&oggFile, -1);

    //We can always assume 16 bits for ogg vorbis
    if (fileInfo->channels == 1)
    {
         resultSoundData.channels = AL_FORMAT_MONO16;
    }
    else
    {
        resultSoundData.channels = AL_FORMAT_STEREO16;
    }

    resultSoundData.sampleRate = fileInfo->rate;
    resultSoundData.bitsPerSample = 16;

    // size = #samples * #channels * 2 (for 16 bit).
    long sizeToRead = ov_pcm_total(&oggFile, -1) * fileInfo->channels * 2;
    char readBuffer[sizeToRead];
    int bitStream;
    std::vector<char> dataBuffer;

    constexpr int endian = 0; //0 for little endian 1 for big endian
    constexpr int wordSize = 2; //2 for 16 bits files
    constexpr int sgned = 1; //typically signed (0 is unsigned)

    if (streamed)
    {
        sizeToRead = StreamingBufferSize;
        resultSoundData.streamedData = true;
    }

    int sizeRead = 0;
    do
    {
        //read from the file a readBuffer size amount of data
        long result = ov_read(&oggFile, readBuffer, sizeToRead, endian, wordSize, sgned, &bitStream);
        if (result > 0)
        {
            sizeRead += result;
        }
        else if (result < 0)
        {
            spdlog::get("grapito")->error("ERROR: Can read from file");
        }
        else
        {
            spdlog::get("grapito")->warn("WARNING: File is not of the size we thought it was");
            break;
        }
        //Add to the result dataBuffer the data that was read
        dataBuffer.insert(dataBuffer.end(), readBuffer, readBuffer + result);
    } while (sizeRead < sizeToRead);

    if (sizeRead == 0)
    {
        spdlog::get("grapito")->error("ERROR: did not read any data from ogg file");
    }

    ov_clear(&oggFile);

    ALuint buffer;
    alCall(alGenBuffers, 1, &buffer);
    alCall(alBufferData, buffer, resultSoundData.channels, dataBuffer.data(), dataBuffer.size(), resultSoundData.sampleRate);

    resultSoundData.buffers.push_back(buffer);

    return resultSoundData;
}

ALuint SoundManager::playSound(OggSoundData & aSoundData)
{
    ALuint source;
    alCall(alGenSources, 1, &source);
    alCall(alSourcef, source, AL_PITCH, 1);

    alCall(alSourcef, source, AL_GAIN, aSoundData.gain);
    alCall(alSource3f, source, AL_POSITION, aSoundData.position.x(), aSoundData.position.y(), aSoundData.position.z());
    alCall(alSource3f, source, AL_VELOCITY, aSoundData.velocity.x(), aSoundData.velocity.y(), aSoundData.velocity.z());

    alCall(alSourcei, source, AL_LOOPING, aSoundData.looping);
    alCall(alSourcei, source, AL_BUFFER, aSoundData.buffers[0]);
    alCall(alSourcePlay, source);

    return source;
}

bool SoundManager::stopSound(ALuint aSource)
{
    return alCall(alSourceStop, aSource);
}

bool SoundManager::pauseSound(ALuint aSource)
{
    return alCall(alSourcePause, aSource);
}

OggSoundData & SoundManager::loadFromCacheOrFetch(FetchFile aFileToFetch)
{
    std::size_t hashId = std::hash<std::string>{}(aFileToFetch.filePath);
    auto soundIterator = soundCache.find(hashId);

    if (soundIterator == soundCache.end())
    {
        filesystem::path absFilePath = mResourceManager->pathFor(aFileToFetch.filePath);
        std::ifstream inputFileStream{absFilePath.c_str()};
        OggSoundData loadedData = loadOggFile(inputFileStream, aFileToFetch.streamed);

        auto iteratorPair = soundCache.insert(std::make_pair(hashId, loadedData));
        soundIterator = iteratorPair.first;
    }

    return soundIterator->second;
}

void SoundManager::prefetch(std::vector<FetchFile> aFileToFetchList)
{
    for (FetchFile file : aFileToFetchList)
    {
        loadFromCacheOrFetch(file);
    }
}
}
}
