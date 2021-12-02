#include "SoundUtilities.h"

#include <vorbis/vorbisfile.h>

namespace ad {
namespace grapito {

constexpr int BufferSize = 32768;

std::vector<char> loadOggFile(const std::string &aFilename, ALenum & outFormat, ALsizei & outFreq)
{
    FILE * f = std::fopen(aFilename.c_str(), "rb");
    vorbis_info * fileInfo;
    OggVorbis_File oggFile;

    //Get ogg vorbis file format from the file
    ov_open(f, &oggFile, nullptr, 0);
    //Get the info to fill out the format and frequency of the audio file
    fileInfo = ov_info(&oggFile, -1);

    if (fileInfo->channels == 1)
    {
        outFormat = AL_FORMAT_MONO16;
    }
    else
    {
        outFormat = AL_FORMAT_STEREO16;
    }

    outFreq = fileInfo->rate;

    long bytes;
    char readBuffer[BufferSize];
    int endian = 0; //0 for little endian 1 for big endian
    int bitStream;
    std::vector<char> dataBuffer;
    do
    {
        //read from the file a readBuffer size amount of data
        bytes = ov_read(&oggFile, readBuffer, BufferSize, endian, 2, 1, &bitStream);
        //Add to the result dataBuffer the data that was read
        dataBuffer.insert(dataBuffer.end(), readBuffer, readBuffer + bytes);
    } while (bytes > 0);

    ov_clear(&oggFile);

    return dataBuffer;
}

void playSound(std::vector<char> aAudioData, ALenum aFormat, ALsizei aFreq)
{
    ALuint buffer;
    alCall(alGenBuffers, 1, &buffer);
    alCall(alBufferData, buffer, aFormat, aAudioData.data(), aAudioData.size(), aFreq);

    ALuint source;
    alCall(alGenSources, 1, &source);
    alCall(alSourcef, source, AL_PITCH, 1);
    /*
     * possible option for the source
    alCall(alSourcef, source, AL_GAIN, 1.0f);
    alCall(alSource3f, source, AL_POSITION, 0, 0, 0);
    alCall(alSource3f, source, AL_VELOCITY, 0, 0, 0);
    */
    alCall(alSourcei, source, AL_LOOPING, AL_FALSE);
    alCall(alSourcei, source, AL_BUFFER, buffer);
    alCall(alSourcePlay, source);
}
}
}
