#include "sequencer.h"

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
    
    do switch (c = *m_seq++)
    {
    case ' ': continue;
    
    case '\0':
    case 'H': // HALT
        stop();
        return;
        
//    case 'V': m_seq++; continue; // VOICE not implemented yet
        
    case 'O': // OCTAVE
        m_oct = readNum();
        break;
        
    case 'T':
        m_tempo = readNum();
        break;
    
//    case 'U': m_seq++; continue; // VOLUME no implemented yet
    
    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
        --m_seq;
        m_dur = readNum(); // numerator
        if (testNext('/'))
            m_dur /= readNum(); // denominator
        break;
        
    case '/':
        m_dur = 1.f / readNum();
        break;        
    
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    {
        float f = m_noteFreq[c - 'A'];
        float d = 240.f / m_tempo * m_dur;
        
        // apply octave
        if (*m_seq >= '0' && *m_seq <= '9')
            f *= 1 << readNum();
        else
            f *= 1 << m_oct;
        
        // apply halftone
        if (testNext('#')) // sharp
            f *= 1.0595f;
        else if (testNext('b')) // flat
            f *= 0.94383446f;
        
        // half a duration longer
        if (testNext('.'))
            d *= 1.5f;
        
        playToneFunc(f, d * 0.9f); // duration is truncated
        wait(d);
    } return;
    
    case '&': // pause
        wait(240.f / m_tempo * m_dur);
        return;
        
    case '(':
        m_stack.push_back(m_seq);
        break;
        
    case ')':
    {
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
    } break;
    }
    while (*m_seq);
}

int Sequencer::readNum()
{
    int num = *m_seq++ - '0';
    while (*m_seq >= '0' && *m_seq <= '9')
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

void Sequencer::wait(float dur)
{
    m_timer.start(static_cast<int>(dur * 1000));
}
