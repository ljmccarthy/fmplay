#!/usr/bin/env python3
#
# Quick and dirty VGM parser

import sys
from construct import Struct, Embedded, Const, Int32ul, Int16ul, Byte, If, IfThenElse, FlagsEnum

VgmHeader = Struct(
    Const(b'Vgm '),
    'eof_offset'        / Int32ul,
    'version'           / Int32ul,
    'SN76489_clock'     / Int32ul,
    'YM2413_clock'      / Int32ul,
    'gd3_offset'        / Int32ul,
    'nr_samples'        / Int32ul,
    'loop_offset'       / Int32ul,
    'loop_nr_samples'   / Int32ul,
    'frame_rate'        / Int32ul,
    'SN76489_feedback'  / Int16ul,
    'SN76489_shift_reg' / Byte,
    'SN76489_flags'     /
        FlagsEnum(Byte,
            freq0_is_0x400 = 0x01,
            output_negate  = 0x02,
            stereo         = 0x04,
            clock_divider  = 0x08,
        ),
    'YM2612_clock'      / Int32ul,
    'YM2151_clock'      / Int32ul,
    'data_offset'       / Int32ul,
    'sega_pcm_clock'    / Int32ul,
    'sega_pcm_if_reg'   / Int32ul,
)

class VgmParser(object):
    def __init__(self, buf, pos):
        self._buf = buf
        self._pos = pos

    def has_more(self):
        return self._pos < len(self._buf)

    def read(self, size):
        i = self._pos
        j = i + size
        assert j <= len(self._buf)
        self._pos = j
        return self._buf[i:j]

    def byte(self):
        return Byte.parse(self.read(1))

    def uint16(self):
        return Int16ul.parse(self.read(2))

    def uint32(self):
        return Int32ul.parse(self.read(4))

    def parse_end(self):
        self._pos = len(self._buf)

    def parse_data_block(self):
        assert self.byte() == 0x66
        datatype = self.byte()
        size = self.uint32()
        data = self.read(size)
        print(f'Data block of size {size} bytes')

    def parse_ym2612_write_port0(self):
        address = self.byte()
        value = self.byte()
        print(f'YM2612.0: write {value:02x} -> {address:02x}')
        #print(f'YM2612Write(0, {hex(address)});')
        #print(f'YM2612Write(1, {hex(value)});')

    def parse_ym2612_write_port1(self):
        address = self.byte()
        value = self.byte()
        print(f'YM2612.1: write {value:02x} -> {address:02x}')
        #print(f'YM2612Write(2, {hex(address)});')
        #print(f'YM2612Write(3, {hex(value)});')

    def parse_psg_write_port_06(self):
        value = self.byte()
        print(f'PSG: write {value} -> port 0x06')

    def parse_psg_write(self):
        value = self.byte()
        print(f'PSG: write {value}')

    def wait(self, n):
        print(f'wait {n} samples')

    def parse_wait(self):
        self.wait(self.uint16())

    def parse_wait_60thsec(self):
        self.wait(735)

    def parse_wait_50thsec(self):
        self.wait(882)

    def parse_pcm_seek(self):
        seek_pos = self.uint32()
        #print(f'seek to {seek_pos} in PCM data bank')

    def ym2612_write_pcm(self, n):
        #print(f'YM2612.0: write data bank to DAC (0x2A)')
        self.wait(n)

    commands = {
        0x4F: parse_psg_write_port_06,
        0x50: parse_psg_write,
        0x52: parse_ym2612_write_port0,
        0x53: parse_ym2612_write_port1,
        0x61: parse_wait,
        0x62: parse_wait_60thsec,
        0x63: parse_wait_50thsec,
        0x66: parse_end,
        0x67: parse_data_block,
        0xE0: parse_pcm_seek,
    }

    def next(self):
        cmd = self.byte()
        if cmd in self.commands:
            self.commands[cmd](self)
        elif cmd & 0xF0 == 0x70:
            self.wait((cmd & 0x0F) + 1)
        elif cmd & 0xF0 == 0x80:
            self.ym2612_write_pcm(cmd & 0x0F)
        else:
            raise Exception(f'unknown command: {hex(cmd)}')

def read_vgm(filename):
    with open(filename, 'rb') as f:
        buf = f.read()
    hdr = VgmHeader.parse(buf)
    print(hdr)
    offset = 0x40 if hdr.version < 0x150 else hdr.data_offset + 0x34
    parser = VgmParser(buf, offset)
    while parser.has_more():
        parser.next()

def main():
    if len(sys.argv) != 2:
        sys.stderr.write('usage: parse-vgm.py FILE.vgm\n')
        sys.exit(1)
    else:
        read_vgm(sys.argv[1])

if __name__ == '__main__':
    main()
