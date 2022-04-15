
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
#define kPROGRAMMES_ID_PROPERTY_NAME		    	0
#define kPROGRAMMES_ID_PROPERTY_LONGNAME   			1
#define kPROGRAMMES_ID_PROPERTY_DESCRIPTION   		2
#define kPROGRAMMES_ID_PROPERTY_LONGDESCRIPTION		3
#define kPROGRAMMES_ID_PROPERTY_STARTTIME		    4
#define kPROGRAMMES_ID_PROPERTY_DURATION			5
#define kPROGRAMMES_ID_PROPERTY_CHANNELID	        6
#define kPROGRAMMES_ID_PROPERTY_EPISODE				7
#define kPROGRAMMES_ID_PROPERTY_TOTALEPISODE		8
#define kPROGRAMMES_ID_PROPERTY_PROGRAMID			9
#define kPROGRAMMES_ID_PROPERTY_PROGRAMIDTYPE		10
#define kPROGRAMMES_NUM_PROPERTY_IDENTIFIERS           11
#endif
enum kPROGRAMMES_ID_PROPERTY {
	kPROGRAMMES_ID_PROPERTY_NAME,
	kPROGRAMMES_ID_PROPERTY_LONGNAME,
	kPROGRAMMES_ID_PROPERTY_DESCRIPTION,
	kPROGRAMMES_ID_PROPERTY_LONGDESCRIPTION,
	kPROGRAMMES_ID_PROPERTY_STARTTIME,
	kPROGRAMMES_ID_PROPERTY_DURATION,
	kPROGRAMMES_ID_PROPERTY_CHANNELID,
	kPROGRAMMES_ID_PROPERTY_EPISODE,
	kPROGRAMMES_ID_PROPERTY_TOTALEPISODE,
	kPROGRAMMES_ID_PROPERTY_PROGRAMID,
	kPROGRAMMES_ID_PROPERTY_PROGRAMIDTYPE,
	kPROGRAMMES_NUM_PROPERTY_IDENTIFIERS
};


#define kPROGRAMMES_ID_METHOD_SETVALUE               	        0
#define kPROGRAMMES_NUM_METHOD_IDENTIFIERS                	    1

bool            v_bPROGRAMMESIdentifiersInitialized = false;

NPIdentifier    v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_NUM_PROPERTY_IDENTIFIERS];
const NPUTF8 *  v_PROGRAMMESPropertyNames[kPROGRAMMES_NUM_PROPERTY_IDENTIFIERS] = {
	"name",
    "longname",
    "description",
    "longdescription",
	"startTime",
    "duration",
    "channelID",
    "episode",
    "totalEpisodes",
    "programmeID",
    "programmeIDType"
    };

NPIdentifier    v_PROGRAMMESMethodIdentifiers[kPROGRAMMES_NUM_METHOD_IDENTIFIERS];
const NPUTF8 *  v_PROGRAMMESMethodNames[kPROGRAMMES_NUM_METHOD_IDENTIFIERS] = {
	"setValue"
	};

static  void    PROGRAMMESinitializeIdentifiers(void)
{
    sBrowserFuncs->getstringidentifiers( v_PROGRAMMESPropertyNames, kPROGRAMMES_NUM_PROPERTY_IDENTIFIERS, v_PROGRAMMESPropertyIdentifiers );
    sBrowserFuncs->getstringidentifiers( v_PROGRAMMESMethodNames,   kPROGRAMMES_NUM_METHOD_IDENTIFIERS,   v_PROGRAMMESMethodIdentifiers );
}


NPClass  stPROGRAMMESclass;
NPClass* pPROGRAMMESclass = NULL;

NPClass* fillPROGRAMMESpclass(void)
{
    TRACEINFO;
    if (pPROGRAMMESclass == NULL)
    {
        stPROGRAMMESclass.allocate          		= PROGRAMMES_Allocate;
        stPROGRAMMESclass.deallocate        	= PROGRAMMES_Deallocate;
        stPROGRAMMESclass.invalidate        	= PROGRAMMES_Invalidate;
        stPROGRAMMESclass.hasMethod         = NULL;
        stPROGRAMMESclass.invoke            		= NULL;
        stPROGRAMMESclass.invokeDefault      = NULL;
        stPROGRAMMESclass.hasProperty         = PROGRAMMES_HasProperty;
        stPROGRAMMESclass.getProperty       	 = PROGRAMMES_GetProperty;
        stPROGRAMMESclass.setProperty       	 = NULL;
        stPROGRAMMESclass.removeProperty    = NULL;
        stPROGRAMMESclass.enumerate            = PROGRAMMES_Enumerate;
        pPROGRAMMESclass = &stPROGRAMMESclass;
    }

    return pPROGRAMMESclass;
}


NPObject *          PROGRAMMES_Allocate(NPP npp, NPClass *theClass)
{
    TRACEINFO;

    NPObject* result;

    if (!v_bPROGRAMMESIdentifiersInitialized)
    {
        v_bPROGRAMMESIdentifiersInitialized = true;
        PROGRAMMESinitializeIdentifiers();
    }

	NPObj_Programmes* programmes = (NPObj_Programmes *)MEMALLOC(sizeof(NPObj_Programmes));
	programmes->npp = npp;

	send_cmd(OOIF_GET_CURRENT_CHANNEL, (void*) programmes);
	
	result = (NPObject*)programmes;
	/*OnPROGRAMMES_Allocate();*/
    return  result;
}


