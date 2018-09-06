#ifndef sdl_hpp_included__
#define sdl_hpp_included__

#include <SDL.h>

// RAII wrappers for SDL

class SDL {
public:
    explicit SDL(uint32_t flags) { SDL_Init(flags); }
    ~SDL() { SDL_Quit(); }
    SDL(const SDL&) = delete;
    SDL& operator=(const SDL&) = delete;
};

#endif // sdl_hpp_included__
