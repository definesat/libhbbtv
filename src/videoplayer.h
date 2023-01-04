#ifndef    __VIDEOPLAYER_H__
#define    __VIDEOPLAYER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "npapi.h"
#include "nptypes.h"
#include "npfunctions.h"
#include "npruntime.h"

#include "hbbtvbrowserplugin.h"

#define FULLSIZE_WIDTH 1280
#define FULLSIZE_HEIGHT 720

typedef struct
{
	NPObject header;
	NPP npp;
	NPBool      fullscreen;
	int32_t     width;
    int32_t     height;

	NPObject	*listener;

	char className[1024];
    //~ char*           pcArg_onChannelChangeSucceeded;
    //~ NPObject*       onChannelChangeSucceeded;
    //NPObject*   channelConfig;

} NPObj_VidPly;


NPClass* fillVIDPLYpclass(void);

NPObject *  VIDPLY_Allocate(NPP npp, NPClass *aClass);
void        VIDPLY_Deallocate(NPObject *obj);
void        VIDPLY_Invalidate(NPObject *obj);
bool        VIDPLY_HasMethod(NPObject *obj, NPIdentifier name);
bool        VIDPLY_Invoke(NPObject *obj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool        VIDPLY_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool        VIDPLY_HasProperty(NPObject *obj, NPIdentifier name);
bool        VIDPLY_GetProperty(NPObject *obj, NPIdentifier name, NPVariant *result);
bool        VIDPLY_SetProperty(NPObject *obj, NPIdentifier name, const NPVariant *value);
bool        VIDPLY_RemoveProperty(NPObject *npobj, NPIdentifier name);
bool        VIDPLY_Enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count);

void		VIDPLY_Invoke_Stop(NPObj_VidPly* obj,const NPVariant* args, uint32_t argCount, NPVariant* result);
void		VIDPLY_Invoke_Play(NPObj_VidPly* obj,const NPVariant* args, uint32_t argCount, NPVariant* result);

void		VIDPLY_setsize(NPObj_VidPly* obj,int32_t witdh, int32_t height);
#endif
