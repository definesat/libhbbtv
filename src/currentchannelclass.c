
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
#include "currentchannelclass.h"
#include "hbbtvcommonutil.h"

#if 0
Integer channelType
Integer idType
String ccid
String tunerID
Integer onid
Integer tsid
Integer sid
Integer sourceID
Integer freq
Integer cni
String name
Integer majorChannel
Integer minorChannel
String dsd
Boolean favourite
StringCollection favIDs
Boolean locked
Boolean manualBlock
String ipBroadcastID
Integer channelMaxBitRate
Integer channelTTR
#endif

#define kCURRENTCHANNEL_ID_PROPERTY_CHANNELTYPE		    0
#define kCURRENTCHANNEL_ID_PROPERTY_IDTYPE   					    1
#define kCURRENTCHANNEL_ID_PROPERTY_CCID   							2
#define kCURRENTCHANNEL_ID_PROPERTY_TUNERID   					3
#define kCURRENTCHANNEL_ID_PROPERTY_ONID   						    4
#define kCURRENTCHANNEL_ID_PROPERTY_TSID   							5
#define kCURRENTCHANNEL_ID_PROPERTY_SID   					        6
#define kCURRENTCHANNEL_ID_PROPERTY_SOURCEID   				7
#define kCURRENTCHANNEL_ID_PROPERTY_FREQ   						8
#define kCURRENTCHANNEL_ID_PROPERTY_CNI   							9
#define kCURRENTCHANNEL_ID_PROPERTY_NAME   						10

#if 1
#define kCURRENTCHANNEL_NUM_PROPERTY_IDENTIFIERS           11
#else
#define kCURRENTCHANNEL_ID_PROPERTY_MAJORCHANNEL  	    11
#define kCURRENTCHANNEL_ID_PROPERTY_MINORCHANNEL   		12
#define kCURRENTCHANNEL_ID_PROPERTY_DSD           		            13
#define kCURRENTCHANNEL_ID_PROPERTY_FAVOURITE                  14
#define kCURRENTCHANNEL_ID_PROPERTY_FAVIDS                         15
#define kCURRENTCHANNEL_ID_PROPERTY_LOCKED                       16
#define kCURRENTCHANNEL_ID_PROPERTY_MANUALBLOCK            17
#define kCURRENTCHANNEL_ID_PROPERTY_IPBROADCASTID           18
#define kCURRENTCHANNEL_ID_PROPERTY_CHANNELMAXBITRATE  19
#define kCURRENTCHANNEL_ID_PROPERTY_CHANNELTTR                20
#define kCURRENTCHANNEL_NUM_PROPERTY_IDENTIFIERS              21
#endif

#define kCURRENTCHANNEL_ID_METHOD_SETVALUE               	        0
#define kCURRENTCHANNEL_NUM_METHOD_IDENTIFIERS                	    1

bool            v_bCURRENTCHANNELIdentifiersInitialized = false;

NPIdentifier    v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_NUM_PROPERTY_IDENTIFIERS];
const NPUTF8 *  v_CURRENTCHANNELPropertyNames[kCURRENTCHANNEL_NUM_PROPERTY_IDENTIFIERS] = {
	"channelType",
    "idType",
    "ccid",
    "tunerID",
	"onid",
    "tsid",
    "sid",
    "sourceID",
    "freq",
    "cni",
    "name"
    };

NPIdentifier    v_CURRENTCHANNELMethodIdentifiers[kCURRENTCHANNEL_NUM_METHOD_IDENTIFIERS];
const NPUTF8 *  v_CURRENTCHANNELMethodNames[kCURRENTCHANNEL_NUM_METHOD_IDENTIFIERS] = {
	"setValue"
	};

static  void    CURRENTCHANNELinitializeIdentifiers(void)
{
    sBrowserFuncs->getstringidentifiers( v_CURRENTCHANNELPropertyNames, kCURRENTCHANNEL_NUM_PROPERTY_IDENTIFIERS, v_CURRENTCHANNELPropertyIdentifiers );
    sBrowserFuncs->getstringidentifiers( v_CURRENTCHANNELMethodNames,   kCURRENTCHANNEL_NUM_METHOD_IDENTIFIERS,   v_CURRENTCHANNELMethodIdentifiers );
}


