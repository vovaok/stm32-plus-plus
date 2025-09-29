//#include "canopenlite.h"
//
//CanOpenLite::CanOpenLite(CanInterface *can, uint8_t address) :
//    m_can(can),
//    m_address(address)
//{
//    writeSDOEnd = 0L;
//    memset(&m_seg, 0, sizeof(Segment));
//    for (int i=0; i<8; i++)
//        m_can->addFilter(0x600 | address, 0x07F);
//    m_can->open();
//}
//
//void CanOpenLite::loop()
//{
//    Can::Message msgRx, msgTx;
//    
//    if (m_can->receive(msgRx))
//    {
//        uint16_t cobid = msgRx.id & 0x780;
////        uint8_t addr = msgRx.id & 0x7F;
//        
//        if (cobid == SDO_Request)
//        {
//            msgTx.id = SDO_Response | m_address;
//            msgTx.size = 8;
//            for (int i=0; i<8; i++)
//                msgTx.data[i] = 0;
//            SDO &sdoTx = *reinterpret_cast<SDO*>(msgTx.data);
//            const SDO &sdo = *reinterpret_cast<const SDO*>(msgRx.data);
//            
//            if (sdo.ccs == ccsInitiateDownload)
//            {
//                sdoTx.ccs = scsInitiateDownload;
//                sdoTx.id = sdo.id;
//                sdoTx.subid = sdo.subid;
//                if (sdo.e) // expedited
//                {
//                    bool r = writeSDO(sdo.id, sdo.subid, sdo.data, 4 - sdo.n, 0);
//                    if (!r)
//                    {
//                        sdoTx.ccs = ccsAbortTransfer;
//                        sdoTx.value = 0x06020000; // Object does not exist in the object dictionary.
//                    }
//                }
//                else // segmented
//                {
//                    bool r = writeSDO(sdo.id, sdo.subid, 0L, 0, 0);
//                    if (r)
//                    {
//                        m_seg.id = sdo.id;
//                        m_seg.subid = sdo.subid;
//                        m_seg.size = sdo.value;
//                        m_seg.offset = 0;
//                    }
//                    else
//                    {
//                        sdoTx.ccs = ccsAbortTransfer;
//                        sdoTx.value = 0x06020000; // Object does not exist in the object dictionary.
//                    }
//                }
//            }
//            else if (sdo.ccs == ccsInitiateUpload)
//            {
//                sdoTx.ccs = scsInitiateUpload;
//                sdoTx.id = sdo.id;
//                sdoTx.subid = sdo.subid;
//                uint8_t *ptr;
//                uint32_t size;
//                bool r = readSDO(sdo.id, sdo.subid, &ptr, &size, 0);
//                if (r)
//                {
//                    if (size <= 4) // expedited
//                    {
//                        sdoTx.e = 1;
//                        sdoTx.s = 1;
//                        sdoTx.n = 4 - size;
//                        for (int i=0; i<size; i++)
//                            sdoTx.data[i] = ptr[i];
//                    }
//                    else // segmented
//                    {
//                        //! @todo
//                        sdoTx.s = 1;
//                        sdoTx.value = size;
//                        m_seg.id = sdo.id;
//                        m_seg.subid = sdo.subid;
//                        m_seg.data = ptr;
//                        m_seg.size = size;
//                        m_seg.offset = 0;
//                    }
//                }
//                else
//                {
//                    sdoTx.ccs = ccsAbortTransfer;
//                    sdoTx.value = 0x06020000; // Object does not exist in the object dictionary.
//                }
//            }
//            else if (sdo.ccs == ccsSegmentDownload)
//            {
//                if (m_seg.size)
//                {
//                    const SDO_seg &sdoRx = *reinterpret_cast<const SDO_seg*>(msgRx.data);
//                    SDO_seg &sdoTx = *reinterpret_cast<SDO_seg*>(msgTx.data);
//                    sdoTx.ccs = scsSegmentDownload;
//                    sdoTx.t = sdoRx.t;
//                    uint8_t size = 7 - sdoRx.n;
//                    writeSDO(m_seg.id, m_seg.subid, sdoRx.data, size, m_seg.offset);
//                    m_seg.offset += size;
//                    if (m_seg.offset >= m_seg.size)
//                    {
//                        // the endec
//                        if (sdoRx.c)
//                        {
//                            // this is real endec, it's OK
//                            if (writeSDOEnd)
//                                writeSDOEnd(m_seg.id, m_seg.subid, m_seg.offset);
//                        }
//                        m_seg.size = 0;
//                    }   
//                }
//                else
//                {
//                    sdoTx.ccs = ccsAbortTransfer;
//                    sdoTx.value = 0x06020000; // Object does not exist in the object dictionary.
//                }
//            }
//            else if (sdo.ccs == ccsSegmentUpload)
//            {
//                const SDO_seg &sdoRx = *reinterpret_cast<const SDO_seg*>(msgRx.data);
//                SDO_seg &sdoTx = *reinterpret_cast<SDO_seg*>(msgTx.data);
//                sdoTx.ccs = scsSegmentUpload;
//                uint32_t size = m_seg.size - m_seg.offset;
//                if (size > 7)
//                {
//                    size = 7;
//                }
//                else
//                {
//                    sdoTx.n = 7 - size;
//                    sdoTx.c = 1;
//                }
//                sdoTx.t = sdoRx.t;
//                for (int i=0; i<size; i++)
//                    sdoTx.data[i] = m_seg.data[m_seg.offset++];
//            }
//            else if (sdo.ccs == ccsAbortTransfer)
//            {
//                m_seg.data = 0;
//                m_seg.size = 0;
//                m_seg.offset = 0;
//                //! @todo some error handling
//            }
//            
//            m_can->send(msgTx);
//        }
//    }
//}
