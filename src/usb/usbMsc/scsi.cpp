#include "scsi.h"
#include "mscbot.h"

const uint8_t  MSC_Page00_Inquiry_Data[] = {//7						
	0x00,		
	0x00, 
	0x00, 
	(LENGTH_INQUIRY_PAGE00 - 4),
	0x00, 
	0x80, 
	0x83 
};  
/* USB Mass storage sense 6  Data */
const uint8_t  MSC_Mode_Sense6_data[] = {
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00, 
	0x00,
	0x00
};	
/* USB Mass storage sense 10  Data */
const uint8_t  MSC_Mode_Sense10_data[] = {
	0x00,
	0x06, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00, 
	0x00
};

SCSI::SCSI() :
    msc(0L),
    m_sense_head(0), m_sense_tail(0)
{
  
}

int8_t SCSI::processCmd(uint8_t lun, uint8_t *params)
{
    switch (params[0])
    {
      case SCSI_TEST_UNIT_READY:
        return testUnitReady(lun, params);

      case SCSI_REQUEST_SENSE:
        return requestSense(lun, params);
        
      case SCSI_INQUIRY:
        return inquiry(lun, params);

      case SCSI_START_STOP_UNIT:
        return startStopUnit(lun, params);

      case SCSI_ALLOW_MEDIUM_REMOVAL:
        return startStopUnit(lun, params);

      case SCSI_MODE_SENSE6:
        return modeSense6 (lun, params);

      case SCSI_MODE_SENSE10:
        return modeSense10 (lun, params);

      case SCSI_READ_FORMAT_CAPACITIES:
        return readFormatCapacity(lun, params);

      case SCSI_READ_CAPACITY10:
        return readCapacity10(lun, params);

      case SCSI_READ10:
        return read10(lun, params); 

      case SCSI_WRITE10:
        return write10(lun, params);

      case SCSI_VERIFY10:
        return verify10(lun, params);

      default:
        senseCode(lun, ILLEGAL_REQUEST, INVALID_CDB);    
        return -1;
    }
}

void SCSI::senseCode(uint8_t lun, uint8_t sKey, uint8_t ASC)
{
    m_sense[m_sense_tail].Skey  = sKey;
    m_sense[m_sense_tail].w.ASC = ASC << 8;
    m_sense_tail++;
    if (m_sense_tail == SENSE_LIST_DEPTH)
        m_sense_tail = 0;
}