void        PROGRAMMES_Deallocate(NPObject* obj)
{
    TRACEINFO;
    NPObj_Programmes* programmesobj = (NPObj_Programmes*)obj;
    //MEMFREE(programmesobj->value);
    //MEMFREE(programmesobj->maximumValue);
    MEMFREE(programmesobj);
    return;
}

void        PROGRAMMES_Invalidate(NPObject* obj)
{
    TRACEINFO;
    return;
}

bool        PROGRAMMES_HasMethod(NPObject* obj, NPIdentifier name)
{
    TRACEINFO;
	bool result = false;
    int i = 0;
    NPUTF8* utf8methodname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    while ((i < kPROGRAMMES_NUM_METHOD_IDENTIFIERS) && (result == false))
    {
        if ( name == v_PROGRAMMESMethodIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }

    return result;
}

bool        PROGRAMMES_Invoke(NPObject* obj, NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    TRACEINFO;
    bool fctresult = false;
    if (name == v_PROGRAMMESMethodIdentifiers[kPROGRAMMES_ID_METHOD_SETVALUE])
    {
		PROGRAMMES_Invoke_SetValue(obj, args, argCount, result);
		fctresult = true;
    }
    else
    {
    	fctresult = false;
    }
    return fctresult;
}

bool        PROGRAMMES_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    TRACEINFO;
    return true;
}

bool        PROGRAMMES_HasProperty(NPObject* obj, NPIdentifier name)
{
    //TRACEINFO;
    bool result = false;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);

    int i = 0;
    while ((i < kPROGRAMMES_NUM_PROPERTY_IDENTIFIERS) && (result == false))
    {
        if ( name == v_PROGRAMMESPropertyIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }
    if (!result)
        debug(stderr, "PROGRAMMES_HasProperty:%s\n", utf8propertyname);
    return result;
}

bool        PROGRAMMES_GetProperty(NPObject* obj, NPIdentifier name, NPVariant* result)
{
    TRACEINFO;
    bool fctresult = false;
    NPObj_Programmes* programmesobj = (NPObj_Programmes*)obj;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug("PROGRAMMES_GetProperty:%s\n", utf8propertyname);


#if 0
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_CHANNELTYPE])
	{
		DOUBLE_TO_NPVARIANT(programmesobj->channelType, *result);
    	fctresult = true;
	}
	else
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_IDTYPE])
	{
		DOUBLE_TO_NPVARIANT(programmesobj->idType, *result);
    	fctresult = true;
	}
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_CCID])
	{
		DOUBLE_TO_NPVARIANT(programmesobj->ccid, *result);
    	fctresult = true;
	}
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_TUNERID])
	{
		DOUBLE_TO_NPVARIANT(programmesobj->tunerID, *result);
    	fctresult = true;
	}
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_ONID])
	{
		DOUBLE_TO_NPVARIANT(programmesobj->onid, *result);
    	fctresult = true;
	}
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_TSID])
	{
		DOUBLE_TO_NPVARIANT(programmesobj->tsid, *result);
    	fctresult = true;
	}
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_SID])
	{
		DOUBLE_TO_NPVARIANT(programmesobj->sid, *result);
    	fctresult = true;
	}
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_SOURCEID])
	{
		DOUBLE_TO_NPVARIANT(programmesobj->sourceID, *result);
    	fctresult = true;
	}
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_FREQ])
	{
		DOUBLE_TO_NPVARIANT(programmesobj->freq, *result);
    	fctresult = true;
	}
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_CNI])
	{
		DOUBLE_TO_NPVARIANT(programmesobj->cni, *result);
    	fctresult = true;
	}
#endif
	if (name == v_PROGRAMMESPropertyIdentifiers[kPROGRAMMES_ID_PROPERTY_NAME])
	{
		int len = strlen(programmesobj->name);
		if (len)
		{
			NPUTF8  *utf8_chars = (NPUTF8  *) MEMALLOC(len);
			memset( utf8_chars, 0, len );
			strncpy( utf8_chars, programmesobj->name, len);
			STRINGZ_TO_NPVARIANT(utf8_chars, *result);
			fctresult = true;
		}
	}

	if (fctresult)
	{

	}
    return fctresult;
}

bool        PROGRAMMES_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    TRACEINFO;
    return true;
}


bool        PROGRAMMES_Enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    TRACEINFO;
    return true;
}


/** implementation **/
void PROGRAMMES_Invoke_SetValue(NPObject* obj,const NPVariant* args, uint32_t argCount, NPVariant *result)
{
	TRACEINFO;
	//~ NOTIMPLEMENTED; // in progress
	double param;
	NPObj_Programmes* programmesobj = (NPObj_Programmes*)obj;
#if 0
	double* value = programmesobj->value;
	debug("nb args : %i, %f\n", argCount,NPVARIANT_TO_DOUBLE(args[0]) );
	if (!NPVARIANT_IS_DOUBLE(args[0])) {
		debug("\t%s : error\n",__FUNCTION__);
		return;
    }
    else {
		param = NPVARIANT_TO_DOUBLE( args[0] );
		debug("\t%s : keymask:  %f\n", __FUNCTION__, param);
		*value = param;
		OnPROGRAMMES_SetValue(param);
	}
#endif
}
