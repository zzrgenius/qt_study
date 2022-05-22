#ifndef TinyFrameH
#define TinyFrameH

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * TinyFrame protocol library
 *
 * (c) Ondřej Hruška 2017-2018, MIT License
 * no liability/warranty, free for any use, must retain this notice & license
 *
 * Upstream URL: https://github.com/MightyPork/TinyFrame
 */

#define TF_VERSION "2.3.0"

//---------------------------------------------------------------------------
#include <stdbool.h> // for bool
#include <stddef.h>  // for NULL
#include <stdint.h>  // for uint8_t etc
#include <string.h>  // for memset()
//---------------------------------------------------------------------------

// Checksum type (0 = none, 8 = ~XOR, 16 = CRC16 0x8005, 32 = CRC32)
#define TF_CKSUM_NONE 0 // no checksums
#define TF_CKSUM_XOR 8  // inverted xor of all payload bytes
#define TF_CKSUM_CRC8 9 // Dallas/Maxim CRC8 (1-wire)
#define TF_CKSUM_CRC16                                                         \
  16 // CRC16 with the polynomial 0x8005 (x^16 + x^15 + x^2 + 1)
#define TF_CKSUM_CRC32 32   // CRC32 with the polynomial 0xedb88320
#define TF_CKSUM_CUSTOM8 1  // Custom 8-bit checksum
#define TF_CKSUM_CUSTOM16 2 // Custom 16-bit checksum
#define TF_CKSUM_CUSTOM32 3 // Custom 32-bit checksum

#include "TF_Config.h"

// region Resolve data types

#if TF_LEN_BYTES == 1
typedef uint8_t TF_LEN;
#elif TF_LEN_BYTES == 2
typedef uint16_t TF_LEN;
#elif TF_LEN_BYTES == 4
typedef uint32_t TF_LEN;
#else
#error Bad value of TF_LEN_BYTES, must be 1, 2 or 4
#endif

#if TF_TYPE_BYTES == 1
typedef uint8_t TF_TYPE;
#elif TF_TYPE_BYTES == 2
typedef uint16_t TF_TYPE;
#elif TF_TYPE_BYTES == 4
typedef uint32_t TF_TYPE;
#else
#error Bad value of TF_TYPE_BYTES, must be 1, 2 or 4
#endif

#if TF_ID_BYTES == 1
typedef uint8_t TF_ID;
#elif TF_ID_BYTES == 2
typedef uint16_t TF_ID;
#elif TF_ID_BYTES == 4
typedef uint32_t TF_ID;
#else
#error Bad value of TF_ID_BYTES, must be 1, 2 or 4
#endif

#if (TF_CKSUM_TYPE == TF_CKSUM_XOR) || (TF_CKSUM_TYPE == TF_CKSUM_NONE) ||     \
    (TF_CKSUM_TYPE == TF_CKSUM_CUSTOM8) || (TF_CKSUM_TYPE == TF_CKSUM_CRC8)
// ~XOR (if 0, still use 1 byte - it won't be used)
typedef uint8_t TF_CKSUM;
#elif (TF_CKSUM_TYPE == TF_CKSUM_CRC16) || (TF_CKSUM_TYPE == TF_CKSUM_CUSTOM16)
// CRC16
typedef uint16_t TF_CKSUM;
#elif (TF_CKSUM_TYPE == TF_CKSUM_CRC32) || (TF_CKSUM_TYPE == TF_CKSUM_CUSTOM32)
// CRC32
typedef uint32_t TF_CKSUM;
#else
#error Bad value for TF_CKSUM_TYPE
#endif

// endregion

//---------------------------------------------------------------------------

/** Peer bit enum (used for init) */
typedef enum {
  TF_SLAVE = 0,
  TF_MASTER = 1,
} TF_Peer;

/** Response from listeners */
typedef enum {
  TF_NEXT = 0,  //!< Not handled, let other listeners handle it
  TF_STAY = 1,  //!< Handled, stay
  TF_RENEW = 2, //!< Handled, stay, renew - useful only with listener timeout
  TF_CLOSE = 3, //!< Handled, remove self
} TF_Result;

/** Data structure for sending / receiving messages */
typedef struct TF_Msg_ {
  TF_ID frame_id;   //!< message ID
  bool is_response; //!< internal flag, set when using the Respond function.
                    //!< frame_id is then kept unchanged.
  TF_TYPE type;     //!< received or sent message type

  /**
   * Buffer of received data, or data to send.
   *
   * - If (data == NULL) in an ID listener, that means the listener timed out
   * and the user should free any userdata and take other appropriate actions.
   *
   * - If (data == NULL) and length is not zero when sending a frame, that
   * starts a multi-part frame. This call then must be followed by sending the
   * payload and closing the frame.
   */
  const uint8_t *data;
  TF_LEN len; //!< length of the payload

  /**
   * Custom user data for the ID listener.
   *
   * This data will be stored in the listener slot and passed to the ID callback
   * via those same fields on the received message.
   */
  void *userdata;
  void *userdata2;
} TF_Msg;

/**
 * Clear message struct
 *
 * @param msg - message to clear in-place
 */
static inline void TF_ClearMsg(TF_Msg *msg) { memset(msg, 0, sizeof(TF_Msg)); }

/** TinyFrame struct typedef */
typedef struct TinyFrame_ TinyFrame;
typedef TF_Result (*TF_Listener)(TinyFrame *tf, TF_Msg *msg);

