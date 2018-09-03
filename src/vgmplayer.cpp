#include <algorithm>
#include <stdexcept>
#include <string>
#include "fmplay.hpp"
#include "psg.hpp"
#include "vgmplayer.hpp"
#include "ym2612.hpp"

static uint32_t
parse_uint32_le(const uint8_t *data)
{
    return uint32_t(data[0])
         | uint32_t(data[1]) << 8
         | uint32_t(data[2]) << 16
         | uint32_t(data[3]) << 24;
}

static uint32_t
parse_uint16_le(const uint8_t *data)
{
    return uint16_t(data[0])
         | uint16_t(data[1]) << 8;
}

VgmPlayer::VgmPlayer(const char *filename)
    : vgm_data(read_file(filename))
{
    if (vgm_data.size() < 0x40) {
        throw std::runtime_error("invalid VGM file: file header too small");
    }
    if (!std::equal(&vgm_data[0], &vgm_data[4], "Vgm ")) {
        throw std::runtime_error("invalid VGM file: wrong file type");
    }
    uint32_t version = parse_uint32_le(&vgm_data[0x08]);
    if (version < 0x150) {
        vgm_offset = 0x40;
    }
    else {
        vgm_offset = 0x34 + parse_uint32_le(&vgm_data[0x34]);
    }
    if (vgm_offset >= vgm_data.size()) {
        throw std::runtime_error("invalid VGM file: data offset out of range");
    }
    ym2612_clock = parse_uint32_le(&vgm_data[0x2C]);
    vgm_end = vgm_data.size();
    play_pos = vgm_offset;
    wait_pending = 0;
    pcm_pos = 0;

#ifndef UNIT_TEST
    psg_init(PSG_DISCRETE);
    psg_reset();
    YM2612Init();
    YM2612Config(YM2612_ENHANCED);
    YM2612ResetChip();
#endif // UNIT_TEST
}

bool
VgmPlayer::play(stereo<int16_t> *out, stereo<int16_t> *end)
{
    while (true) {
        while (!wait_pending) {
            if (play_pos >= vgm_end) {
                std::fill(out, end, stereo<int16_t>{0, 0});
                return false;
            }
            process_command();
        }
        uint32_t wait_now = std::min<uint32_t>(wait_pending, end - out);
        if (wait_now) {
            YM2612Update(out, wait_now);
            out += wait_now;
            wait_pending -= wait_now;
        }
        if (wait_pending) {
            return true;
        }
    }
}

void
VgmPlayer::process_command()
{
    uint8_t cmd = vgm_data[play_pos++];
    switch (cmd) {
        case 0x4F: {
            uint8_t val = vgm_data[play_pos++];
            break;
        }
        case 0x50: {
            uint8_t val = vgm_data[play_pos++];
            break;
        }
        case 0x52: {
            uint8_t adr = vgm_data[play_pos++];
            uint8_t val = vgm_data[play_pos++];
            YM2612Write(0, adr);
            YM2612Write(1, val);
            break;
        }
        case 0x53: {
            uint8_t adr = vgm_data[play_pos++];
            uint8_t val = vgm_data[play_pos++];
            YM2612Write(2, adr);
            YM2612Write(3, val);
            break;
        }
        case 0x61: {
            wait_pending = parse_uint16_le(&vgm_data[play_pos]);
            play_pos += 2;
            break;
        }
        case 0x62:
            wait_pending = 735;
            break;
        case 0x63:
            wait_pending = 882;
            break;
        case 0x66:
            vgm_end = std::min(vgm_end, play_pos);
            break;
        case 0x67: {
            play_pos++;  // skip 0x66 byte
            uint8_t datatype = vgm_data[play_pos++];
            if (datatype != 0) {
                std::runtime_error("unrecognised data type in data block");
            }
            uint32_t size = parse_uint32_le(&vgm_data[play_pos]);
            play_pos += 4;
            data_bank.insert(data_bank.end(), &vgm_data[play_pos], &vgm_data[play_pos + size]);
            play_pos += size;
            break;
        }
        case 0xE0: {
            uint32_t seek_to = parse_uint32_le(&vgm_data[play_pos]);
            play_pos += 4;
            if (seek_to >= data_bank.size()) {
                throw std::runtime_error("PCM data bank seek out of range");
            }
            pcm_pos = seek_to;
            break;
        }
        default: {
            switch (cmd & 0xF0) {
                case 0x70:
                    wait_pending = (cmd & 0x0F) + 1;
                    break;
                case 0x80:
                    wait_pending = cmd & 0x0F;
                    if (pcm_pos >= data_bank.size()) {
                        throw std::runtime_error("PCM data bank seek out of range");
                    }
                    YM2612Write(0, 0x2A);
                    YM2612Write(1, data_bank[pcm_pos++]);
                    break;
                default:
                    fprintf(stderr, "unrecognised VGM command %02x\n", cmd);
                    throw std::runtime_error("unrecognised VGM command");
            }
        }
    }
}

#ifdef UNIT_TEST
#include <iostream>

int main()
{
    {
        VgmPlayer player("test.vgm");
    }
    std::cout << "OK" << std::endl;
    return 0;
}
#endif // UNIT_TEST
