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
#include "videobroadcast.h"
#include "currentchannelclass.h"
#include "hbbtvcommonutil.h"

#if 0
#define kVIDBRC_ID_PROPERTY_WIDTH	                   	0
#define kVIDBRC_ID_PROPERTY_HEIGHT	                  	1
#define kVIDBRC_ID_PROPERTY_FULLSCREEN					2
#define kVIDBRC_ID_PROPERTY_ONCHANNELCHANGEERROR		3
#define kVIDBRC_ID_PROPERTY_PLAYSTATE					4
#define kVIDBRC_ID_PROPERTY_ONPLAYSTATECHANGE			5
#define kVIDBRC_ID_PROPERTY_ONCHANNELCHANGESUCCEEDED	6
#define kVIDBRC_ID_PROPERTY_ONFULLSCREENCHANGE			7
#define kVIDBRC_ID_PROPERTY_ONFOCUS						8
#define kVIDBRC_ID_PROPERTY_ONBLUR						9
#define kVIDBRC_ID_PROPERTY_CURRENTCHANNEL				10

#define kVIDBRC_NUM_PROPERTY_IDENTIFIERS                11
#else

enum kVIDBRC_ID_PROPERTY {
#if 0
	kVIDBRC_ID_PROPERTY_WIDTH,
	kVIDBRC_ID_PROPERTY_HEIGHT,
#endif
	kVIDBRC_ID_PROPERTY_FULLSCREEN,
	kVIDBRC_ID_PROPERTY_ONCHANNELCHANGEERROR,
	kVIDBRC_ID_PROPERTY_PLAYSTATE,
	kVIDBRC_ID_PROPERTY_ONPLAYSTATECHANGE,
	kVIDBRC_ID_PROPERTY_ONCHANNELCHANGESUCCEEDED,
	kVIDBRC_ID_PROPERTY_ONFULLSCREENCHANGE,
	kVIDBRC_ID_PROPERTY_ONFOCUS,
	kVIDBRC_ID_PROPERTY_ONBLUR,
	kVIDBRC_ID_PROPERTY_PROGRAMMES,
	kVIDBRC_ID_PROPERTY_CURRENTCHANNEL,
		
	kVIDBRC_NUM_PROPERTY_IDENTIFIERS

};
#if 0
#define kVIDBRC_ID_PROPERTY_FULLSCREEN					0
#define kVIDBRC_ID_PROPERTY_ONCHANNELCHANGEERROR		1
#define kVIDBRC_ID_PROPERTY_PLAYSTATE					2
#define kVIDBRC_ID_PROPERTY_ONPLAYSTATECHANGE			3
#define kVIDBRC_ID_PROPERTY_ONCHANNELCHANGESUCCEEDED	4
#define kVIDBRC_ID_PROPERTY_ONFULLSCREENCHANGE			5
#define kVIDBRC_ID_PROPERTY_ONFOCUS						6
#define kVIDBRC_ID_PROPERTY_ONBLUR						7
#define kVIDBRC_ID_PROPERTY_CURRENTCHANNEL				8

#define kVIDBRC_NUM_PROPERTY_IDENTIFIERS                9
#endif
#endif
#if 0
#define kVIDBRC_ID_PROPERTY_GET_ATTRIBUTE                      11
#define kVIDBRC_ID_PROPERTY_CLASS_NAME                        12
#define kVIDBRC_NUM_PROPERTY_IDENTIFIERS                13
#endif
#define kVIDBRC_ID_METHOD_GETCHANNELCONFIG              0
#define kVIDBRC_ID_METHOD_BINDTOCURRENTCHANNEL          1
#define kVIDBRC_ID_METHOD_CREATECHANNELOBJECT			2
#define kVIDBRC_ID_METHOD_CREATECHANNELOBJECT2   	    3
#define kVIDBRC_ID_METHOD_SETCHANNEL              		4
#define kVIDBRC_ID_METHOD_PREVCHANNEL					5
#define kVIDBRC_ID_METHOD_NEXTCHANNEL					6
#define kVIDBRC_ID_METHOD_SETFULLSCREEN					7
#define kVIDBRC_ID_METHOD_GETVOLUME						8
#define kVIDBRC_ID_METHOD_RELEASE						9
#define kVIDBRC_ID_METHOD_STOP							10
/*#define kVIDBRC_ID_METHOD_PLAY							11*/
#define kVIDBRC_NUM_METHOD_IDENTIFIERS					11