NPClass  stCURRENTCHANNELclass;
NPClass* pCURRENTCHANNELclass = NULL;

NPClass* fillCURRENTCHANNELpclass(void)
{
    TRACEINFO;
    if (pCURRENTCHANNELclass == NULL)
    {
        stCURRENTCHANNELclass.allocate          		= CURRENTCHANNEL_Allocate;
        stCURRENTCHANNELclass.deallocate        	= CURRENTCHANNEL_Deallocate;
        stCURRENTCHANNELclass.invalidate        	= CURRENTCHANNEL_Invalidate;
        stCURRENTCHANNELclass.hasMethod         = NULL;
        stCURRENTCHANNELclass.invoke            		= NULL;
        stCURRENTCHANNELclass.invokeDefault      = NULL;
        stCURRENTCHANNELclass.hasProperty         = CURRENTCHANNEL_HasProperty;
        stCURRENTCHANNELclass.getProperty       	 = CURRENTCHANNEL_GetProperty;
        stCURRENTCHANNELclass.setProperty       	 = NULL;
        stCURRENTCHANNELclass.removeProperty    = NULL;
        stCURRENTCHANNELclass.enumerate            = CURRENTCHANNEL_Enumerate;
        pCURRENTCHANNELclass = &stCURRENTCHANNELclass;
    }

    return pCURRENTCHANNELclass;
}


NPObject *          CURRENTCHANNEL_Allocate(NPP npp, NPClass *theClass)
{
    TRACEINFO;

    NPObject* result;

    if (!v_bCURRENTCHANNELIdentifiersInitialized)
    {
        v_bCURRENTCHANNELIdentifiersInitialized = true;
        CURRENTCHANNELinitializeIdentifiers();
    }

	NPObj_CurrentChannel* newcurrentchannel = (NPObj_CurrentChannel *)MEMALLOC(sizeof(NPObj_CurrentChannel));
	newcurrentchannel->npp = npp;

	send_cmd(OOIF_GET_CURRENT_CHANNEL, (void*) newcurrentchannel);
	
	result = (NPObject*)newcurrentchannel;
	/*OnCURRENTCHANNEL_Allocate();*/
    return  result;
}


void        CURRENTCHANNEL_Deallocate(NPObject* obj)
{
    TRACEINFO;
    NPObj_CurrentChannel* currentchannelobj = (NPObj_CurrentChannel*)obj;
    //MEMFREE(currentchannelobj->value);
    //MEMFREE(currentchannelobj->maximumValue);
    MEMFREE(currentchannelobj);
    return;
}

void        CURRENTCHANNEL_Invalidate(NPObject* obj)
{
    TRACEINFO;
    return;
}

bool        CURRENTCHANNEL_HasMethod(NPObject* obj, NPIdentifier name)
{
    TRACEINFO;
	bool result = false;
    int i = 0;
    NPUTF8* utf8methodname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    while ((i < kCURRENTCHANNEL_NUM_METHOD_IDENTIFIERS) && (result == false))
    {
        if ( name == v_CURRENTCHANNELMethodIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }

    return result;
}

bool        CURRENTCHANNEL_Invoke(NPObject* obj, NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    TRACEINFO;
    bool fctresult = false;
    if (name == v_CURRENTCHANNELMethodIdentifiers[kCURRENTCHANNEL_ID_METHOD_SETVALUE])
    {
		CURRENTCHANNEL_Invoke_SetValue(obj, args, argCount, result);
		fctresult = true;
    }
    else
    {
    	fctresult = false;
    }
    return fctresult;
}

bool        CURRENTCHANNEL_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    TRACEINFO;
    return true;
}

