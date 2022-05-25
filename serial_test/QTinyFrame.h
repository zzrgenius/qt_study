#ifndef QTINYFRAME_H
#define QTINYFRAME_H

#include "TinyFrame/TinyFrame.h"
#include <QByteArray>
#include <QIODevice>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QTimer>

#include <QSerialPort>
#include <QSerialPortInfo>

class QTinyFrame : public QObject {
  Q_OBJECT
public:
  explicit QTinyFrame(QObject *parent = nullptr);
  ~QTinyFrame();
  void setConnection(QIODevice *connection);
  TinyFrame *demo_tf = nullptr;
  QSerialPort *serial_port;

  void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len);
  /** Claim the TX interface before composing and sending a frame */
  bool TF_ClaimTx(TinyFrame *tf);
  //  {    // take mutex    return true; // we succeeded  }

  /** Free the TX interface after composing and sending a frame */
  void TF_ReleaseTx(TinyFrame *tf);
  //{    // release mutex  }

  /**
   * Accept incoming bytes & parse frames
   *
   * @param tf - instance
   * @param buffer - byte buffer to process
   * @param count - nr of bytes in the buffer
   */
  void TF_Accept(TinyFrame *tf, const uint8_t *buffer, uint32_t count);
  /**
   * Initialize the TinyFrame engine.
   * This can also be used to completely reset it (removing all listeners
   * etc).
   *
   * The field .userdata (or .usertag) can be used to identify different
   * instances in the TF_WriteImpl() function etc. Set this field after the
   * init.
   *
   * This function is a wrapper around TF_InitStatic that calls malloc() to
   * obtain the instance.
   *
   * @param tf - instance
   * @param peer_bit - peer bit to use for self
   * @return TF instance or NULL
   */
  TinyFrame *TF_Init(TF_Peer peer_bit);

  /**
   * Initialize the TinyFrame engine using a statically allocated instance
   * struct.
   *
   * The .userdata / .usertag field is preserved when TF_InitStatic is called.
   *
   * @param tf - instance
   * @param peer_bit - peer bit to use for self
   * @return success
   */
  bool TF_InitStatic(TinyFrame *tf, TF_Peer peer_bit);

  /**
   * De-init the dynamically allocated TF instance
   *
   * @param tf - instance
   */
  void TF_DeInit(TinyFrame *tf);

  /**
   * Accept a single incoming byte
   *
   * @param tf - instance
   * @param c - a received char
   */
  void TF_AcceptChar(TinyFrame *tf, uint8_t c);

signals:
private slots:
  //  void dataReady();

