#include <SDL.h>
#include "util.hpp"

static constexpr uint32_t note_freqs[] = {
    169, 159, 150, 142, 134, 126, 119, 113, 106, 100, 95, 89,
};

class Synth : noncopyable {
public:
    void fill_audio_buffer(uint32_t *buffer, size_t len);
    static void sdl_audio_callback(void *userdata, Uint8 *stream_bytes, int len);

private:
    uint32_t count = 0;
    uint32_t sample = 0;
    int note = 0;
    int note_dir = 1;
};

void
Synth::fill_audio_buffer(uint32_t *buffer, size_t len)
{
    int nr_samples = len / 4;
    uint32_t note_freq = note_freqs[note];

    for (int i = 0; i < nr_samples; i++) {
        if (++count % note_freq == 0) {
            sample = sample ? 0 : UINT32_MAX;
        }
        if (count % 3000 == 0) {
            note += note_dir;
            if (note == 0 || note == array_size(note_freqs) - 1) {
                note_dir = -note_dir;
            }
            note_freq = note_freqs[note];
        }
        buffer[i] = sample;
    }
}

void
Synth::sdl_audio_callback(void *userdata, Uint8 *stream_bytes, int len)
{
    auto synth = reinterpret_cast<Synth *>(userdata);
    auto buffer = reinterpret_cast<uint32_t *>(stream_bytes);
    synth->fill_audio_buffer(buffer, len);
}

SDL_AudioDeviceID
open_audio_device(int freq, SDL_AudioCallback callback, void *userdata)
{
    SDL_AudioSpec desired = {
        freq: freq,
        format: AUDIO_F32,
        channels: 1,
        samples: 4096,
        callback: callback,
        userdata: userdata,
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

int
main(const int argc, const char *const *argv)
{
    SDL_Init(SDL_INIT_AUDIO);
    Synth synth;
    SDL_AudioDeviceID device = open_audio_device(44100, &Synth::sdl_audio_callback, &synth);
    SDL_PauseAudioDevice(device, 0);
    SDL_Delay(10000);
    SDL_CloseAudioDevice(device);
    SDL_Quit();
    return 0;
}
