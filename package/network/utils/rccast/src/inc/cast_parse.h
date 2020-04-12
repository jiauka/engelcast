/*
 *  Chromecast control utils
 */

#ifndef __CAST_PARSE_H
#define __CAST_PARSE_H

#include "jansson.h"

int 		GetMediaItem_I(json_t *root, int n, char *item);
double 		GetMediaItem_F(json_t *root, int n, char *item);
const char* GetMediaItem_S(json_t *root, int n, char *item);
const char* GetAppIdItem(json_t *root, char* appId, char *item);
const char* GetMediaInfoItem_S(json_t *root, int n, char *item);
bool 		GetMediaVolume(json_t *root, int n, double *volume, bool *muted);

#endif

