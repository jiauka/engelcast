/*
 *  Chromecast control utils
 */


#include <stdlib.h>
#include <math.h>

#include "platform.h"
#include "log_util.h"
#include "util.h"
#include "castcore.h"
#include "cast_util.h"

extern log_level cast_loglevel;
static log_level *loglevel = &cast_loglevel;

/*----------------------------------------------------------------------------*/
bool CastIsConnected(void *p)
{
	tCastCtx *Ctx = (tCastCtx*) p;
	bool status;

	pthread_mutex_lock(&Ctx->Mutex);
	status = (Ctx->ssl != NULL);
	pthread_mutex_unlock(&Ctx->Mutex);
	return status;
}


/*----------------------------------------------------------------------------*/
bool CastIsMediaSession(void *p)
{
	tCastCtx *Ctx = (tCastCtx*) p;
	bool status;

	if (!Ctx) return false;

	pthread_mutex_lock(&Ctx->Mutex);
	status = Ctx->mediaSessionId != 0;
	pthread_mutex_unlock(&Ctx->Mutex);

	return status;
}


/*----------------------------------------------------------------------------*/
void CastGetStatus(struct sCastCtx *Ctx)
{
	// SSL context might not be set yet
	if (!Ctx) return;

	pthread_mutex_lock(&Ctx->Mutex);
	SendCastMessage(Ctx, CAST_RECEIVER, NULL, "{\"type\":\"GET_STATUS\",\"requestId\":%d}", Ctx->reqId++);
	pthread_mutex_unlock(&Ctx->Mutex);
}


/*----------------------------------------------------------------------------*/
void CastGetMediaStatus(struct sCastCtx *Ctx)
{
	// SSL context might not be set yet
	if (!Ctx) return;

	pthread_mutex_lock(&Ctx->Mutex);
	if (Ctx->mediaSessionId) {
		SendCastMessage(Ctx, CAST_MEDIA, Ctx->transportId,
						"{\"type\":\"GET_STATUS\",\"requestId\":%d,\"mediaSessionId\":%d}",
						Ctx->reqId++, Ctx->mediaSessionId);
    }
	pthread_mutex_unlock(&Ctx->Mutex);
}

/*----------------------------------------------------------------------------*/
void CastGetCurrentMediaStatus(struct sCastCtx *Ctx)
{
	// SSL context might not be set yet
	if (!Ctx) return;

	pthread_mutex_lock(&Ctx->Mutex);
	SendCastMessage(Ctx, CAST_MEDIA, Ctx->transportId,
						"{\"type\":\"GET_STATUS\",\"requestId\":%d,\"mediaSessionId\":%d}",
						Ctx->reqId++, Ctx->mediaSessionId);
	pthread_mutex_unlock(&Ctx->Mutex);
}


