#!/usr/bin/env python3
#
# Music scale frequency generator
# Luke McCarthy 2018-08-29
#
# References:
#   https://pages.mtu.edu/~suits/NoteFreqCalcs.html
#   https://pages.mtu.edu/~suits/notefreqs.html
#   https://en.wikipedia.org/wiki/A440_(pitch_standard)

base_pitch = 440        # A4 = 440Hz (concert pitch)
sample_rate = 44100     # CD quality sample rate

octave_notes = ['C', 'C♯', 'D', 'D♯', 'E', 'F', 'F♯', 'G', 'G♯', 'A', 'A♯', 'B']

def octave_note_freqs(octave):
    r = (octave - 4) * 12 - 9  # Half step difference of this octave's C to A4
    a = 2**(1/12.)             # The twelth root of 2 (magic number)
    return [(note, base_pitch * a**(i+r)) for i, note in enumerate(octave_notes)]

def print_octave_freqs(octave):
    for note, freq in octave_note_freqs(octave):
        ss = int(round(sample_rate / freq))
        print(f'{note:>5}{octave} = {freq:.2f} Hz (~ {ss} samples/sec)')

if __name__ == '__main__':
    for octave in range(3, 6):
        print_octave_freqs(octave)