/* HbbTV-errata-1 Table A.4 */
enum ePlayState {    
    ePlayState_Unrealized = 0,         
    ePlayState_Connecting,
    ePlayState_Presenting,
    ePlayState_Stop
};

bool            v_bVIDBRCIdentifiersInitialized = false;

NPIdentifier    v_VIDBRCPropertyIdentifiers[kVIDBRC_NUM_PROPERTY_IDENTIFIERS];
const NPUTF8 *  v_VIDBRCPropertyNames[kVIDBRC_NUM_PROPERTY_IDENTIFIERS] = {
#if 0
	"width",
	"height",
#endif	
	"fullScreen",
	"onChannelChangeError",
	"playState",
	"onPlayStateChange",
	"onChannelChangeSucceeded",
	"onFullScreenChange",
	"onFocus",
	"onBlur",
	"programmes",
	"currentChannel"/*,*/
#if 0    
 /*silver*/
       "className", 
       "getAttribute"
	   "currentChannel"
#endif
	};

NPIdentifier    v_VIDBRCMethodIdentifiers[kVIDBRC_NUM_METHOD_IDENTIFIERS];
const NPUTF8 *  v_VIDBRCMethodNames[kVIDBRC_NUM_METHOD_IDENTIFIERS] = {
	"getChannelConfig",
	"bindToCurrentChannel",
	"createChannelObject",
	"createChannelObject2",
	"setChannel",
	"prevChannel",
	"nextChannel",
	"setFullScreen",
	"getVolume",
	"release",
	"stop"
	
/*	"play"*/
};

static  void    VIDBRCinitializeIdentifiers(void)
{
    sBrowserFuncs->getstringidentifiers( v_VIDBRCPropertyNames, kVIDBRC_NUM_PROPERTY_IDENTIFIERS, v_VIDBRCPropertyIdentifiers );
    sBrowserFuncs->getstringidentifiers( v_VIDBRCMethodNames,   kVIDBRC_NUM_METHOD_IDENTIFIERS,   v_VIDBRCMethodIdentifiers );
}

NPClass  stVIDBRCclass;
NPClass* pVIDBRCclass = NULL;

NPClass* fillVIDBRCpclass(void)
{
    TRACEINFO;
    if (pVIDBRCclass == NULL)
    {
        stVIDBRCclass.allocate          = VIDBRC_Allocate;
        stVIDBRCclass.deallocate        = VIDBRC_Deallocate;
        stVIDBRCclass.invalidate        = VIDBRC_Invalidate;
        stVIDBRCclass.hasMethod         = VIDBRC_HasMethod;
        stVIDBRCclass.invoke            = VIDBRC_Invoke;
        stVIDBRCclass.invokeDefault     = VIDBRC_InvokeDefault;
        stVIDBRCclass.hasProperty       = VIDBRC_HasProperty;
        stVIDBRCclass.getProperty       = VIDBRC_GetProperty;
        stVIDBRCclass.setProperty       = VIDBRC_SetProperty;
        stVIDBRCclass.removeProperty    = VIDBRC_RemoveProperty;
        stVIDBRCclass.enumerate         = VIDBRC_Enumerate;
        pVIDBRCclass = &stVIDBRCclass;
    }

    return pVIDBRCclass;
}


NPObject *          VIDBRC_Allocate(NPP npp, NPClass *theClass)
{
    TRACEINFO;


    if (!v_bVIDBRCIdentifiersInitialized)
    {
        v_bVIDBRCIdentifiersInitialized = true;
        VIDBRCinitializeIdentifiers();
    }

	NPObj_VidBrc* newvidbrc = (NPObj_VidBrc*)MEMALLOC(sizeof(NPObj_VidBrc));
    memset(newvidbrc, 0, sizeof(NPObj_VidBrc));

    newvidbrc->play_state = ePlayState_Unrealized;

    newvidbrc->npp = npp;

    return (NPObject*) newvidbrc;
}

