#ifndef _SFS_H
#define _SFS_H

#include <stdint.h>
#include <string>

class Sfs
{
public:
    class Entry
    {
    public:
        const char *name() const {return reinterpret_cast<const char *>(this + 1);}
        const char *data() const {return reinterpret_cast<const char *>(this) + m_offset;}
        int size() const {return m_length;}
        const Entry *next() const {return m_next? reinterpret_cast<const Entry *>(reinterpret_cast<const char *>(this) + m_next): 0L;}
        
    private:
        uint8_t m_type; // 0x00=unknown, 0x01=file, 0x02=dir (not used in version 1.0)
        uint8_t m_version;
        uint16_t m_length;
        uint16_t m_offset; // relative address
        uint16_t m_next;
    };
  
    Sfs();
    const char *open(std::string name);
    const Entry *entry() const {return m_entry;}
  
private:    
    const Entry *m_head;
    const Entry *m_entry;
};

#endif