/*----------------------------------------------------------------------------*/
#define LOAD_FLUSH
bool CastLoad(struct sCastCtx *Ctx, char *URI, char *ContentType)
{
	json_t *msg;
	char* str;

	if (!LaunchReceiver(Ctx)) {
		LOG_ERROR(" Cannot connect Cast receiver");
		return false;
	}
    if (strstr(ContentType, "jpeg") != NULL) {
        msg = json_pack("{ss,ss,ss}", "contentId", URI, "streamType", "NONE",
                        "contentType", ContentType);

    }
    else {
        msg = json_pack("{ss,ss,ss}", "contentId", URI, "streamType", "BUFFERED",
                        "contentType", ContentType);
    }
    pthread_mutex_lock(&Ctx->Mutex);

#ifdef LOAD_FLUSH
	if (Ctx->Status == CAST_LAUNCHED && (!Ctx->waitId || Ctx->waitMedia)) {

		/*
		For some reason a LOAD request is pending (maybe not enough data have
		been buffered yet, so LOAD has not been acknowledged, a stop might
		be stuck in the queue and the source does not send any more data, so
		this is a deadlock (see usage with iOS 10.x). Best is to have LOAD
		request flushing the queue then
		*/
		if (Ctx->waitMedia) CastQueueFlush(&Ctx->reqQueue);
#else
	if (Ctx->Status == CAST_LAUNCHED && !Ctx->waitId) {
#endif

		Ctx->waitId = Ctx->reqId++;
		Ctx->waitMedia = Ctx->waitId;
		Ctx->mediaSessionId = 0;

		msg = json_pack("{ss,si,ss,sf,sb,so}", "type", "LOAD",
						"requestId", Ctx->waitId, "sessionId", Ctx->sessionId,
						"currentTime", 0.0, "autoplay", 0,
						"media", msg);

		str = json_dumps(msg, JSON_ENCODE_ANY | JSON_INDENT(1));
		SendCastMessage(Ctx, CAST_MEDIA, Ctx->transportId, "%s", str);
		json_decref(msg);
		NFREE(str);

		LOG_INFO(" Immediate LOAD (id:%u)", Ctx->waitId);
	}
	// otherwise queue it for later
	else {
		tReqItem *req = malloc(sizeof(tReqItem));
#ifndef LOAD_FLUSH
		// if waiting for a media, need to unlock queue and take precedence
		if (Ctx->waitMedia) Ctx->waitId = Ctx->waitMedia = 0;
#endif

		strcpy(req->Type, "LOAD");
		req->data.msg = msg;
		QueueInsert(&Ctx->reqQueue, req);
		LOG_INFO(" Queuing %s", req->Type);
	}

	pthread_mutex_unlock(&Ctx->Mutex);

	return true;
}


/*----------------------------------------------------------------------------*/
void CastSimple(struct sCastCtx *Ctx, char *Type)
{
	// lock on wait for a Cast response
	pthread_mutex_lock(&Ctx->Mutex);

	if (Ctx->Status == CAST_LAUNCHED && !Ctx->waitId) {
		// no media session, nothing to do
		if (Ctx->mediaSessionId) {
			Ctx->waitId = Ctx->reqId++;

			SendCastMessage(Ctx, CAST_MEDIA, Ctx->transportId,
							"{\"type\":\"%s\",\"requestId\":%d,\"mediaSessionId\":%d}",
							Type, Ctx->waitId, Ctx->mediaSessionId);

			LOG_INFO(" Immediate %s (id:%u)", Type, Ctx->waitId);

		}
		else {
			LOG_WARN(" %s req w/o a session", Type);
	   }

	}
	else {
		tReqItem *req = malloc(sizeof(tReqItem));
		strcpy(req->Type, Type);
		QueueInsert(&Ctx->reqQueue, req);
		LOG_INFO(" Queuing %s", req->Type);
	}

	pthread_mutex_unlock(&Ctx->Mutex);
}


/*----------------------------------------------------------------------------*/
void CastStop(struct sCastCtx *Ctx)
{
	// lock on wait for a Cast response
	pthread_mutex_lock(&Ctx->Mutex);

	CastQueueFlush(&Ctx->reqQueue);

	// if a session is active, stop can be sent-immediately
	if (Ctx->mediaSessionId) {

		Ctx->waitId = Ctx->reqId++;

		// version 1.24
		if (Ctx->stopReceiver) {
			SendCastMessage(Ctx, CAST_RECEIVER, NULL,
						"{\"type\":\"STOP\",\"requestId\":%d,\"sessionId\":%d}", Ctx->waitId, Ctx->mediaSessionId);
			Ctx->Status = CAST_CONNECTED;

		}
		else {
			SendCastMessage(Ctx, CAST_MEDIA, Ctx->transportId,
							"{\"type\":\"STOP\",\"requestId\":%d,\"mediaSessionId\":%d}",
							Ctx->waitId, Ctx->mediaSessionId);
		}

		Ctx->mediaSessionId = 0;
		LOG_INFO(" Immediate STOP (id:%u)", Ctx->waitId);

	// waiting for a session, need to queue the stop
	} else if (Ctx->waitMedia) {

		tReqItem *req = malloc(sizeof(tReqItem));
		strcpy(req->Type, "STOP");
		QueueInsert(&Ctx->reqQueue, req);
		LOG_INFO(" Queuing %s", req->Type);

	// launching happening, just go back to CONNECT mode
	} else if (Ctx->Status == CAST_LAUNCHING) {
			Ctx->Status = CAST_CONNECTED;
			LOG_WARN("Stop while still launching receiver");
	// a random stop
	} else {
		LOG_WARN("Stop w/o session or connect");
	}

	pthread_mutex_unlock(&Ctx->Mutex);
}


/*----------------------------------------------------------------------------*/
void CastPowerOff(struct sCastCtx *Ctx)
{
	CastDisconnect(Ctx);
}


/*----------------------------------------------------------------------------*/
void CastPowerOn(struct sCastCtx *Ctx)
{
	CastConnect(Ctx);
}


/*----------------------------------------------------------------------------*/
void CastSetDeviceVolume(struct sCastCtx *Ctx, double Volume, bool Queue)
{

	if (Volume > 1.0) Volume = 1.0;

	pthread_mutex_lock(&Ctx->Mutex);

	if (Ctx->Status == CAST_LAUNCHED && (!Ctx->waitId || !Queue)) {

		if (Volume) {
			SendCastMessage(Ctx, CAST_RECEIVER, NULL,
						"{\"type\":\"SET_VOLUME\",\"requestId\":%d,\"volume\":{\"level\":%0.4lf}}",
						Ctx->reqId++, Volume);

			SendCastMessage(Ctx, CAST_RECEIVER, NULL,
						"{\"type\":\"SET_VOLUME\",\"requestId\":%d,\"volume\":{\"muted\":false}}",
						Ctx->reqId);

		} else {
			SendCastMessage(Ctx, CAST_RECEIVER, NULL,
						"{\"type\":\"SET_VOLUME\",\"requestId\":%d,\"volume\":{\"muted\":true}}",
						Ctx->reqId);
		}

		// Only set waitId if this is NOT queue bypass
		if (Queue) Ctx->waitId = Ctx->reqId;

		LOG_INFO(" Immediate VOLUME (id:%u)", Ctx->reqId);

		Ctx->reqId++;
	}
	// otherwise queue it for later
	else {
		tReqItem *req = malloc(sizeof(tReqItem));
		strcpy(req->Type, "SET_VOLUME");
		req->data.Volume = Volume;
		QueueInsert(&Ctx->reqQueue, req);
		LOG_INFO(" Queuing %s", req->Type);
	}

	pthread_mutex_unlock(&Ctx->Mutex);
}

/*----------------------------------------------------------------------------*/
int CastSeek(char *ControlURL, unsigned Interval)
{
	int rc = 0;

	return rc;
}