private:
  QByteArray buffer;
  QMutex frame_mutex;
  QTimer *timer_handshake = nullptr;
  QTimer *timer_tf = nullptr;

  /** Initialize a checksum */
  TF_CKSUM TF_CksumStart(void);

  /** Update a checksum with a byte */
  TF_CKSUM TF_CksumAdd(TF_CKSUM cksum, uint8_t byte);

  /** Finalize the checksum calculation */
  TF_CKSUM TF_CksumEnd(TF_CKSUM cksum);

  void TF_SendFrame_End(TinyFrame *tf);
  bool TF_SendFrame(TinyFrame *tf, TF_Msg *msg, TF_Listener listener,
                    TF_Listener_Timeout ftimeout, TF_TICKS timeout);
  inline uint32_t TF_ComposeTail(uint8_t *outbuff, TF_CKSUM *cksum);

  // ---------------------------------- INIT ------------------------------

  // ---------------------------------- API CALLS
  // --------------------------------------

  /**
   * This function should be called periodically.
   * The time base is used to time-out partial frames in the parser and
   * automatically reset it.
   * It's also used to expire ID listeners if a timeout is set when registering
   * them.
   *
   * A common place to call this from is the SysTick handler.
   *
   * @param tf - instance
   */
  void TF_Tick(TinyFrame *tf);

  /**
   * Reset the frame parser state machine.
   * This does not affect registered listeners.
   *
   * @param tf - instance
   */
  void TF_ResetParser(TinyFrame *tf);

  // ---------------------------- MESSAGE LISTENERS
  // -------------------------------

  /**
   * Register a frame type listener.
   *
   * @param tf - instance
   * @param msg - message (contains frame_id and userdata)
   * @param cb - callback
   * @param ftimeout - time out callback
   * @param timeout - timeout in ticks to auto-remove the listener (0 = keep
   * forever)
   * @return slot index (for removing), or TF_ERROR (-1)
   */
  bool TF_AddIdListener(TinyFrame *tf, TF_Msg *msg, TF_Listener cb,
                        TF_Listener_Timeout ftimeout, TF_TICKS timeout);

  /**
   * Remove a listener by the message ID it's registered for
   *
   * @param tf - instance
   * @param frame_id - the frame we're listening for
   */
  bool TF_RemoveIdListener(TinyFrame *tf, TF_ID frame_id);

  /**
   * Register a frame type listener.
   *
   * @param tf - instance
   * @param frame_type - frame type to listen for
   * @param cb - callback
   * @return slot index (for removing), or TF_ERROR (-1)
   */
  bool TF_AddTypeListener(TinyFrame *tf, TF_TYPE frame_type, TF_Listener cb);

  /**
   * Remove a listener by type.
   *
   * @param tf - instance
   * @param type - the type it's registered for
   */
  bool TF_RemoveTypeListener(TinyFrame *tf, TF_TYPE type);

  /**
   * Register a generic listener.
   *
   * @param tf - instance
   * @param cb - callback
   * @return slot index (for removing), or TF_ERROR (-1)
   */
  bool TF_AddGenericListener(TinyFrame *tf, TF_Listener cb);

  /**
   * Remove a generic listener by function pointer
   *
   * @param tf - instance
   * @param cb - callback function to remove
   */
  bool TF_RemoveGenericListener(TinyFrame *tf, TF_Listener cb);

  /**
   * Renew an ID listener timeout externally (as opposed to by returning
   * TF_RENEW from the ID listener)
   *
   * @param tf - instance
   * @param id - listener ID to renew
   * @return true if listener was found and renewed
   */
  bool TF_RenewIdListener(TinyFrame *tf, TF_ID id);

  // ---------------------------- FRAME TX FUNCTIONS
  // ------------------------------

  /**
   * Send a frame, no listener
   *
   * @param tf - instance
   * @param msg - message struct. ID is stored in the frame_id field
   * @return success
   */
  bool TF_Send(TinyFrame *tf, TF_Msg *msg);

  /**
   * Like TF_Send, but without the struct
   */
  bool TF_SendSimple(TinyFrame *tf, TF_TYPE type, const uint8_t *data,
                     TF_LEN len);

  /**
   * Send a frame, and optionally attach an ID listener.
   *
   * @param tf - instance
   * @param msg - message struct. ID is stored in the frame_id field
   * @param listener - listener waiting for the response (can be NULL)
   * @param ftimeout - time out callback
   * @param timeout - listener expiry time in ticks
   * @return success
   */
  bool TF_Query(TinyFrame *tf, TF_Msg *msg, TF_Listener listener,
                TF_Listener_Timeout ftimeout, TF_TICKS timeout);

  /**
   * Like TF_Query(), but without the struct
   */
  bool TF_QuerySimple(TinyFrame *tf, TF_TYPE type, const uint8_t *data,
                      TF_LEN len, TF_Listener listener,
                      TF_Listener_Timeout ftimeout, TF_TICKS timeout);

  /**
   * Send a response to a received message.
   *
   * @param tf - instance
   * @param msg - message struct. ID is read from frame_id. set ->renew to reset
   * listener timeout
   * @return success
   */
  bool TF_Respond(TinyFrame *tf, TF_Msg *msg);

  // ------------------------ MULTIPART FRAME TX FUNCTIONS
  // ----------------------------- Those routines are used to send long frames
  // without having all the data available at once (e.g. capturing it from a
  // peripheral or reading from a large memory buffer)

  /**
   * TF_Send() with multipart payload.
   * msg.data is ignored and set to NULL
   */
  bool TF_Send_Multipart(TinyFrame *tf, TF_Msg *msg);

  /**
   * TF_SendSimple() with multipart payload.
   */
  bool TF_SendSimple_Multipart(TinyFrame *tf, TF_TYPE type, TF_LEN len);

  /**
   * TF_QuerySimple() with multipart payload.
   */
  bool TF_QuerySimple_Multipart(TinyFrame *tf, TF_TYPE type, TF_LEN len,
                                TF_Listener listener,
                                TF_Listener_Timeout ftimeout, TF_TICKS timeout);

  /**
   * TF_Query() with multipart payload.
   * msg.data is ignored and set to NULL
   */
  bool TF_Query_Multipart(TinyFrame *tf, TF_Msg *msg, TF_Listener listener,
                          TF_Listener_Timeout ftimeout, TF_TICKS timeout);

  /**
   * TF_Respond() with multipart payload.
   * msg.data is ignored and set to NULL
   */
  void TF_Respond_Multipart(TinyFrame *tf, TF_Msg *msg);

  /**
   * Send the payload for a started multipart frame. This can be called multiple
   * times if needed, until the full length is transmitted.
   *
   * @param tf - instance
   * @param buff - buffer to send bytes from
   * @param length - number of bytes to send
   */
  void TF_Multipart_Payload(TinyFrame *tf, const uint8_t *buff,
                            uint32_t length);

  /**
   * Close the multipart message, generating chekcsum and releasing the Tx lock.
   *
   * @param tf - instance
   */
  void TF_Multipart_Close(TinyFrame *tf);

  void tf_handle_tick(void);
  bool TF_SendFrame_Begin(TinyFrame *tf, TF_Msg *msg, TF_Listener listener,
                          TF_Listener_Timeout ftimeout, TF_TICKS timeout);

  void TF_SendFrame_Chunk(TinyFrame *tf, const uint8_t *buff, uint32_t length);
  inline void renew_id_listener(struct TF_IdListener_ *lst);

  /** Clean up Generic listener */
  inline void cleanup_generic_listener(TinyFrame *tf, TF_COUNT i,
                                       struct TF_GenericListener_ *lst);

  inline uint32_t TF_ComposeHead(TinyFrame *tf, uint8_t *outbuff, TF_Msg *msg);
  void TF_HandleReceivedMessage(TinyFrame *tf);

  /** Clean up Type listener */
  inline void cleanup_type_listener(TinyFrame *tf, TF_COUNT i,
                                    struct TF_TypeListener_ *lst);

  void cleanup_id_listener(TinyFrame *tf, TF_COUNT i,
                           struct TF_IdListener_ *lst);
  void pars_begin_frame(TinyFrame *tf);

  inline uint32_t TF_ComposeBody(uint8_t *outbuff, const uint8_t *data,
                                 TF_LEN data_len, TF_CKSUM *cksum);
};
#endif // QTINYFRAME_H