void		VIDBRC_Deallocate(NPObject* obj)
{
    TRACEINFO;
    OnVIDBRC_SetFullScreen(true);
    NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
    /*
    if (vidbrc->pcArg_onChannelChangeSucceeded)
    {
        MEMFREE(vidbrc->pcArg_onChannelChangeSucceeded);
 //       vidbrc->pcArg_onChannelChangeSucceeded = NULL;
    }
	if ( vidbrc->onChannelChangeSucceeded )
	{
		// sBrowserFuncs->releaseobject(vidbrc->onChannelChangeSucceeded);
	//	vidbrc->onChannelChangeSucceeded = NULL;
	}

    */
	
    MEMFREE(obj);
    return;
}

  void        VIDBRC_Invalidate(NPObject* obj)
{
    TRACEINFO;
    return;
}

  bool        VIDBRC_HasMethod(NPObject* obj, NPIdentifier name)
{
    //TRACEINFO;

    bool result = false;
    int i = 0;
    NPUTF8* utf8methodname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug( "VIDBRC_HasMethod:%s\n", utf8methodname);

    while ((i < kVIDBRC_NUM_METHOD_IDENTIFIERS) && (result == false))
    {
        if ( name == v_VIDBRCMethodIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }

    return result;
}

bool        VIDBRC_Invoke(NPObject* obj, NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    //TRACEINFO;
	bool fctresult = false;

    NPUTF8* utf8methodname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug( "VIDBRC_Invoke:%s\n", utf8methodname);

    if (name == v_VIDBRCMethodIdentifiers[kVIDBRC_ID_METHOD_GETCHANNELCONFIG])
    {
    	VIDBRC_Invoke_getChannelConfig((NPObj_VidBrc*)obj, args, argCount, result);
    	fctresult = true;
    }

    else if (name == v_VIDBRCMethodIdentifiers[kVIDBRC_ID_METHOD_SETFULLSCREEN])
    {
    	VIDBRC_Invoke_setFullScreen((NPObj_VidBrc*)obj, args, argCount);
    	fctresult = true;
    }

    else if (name == v_VIDBRCMethodIdentifiers[kVIDBRC_ID_METHOD_BINDTOCURRENTCHANNEL])
    {
    	VIDBRC_Invoke_bindToCurrentChannel(obj, args, argCount);
    	fctresult = true;
    }

    else if (name == v_VIDBRCMethodIdentifiers[kVIDBRC_ID_METHOD_STOP])
    {
    	VIDBRC_Invoke_Stop((NPObj_VidBrc*)obj, args, argCount, result);
    	fctresult = true;
    }
    else if (name == v_VIDBRCMethodIdentifiers[kVIDBRC_ID_METHOD_SETCHANNEL])
    {    
        VIDBRC_Invoke_setChannel((NPObj_VidBrc*)obj, args, argCount, result);
        fctresult = true;        
    }
    else if (name == v_VIDBRCMethodIdentifiers[kVIDBRC_ID_METHOD_PREVCHANNEL])
    {
        VIDBRC_Invoke_prevChannel((NPObj_VidBrc*)obj, args, argCount, result); 
        fctresult = true;       
    }       
    else if (name == v_VIDBRCMethodIdentifiers[kVIDBRC_ID_METHOD_NEXTCHANNEL])
    {
        VIDBRC_Invoke_nextChannel((NPObj_VidBrc*)obj, args, argCount, result);
        fctresult = true;        
    }    
    else if (name == v_VIDBRCMethodIdentifiers[kVIDBRC_ID_METHOD_RELEASE])
    {
        VIDBRC_Invoke_release((NPObj_VidBrc*)obj, args, argCount, result);
        fctresult = true;              
    }    

#if 0
    if (name == v_VIDBRCMethodIdentifiers[kVIDBRC_ID_METHOD_PLAY])
    {
    	VIDBRC_Invoke_Play((NPObj_VidBrc*)obj, args, argCount, result);
    	fctresult = true;
    }
#endif

    return fctresult;
}

 bool        VIDBRC_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    TRACEINFO;

    return true;
}

  bool        VIDBRC_HasProperty(NPObject* obj, NPIdentifier name)
{
    //TRACEINFO;    
    bool result = false;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug( "VIDBRC_HasProperty:%s\n", utf8propertyname);

    int i = 0;
    while ((i < kVIDBRC_NUM_PROPERTY_IDENTIFIERS) && (result == false))
    {
        if ( name == v_VIDBRCPropertyIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }
    if (!result)
        debug( "VIDBRC_HasProperty:%s\n", utf8propertyname);
    return result;
}

 bool        VIDBRC_GetProperty(NPObject* obj, NPIdentifier name, NPVariant* result)
{
    //TRACEINFO;
    bool fctresult = false;
    NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug( "VIDBRC_GetProperty:%s\n", utf8propertyname);
#if 0
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_WIDTH]) {
		DOUBLE_TO_NPVARIANT(vidbrc->width,*result);
    	fctresult = true;
    } else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_HEIGHT]) {
		DOUBLE_TO_NPVARIANT(vidbrc->height,*result);
		fctresult = true;
	} else
