#ifndef sdl_hpp_included__
#define sdl_hpp_included__

#include <SDL.h>

// RAII wrappers for SDL

namespace SDL {

class SDL {
public:
    explicit SDL(uint32_t flags)
        { SDL_Init(flags); }
    ~SDL()
        { SDL_Quit(); }

    SDL(const SDL&) = delete;
    SDL& operator=(const SDL&) = delete;
};

class AudioDeviceLock {
public:
    explicit AudioDeviceLock(SDL_AudioDeviceID device) : device(device)
        { SDL_LockAudioDevice(device); }

    ~AudioDeviceLock()
        { SDL_UnlockAudioDevice(device); }

    AudioDeviceLock(const AudioDeviceLock&) = delete;
    AudioDeviceLock& operator=(const AudioDeviceLock&) = delete;

private:
    const SDL_AudioDeviceID device;
};

class AudioDeviceUnlock {
public:
    explicit AudioDeviceUnlock(SDL_AudioDeviceID device) : device(device)
        { SDL_UnlockAudioDevice(device); }

    ~AudioDeviceUnlock()
        { SDL_LockAudioDevice(device); }

    AudioDeviceUnlock(const AudioDeviceUnlock&) = delete;
    AudioDeviceUnlock& operator=(const AudioDeviceUnlock&) = delete;

private:
    const SDL_AudioDeviceID device;
};

} // namespace SDL

#endif // sdl_hpp_included__
