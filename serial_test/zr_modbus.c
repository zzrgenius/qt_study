
#include "zr_modbus.h"
 quint16 calcrc(char *buf, int len)
{
    quint16 crc     = 0xffff;
    quint16 crcpoly = 0xa001;

    for (int i = 0; i < len; i++) {
        quint8 x = buf[i];
        crc ^= x;
        for (int k = 0; k < 8; k++) {
            quint16 usepoly = crc & 0x1;
            crc >>= 1;
            if (usepoly)
                crc ^= crcpoly;
        }
    }

    return crc;
}
