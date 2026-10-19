#include "sequencer.h"
#include <math.h>

float Sequencer::m_noteFreq[7] =
{
//    A        B        C        D        E        F        G      
    27.500f, 30.868f, 16.352f, 18.354f, 20.602f, 21.827f, 24.500f
};

Sequencer::Sequencer()
{
    m_timer.onTimeout = EVENT(&Sequencer::parseNext);
    m_timer.setSingleShot(true);
}

void Sequencer::play(ByteArray &&seq)
{
    stop();
    
    m_buffer = std::move(seq);
    m_seq = m_buffer.data();
    m_stack.clear();
//    m_index = -1;
    parseNext();
}

void Sequencer::stop()
{
//    m_index = -1;
    m_stack.clear();
    m_timer.stop();
}

void Sequencer::parseNext()
{
    if (!m_seq)
        return;
    
    char c;
    float f = 0;   // current frequency
    float len = 0; // current length
    
    do switch (c = toupper(*m_seq++))
    {
    case ' ': continue;
    
    case '\0':
    case 'H': // HALT
        stop();
        return;
        
    case 'O': // OCTAVE
        m_oct = readNum();
        break;
        
    case '<': // octave down
        --m_oct;
        break;
        
    case '>': // octave up
        ++m_oct;
        break;
        
    case 'T':
        m_tempo = readNum();
        break;
        
    case 'U': // VOLUME no implemented yet
        if (nextIsDigit())
            readNum();
        break;
        
    case 'V': // VOICE not implemented yet
        if (nextIsDigit())
            readNum();
        break;
    
    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
        --m_seq;
        len = readNum(); // numerator
        if (testNext('/'))
            m_len = len/readNum(); // denominator
        break;
        
    case '/':
    case 'L':
        m_len = 1.f / readNum();
        break; 
        
    case 'M': // legato/normal/staccato
        switch (*m_seq++)
        {
        case 'L': case 'l': m_dur = 1.f; break;
        case 'N': case 'n': m_dur = 7.f/8.f; break;
        case 'S': case 's': m_dur = 3.f/4.f; break;
        }
        break;
        
    case '(':
        m_stack.push_back(m_seq);
        break;
        
    case ')':
        if (m_stack.empty())
        {
            m_seq = m_buffer.data();
        }
        else
        {
            const char *seq = m_stack.back();
            m_stack.back() = m_seq;
            if (m_seq == seq)
                m_stack.pop_back();
            m_seq = seq;
        }
        break;
    
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': // plain note
        // obtain frequency
        f = m_noteFreq[c - 'A'] * (1 << m_oct);
        
        // apply halftone
        if (testNext('#') || testNext('+')) // sharp
            f *= 1.0595f;
        else if (testNext('b') || testNext('-')) // flat
            f *= 0.94383446f;
        // fallthrough
    
    case 'P': case 'R': case '&': // pause
        // default length
        len = m_len;
        
        // ovveride length
        if (nextIsDigit())
            len = 1.f / readNum();
        
        // half a length longer
        while (testNext('.'))
            len *= 1.5f;
        // fallthrough
    
    case 'N': // play arbitrary note
        if (c == 'N' && nextIsDigit())
        {
            int num = readNum();
            f = num? 16.352f * powf(1.0594631f, num - 1): 0;
        }
        
        float d = 240.f / m_tempo * len;
        if (f)
            playToneFunc(f, d * m_dur);        
        // wait for note is being played
        wait(d);
        return;
    }
    while (*m_seq);
}

int Sequencer::readNum()
{
    int num = 0;
    while (nextIsDigit())
        num = num * 10 + (*m_seq++ - '0');
    return num;
}

bool Sequencer::testNext(char c)
{
    if (*m_seq != c)
        return false;    
    m_seq++;
    return true;
}
                     
bool Sequencer::nextIsDigit()
{
    return *m_seq >= '0' && *m_seq <= '9';
}

void Sequencer::wait(float dur)
{
    m_timer.start(static_cast<int>(dur * 1000));
}
