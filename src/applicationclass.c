/*
 *		Copyright (c) 2010-2011 Telecom-Paristech
 *                 All Rights Reserved
 *	GPAC is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  GPAC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *		Authors:    Stanislas Selle
 *
 */
#include "applicationprivatedataclass.h"
#include "applicationclass.h"
#include "hbbtvcommonutil.h"

#define kAPPLICATION_ID_PROPERTY_VISIBLE    		            0
#define kAPPLICATION_ID_PROPERTY_PRIVATEDATA	                1
#define kAPPLICATION_NUM_PROPERTY_IDENTIFIERS                   2

#if 0
#define kAPPLICATION_ID_METHOD_CREATEAPPLICATION               0
#define kAPPLICATION_ID_METHOD_DESTROYAPPLICATION              1

#define kAPPLICATION_ID_METHOD_SHOW                             2
#define kAPPLICATION_ID_METHOD_HIDE                             3

#define kAPPLICATION_ID_METHOD_ACTIVATE                         4
#define kAPPLICATION_ID_METHOD_ACTIVATEINPUT                   5

#define kAPPLICATION_NUM_METHOD_IDENTIFIERS              		6
#else
enum ID_METHID {
#if 1
	kAPPLICATION_ID_METHOD_CREATEAPPLICATION,
		kAPPLICATION_ID_METHOD_DESTROYAPPLICATION,
#endif
	kAPPLICATION_ID_METHOD_SHOW,
		kAPPLICATION_ID_METHOD_HIDE,
#if 0		
		kAPPLICATION_ID_METHOD_ACTIVATE,
		kAPPLICATION_ID_METHOD_ACTIVATEINPUT,
#endif
		kAPPLICATION_NUM_METHOD_IDENTIFIERS

};


#endif

static  bool            v_bAPPLICATIONIdentifiersInitialized = false;

static  NPIdentifier    v_APPLICATIONPropertyIdentifiers[kAPPLICATION_NUM_PROPERTY_IDENTIFIERS];
static  const NPUTF8 *  v_APPLICATIONPropertyNames[kAPPLICATION_NUM_PROPERTY_IDENTIFIERS] = {
	"visible",
	"privateData"
};

static  NPIdentifier    v_APPLICATIONMethodIdentifiers[kAPPLICATION_NUM_METHOD_IDENTIFIERS];
static  const NPUTF8 *  v_APPLICATIONMethodNames[kAPPLICATION_NUM_METHOD_IDENTIFIERS] = {
#if 1
    "createApplication",
    "destroyApplication",
#endif
    "show",
    "hide",
#if 0    
	"activate",
	"activateInput"
#endif

#if 0
	"deactivate",
	"deactivateInput",
	"onKeyUp",
	"onKeyDown"
#endif
};


static  void    APPLICATIONinitializeIdentifiers(void)
{
    sBrowserFuncs->getstringidentifiers( v_APPLICATIONPropertyNames, kAPPLICATION_NUM_PROPERTY_IDENTIFIERS, v_APPLICATIONPropertyIdentifiers );
    sBrowserFuncs->getstringidentifiers( v_APPLICATIONMethodNames,   kAPPLICATION_NUM_METHOD_IDENTIFIERS,   v_APPLICATIONMethodIdentifiers );
}

NPClass  stAPPLICATIONclass;
NPClass* pAPPLICATIONclass = NULL;

NPClass* fillAPPLICATIONpclass(void)
{
    TRACEINFO;
    if (pAPPLICATIONclass == NULL)
    {
        stAPPLICATIONclass.allocate          = APPLICATION_Allocate;
        stAPPLICATIONclass.deallocate        = APPLICATION_Deallocate;
        stAPPLICATIONclass.invalidate        = APPLICATION_Invalidate;
        stAPPLICATIONclass.hasMethod         = APPLICATION_HasMethod;
        stAPPLICATIONclass.invoke            = APPLICATION_Invoke;
        stAPPLICATIONclass.invokeDefault     = APPLICATION_InvokeDefault;
        stAPPLICATIONclass.hasProperty       = APPLICATION_HasProperty;
        stAPPLICATIONclass.getProperty       = APPLICATION_GetProperty;
        stAPPLICATIONclass.setProperty       = APPLICATION_SetProperty;
        stAPPLICATIONclass.removeProperty    = APPLICATION_RemoveProperty;
        stAPPLICATIONclass.enumerate         = APPLICATION_Enumerate;
        pAPPLICATIONclass = &stAPPLICATIONclass;
    }

    return pAPPLICATIONclass;
}


