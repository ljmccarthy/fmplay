#include <iostream>
#include "sdl.hpp"
#include "vgmplayer.hpp"

static SDL_AudioDeviceID device;
static int retcode = 0;

static void
sdl_audio_callback(void *userdata, uint8_t *stream_bytes, int len)
{
    auto player = reinterpret_cast<VgmPlayer *>(userdata);
    auto buffer = reinterpret_cast<typename VgmPlayer::sample_t *>(stream_bytes);
    bool more;
    try {
        more = player->play(buffer, buffer + len / sizeof(VgmPlayer::sample_t));
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        more = false;
        retcode = 1;
    }
    if (!more) {
        SDL::AudioDeviceLock lock(device);
        SDL_PauseAudioDevice(device, 1);
    }
}

SDL_AudioDeviceID
open_audio_device(int freq, SDL_AudioCallback callback, void *userdata)
{
    SDL_AudioSpec desired = {
        .freq = freq,
        .format = AUDIO_S16SYS,
        .channels = 2,
        .samples = 4096,
        .callback = callback,
        .userdata = userdata,
    }, obtained = {};

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (device == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
        exit(1);
    }
    if (obtained.freq != freq) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to obtain audio device with correct frequency\n");
        exit(1);
    }
    return device;
}

static void
play_vgm(const char *filename)
{
    VgmPlayer player(filename);
    device = open_audio_device(44100, sdl_audio_callback, &player);
    SDL::AudioDeviceLock lock(device);
    SDL_PauseAudioDevice(device, 0);
    while (SDL_GetAudioDeviceStatus(device) == SDL_AUDIO_PLAYING) {
        SDL::AudioDeviceUnlock unlock(device);
        SDL_Delay(100);
    }
}

int
main(const int argc, const char *const *argv)
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " FILE.vgm" << std::endl;
        return 1;
    }
    SDL::SDL sdl(SDL_INIT_AUDIO);
    try {
        play_vgm(argv[1]);
        return retcode;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
}