int8_t SCSI::testUnitReady(uint8_t lun, uint8_t *params)
{
    if (msc->cbw.dDataLength != 0)
    {
        senseCode(msc->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
        return -1;
    }
    if (mLuns[lun]->isReady() != 0)
    {
        senseCode(lun, NOT_READY, MEDIUM_NOT_PRESENT);
        return -1;
    } 
    msc->dataLen = 0;
    return 0;
}

int8_t SCSI::inquiry(uint8_t lun, uint8_t *params)
{
    uint8_t* pPage;
    uint16_t len;

    if (params[1] & 0x01)/*Evpd is set*/
    {
        pPage = (uint8_t *)MSC_Page00_Inquiry_Data;
        len = sizeof(MSC_Page00_Inquiry_Data);
    }
    else
    {
        pPage = mLuns[lun]->getInquiry();
        len = pPage[4] + 5;
        if (params[4] <= len)
            len = params[4];
    }
    msc->dataLen = len;

    while (len) 
    {
        len--;
        msc->data[len] = pPage[len];
    }
    return 0;
}

int8_t SCSI::readFormatCapacity(uint8_t lun, uint8_t *params)
{
    uint32_t blk_size;
    uint32_t blk_nbr;
    uint16_t i;

    for (i=0; i<12; i++) 
    {
        msc->data[i] = 0;
    }

    if (mLuns[lun]->getCapacity(&blk_nbr, &blk_size) != 0)
    {
        senseCode(lun, NOT_READY, MEDIUM_NOT_PRESENT);
        return -1;
    } 
    else
    {
        msc->data[3] = 0x08;
        msc->data[4] = (uint8_t)(blk_nbr - 1 >> 24);
        msc->data[5] = (uint8_t)(blk_nbr - 1 >> 16);
        msc->data[6] = (uint8_t)(blk_nbr - 1 >>  8);
        msc->data[7] = (uint8_t)(blk_nbr - 1);

        msc->data[8] = 0x02;
        msc->data[9] = (uint8_t)(blk_size >>  16);
        msc->data[10] = (uint8_t)(blk_size >>  8);
        msc->data[11] = (uint8_t)(blk_size);

        msc->dataLen = 12;
        return 0;
    }
}

int8_t SCSI::readCapacity10(uint8_t lun, uint8_t *params)
{
    if (mLuns[lun]->getCapacity(&m_blk_nbr, &m_blk_size) != 0)
    {
        senseCode(lun, NOT_READY, MEDIUM_NOT_PRESENT);
        return -1;
    } 
    else
    {
        msc->data[0] = (uint8_t)(m_blk_nbr - 1 >> 24);
        msc->data[1] = (uint8_t)(m_blk_nbr - 1 >> 16);
        msc->data[2] = (uint8_t)(m_blk_nbr - 1 >>  8);
        msc->data[3] = (uint8_t)(m_blk_nbr - 1);
        msc->data[4] = (uint8_t)(m_blk_size >>  24);
        msc->data[5] = (uint8_t)(m_blk_size >>  16);
        msc->data[6] = (uint8_t)(m_blk_size >>  8);
        msc->data[7] = (uint8_t)(m_blk_size);
        msc->dataLen = 8;
        return 0;
    }
}

int8_t SCSI::requestSense(uint8_t lun, uint8_t *params)
{
    uint8_t i;
  
    for (i=0; i<REQUEST_SENSE_DATA_LEN; i++) 
    {
        msc->data[i] = 0;
    }

    msc->data[0]	= 0x70;		
    msc->data[7]	= REQUEST_SENSE_DATA_LEN - 6;	

    if ((m_sense_head != m_sense_tail))
    {
        msc->data[2]     = m_sense[m_sense_head].Skey;		
        msc->data[12]    = m_sense[m_sense_head].w.b.ASCQ;	
        msc->data[13]    = m_sense[m_sense_head].w.b.ASC;	
        m_sense_head++;
        if (m_sense_head == SENSE_LIST_DEPTH)
            m_sense_head = 0;
    }
    msc->dataLen = REQUEST_SENSE_DATA_LEN;  
    if (params[4] <= REQUEST_SENSE_DATA_LEN)
    {
        msc->dataLen = params[4];
    }
    return 0;
}

int8_t SCSI::startStopUnit(uint8_t lun, uint8_t *params)
{
    msc->dataLen = 0;
    return 0;
}

int8_t SCSI::modeSense6(uint8_t lun, uint8_t *params)
{
    uint16_t len = 8;
    msc->dataLen = len;

    while (len) 
    {
        len--;
        msc->data[len] = MSC_Mode_Sense6_data[len];
    }
    return 0;
}

int8_t SCSI::modeSense10(uint8_t lun, uint8_t *params)
{
    uint16_t len = 8;
    msc->dataLen = len;

    while (len) 
    {
        len--;
        msc->data[len] = MSC_Mode_Sense10_data[len];
    }
    return 0;
}

int8_t SCSI::write10(uint8_t lun , uint8_t *params)
{
    if (msc->state == MscBot::botIdle)
    {
        /* case 8 : Hi <> Do */
        if ((msc->cbw.bmFlags & 0x80) == 0x80)
        {
            senseCode(msc->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
            return -1;
        }
        /* Check whether Media is ready */
        if (mLuns[lun]->isReady() !=0 )
        {
            senseCode(lun, NOT_READY, MEDIUM_NOT_PRESENT);
            return -1;
        } 
        /* Check If media is write-protected */
        if (mLuns[lun]->isWriteProtected() != 0)
        {
            senseCode(lun, NOT_READY, WRITE_PROTECTED);
            return -1;
        } 

        m_blk_addr = (params[2] << 24) | \
                     (params[3] << 16) | \
                     (params[4] <<  8) | \
                      params[5];
        m_blk_len = (params[7] <<  8) | \
                     params[8];  
        
        /* check if LBA address is in the right range */
        if  (checkAddressRange(lun, m_blk_addr, m_blk_len) < 0)
        {
            return -1; /* error */      
        }
        
        m_blk_addr *= m_blk_size;
        m_blk_len  *= m_blk_size;
        
        /* cases 3,11,13 : Hn,Ho <> D0 */
        if (msc->cbw.dDataLength != m_blk_len)
        {
            senseCode(msc->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
            return -1;
        }
        
        /* Prepare EP to receive first data packet */
        msc->state = MscBot::botDataOut;
        msc->prepareRx(msc->data, MIN(m_blk_len, MSC_MEDIA_PACKET));
        
//        DCD_EP_PrepareRx (cdev,
//                          MSC_OUT_EP,
//                          MSC_BOT_Data, 
//                          MIN (SCSI_blk_len, MSC_MEDIA_PACKET));  
    }
    else /* Write Process ongoing */
    {
        return processWrite(lun);
    }
    return 0;
}

int8_t SCSI::read10(uint8_t lun , uint8_t *params)
{
    if (msc->state == MscBot::botIdle)
    {
        /* case 10 : Ho <> Di */
        if ((msc->cbw.bmFlags & 0x80) != 0x80)
        {
            senseCode(msc->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
            return -1;
        }    
        if (mLuns[lun]->isReady() !=0)
        {
            senseCode(lun, NOT_READY, MEDIUM_NOT_PRESENT);
            return -1;
        } 

        m_blk_addr = (params[2] << 24) | \
                     (params[3] << 16) | \
                     (params[4] <<  8) | \
                      params[5];

        m_blk_len =  (params[7] <<  8) | \
                      params[8];  



        if(checkAddressRange(lun, m_blk_addr, m_blk_len) < 0)
        {
            return -1; /* error */
        }

        msc->state = MscBot::botDataIn;
        m_blk_addr *= m_blk_size;
        m_blk_len  *= m_blk_size;

        /* cases 4,5 : Hi <> Dn */
        if (msc->cbw.dDataLength != m_blk_len)
        {
            senseCode(msc->cbw.bLUN, ILLEGAL_REQUEST, INVALID_CDB);
            return -1;
        }
    }
    msc->dataLen = MSC_MEDIA_PACKET;  

    return processRead(lun);
}

int8_t SCSI::verify10(uint8_t lun, uint8_t *params)
{
    if ((params[1]& 0x02) == 0x02) 
    {
        senseCode(lun, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
        return -1; /* Error, Verify Mode Not supported*/
    }

    if(checkAddressRange(lun, m_blk_addr, m_blk_len) < 0)
    {
        return -1; /* error */      
    }
    msc->dataLen = 0;
    return 0;
}

int8_t SCSI::checkAddressRange (uint8_t lun, uint32_t blk_offset, uint16_t blk_nbr)
{
    if ((blk_offset + blk_nbr) > m_blk_nbr)
    {
        senseCode(lun, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
        return -1;
    }
    return 0;
}

int8_t SCSI::processRead(uint8_t lun)
{
    uint32_t len;
  
    len = MIN(m_blk_len, MSC_MEDIA_PACKET); 

    if (mLuns[lun]->read(msc->data, m_blk_addr / m_blk_size, len / m_blk_size) < 0)
    {
        senseCode(lun, HARDWARE_ERROR, UNRECOVERED_READ_ERROR);
        return -1; 
    }
    
    msc->doSendData(msc->data, len);
    
//    DCD_EP_Tx (cdev, 
//             MSC_IN_EP,
//             MSC_BOT_Data,
//             len);

    m_blk_addr   += len; 
    m_blk_len    -= len;  

    /* case 6 : Hi = Di */
    msc->csw.dDataResidue -= len;

    if (m_blk_len == 0)
    {
        msc->state = MscBot::botLastDataIn;
    }
    return 0;
}

int8_t SCSI::processWrite(uint8_t lun)
{
    uint32_t len;
  
    len = MIN(m_blk_len, MSC_MEDIA_PACKET); 

    if (mLuns[lun]->write(msc->data, m_blk_addr / m_blk_size, len / m_blk_size) < 0)
    {
        senseCode(lun, HARDWARE_ERROR, WRITE_FAULT);     
        return -1; 
    }

    m_blk_addr  += len; 
    m_blk_len   -= len; 

    /* case 12 : Ho = Do */
    msc->csw.dDataResidue -= len;

    if (m_blk_len == 0)
    {
        msc->sendCSW(MscBot::cswCmdPassed);
    }
    else
    {
        msc->prepareRx(msc->data, MIN(m_blk_len, MSC_MEDIA_PACKET));
//    /* Prapare EP to Receive next packet */
//    DCD_EP_PrepareRx (cdev,
//                      MSC_OUT_EP,
//                      MSC_BOT_Data, 
//                      MIN (SCSI_blk_len, MSC_MEDIA_PACKET)); 
    }

    return 0;
}
//---------------------------------------------------------------------------

void SCSI::appendDrive(ScsiInterface *iface)
{
    mLuns.push_back(iface);
    msc->setMaxLun(mLuns.size() - 1);
}