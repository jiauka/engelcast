/*
 *  Chromecast control utils
 */

#ifndef __CAST_UTIL_H
#define __CAST_UTIL_H

typedef enum { CAST_PLAY, CAST_PAUSE, CAST_STOP } tCastAction;

struct sq_metadata_s;
struct sMRConfig;
struct sCastCtx;

void	CastGetStatus(struct sCastCtx *Ctx);
void	CastGetMediaStatus(struct sCastCtx *Ctx);
void    CastGetCurrentMediaStatus(struct sCastCtx *Ctx);

void 	CastPowerOff(struct sCastCtx *Ctx);
void 	CastPowerOn(struct sCastCtx *Ctx);

void 	CastStop(struct sCastCtx *Ctx);
#define CastPlay(Ctx)	CastSimple(Ctx, "PLAY")
#define CastPause(Ctx)	CastSimple(Ctx, "PAUSE")
void 	CastSimple(struct sCastCtx *Ctx, char *Type);
bool	CastLoad(struct sCastCtx *Ctx, char *URI, char *ContentType);
void 	CastSetDeviceVolume(struct sCastCtx *p, double Volume, bool Queue);

#endif

