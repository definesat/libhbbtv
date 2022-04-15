
#ifndef __CAPABILITYCLASS_H__
#define __CAPABILITYCLASS_H__

#include "hbbtvbrowserplugin.h"


NPClass* fillCAPABILITYpclass(void);

NPObject *  CAPABILITY_Allocate(NPP npp, NPClass *aClass);
void        CAPABILITY_Deallocate(NPObject *obj);
void        CAPABILITY_Invalidate(NPObject *obj);
bool        CAPABILITY_HasMethod(NPObject *obj, NPIdentifier name);
bool        CAPABILITY_Invoke(NPObject *obj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool        CAPABILITY_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool        CAPABILITY_HasProperty(NPObject *obj, NPIdentifier name);
bool        CAPABILITY_GetProperty(NPObject *obj, NPIdentifier name, NPVariant *result);
bool        CAPABILITY_SetProperty(NPObject *obj, NPIdentifier name, const NPVariant *value);

bool        CAPABILITY_RemoveProperty(NPObject *npobj, NPIdentifier name);

bool        CAPABILITY_Enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count);


#endif