/**
 * TinyFrame Type Listener callback
 *
 * @param tf - instance
 * @param msg - the received message, userdata is populated inside the object
 * @return listener result
 */
typedef TF_Result (*TF_Listener)(TinyFrame *tf, TF_Msg *msg);

/**
 * TinyFrame Type Listener callback
 *
 * @param tf - instance
 * @param msg - the received message, userdata is populated inside the object
 * @return listener result
 */
typedef TF_Result (*TF_Listener_Timeout)(TinyFrame *tf);

// ---------------------------------- INTERNAL
// ---------------------------------- This is publicly visible only to allow
// static init.

enum TF_State_ {
  TFState_SOF = 0,    //!< Wait for SOF
  TFState_LEN,        //!< Wait for Number Of Bytes
  TFState_HEAD_CKSUM, //!< Wait for header Checksum
  TFState_ID,         //!< Wait for ID
  TFState_TYPE,       //!< Wait for message type
  TFState_DATA,       //!< Receive payload
  TFState_DATA_CKSUM  //!< Wait for Checksum
};

struct TF_IdListener_ {
  TF_ID id;
  TF_Listener fn;
  TF_Listener_Timeout fn_timeout;
  TF_TICKS timeout;     // nr of ticks remaining to disable this listener
  TF_TICKS timeout_max; // the original timeout is stored here (0 = no timeout)
  void *userdata;
  void *userdata2;
};

struct TF_TypeListener_ {
  TF_TYPE type;
  TF_Listener fn;
};

struct TF_GenericListener_ {
  TF_Listener fn;
};

/**
 * Frame parser internal state.
 */
struct TinyFrame_ {
  /* Public user data */
  void *userdata;
  uint32_t usertag;

  // --- the rest of the struct is internal, do not access directly ---

  /* Own state */
  TF_Peer peer_bit; //!< Own peer bit (unqiue to avoid msg ID clash)
  TF_ID next_id;    //!< Next frame / frame chain ID

  /* Parser state */
  enum TF_State_ state;
  TF_TICKS parser_timeout_ticks;
  TF_ID id;                        //!< Incoming packet ID
  TF_LEN len;                      //!< Payload length
  uint8_t data[TF_MAX_PAYLOAD_RX]; //!< Data byte buffer
  TF_LEN rxi;                      //!< Field size byte counter
  TF_CKSUM cksum;                  //!< Checksum calculated of the data stream
  TF_CKSUM ref_cksum;              //!< Reference checksum read from the message
  TF_TYPE type;                    //!< Collected message type number
  bool discard_data; //!< Set if (len > TF_MAX_PAYLOAD) to read the frame, but
                     //!< ignore the data.

  /* Tx state */
  // Buffer for building frames
  uint8_t sendbuf[TF_SENDBUF_LEN]; //!< Transmit temporary buffer

  uint32_t
      tx_pos; //!< Next write position in the Tx buffer (used for multipart)
  uint32_t tx_len;   //!< Total expected Tx length
  TF_CKSUM tx_cksum; //!< Transmit checksum accumulator

#if !TF_USE_MUTEX
  bool soft_lock; //!< Tx lock flag used if the mutex feature is not enabled.
#endif

  /* --- Callbacks --- */

  /* Transaction callbacks */
  struct TF_IdListener_ id_listeners[TF_MAX_ID_LST];
  struct TF_TypeListener_ type_listeners[TF_MAX_TYPE_LST];
  struct TF_GenericListener_ generic_listeners[TF_MAX_GEN_LST];

  // Those counters are used to optimize look-up times.
  // They point to the highest used slot number,
  // or close to it, depending on the removal order.
  TF_COUNT count_id_lst;
  TF_COUNT count_type_lst;
  TF_COUNT count_generic_lst;
};

// ------------------------ TO BE IMPLEMENTED BY USER ------------------------

/**
 * 'Write bytes' function that sends data to UART
 *
 * ! Implement this in your application code !
 */
// extern void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len);
// extern "C" void
//  Mutex functions
#if TF_USE_MUTEX

/** Claim the TX interface before composing and sending a frame */
extern bool TF_ClaimTx(TinyFrame *tf);

/** Free the TX interface after composing and sending a frame */
extern void TF_ReleaseTx(TinyFrame *tf);

#endif

// Custom checksum functions
#if (TF_CKSUM_TYPE == TF_CKSUM_CUSTOM8) ||                                     \
    (TF_CKSUM_TYPE == TF_CKSUM_CUSTOM16) ||                                    \
    (TF_CKSUM_TYPE == TF_CKSUM_CUSTOM32)

/**
 * Initialize a checksum
 *
 * @return initial checksum value
 */
extern TF_CKSUM TF_CksumStart(void);

/**
 * Update a checksum with a byte
 *
 * @param cksum - previous checksum value
 * @param byte - byte to add
 * @return updated checksum value
 */
extern TF_CKSUM TF_CksumAdd(TF_CKSUM cksum, uint8_t byte);

/**
 * Finalize the checksum calculation
 *
 * @param cksum - previous checksum value
 * @return final checksum value
 */
extern TF_CKSUM TF_CksumEnd(TF_CKSUM cksum);

#endif
#ifdef __cplusplus
}
#endif // __cplusplus

#endif