NPObject *          APPLICATION_Allocate(NPP npp, NPClass *theClass)
{
    TRACEINFO;
    if (!v_bAPPLICATIONIdentifiersInitialized)
    {
        v_bAPPLICATIONIdentifiersInitialized = true;
        APPLICATIONinitializeIdentifiers();
    }

    NPObj_Application* newapplication = (NPObj_Application*)MEMALLOC(sizeof(NPObj_Application));
    debug("\t%s : Allocation at \x1b[%i;3%im%p\n\x1b[0m",__FUNCTION__, 1, 1, newapplication );
	newapplication->npp = npp;
	newapplication->visible = true;
    newapplication->privateData = sBrowserFuncs->createobject(npp, fillAPPPRIVDATApclass());
    debug("\t%s : Create privateData property at \x1b[%i;3%im%p\n\x1b[0m ",__FUNCTION__,  1, 1, newapplication->privateData );
	newapplication->visible = true;
    return (NPObject*)newapplication;
}

  void        APPLICATION_Deallocate(NPObject* obj)
{
    TRACEINFO;
    NPObj_Application* applicationobj = (NPObj_Application*)obj;
    sBrowserFuncs->releaseobject(applicationobj->privateData);
    MEMFREE(applicationobj);
    return;
}

  void        APPLICATION_Invalidate(NPObject* obj)
{
    TRACEINFO;
    return;
}

  bool        APPLICATION_HasMethod(NPObject* obj, NPIdentifier name)
{
    //TRACEINFO;
    NPUTF8* utf8methodname = (char*)sBrowserFuncs->utf8fromidentifier(name);


    bool result = false;
    int i = 0;

    while ((i < kAPPLICATION_NUM_METHOD_IDENTIFIERS) && (result == false))
    {
        if ( name == v_APPLICATIONMethodIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }
    if (!result)
        debug("APPLICATION_HasMethod:%s\n", utf8methodname);
    return result;
}

NPVariant appUrl;

void print_NPVariant(NPVariant* args)
{

	switch(args[0].type)
	{
		case NPVariantType_Void :
			debug("################# NPVariantType_Void\n");
			break;		
		case NPVariantType_Null :
			debug("################# NPVariantType_Null\n");
			break;
		case NPVariantType_Bool :
		{
			bool val = NPVARIANT_TO_BOOLEAN(args[0]);
			debug("################# NPVariantType_Bool : %d\n", val);
			break;
		}
		case NPVariantType_Int32 :
		{
			int val = NPVARIANT_TO_INT32(args[0]);
			debug("################# NPVariantType_Int32 : %d\n", val);
			break;
		}
		case NPVariantType_Double :
		{
			int val = NPVARIANT_TO_DOUBLE(args[0]);
			debug("################# NPVariantType_Double : %d\n", val );
			break;
		}
		case NPVariantType_String :
			debug("################# NPVariantType_String : %s len %d\n",  args[0].value.stringValue.UTF8Characters/* NPVARIANT_TO_STRING(args[0])*/, args[0].value.stringValue.UTF8Length);
			break;
		case NPVariantType_Object :
			debug("################# NPVariantType_Object\n");

			break;
	}

}

bool parse_url(char *url, int len, int *value)
{
	int i, j, k, l;
	int tmp =0;

	// check 1
	if (len <= 18 )
		return false;
	
	// check 2
	for (i = 0; i < len - 18;i ++)
	{
		if (url[18+i] == '.')
			break;
	}

	if ((i == (len - 18)) || (i == 0) || (i == (len-18-1)))
	{
		return false;
	}

	// check 3
	for (l = i+1 ; l < len; l++)
	{
		if (url[18+l] >= '0' && url[18+l] <= '9')
		{
		}
		else if (url[18+l] >= 'a' && url[18+l] <= 'f')
		{
		}
		else if (url[18+l] >= 'A' && url[18+l] <= 'F')
		{
		}
		else
		{
			break;
		}
	}

	if (l != len)
	{
		if (l == 0)
		{
			return false;
		}
	}

	k = 0;
	tmp = 0;
	
	for (j = i-1; j >= 0; j--)
	{
		if (url[18+j] >= '0' && url[18+j] <= '9')
		{
			tmp += ((url[18+j] - '0') << k*4);			
		}
		else if (url[18+j] >= 'a' && url[18+j] <= 'f')
		{
			tmp += (((url[18+j] - 'a') + 0xa) << k*4);
		}
		else if (url[18+j] >= 'A' && url[18+j] <= 'F')
		{
			tmp += (((url[18+j] - 'A') + 0xa) << k*4);
		}
		else
		{
			debug("orgid error\n");
			return false;
		}
		k++;
	}

	debug("orgid 0x%x\n", tmp);
	value[0] = tmp;
	k = 0;	
	tmp = 0;
	
	for (j = l-1; j >= i+1; j--)
	{
		if (url[18+j] >= '0' && url[18+j] <= '9')
		{
			tmp += ((url[18+j] - '0') << k*4);			
		}
		else if (url[18+j] >= 'a' && url[18+j] <= 'f')
		{
			tmp += (((url[18+j] - 'a') + 0xa) << k*4);
		}
		else if (url[18+j] >= 'A' && url[18+j] <= 'F')
		{
			tmp += (((url[18+j] - 'A') + 0xa) << k*4);
		}
		else
		{
			debug("appid error\n");
			return false;
		}
		k++;
	}
	debug("appid 0x%x\n", tmp);
	value[1] = tmp;

	return true;
}

bool        APPLICATION_Invoke(NPObject* obj, NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    TRACEINFO;
	bool fctresult = false;
#if 1
    if (name == v_APPLICATIONMethodIdentifiers[kAPPLICATION_ID_METHOD_CREATEAPPLICATION])
    {
    	char tmp_c_value[1024];
		int *tmp_value = (int *)tmp_c_value;
		memset(tmp_c_value, 0, 1024);
		
		APPLICATION_Invoke_CreateApplication(obj, args, argCount);
		print_NPVariant(&args[0]);


		if (strncmp(args[0].value.stringValue.UTF8Characters, "dvb://current.ait", 17) == 0)
		{

			if (parse_url(args[0].value.stringValue.UTF8Characters, args[0].value.stringValue.UTF8Length, tmp_value))
			{
				if (send_cmd(HBBTV_GETURLAIT, (void *)tmp_value))
				{
					debug("[%s] url %s\n", __FUNCTION__, tmp_c_value);
					appUrl.value.stringValue.UTF8Length = strlen(tmp_c_value);
					appUrl.value.stringValue.UTF8Characters = (NPUTF8 *)sBrowserFuncs->memalloc(appUrl.value.stringValue.UTF8Length);
					appUrl.type = NPVariantType_String;
					memcpy(appUrl.value.stringValue.UTF8Characters, tmp_c_value, appUrl.value.stringValue.UTF8Length);

					//memcpy(result, &appUrl1, sizeof(NPVariant));

				}
				else
				{
					return false;
				}
			}			
		}
		else if (strncmp(args[0].value.stringValue.UTF8Characters, "http", 4) == 0)
		{
			appUrl.value.stringValue.UTF8Length = args[0].value.stringValue.UTF8Length;
			appUrl.value.stringValue.UTF8Characters = (NPUTF8 *)sBrowserFuncs->memalloc(args[0].value.stringValue.UTF8Length);
			appUrl.type = args[0].type;
			memcpy(appUrl.value.stringValue.UTF8Characters, args[0].value.stringValue.UTF8Characters, args[0].value.stringValue.UTF8Length);

			//memcpy(result, &appUrl1, sizeof(NPVariant));
		}
		else
		{
			return false;
		}

#if 1
		//NPObject sWindowObj;
		NPObject *		sWindowObj = NULL;
		NPObj_Application* newapplication = (NPObj_Application*)obj;
		NPIdentifier identifier;
		NPVariant variantValue, variantValue1;

		// Get the Dom window object.
		if (newapplication->npp)
		{
			//typedef NPError (*NPN_GetValueProcPtr)(NPP instance, NPNVariable variable, void *ret_value);
			sBrowserFuncs->getvalue(newapplication->npp, NPNVWindowNPObject, &sWindowObj );

			if (!sWindowObj)
			{
				debug("sWindowObj is NULL\n");
				return false;
			}

		}
		else
		{
			debug("newapplication->npp is NULL \n");
			return false;
		}

		// Create a "location" identifier.
		//typedef NPIdentifier (*NPN_GetStringIdentifierProcPtr) (const NPUTF8 *name);
		identifier = sBrowserFuncs->getstringidentifier((const NPUTF8 *)"location");

		if (!identifier)
		{
			debug("location identifier is NULL\n");
			return false;
		}

		// Get the location property from the window object (which is another object).
		//typedef bool (*NPN_GetPropertyProcPtr) (NPP npp, NPObject *obj, NPIdentifier  propertyName, NPVariant *result);
		sBrowserFuncs->getproperty(newapplication->npp, sWindowObj, identifier, &variantValue);
		// Get a pointer to the "location" object.
		NPObject *locationObj = variantValue.value.objectValue;
		// Create a "href" identifier.
		identifier = sBrowserFuncs->getstringidentifier("href");
		// Get the location property from the location object.
		// bool b2 = NPN_GetProperty( obj->npp, locationObj, identifier, &variantValue );
		//typedef bool (*NPN_SetPropertyProcPtr) (NPP npp, NPObject *obj, NPIdentifier  propertyName, const NPVariant *value);
		sBrowserFuncs->getproperty( newapplication->npp, locationObj, identifier, &variantValue1 );
		print_NPVariant(&variantValue1);

		bool b2 = sBrowserFuncs->setproperty( newapplication->npp, locationObj, identifier, &appUrl);
		print_NPVariant(&appUrl);

		sBrowserFuncs->memfree(appUrl.value.stringValue.UTF8Characters);
		memset(&appUrl, 0, sizeof(NPVariant));

#endif

		fctresult = true;
    }
    else if (name == v_APPLICATIONMethodIdentifiers[kAPPLICATION_ID_METHOD_DESTROYAPPLICATION])
    {
		APPLICATION_Invoke_DestroyApplication(obj, args, argCount);
		fctresult = true;
    }
    else
#endif		
	if (name == v_APPLICATIONMethodIdentifiers[kAPPLICATION_ID_METHOD_HIDE])
    {
		APPLICATION_Invoke_Hide(obj, args, argCount);
		fctresult = true;
    }
	else if (name == v_APPLICATIONMethodIdentifiers[kAPPLICATION_ID_METHOD_SHOW])
    {
		APPLICATION_Invoke_Show(obj, args, argCount);
		fctresult = true;
    }
#if 0	
	else if (name == v_APPLICATIONMethodIdentifiers[kAPPLICATION_ID_METHOD_ACTIVATE])
	{
		APPLICATION_Invoke_Activate(obj, args, argCount);
		fctresult = true;
	}
	else if (name == v_APPLICATIONMethodIdentifiers[kAPPLICATION_ID_METHOD_ACTIVATEINPUT])
	{
		APPLICATION_Invoke_ActivateInput(obj, args, argCount);
		fctresult = true;
	}
#endif	
    else
    {
        fctresult = false;
    }
    return fctresult;
	
}

 bool        APPLICATION_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    TRACEINFO;
    return true;
}

  bool        APPLICATION_HasProperty(NPObject* obj, NPIdentifier name)
{
    //TRACEINFO;
    bool result = false;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);

    int i = 0;
    while ((i < kAPPLICATION_NUM_PROPERTY_IDENTIFIERS) && (result == false))
    {
        if ( name == v_APPLICATIONPropertyIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }
    if (!result)
        debug("APPLICATION_HasProperty:%s\n", utf8propertyname);
    return result;
}