#endif
	
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_FULLSCREEN]) {
		BOOLEAN_TO_NPVARIANT(vidbrc->fullscreen, *result);
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_PLAYSTATE]) {
          INT32_TO_NPVARIANT(vidbrc->play_state,*result);
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONPLAYSTATECHANGE]) {
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONCHANNELCHANGEERROR]) {
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONCHANNELCHANGESUCCEEDED]) {
   /*         if ( vidbrc->pcArg_onChannelChangeSucceeded != NULL )
            {
               char*  dup_str = strdup(vidbrc->pcArg_onChannelChangeSucceeded);
               NPString npstr = { dup_str, (uint32_t)(strlen(dup_str)) };
               result->type = NPVariantType_String;
               result->value.stringValue = npstr;
               fctresult = true;
            }
            else
            {
                OBJECT_TO_NPVARIANT(vidbrc->onPlayStateChange,*result);
            }*/
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONFULLSCREENCHANGE]) {
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONFOCUS]) {
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONBLUR]) {
		fctresult = true;
	} else
	if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_CURRENTCHANNEL]) {
		debug( "==================> kVIDBRC_ID_PROPERTY_CURRENTCHANNEL\n");
#if 1

		NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
		NPP npp = vidbrc->npp;

		vidbrc->currentChannel = sBrowserFuncs->createobject(npp, fillCURRENTCHANNELpclass());
        result->type = NPVariantType_Object;
        result->value.objectValue = vidbrc->currentChannel;
		fctresult = true;
#endif
	}else

	if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_PROGRAMMES]) {

		NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
		NPP npp = vidbrc->npp;

		vidbrc->programmes = sBrowserFuncs->createobject(npp, fillCURRENTCHANNELpclass());
        result->type = NPVariantType_Object;
        result->value.objectValue = vidbrc->programmes;

		fctresult = true;
	}
    return fctresult;
}

  bool        VIDBRC_SetProperty(NPObject *obj, NPIdentifier name, const NPVariant *value)
{
	TRACEINFO;
	bool fctresult = false;
	NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug( "VIDBRC_SetProperty:%s\n", utf8propertyname);


#if 0
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_WIDTH]) {
		vidbrc->width = NPVARIANT_TO_DOUBLE(*value);
		VIDBRC_setsize(vidbrc, vidbrc->width, vidbrc->height);
    	fctresult = true;
		debug("VIDBRC_SetProperty kVIDBRC_ID_PROPERTY_WIDTH %d \n", vidbrc->width);
    } else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_HEIGHT]) {
		vidbrc->height = NPVARIANT_TO_DOUBLE(*value);
		VIDBRC_setsize(vidbrc, vidbrc->width, vidbrc->height);
		fctresult = true;
		debug("VIDBRC_SetProperty kVIDBRC_ID_PROPERTY_HEIGHT %d \n", vidbrc->height);

	} else
