/*
 *  Chromecast protocol handler 
 */

#ifndef __CASTITF_H
#define __CASTITF_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <pb_encode.h>
#include <pb_decode.h>
#include "jansson.h"
#include "castmessage.pb.h"

void InitSSL(void);
void EndSSL(void);

struct sCastCtx;

json_t*	GetTimedEvent(void *p, u32_t msWait);
void*	CreateCastDevice(bool stopReceiver, struct in_addr ip, u16_t port, double MediaVolume);
bool 	UpdateCastDevice(struct sCastCtx *Ctx, struct in_addr ip, u16_t port);
void 	DeleteCastDevice(struct sCastCtx *Ctx);
bool	CastIsConnected(struct sCastCtx *Ctx);
bool 	CastIsMediaSession(struct sCastCtx *Ctx);

#endif
