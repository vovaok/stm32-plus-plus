#include "sfs.h"

extern uint8_t sfs_data[];

Sfs::Sfs()
{
    m_head = reinterpret_cast<Entry*>(sfs_data);
    m_entry = 0L;
}

const char *Sfs::open(std::string name)
{
    for (m_entry = m_head; m_entry; m_entry = m_entry->next())
    {
        std::string ename = m_entry->name();
        if (name == ename)
            return m_entry->data();
    }
    return 0L;
}