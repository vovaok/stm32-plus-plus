#pragma once

#include "core/timer.h"
#include "core/bytearray.h"

class Sequencer
{
public:
    Sequencer();
    
    void play(ByteArray &&seq);
    void stop();
    
    std::function<void(float, float)> playToneFunc; // (freq_Hz, dur_s)
    
private:
    ByteArray m_buffer;
    const char *m_seq = nullptr;
//    ByteArray m_seq;
//    int m_index = -1;
    Timer m_timer;
    
    std::vector<const char *> m_stack;
    float m_tempo = 120;
    int m_oct = 4; // the 1st octave
    float m_dur = .25f; // 1/4
    
    static float m_noteFreq[7];
    
    void parseNext();
    int readNum();
    bool testNext(char c);
    void wait(float dur);
};