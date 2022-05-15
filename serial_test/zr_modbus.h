#ifndef ZR_MODBUS_H
#define ZR_MODBUS_H
#include <QByteArray>
#include <QtGlobal>
quint16 crc16ForModbus(const QByteArray &data);
#if 0
#ifdef __cplusplus
extern "C" {
#endif
quint16 calcrc(const char *buf, int len);
#ifdef __cplusplus
}
#endif
#endif
#endif // ZR_MODBUS_H
