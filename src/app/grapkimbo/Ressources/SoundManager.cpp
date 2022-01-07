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
    std::streamsize lengthRead = elementSize * elementCount;
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
    if (stream.fail() && stream.eof())
    {
        spdlog::get("grapito")->info("INFO: file read reach EOF");
    }

    return static_cast<size_t>(bytesRead);
};

static int seekOggInputStreamCallback(void * aDatasource, ogg_int64_t offset, int origin)
{
    return -1;
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

OggSoundData loadOggFileFromPath(const filesystem::path & aPath, bool streamed)
{
    std::ifstream soundStream{aPath.string(),std::ios::binary};
    StringId soundStringId{aPath.stem().string()};
    return loadOggFile(soundStream, soundStringId, streamed);
}

OggSoundData loadOggFile(std::istream & aInputStream, StringId aSoundId, bool streamed)
{
    OggVorbis_File oggFile;
    OggSoundData resultSoundData{.soundId = aSoundId};

    //Get ogg vorbis file format from the file
    //std::cout << aInputStream.rdbuf();
    int result = ov_open_callbacks(&aInputStream, &oggFile, nullptr, 0, OggVorbisCallbacks);
    if (result == OV_EREAD)
    {
        spdlog::get("grapito")->error("A read from the media returned an error");
    }
    if (result == OV_ENOTVORBIS)
    {
        spdlog::get("grapito")->error("File is not a vorbis file");
    }
    else if (result < 0)
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
    char readBuffer[2048];
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
        long result = ov_read(&oggFile, readBuffer, 2048, endian, wordSize, sgned, &bitStream);
        if (result > 0)
        {
            sizeRead += result;
            //Add to the result dataBuffer the data that was read
            dataBuffer.insert(dataBuffer.end(), readBuffer, readBuffer + result);
        }
        else if (result == OV_HOLE)
        {
            spdlog::get("grapito")->warn("WARNING: Page corrupted or missing");
        }
        else if (result == OV_EBADLINK)
        {
            spdlog::get("grapito")->error("ERROR: invalid stream section");
        }
        else if (result == OV_EINVAL)
        {
            spdlog::get("grapito")->error("ERROR: corrupted file header or can't open file");
        }
        else
        {
            //We reached the end of the file
            break;
        }
    } while (true);

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

ALuint SoundManager::playSound(StringId & aSoundId, ALboolean looping)
{
    const OggSoundData & soundData = mLoadedSoundList.at(aSoundId);
    ALuint source;
    alCall(alGenSources, 1, &source);
    alCall(alSourcef, source, AL_PITCH, 1);

    alCall(alSourcef, source, AL_GAIN, soundData.gain);
    alCall(alSource3f, source, AL_POSITION, soundData.position.x(), soundData.position.y(), soundData.position.z());
    alCall(alSource3f, source, AL_VELOCITY, soundData.velocity.x(), soundData.velocity.y(), soundData.velocity.z());

    alCall(alSourcei, source, AL_LOOPING, looping);
    alCall(alSourcei, source, AL_BUFFER, soundData.buffers[0]);
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

void SoundManager::storeDataInLoadedSound(const OggSoundData & aSoundData)
{
    mLoadedSoundList.insert({aSoundData.soundId, aSoundData});
}

}
}