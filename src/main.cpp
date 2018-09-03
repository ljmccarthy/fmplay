#include <iostream>
#include <SDL.h>
#include "vgmplayer.hpp"

static SDL_AudioDeviceID device;

static void
sdl_audio_callback(void *userdata, uint8_t *stream_bytes, int len)
{
    auto player = reinterpret_cast<VgmPlayer *>(userdata);
    auto buffer = reinterpret_cast<stereo<int16_t> *>(stream_bytes);
    bool more;
    try {
        more = player->play(buffer, buffer + len / 4);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        more = false;
    }
    if (!more) {
        SDL_LockAudioDevice(device);
        SDL_PauseAudioDevice(device, 1);
        SDL_UnlockAudioDevice(device);
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
    SDL_LockAudioDevice(device);
    SDL_PauseAudioDevice(device, 0);
    while (SDL_GetAudioDeviceStatus(device) == SDL_AUDIO_PLAYING) {
        SDL_UnlockAudioDevice(device);
        SDL_Delay(100);
        SDL_LockAudioDevice(device);
    }
}

int
main(const int argc, const char *const *argv)
{
    if (argc != 2) {
        return 1;
    }
    SDL_Init(SDL_INIT_AUDIO);
    play_vgm(argv[1]);
    SDL_Quit();
    return 0;
}