bool        CURRENTCHANNEL_HasProperty(NPObject* obj, NPIdentifier name)
{
    //TRACEINFO;
    bool result = false;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);

    int i = 0;
    while ((i < kCURRENTCHANNEL_NUM_PROPERTY_IDENTIFIERS) && (result == false))
    {
        if ( name == v_CURRENTCHANNELPropertyIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }
    if (!result)
        debug(stderr, "CURRENTCHANNEL_HasProperty:%s\n", utf8propertyname);
    return result;
}

bool        CURRENTCHANNEL_GetProperty(NPObject* obj, NPIdentifier name, NPVariant* result)
{
    TRACEINFO;
    bool fctresult = false;
    NPObj_CurrentChannel* currentchannelobj = (NPObj_CurrentChannel*)obj;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug("CURRENTCHANNEL_GetProperty:%s\n", utf8propertyname);


#if 1
#if 0
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_CHANNELTYPE])
	{
		DOUBLE_TO_NPVARIANT(currentchannelobj->channelType, *result);
    	fctresult = true;
	}
	else
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_IDTYPE])
	{
		DOUBLE_TO_NPVARIANT(currentchannelobj->idType, *result);
    	fctresult = true;
	}
	else
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_CCID])
	{
		DOUBLE_TO_NPVARIANT(currentchannelobj->ccid, *result);
    	fctresult = true;
	}
	else
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_TUNERID])
	{
		DOUBLE_TO_NPVARIANT(currentchannelobj->tunerID, *result);
    	fctresult = true;
	}
	else
#endif
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_ONID])
	{
		DOUBLE_TO_NPVARIANT(currentchannelobj->onid, *result);
    	fctresult = true;
	}
	else
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_TSID])
	{
		DOUBLE_TO_NPVARIANT(currentchannelobj->tsid, *result);
    	fctresult = true;
	}
	else
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_SID])
	{
		DOUBLE_TO_NPVARIANT(currentchannelobj->sid, *result);
    	fctresult = true;
	}
#if 0	
	else
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_SOURCEID])
	{
		DOUBLE_TO_NPVARIANT(currentchannelobj->sourceID, *result);
    	fctresult = true;
	}
	else
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_FREQ])
	{
		DOUBLE_TO_NPVARIANT(currentchannelobj->freq, *result);
    	fctresult = true;
	}
	else
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_CNI])
	{
		DOUBLE_TO_NPVARIANT(currentchannelobj->cni, *result);
    	fctresult = true;
	}
	else
#endif		
#endif
	if (name == v_CURRENTCHANNELPropertyIdentifiers[kCURRENTCHANNEL_ID_PROPERTY_NAME])
	{
		int len = strlen(currentchannelobj->name);
		if (len)
		{
			NPUTF8  *utf8_chars = (NPUTF8  *) MEMALLOC(len);
			memset( utf8_chars, 0, len );
			strncpy( utf8_chars, currentchannelobj->name, len);
			STRINGZ_TO_NPVARIANT(utf8_chars, *result);
			fctresult = true;
		}
	}

	if (fctresult)
	{

	}
    return fctresult;
}

bool        CURRENTCHANNEL_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    TRACEINFO;
    return true;
}


bool        CURRENTCHANNEL_Enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    TRACEINFO;
    return true;
}


/** implementation **/
void CURRENTCHANNEL_Invoke_SetValue(NPObject* obj,const NPVariant* args, uint32_t argCount, NPVariant *result)
{
	TRACEINFO;
	//~ NOTIMPLEMENTED; // in progress
	double param;
	NPObj_CurrentChannel* currentchannelobj = (NPObj_CurrentChannel*)obj;
#if 0
	double* value = currentchannelobj->value;
	debug("nb args : %i, %f\n", argCount,NPVARIANT_TO_DOUBLE(args[0]) );
	if (!NPVARIANT_IS_DOUBLE(args[0])) {
		debug("\t%s : error\n",__FUNCTION__);
		return;
    }
    else {
		param = NPVARIANT_TO_DOUBLE( args[0] );
		debug("\t%s : keymask:  %f\n", __FUNCTION__, param);
		*value = param;
		OnCURRENTCHANNEL_SetValue(param);
	}
#endif
}
