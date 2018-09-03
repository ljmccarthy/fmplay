#ifndef vgmplayer_hpp_included__
#define vgmplayer_hpp_included__

#include <string>
#include <vector>
#include "sample_formats.hpp"

class VgmPlayer
{
public:
    explicit VgmPlayer(const char *filename);
    bool play(stereo<int16_t> *out, uint32_t num_samples);

private:
    void process_command();

    std::vector<uint8_t> vgm_data;
    std::vector<uint8_t> data_bank;
    uint32_t vgm_offset;
    uint32_t vgm_end;
    uint32_t ym2612_clock;
    uint32_t play_pos;
    uint32_t wait_pending;
    uint32_t pcm_pos;
};

#endif // vgmplayer_hpp_included__