#endif
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_FULLSCREEN]) {
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_PLAYSTATE]) {
          vidbrc->play_state = NPVARIANT_TO_INT32(*value);
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONPLAYSTATECHANGE]) {
           debug( "################# type %d\n", value->type);
           switch(value->type)
           {
               case NPVariantType_Void :
                   debug( "################# NPVariantType_Void\n");
                   break;      
               case NPVariantType_Null :
                   debug( "################# NPVariantType_Null\n");
                   break;
               case NPVariantType_Bool :
                   debug( "################# NPVariantType_Bool : %d\n", NPVARIANT_TO_BOOLEAN(*value));
                   break;
               case NPVariantType_Int32 :
                   debug( "################# NPVariantType_Int32 : %d\n", NPVARIANT_TO_INT32(*value));
                   break;
               case NPVariantType_Double :
                   debug( "################# NPVariantType_Double : %ld\n", NPVARIANT_TO_DOUBLE(*value));
                   break;
               case NPVariantType_String :
                   debug( "################# NPVariantType_String : %s\n", NPVARIANT_TO_STRING(*value));
                   break;
               case NPVariantType_Object :
                   debug( "################# NPVariantType_Object\n");
                   int *test = (int *)NPVARIANT_TO_OBJECT(*value);

                   debug( "################# NPVariantType_Object %d %d\n",test[1], test[2]);
                    vidbrc->play_state = test[1];
                   //set_playstate(test[1]);
               
                   break;
           }        
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONCHANNELCHANGEERROR]) {
		fctresult = true;
	} else
	if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONCHANNELCHANGESUCCEEDED]) {
    /*    if ( vidbrc->onChannelChangeSucceeded != NULL )
        {
            sBrowserFuncs->releaseobject(vidbrc->onChannelChangeSucceeded);
            vidbrc->onChannelChangeSucceeded = NULL;
        }
        if ( vidbrc->pcArg_onChannelChangeSucceeded )
        {
            MEMFREE( vidbrc->pcArg_onChannelChangeSucceeded );
            vidbrc->pcArg_onChannelChangeSucceeded = NULL;
        }
        vidbrc->onChannelChangeSucceeded = NPVARIANT_TO_OBJECT(*value);
        sBrowserFuncs->retainobject( vidbrc->onChannelChangeSucceeded);*/
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONFULLSCREENCHANGE]) {
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONFOCUS]) {
		fctresult = true;
	} else
    if (name == v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_ONBLUR]) {
		fctresult = true;
	}
    return fctresult;
}

 bool        VIDBRC_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    TRACEINFO;
    return true;
}


 bool        VIDBRC_Enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    TRACEINFO;
    return true;
}

/** implementation of methods **/
void		VIDBRC_Invoke_getChannelConfig(NPObj_VidBrc* obj,const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    TRACEINFO;
	NOTIMPLEMENTED;
	NULL_TO_NPVARIANT(*result);
	return;
}

void		VIDBRC_Invoke_setFullScreen(NPObj_VidBrc* obj,const NPVariant* args, uint32_t argCount)
{
	TRACEINFO;
	//NOTIMPLEMENTED; ///inprogress

	NPP npp = obj->npp;
	int param;
	if (argCount != 1 || !NPVARIANT_IS_BOOLEAN(args[0])) {
		return;
    }
    else {
		param = NPVARIANT_TO_BOOLEAN( args[0] );
		if (param != obj->fullscreen)
		{
			if (param){
				obj->fullscreen = true;
				VIDBRC_setsize(obj, FULLSIZE_WIDTH, FULLSIZE_HEIGHT);
			}
			else{
				obj->fullscreen = false;
			}

			HBBTVPluginData* pdata = (HBBTVPluginData*)npp->pdata;
			if(pdata){
				if (pdata->window){
					OnVIDBRC_SetFullScreen(param);
				}
			}
		}
}

}