bool        APPLICATION_GetProperty(NPObject* obj, NPIdentifier name, NPVariant* result)
{
    TRACEINFO;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug("APPLICATION_GetProperty:%s\n", utf8propertyname);

    bool fctresult = false;
    NPObj_Application* AppliObj = (NPObj_Application*)obj;

    if (name == v_APPLICATIONPropertyIdentifiers[kAPPLICATION_ID_PROPERTY_PRIVATEDATA])
    {
		sBrowserFuncs->retainobject(AppliObj->privateData);
		OBJECT_TO_NPVARIANT(AppliObj->privateData, *result);
    	fctresult = true;
    }
    else  if (name == v_APPLICATIONPropertyIdentifiers[kAPPLICATION_ID_PROPERTY_VISIBLE])
    {
		BOOLEAN_TO_NPVARIANT(AppliObj->visible, *result);
		fctresult = true;
    }
    

    return fctresult;
}

  bool        APPLICATION_SetProperty(NPObject *obj, NPIdentifier name, const NPVariant *value)
{
    TRACEINFO;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug("APPLICATION_SetProperty:%s\n", utf8propertyname);
    return true;
}

 bool        APPLICATION_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    TRACEINFO;
    return true;
}


 bool        APPLICATION_Enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    TRACEINFO;
    return true;
}

