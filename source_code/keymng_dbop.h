
// keymng_dbop.h
#ifndef _KEYMNG_DBOP_H_
#define _KEYMNG_DBOP_H_

#include "keymngserverop.h"
#include "keymng_shmop.h"

#ifdef __cplusplus
extern "C" {
#endif

int KeyMngsvr_Agree_DBOp_GetKeyID(void *dbhdl, int *keyid);

int KeyMngsvr_Agree_DBOp_WriteSecKey(void *dbhdl, NodeSHMInfo *pNodeInfo);

int KeyMngsvr_Revoke_DBOp_UpdateSecKey(void *dbhdl, NodeSHMInfo *pNodeInfo);


#ifdef __cplusplus
}
#endif

#endif



