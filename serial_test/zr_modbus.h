#ifndef ZR_MODBUS_H
#define ZR_MODBUS_H
#include <QtGlobal>

#ifdef __cplusplus
extern "C" {
#endif
quint16 calcrc(char *buf, int len);

#ifdef __cplusplus
}
#endif
#endif  // ZR_MODBUS_H
