#include "scsiinterface.h"

const ScsiInterface::Inquiry ScsiInterface::m_defInq =
{
    0x00, 0x80, 0x02, 0x02, sizeof(Inquiry)-5, 0x00, 0x00, 0x00,
    "SCSI",
    "Mass storage",
    '1', '.', '0', '0'
};

ScsiInterface::ScsiInterface() :
    inquiry(m_defInq)
{
}

void ScsiInterface::setVendor(const char *s)
{
    char *dst = inquiry.vendor;
    for (int i=0; i<8; i++)
      *dst++ = *s? *s++: '\0';
}

void ScsiInterface::setProduct(const char *s)
{
    char *dst = inquiry.product;
    for (int i=0; i<16 && *s; i++)
        *dst++ = *s? *s++: '\0';
}

void ScsiInterface::setRevision(const char *s)
{
    char *dst = inquiry.revision;
    for (int i=0; i<4 && *s; i++)
        *dst++ = *s? *s++: '\0';
}