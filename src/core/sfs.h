#ifndef _SFS_H
#define _SFS_H

#include <stdint.h>
#include <core/bytearray.h>

class Sfs
{
public:    
    class Entry // version 2.0
    {
    public:
        const char *name() const {return reinterpret_cast<const char *>(this + 1);}
        const char *data() const {return reinterpret_cast<const char *>(this) + m_offset;}
        int size() const {return m_length;}
        const Entry *next() const {return m_flags? reinterpret_cast<const Entry *>(reinterpret_cast<const char *>(this) + m_offset + m_length): nullptr;}

    private:
        uint8_t m_type: 4; // 0x00=unknown, 0x01=file, 0x02=dir (not used in version 2.0)
        uint8_t m_flags: 4; // 0x01=has next
        uint8_t m_version;
        uint16_t m_offset; // relative address
        uint32_t m_length;
    };
    
//    class Entry // version 1.0
//    {
//    public:
//        const char *name() const {return reinterpret_cast<const char *>(this + 1);}
//        const char *data() const {return reinterpret_cast<const char *>(this) + m_offset;}
//        int size() const {return m_length;}
//        const Entry *next() const {return m_next? reinterpret_cast<const Entry *>(reinterpret_cast<const char *>(this) + m_next): nullptr;}
//
//    private:
//        uint8_t m_type; // 0x00=unknown, 0x01=file, 0x02=dir (not used in version 1.0)
//        uint8_t m_version;
//        uint16_t m_length;
//        uint16_t m_offset; // relative address
//        uint16_t m_next;
//    };

    Sfs();
    const char *open(const ByteArray &name);
    const Entry *entry() const {return m_entry;}
    const char *data() const {return m_entry? m_entry->data(): nullptr;}
    int size() const {return m_entry? m_entry->size(): 0;}

private:
    const Entry *m_head = nullptr;
    const Entry *m_entry = nullptr;
};

#endif