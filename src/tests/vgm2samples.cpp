#include <fstream>
#include <iostream>
#include "vgmplayer.hpp"
#include "util.hpp"

static void
vgm2stream(const char *infile, const char *outfile)
{
    VgmPlayer player(infile);
    std::vector<VgmPlayer::sample_t> buf(4096);
    std::ofstream out(outfile, std::ofstream::binary);

    while (player.play(&buf[0], &buf[buf.size()])) {
        for (auto sample: buf) {
            char sample_bytes[] = {
                static_cast<char>(sample.l & 0xFF),
                static_cast<char>(sample.l >> 8 & 0xFF),
                static_cast<char>(sample.r & 0xFF),
                static_cast<char>(sample.r >> 8 & 0xFF),
            };
            out.write(sample_bytes, array_size(sample_bytes));
        }
    }
}

int
main(const int argc, const char *const *argv)
{
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " MUSIC.vgm SAMPLES.bin" << std::endl;
        return 1;
    }
    try {
        vgm2stream(argv[1], argv[2]);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
}
