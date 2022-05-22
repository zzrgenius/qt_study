#include "TinyFrame/TF_Config.h"
#include "TinyFrame/TinyFrame.h"
#include <QByteArray>
#include <QByteArrayList>
#include <QDataStream>
#include <QDebug>
#include <QSerialPort>
//
// TF callbacks
//
#if 0

static TF_Result tf_write_file_size_clbk(TinyFrame *tf, TF_Msg *msg) {
  if (msg->type == eCmdWriteFileSize) {
    if (comhdlc_get_instance()) {
      comhdlc_get_instance()->transfer_file_chunk();
    }

    return TF_CLOSE;
  }

  return TF_NEXT;
}
static TF_Result tf_write_file_clbk(TinyFrame *t f, TF_Msg *msg) {
  Q_UNUSED(tf);
  Q_ASSERT(msg != nullptr);

  if (msg->type == eCmdWriteFile) {
    if (comhdlc_get_instance()) {
      comhdlc_get_instance()->transfer_file_chunk();
    }

    qDebug() << "[INFO] File write callback TinyFrame";

    return TF_STAY;
  }

  return TF_NEXT;
}
static TF_Result tf_handshake_clbk(TinyFrame *tf, TF_Msg *msg) {
  Q_UNUSED(tf);
  Q_ASSERT(msg != nullptr);

  if (msg->type == eComHdlcAnswer_HandShake) {
    if (comhdlc_get_instance()) {
      comhdlc_get_instance()->handshake_routine_stop();
      emit comhdlc_get_instance()->device_connected(true);
    }

    return TF_CLOSE;
  }

  return TF_NEXT;
}
#endif

// extern "C" void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t
// len);

// void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len) {
//   Q_UNUSED(tf);
//   Q_ASSERT(buff != nullptr);

//  if (comhdlc_get_instance()) {
//    comhdlc_get_instance()->comport_send_buff(buff, len);
//  }
//}