void		VIDBRC_Invoke_bindToCurrentChannel(NPObject* obj,const NPVariant* args, uint32_t argCount)
{
    NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
	TRACEINFO;
	NOTIMPLEMENTED;
	//OnBindToCurrentChannel();
	//TODO :
	//

	send_cmd(OOIF_BROADCAST_PLAY, NULL);
	//sleep(1);
    vidbrc->play_state = ePlayState_Presenting;
}

/** implementation of intermediary function **/

void		VIDBRC_setsize(NPObj_VidBrc* obj,int32_t width, int32_t height)
{
            TRACEINFO;    
		///set the size
#if 0		
		debug( "1. ==================>VIDBRC_setsize\n");
		obj->width = width;
		obj->height = height;

		/// get the plugin object and his style object
		NPIdentifier    npIdent;
		NPVariant       npVar;
		NPObject *      npObjPlugin = NULL;
		NPObject *      npObjStyle = NULL;

		if (obj->npp)
		{
			sBrowserFuncs->getvalue( obj->npp ,
                                     NPNVPluginElementNPObject ,
                                     &npObjPlugin );
		}
		if (npObjPlugin)
		{
			npIdent = sBrowserFuncs->getstringidentifier( (const NPUTF8 *)"style" );
			sBrowserFuncs->getproperty( obj->npp ,
                                                  npObjPlugin,
                                                  npIdent ,
                                                  &npVar );
            npObjStyle = NPVARIANT_TO_OBJECT(npVar);
	   }
	   if (npObjStyle)
	   {
		   INT32_TO_NPVARIANT(width,npVar);
		   npIdent = v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_WIDTH];
		   sBrowserFuncs->setproperty( obj->npp,npObjStyle, npIdent, &npVar);
		   INT32_TO_NPVARIANT(height,npVar);
		   npIdent = v_VIDBRCPropertyIdentifiers[kVIDBRC_ID_PROPERTY_HEIGHT];
		   sBrowserFuncs->setproperty( obj->npp,npObjStyle, npIdent, &npVar);
	   }
#endif
	   debug( "2. ==================>VIDBRC_setsize\n");
	   
}


/** implementation of methods **/
void		VIDBRC_Invoke_Stop(NPObj_VidBrc* obj,const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
    int cmd_result;

    TRACEINFO;

	cmd_result = send_cmd(OOIF_BROADCAST_CHECK, NULL);
	if (cmd_result)
		cmd_result = send_cmd(OOIF_BROADCAST_STOP, NULL);
	
      vidbrc->play_state = ePlayState_Stop;
	BOOLEAN_TO_NPVARIANT(1, *result);
	
	return;
}

void        VIDBRC_Invoke_setChannel(NPObj_VidBrc* obj,const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
    int cmd_result;

    TRACEINFO;
   
      vidbrc->play_state = ePlayState_Connecting;
      //TODO : if channel is null       vidbrc->play_state = ePlayState_Unrealized;
    BOOLEAN_TO_NPVARIANT(1, *result);
    
    return;
}

void        VIDBRC_Invoke_nextChannel(NPObj_VidBrc* obj,const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
    int cmd_result;

    TRACEINFO;
  
    vidbrc->play_state = ePlayState_Connecting;
    BOOLEAN_TO_NPVARIANT(1, *result);
    
    return;
}

void        VIDBRC_Invoke_prevChannel(NPObj_VidBrc* obj,const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
    int cmd_result;

    TRACEINFO;
    
      vidbrc->play_state = ePlayState_Connecting;
    BOOLEAN_TO_NPVARIANT(1, *result);
    
    return;
}

void        VIDBRC_Invoke_release(NPObj_VidBrc* obj,const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    NPObj_VidBrc* vidbrc = (NPObj_VidBrc*)obj;
    int cmd_result;

    TRACEINFO;
    
      vidbrc->play_state = ePlayState_Unrealized;
    BOOLEAN_TO_NPVARIANT(1, *result);
    
    return;
}


void		VIDBRC_Invoke_Play(NPObj_VidBrc* obj,const NPVariant* args, uint32_t argCount, NPVariant* result)
{
	int cmd_result;
	int len;
	TRACEINFO;
	NOTIMPLEMENTED; ///inprogress

    NULL_TO_NPVARIANT(*result);
    return;    
}