/** implementation **/

void APPLICATION_Invoke_CreateApplication(NPObject* obj,const NPVariant* args, uint32_t argCount)
{
	TRACEINFO;
	NOTIMPLEMENTED;
}

void APPLICATION_Invoke_DestroyApplication(NPObject* obj,const NPVariant* args, uint32_t argCount)
{
	TRACEINFO;
	NOTIMPLEMENTED;
}

void APPLICATION_Invoke_Show(NPObject* obj,const NPVariant* args, uint32_t argCount)
{
	//TRACEINFO;
    debug("APPLICATION_Invoke_Show argCount:%d\n", argCount);
	NPObj_Application* AppliObj = (NPObj_Application*)obj;
	OnAPPLICATION_Show();
	AppliObj->visible = true;
	NOTIMPLEMENTED;
}

void APPLICATION_Invoke_Hide(NPObject* obj,const NPVariant* args, uint32_t argCount)
{
	TRACEINFO;

	NPObj_Application* AppliObj = (NPObj_Application*)obj;
	OnAPPLICATION_Hide();
	AppliObj->visible = false;
	NOTIMPLEMENTED;
}

void APPLICATION_Invoke_Activate(NPObject* obj,const NPVariant* args, uint32_t argCount)
{
	TRACEINFO;
	NOTIMPLEMENTED;
	debug("################# APPLICATION_Invoke_Activate argCount %d type %d\n",argCount, args[0].type);
	if (argCount != 0)
	switch(args[0].type)
	{
		case NPVariantType_Void :
			debug("################# NPVariantType_Void\n");
			break;		
		case NPVariantType_Null :
			debug("################# NPVariantType_Null\n");
			break;
		case NPVariantType_Bool :
		{
			bool val = NPVARIANT_TO_BOOLEAN(args[0]);
			debug("################# NPVariantType_Bool : %d\n", val);
			break;
		}
		case NPVariantType_Int32 :
		{
			int val = NPVARIANT_TO_INT32(args[0]);
			debug("################# NPVariantType_Int32 : %d\n", val);
			break;
		}
		case NPVariantType_Double :
		{
			int val = NPVARIANT_TO_DOUBLE(args[0]);
			debug("################# NPVariantType_Double : %d\n", val );
			break;
		}
		case NPVariantType_String :
			debug("################# NPVariantType_String : %s\n", NPVARIANT_TO_STRING(args[0]));
			break;
		case NPVariantType_Object :
			debug("################# NPVariantType_Object\n");

			break;
	}
}

