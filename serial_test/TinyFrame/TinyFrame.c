//---------------------------------------------------------------------------
#include "TinyFrame.h"
#include <stdlib.h> // - for malloc() if dynamic constructor is used
//---------------------------------------------------------------------------


//#if !TF_USE_MUTEX
//// Not thread safe lock implementation, used if user did not provide a better
//// one. This is less reliable than a real mutex, but will catch most bugs caused
//// by inappropriate use fo the API.

///** Claim the TX interface before composing and sending a frame */
//static bool TF_ClaimTx(TinyFrame *tf) {
//  if (tf->soft_lock) {
//    TF_Error("TF already locked for tx!");
//    return false;
//  }

//  tf->soft_lock = true;
//  return true;
//}

///** Free the TX interface after composing and sending a frame */
//static void TF_ReleaseTx(TinyFrame *tf) { tf->soft_lock = false; }
//#endif

// region Listeners
