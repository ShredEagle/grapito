
#include "../commons.h"

#include <memory>
#include <resource/ResourceManager.h>

#include <al.h>
#include <vorbis/vorbisfile.h>

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ad {
namespace grapito {

struct OggSoundData
{
    std::vector<ALuint> buffers;
    int channels;
    long sampleRate;
    ALenum format;
    std::uint8_t bitsPerSample;
    OggVorbis_File oggFile;
    bool streamedData = false;
    Position3 position = Position3::Zero();
    Vec3 velocity = Vec3::Zero();
    float gain = 1.f;
    int looping = AL_FALSE;
};

struct FetchFile
{
    std::string filePath;
    bool streamed;
};

//There is three step to play sound
//First load the file into RAM
//Second load the audio data into audio memory
//Last play the sound
class SoundManager
{
    public:
        void prefetch(std::vector<FetchFile> aFileToFetchList);
        OggSoundData & loadFromCacheOrFetch(FetchFile aFileToFetch);
        OggSoundData loadOggFile(std::istream & aInputStream, bool streamed);
        ALuint playSound(OggSoundData & aSoundData);
        bool stopSound(ALuint aSource);
        bool pauseSound(ALuint aSource);

    private:
        std::unordered_map<std::size_t, OggSoundData> soundCache;
        std::shared_ptr<resource::ResourceManager> mResourceManager;
};

} // namespace grapito
} // namespace ad