void APPLICATION_Invoke_ActivateInput(NPObject* obj,const NPVariant* args, uint32_t argCount)
{
	TRACEINFO;
	NOTIMPLEMENTED;
	debug("################# APPLICATION_Invoke_ActivateInput argCount %d type %d\n",argCount, args[0].type);
	if (argCount != 0)
	switch(args[0].type)
	{
		case NPVariantType_Void :
			debug("################# NPVariantType_Void\n");
			break;		
		case NPVariantType_Null :
			debug("################# NPVariantType_Null\n");
			break;
		case NPVariantType_Bool :
		{
			bool val = NPVARIANT_TO_BOOLEAN(args[0]);
			debug("################# NPVariantType_Bool : %d\n", val);
			break;
		}
		case NPVariantType_Int32 :
		{
			int val = NPVARIANT_TO_INT32(args[0]);
			debug("################# NPVariantType_Int32 : %d\n", val);
			break;
		}
		case NPVariantType_Double :
		{
			int val = NPVARIANT_TO_DOUBLE(args[0]);
			debug("################# NPVariantType_Double : %d\n", val );
			break;
		}
		case NPVariantType_String :
			debug("################# NPVariantType_String : %s\n", NPVARIANT_TO_STRING(args[0]));
			break;
		case NPVariantType_Object :
			debug("################# NPVariantType_Object\n");

			break;
	}


	
}

