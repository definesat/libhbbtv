
#include "capabilityclass.h"


#define kCAPABILITY_ID_PROPERTY_XMLCAPABILITIES			                0
#define kCAPABILITY_ID_PROPERTY_EXTRASDVIDEODECODES 	            1
#define kCAPABILITY_ID_PROPERTY_EXTRAHDVIDEODECODES			    2
#define kCAPABILITY_NUM_PROPERTY_IDENTIFIERS				    			3

#define kCAPABILITY_ID_METHOD_HASCAPBILITY                                    0
#define kCAPABILITY_NUM_METHOD_IDENTIFIERS                					1



bool            v_bCAPABILITYIdentifiersInitialized = false;

NPIdentifier    v_CAPABILITYPropertyIdentifiers[kCAPABILITY_NUM_PROPERTY_IDENTIFIERS];
const NPUTF8 *  v_CAPABILITYPropertyNames[kCAPABILITY_NUM_PROPERTY_IDENTIFIERS] = {
	"xmlCapabilities",
    "extraSDVideoDecodes",
    "extraHDVideoDecodes"
    };

NPIdentifier    v_CAPABILITYMethodIdentifiers[kCAPABILITY_NUM_METHOD_IDENTIFIERS];
const NPUTF8 *  v_CAPABILITYMethodNames[kCAPABILITY_NUM_METHOD_IDENTIFIERS] = {
	"hasCapability"
};

static  void    CAPABILITYinitializeIdentifiers(void)
{
    sBrowserFuncs->getstringidentifiers( v_CAPABILITYPropertyNames, kCAPABILITY_NUM_PROPERTY_IDENTIFIERS, v_CAPABILITYPropertyIdentifiers );
    sBrowserFuncs->getstringidentifiers( v_CAPABILITYMethodNames,   kCAPABILITY_NUM_METHOD_IDENTIFIERS,   v_CAPABILITYMethodIdentifiers );
}


NPClass  stCAPABILITYclass;
NPClass* pCAPABILITYclass = NULL;

NPClass* fillCAPABILITYpclass(void)
{
    TRACEINFO;
    if (pCAPABILITYclass == NULL)
    {
        stCAPABILITYclass.allocate          = CAPABILITY_Allocate;
        stCAPABILITYclass.deallocate        = CAPABILITY_Deallocate;
        stCAPABILITYclass.invalidate        = CAPABILITY_Invalidate;
        stCAPABILITYclass.hasMethod         = CAPABILITY_HasMethod;
        stCAPABILITYclass.invoke            = CAPABILITY_Invoke;
        stCAPABILITYclass.invokeDefault     = CAPABILITY_InvokeDefault;
        stCAPABILITYclass.hasProperty       = CAPABILITY_HasProperty;
        stCAPABILITYclass.getProperty       = CAPABILITY_GetProperty;
        stCAPABILITYclass.setProperty       = CAPABILITY_SetProperty;
        stCAPABILITYclass.removeProperty    = CAPABILITY_RemoveProperty;
        stCAPABILITYclass.enumerate         = CAPABILITY_Enumerate;
        pCAPABILITYclass = &stCAPABILITYclass;
    }

    return pCAPABILITYclass;
}


NPObject *          CAPABILITY_Allocate(NPP npp, NPClass *theClass)
{
    TRACEINFO;

    if (!v_bCAPABILITYIdentifiersInitialized)
    {
        v_bCAPABILITYIdentifiersInitialized = true;
        CAPABILITYinitializeIdentifiers();
    }
	NPObject* newconfiguration = NULL;

    newconfiguration = (NPObject *)MEMALLOC(sizeof(NPObject));

    return newconfiguration;
}


void        CAPABILITY_Deallocate(NPObject* obj)
{
    TRACEINFO;
     MEMFREE(obj);
    return;
}

void        CAPABILITY_Invalidate(NPObject* obj)
{
    TRACEINFO;
    return;
}

bool        CAPABILITY_HasMethod(NPObject* obj, NPIdentifier name)
{
    TRACEINFO;
	bool result = false;
    int i = 0;
    NPUTF8* utf8methodname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    while ((i < kCAPABILITY_NUM_METHOD_IDENTIFIERS) && (result == false))
    {
        if ( name == v_CAPABILITYMethodIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }

    return result;
}

bool        CAPABILITY_Invoke(NPObject* obj, NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    //TRACEINFO;
    NOTIMPLEMENTED;
	bool fctresult = false;

    NPUTF8* utf8methodname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug("CAPABILITY_Invoke:%s\n", utf8methodname);

	if (name ==  v_CAPABILITYMethodIdentifiers[kCAPABILITY_ID_METHOD_HASCAPBILITY])
    {
/*
		Check if the OITF supports the passed capability.
		Returns true if the OITF supports the passed capability, false otherwise.
*/
    	fctresult = false;
    }
    return fctresult;
}

bool        CAPABILITY_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    TRACEINFO;
    return true;
}

bool        CAPABILITY_HasProperty(NPObject* obj, NPIdentifier name)
{
    TRACEINFO;
    bool result = false;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);

    int i = 0;
    while ((i < kCAPABILITY_NUM_PROPERTY_IDENTIFIERS) && (result == false))
    {
        if ( name == v_CAPABILITYPropertyIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }
    return result;
}

bool        CAPABILITY_GetProperty(NPObject* obj, NPIdentifier name, NPVariant* result)
{
    TRACEINFO;
    bool fctresult = false;

    if (name == v_CAPABILITYPropertyIdentifiers[kCAPABILITY_ID_PROPERTY_XMLCAPABILITIES]) {
    	OBJECT_TO_NPVARIANT(NULL,*result);
    	fctresult = true;
    }
	else if (name == v_CAPABILITYPropertyIdentifiers[kCAPABILITY_ID_PROPERTY_EXTRASDVIDEODECODES]) {
		INT32_TO_NPVARIANT(0,*result);
		fctresult = true;
	}
	else if (name == v_CAPABILITYPropertyIdentifiers[kCAPABILITY_ID_PROPERTY_EXTRAHDVIDEODECODES]) {
		INT32_TO_NPVARIANT(0,*result);
		fctresult = true;
	}
    return  fctresult;
}

bool        CAPABILITY_SetProperty(NPObject *obj, NPIdentifier name, const NPVariant *value)
{
    TRACEINFO;
    return false;
}

bool        CAPABILITY_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    TRACEINFO;
    return false;
}


bool        CAPABILITY_Enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    TRACEINFO;
    return true;
}
