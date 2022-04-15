#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "videoplayer.h"

#include "hbbtvcommonutil.h"

#define PLAY_STOP_TO_LIVE

#if 0
#define kVIDPLY_ID_PROPERTY_TYPE                         	0
#define kVIDPLY_ID_PROPERTY_WIDTH	                   		1
#define kVIDPLY_ID_PROPERTY_HEIGHT	                 	2
#define kVIDPLY_ID_PROPERTY_DATA							3
#define kVIDPLY_ID_PROPERTY_SRC                			4

#define kVIDPLY_ID_PROPERTY_PLAYSTATE                  5
#define kVIDPLY_ID_PROPERTY_PLAYPOS                      6
#define kVIDPLY_ID_PROPERTY_PLAYTIME                     7
#define kVIDPLY_ID_PROPERTY_PLAYSTATECHANGE      8
#define kVIDPLY_ID_PROPERTY_FULLSCREEN                9

#define kVIDPLY_NUM_PROPERTY_IDENTIFIERS              10
#else

#if  0
#define kVIDPLY_ID_PROPERTY_TYPE                         	0
#define kVIDPLY_ID_PROPERTY_DATA							1
#define kVIDPLY_ID_PROPERTY_SRC                			2

#define kVIDPLY_ID_PROPERTY_PLAYSTATE                  3
#define kVIDPLY_ID_PROPERTY_PLAYPOS                      4
#define kVIDPLY_ID_PROPERTY_PLAYTIME                     5
#define kVIDPLY_ID_PROPERTY_PLAYSTATECHANGE      6
#define kVIDPLY_ID_PROPERTY_FULLSCREEN                7

#define kVIDPLY_NUM_PROPERTY_IDENTIFIERS              8
#else
enum kVIDPLY_ID_PROPERTY{
 kVIDPLY_ID_PROPERTY_TYPE,
#if 0
 kVIDPLY_ID_PROPERTY_WIDTH,
 kVIDPLY_ID_PROPERTY_HEIGHT,
#endif
 kVIDPLY_ID_PROPERTY_DATA,
 kVIDPLY_ID_PROPERTY_SRC,
	
 kVIDPLY_ID_PROPERTY_PLAYSTATE,
 kVIDPLY_ID_PROPERTY_PLAYPOS,
 kVIDPLY_ID_PROPERTY_PLAYTIME,
 kVIDPLY_ID_PROPERTY_PLAYSTATECHANGE,
 kVIDPLY_ID_PROPERTY_FULLSCREEN,
	
 kVIDPLY_NUM_PROPERTY_IDENTIFIERS
};

#endif



#endif
#define kVIDPLY_ID_METHOD_STOP                            	0
#define kVIDPLY_ID_METHOD_PLAY                            	1
#define kVIDPLY_ID_METHOD_SEEK                            	2
#define kVIDPLY_ID_METHOD_SETFULLSCREEN          3
#define kVIDPLY_NUM_METHOD_IDENTIFIERS			 	4

bool            v_bVIDPLYIdentifiersInitialized = false;

NPIdentifier    v_VIDPLYPropertyIdentifiers[kVIDPLY_NUM_PROPERTY_IDENTIFIERS];
const NPUTF8 *  v_VIDPLYPropertyNames[kVIDPLY_NUM_PROPERTY_IDENTIFIERS] = {
    "type",
#if 0		
	"width",
	"height",
#endif
	"data",
	"src" ,
    "playState" ,
    "playPosition",
    "playTime",
    "onPlayStateChange",
    "fullScreen"
	};

NPIdentifier    v_VIDPLYMethodIdentifiers[kVIDPLY_NUM_METHOD_IDENTIFIERS];
const NPUTF8 *  v_VIDPLYMethodNames[kVIDPLY_NUM_METHOD_IDENTIFIERS] = {
	"stop",
	"play",
	"seek",
	"setFullScreen"
};

static  void    VIDPLYinitializeIdentifiers(void)
{
    sBrowserFuncs->getstringidentifiers( v_VIDPLYPropertyNames, kVIDPLY_NUM_PROPERTY_IDENTIFIERS, v_VIDPLYPropertyIdentifiers );
    sBrowserFuncs->getstringidentifiers( v_VIDPLYMethodNames,   kVIDPLY_NUM_METHOD_IDENTIFIERS,   v_VIDPLYMethodIdentifiers );
}

NPClass  stVIDPLYclass;
NPClass* pVIDPLYclass = NULL;

NPClass* fillVIDPLYpclass(void)
{
    TRACEINFO;
	NOTIMPLEMENTED;
    if (pVIDPLYclass == NULL)
    {
        stVIDPLYclass.allocate          = VIDPLY_Allocate;
        stVIDPLYclass.deallocate        = VIDPLY_Deallocate;
        stVIDPLYclass.invalidate        = VIDPLY_Invalidate;
        stVIDPLYclass.hasMethod         = VIDPLY_HasMethod;
        stVIDPLYclass.invoke            = VIDPLY_Invoke;
        stVIDPLYclass.invokeDefault     = VIDPLY_InvokeDefault;
        stVIDPLYclass.hasProperty       = VIDPLY_HasProperty;
        stVIDPLYclass.getProperty       = VIDPLY_GetProperty;
        stVIDPLYclass.setProperty       = VIDPLY_SetProperty;
        stVIDPLYclass.removeProperty    = VIDPLY_RemoveProperty;
        stVIDPLYclass.enumerate         = VIDPLY_Enumerate;
        pVIDPLYclass = &stVIDPLYclass;
    }

    return pVIDPLYclass;
}

static pthread_t tid;
static int position = 0;
static int playtime = 0;
static int tid_exit = 0;

void *        onPlayStateChange_InvokeDefault(void *obj);

 
void *onPlayStateChange(void *data)
{
	
	NPObj_VidPly* VIDPLY = (NPObj_VidPly*)data;
	int playstate = get_playstate();
	int cur_playstate;
	int cmd_result;
	int check_count = 0;
	position = 0;
	playtime = 0;

	mqd_t mq;
	ssize_t r;
    struct mq_attr attr;
    char cmd[100];
    char buffer[QUEUE_MAX_SIZE + 1];
    int    flags = O_RDWR | O_CREAT;
    mode_t mode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	int         connect_timeout = -1;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = QUEUE_MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mq = mq_open(QUEUE_NAME, flags, mode, &attr);

    if(mq == -1){
        debug("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! queue creation failed, ERRNO: %d\n",errno);
        exit(0);
    }

	while(1)
	{
		struct   timespec tm;

		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += 1;

		memset(buffer, 0x00, sizeof(buffer));
		//r = mq_receive(mq, buffer, QUEUE_MAX_SIZE, NULL);
		r = mq_timedreceive( mq, buffer, QUEUE_MAX_SIZE, NULL,   &tm);
#if 0
	    if(r < 0) {
	        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! queue receive failed, ERRNO: %d\n",errno);
	        //exit(0);
	    }
#endif

		debug( "1. ==================>onPlayStateChange r:%d mq:%d\n", r, mq);
#if 0
		if (VIDPLY->listener != NULL)
#endif
		if (playstate !=  (cur_playstate = get_playstate()))
		{

			if (cur_playstate == PLAYSTATE_CONNECTING)
			{
				int data[3]; //data[0] : pos, data[1] : playtime, data[2] : state
				debug( "c3. ==================>onPlayStateChange %d\n", get_playstate());
				cmd_result = send_cmd(CONTROL_GET_POS_LEN, (void *)data);
				debug( "c4. ==================>pos %d playtime %d state %d timeout:%d\n", data[0], data[1], data[2], connect_timeout);
				if (data[1] > 0)
				{
					set_playstate(PLAYSTATE_PLAYING);
					connect_timeout = -1;
				}
				else
				{
					if (connect_timeout == -1)
					{
						connect_timeout = 30;
					}
					else
					{
						if (connect_timeout == 0)
						{
							//set_playstate(PLAYSTATE_FINISHED);
							set_playstate(PLAYSTATE_ERROR);
							//sBrowserFuncs->pluginthreadasynccall(VIDPLY->npp, onPlayStateChange_InvokeDefault, (void *)VIDPLY);
						}
						connect_timeout--;
					}
				}
			}
			else if (cur_playstate == PLAYSTATE_PLAYING)
			{
				int data[3]; //data[0] : pos, data[1] : playtime, data[2] : state
				
				debug( "3. ==================>onPlayStateChange %d\n", get_playstate());
				cmd_result = send_cmd(CONTROL_GET_POS_LEN, (void *)data);
				debug( "4. ==================>pos %d playtime %d state %d\n", data[0], data[1], data[2]);

				playtime = data[1];
				position = data[0];

				if ((data[1] != 0 && data[2] == 1))
				{
					sBrowserFuncs->pluginthreadasynccall(VIDPLY->npp, onPlayStateChange_InvokeDefault, (void *)VIDPLY);
					playstate = cur_playstate;
					check_count = 0;

				}
				else if ((data[1] != 0 && (data[0] >= (data[1]-500) ) && ((data[0] - data[1]) < 1000))  || data[2] == 0)
				{
					set_playstate(PLAYSTATE_FINISHED);
					check_count = 0;

				}
				else if (data[1] == 0 && data[2] == 1)
				{
					check_count ++;
					if (check_count == 30)
					{
						set_playstate(PLAYSTATE_FINISHED);
						check_count = 0;
					}	

				}
				connect_timeout = -1;
			}
			else
			{
				sBrowserFuncs->pluginthreadasynccall(VIDPLY->npp, onPlayStateChange_InvokeDefault, (void *)VIDPLY);
				playstate = cur_playstate;
				check_count = 0;
				connect_timeout = -1;

			}
		}


		{
			debug( "2. ==================>onPlayStateChange %d\n", get_playstate());
			{
				//cur_playstate = get_playstate();
				if (playstate == PLAYSTATE_PLAYING)
				{
					int data[3]; //data[0] : pos, data[1] : playtime, data[2] : state
					
					debug( "3. ==================>onPlayStateChange %d\n", get_playstate());
					cmd_result = send_cmd(CONTROL_GET_POS_LEN, (void *)data);
					debug( "4. ==================>pos %d playtime %d state %d\n", data[0], data[1], data[2]);
				
					if ((data[1] != 0 && (data[0] >= (data[1]-500) ) && ((data[0] - data[1]) < 1000))  || data[2] == 0)
					{

						playtime = position = data[1];
						set_playstate(PLAYSTATE_FINISHED);
						//onPlayStateChange_InvokeDefault((NPObject *)VIDPLY);
						//sBrowserFuncs->pluginthreadasynccall(VIDPLY->npp, onPlayStateChange_InvokeDefault, (void *)VIDPLY);

					}
					else
					{
						playtime = data[1];
						position = data[0];
					}
				}

			}
		}

		if (tid_exit)
		{
			tid_exit = 0;
		    /* cleanup */
		    mq_close(mq);
		    mq_unlink(QUEUE_NAME);
			break;
		}

		//sleep(1);

	}
}

void *        onPlayStateChange_InvokeDefault(void *obj)
{
	NPObj_VidPly* VIDPLY = (NPObj_VidPly*)obj;
	NPVariant get_result = { NPVariantType_Void };

	if (VIDPLY->listener)
	{
		sBrowserFuncs->invokeDefault(VIDPLY->npp, VIDPLY->listener, NULL,0, &get_result);
		sBrowserFuncs->releasevariantvalue (&get_result);
	}
	
}



NPObject *          VIDPLY_Allocate(NPP npp, NPClass *theClass)
{
    TRACEINFO;
	NOTIMPLEMENTED;

	send_cmd(OOIF_BROADCAST_STOP, NULL);
	
    if (!v_bVIDPLYIdentifiersInitialized)
    {
        v_bVIDPLYIdentifiersInitialized = true;
        VIDPLYinitializeIdentifiers();
    }

	NPObj_VidPly* newVIDPLY = (NPObj_VidPly*)MEMALLOC(sizeof(NPObj_VidPly));
	memset(newVIDPLY, 0, sizeof(NPObj_VidPly));
    newVIDPLY->npp = npp;

    clear_url();

	pthread_create(&tid, NULL, onPlayStateChange, (void *)newVIDPLY);

    return (NPObject*) newVIDPLY;
}

void		VIDPLY_Deallocate(NPObject* obj)
{
    TRACEINFO;
	NOTIMPLEMENTED;
	
	int timeout = 50;

//	send_cmd(OOIF_BROADCAST_PLAY, NULL);

	tid_exit = 1;

	while (tid_exit)
	{
		usleep(100*1000);
		if (--timeout < 0)
			break;
	}	
	if (timeout < 0)
    {
        perror("thread exit error : ");
        exit(0);
    }
	clear_url();
	
    //OnVIDPLY_SetFullScreen(true);
    NPObj_VidPly* vidply = (NPObj_VidPly*)obj;

	if (vidply->listener)
	{
		sBrowserFuncs->releaseobject (vidply->listener);
	}
	
	v_bVIDPLYIdentifiersInitialized = false;
    MEMFREE(obj);
    return;
}

  void        VIDPLY_Invalidate(NPObject* obj)
{
    TRACEINFO;
	NOTIMPLEMENTED;
    return;
}

  bool        VIDPLY_HasMethod(NPObject* obj, NPIdentifier name)
{
    //TRACEINFO;
	//NOTIMPLEMENTED;

    bool result = false;
    int i = 0;
    NPUTF8* utf8methodname = (char*)sBrowserFuncs->utf8fromidentifier(name);


    while ((i < kVIDPLY_NUM_METHOD_IDENTIFIERS) && (result == false))
    {
        if ( name == v_VIDPLYMethodIdentifiers[i] )
        {
			//debug( "VIDPLY_HasMethod:%s true\n", utf8methodname);

            result= true;
        }
        i++;
    }
    if (!result)
    {
    	debug( "VIDPLY_HasMethod:%s\n", utf8methodname);
    	NOTIMPLEMENTED;
    }
    return result;
}

bool        VIDPLY_Invoke(NPObject* obj, NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    //TRACEINFO;

	bool fctresult = false;

    NPUTF8* utf8methodname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug( "VIDPLY_Invoke:%s\n", utf8methodname);

	if (name == v_VIDPLYMethodIdentifiers[kVIDPLY_ID_METHOD_STOP])
    {
    	VIDPLY_Invoke_Stop((NPObj_VidPly*)obj, args, argCount, result);
    	fctresult = true;
    }

    if (name == v_VIDPLYMethodIdentifiers[kVIDPLY_ID_METHOD_PLAY])
    {
    	VIDPLY_Invoke_Play((NPObj_VidPly*)obj, args, argCount, result);
    	fctresult = true;
    }

    if (name == v_VIDPLYMethodIdentifiers[kVIDPLY_ID_METHOD_SEEK])
    {
    	VIDPLY_Invoke_Seek((NPObj_VidPly*)obj, args, argCount, result);
    	fctresult = true;
    }
    if (name == v_VIDPLYMethodIdentifiers[kVIDPLY_ID_METHOD_SETFULLSCREEN])
    {
    	fctresult = true;
    }
    if (!fctresult)
        NOTIMPLEMENTED;
    return fctresult;
}

 bool        VIDPLY_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    TRACEINFO;
	NOTIMPLEMENTED;

    return true;
}

  bool        VIDPLY_HasProperty(NPObject* obj, NPIdentifier name)
{
    //TRACEINFO;
    //NOTIMPLEMENTED;
    bool result = false;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);

    int i = 0;
    while ((i < kVIDPLY_NUM_PROPERTY_IDENTIFIERS) && (result == false))
    {
        if ( name == v_VIDPLYPropertyIdentifiers[i] )
        {
            result= true;
        }
        i++;
    }
    if (!result)
        debug( "VIDPLY_HasProperty:%s\n", utf8propertyname);
    return result;
}

 bool        VIDPLY_GetProperty(NPObject* obj, NPIdentifier name, NPVariant* result)
{
    //TRACEINFO;
    int cmd_result = 0;
    bool fctresult = false;
    NPObj_VidPly* VIDPLY = (NPObj_VidPly*)obj;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);
#if 0
    if (name == v_VIDPLYPropertyIdentifiers[kVIDPLY_ID_PROPERTY_WIDTH]) {
		DOUBLE_TO_NPVARIANT(VIDPLY->width,*result);
    	fctresult = true;
    } 
	else if (name == v_VIDPLYPropertyIdentifiers[kVIDPLY_ID_PROPERTY_HEIGHT]) {
		DOUBLE_TO_NPVARIANT(VIDPLY->height,*result);
		fctresult = true;
	}
    else
#endif		
	if (name == v_VIDPLYPropertyIdentifiers[kVIDPLY_ID_PROPERTY_PLAYSTATE]) {
		
		int state = get_playstate();

		INT32_TO_NPVARIANT(state,*result);
	
		debug( "kVIDPLY_ID_PROPERTY_PLAYSTATE : %d\n", state);

		fctresult = true;
	} 
    else if (name == v_VIDPLYPropertyIdentifiers[kVIDPLY_ID_PROPERTY_PLAYPOS]) {

		int data[2];
		int count = 0;
		int state = get_playstate();

		INT32_TO_NPVARIANT(position,*result);

		debug( "kVIDPLY_ID_PROPERTY_PLAYPOS : %d\n", position);

		fctresult = true;
	} 
    else if (name == v_VIDPLYPropertyIdentifiers[kVIDPLY_ID_PROPERTY_PLAYTIME]) {
		int data[2];
		int count = 0;
		int state = get_playstate();

		INT32_TO_NPVARIANT(playtime,*result);

		debug( "kVIDPLY_ID_PROPERTY_PLAYTIME : %d\n", playtime);
		fctresult = true;
	}
    if (!fctresult)
        debug( "VIDPLY_GetProperty:%s\n", utf8propertyname);
    return fctresult;
}


static bool plugin_object_set_callback (NPObject **listener, const NPVariant  *value)
{  
	if (!NPVARIANT_IS_OBJECT (*value) && !NPVARIANT_IS_NULL (*value))    
		return FALSE;  
	if (*listener)    
		sBrowserFuncs->releaseobject (*listener);  
	*listener = NULL;  
	if (NPVARIANT_IS_OBJECT (*value))    
	{      
		*listener = NPVARIANT_TO_OBJECT (*value);      
		sBrowserFuncs->retainobject (*listener);    
	}  
	return true;
}

static void print_NPVariant(NPVariant* args)
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


bool        VIDPLY_SetProperty(NPObject *obj, NPIdentifier name, const NPVariant *value)
{
    //TRACEINFO;
    bool fctresult = false;
    NPObj_VidPly* VIDPLY = (NPObj_VidPly*)obj;
    NPUTF8* utf8propertyname = (char*)sBrowserFuncs->utf8fromidentifier(name);
    debug( "VIDPLY_SetProperty:%s\n", utf8propertyname);
#if 0
    if (name == v_VIDPLYPropertyIdentifiers[kVIDPLY_ID_PROPERTY_WIDTH]) {
		VIDPLY->width = NPVARIANT_TO_DOUBLE(*value);
    	fctresult = true;
		debug("VIDPLY_SetProperty kVIDPLY_ID_PROPERTY_WIDTH %d \n", VIDPLY->width);
		print_NPVariant(value);
    } else
    if (name == v_VIDPLYPropertyIdentifiers[kVIDPLY_ID_PROPERTY_HEIGHT]) {
		VIDPLY->height = NPVARIANT_TO_DOUBLE(*value);
		fctresult = true;
		debug("VIDPLY_SetProperty kVIDPLY_ID_PROPERTY_HEIGHT %d \n", VIDPLY->height);
		print_NPVariant(value);
	} else
#endif
    if (name == v_VIDPLYPropertyIdentifiers[kVIDPLY_ID_PROPERTY_DATA]) {
        //debug( "################# %s\n", NPVARIANT_TO_STRING(*value).UTF8Characters);
        //debug( "################# %d\n", NPVARIANT_TO_STRING(*value).UTF8Length+1);

        if (set_url(NPVARIANT_TO_STRING(*value).UTF8Characters, NPVARIANT_TO_STRING(*value).UTF8Length+1))
        {
        	send_cmd(CONTROL_STOP, NULL);
        	//send_cmd(CONTROL_PAUSE_PLAYING, NULL);
        	playtime = position = 0;
        }
        fctresult = true;
    } else   
	if (name == v_VIDPLYPropertyIdentifiers[kVIDPLY_ID_PROPERTY_PLAYSTATECHANGE]) 
	{
	    debug( "################# type %d\n", value->type);
		fctresult = plugin_object_set_callback(&VIDPLY->listener, value);
		fctresult = true;
	}

    return fctresult;
}

 bool        VIDPLY_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    TRACEINFO;
	NOTIMPLEMENTED;
    return true;
}


 bool        VIDPLY_Enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    TRACEINFO;
	NOTIMPLEMENTED;
    return true;
}

/** implementation of methods **/
void		VIDPLY_Invoke_Stop(NPObj_VidPly* obj,const NPVariant* args, uint32_t argCount, NPVariant* result)
{
	int cmd_result;

    TRACEINFO;
	/*CONTROL_STOP*/
	OnNoFullscreenSetWindow(0, 0, 0, 0);

	playtime = position = 0;

#if 0 //def PLAY_STOP_TO_LIVE
	cmd_result = send_cmd(OOIF_BROADCAST_PLAY, NULL);
	playtime = position = 0;
#else
	cmd_result = send_cmd(CONTROL_STOP, NULL);
	playtime = position = 0;
#endif

	BOOLEAN_TO_NPVARIANT(1, *result);

	return;
}

void		VIDPLY_Invoke_Seek(NPObj_VidPly* obj,const NPVariant* args, uint32_t argCount, NPVariant* result)
{
	int cmd_result;
	int pos = 0;
	int data[1];

    TRACEINFO;
	//debug( "################# VIDPLY_Invoke_Seek argCount %d\n", argCount);
	//debug( "################# VIDPLY_Invoke_Seek type %d\n", args[0].type);
	data[0] = NPVARIANT_TO_DOUBLE(args[0]);
	debug( "################# VIDPLY_Invoke_Seek NPVariantType_Double : %d\n", data[0]);

	/*CONTROL_SKIP*/
	cmd_result = send_cmd(CONTROL_SKIP, (void *)data);
	BOOLEAN_TO_NPVARIANT(cmd_result, *result);
	return;
}


void		VIDPLY_Invoke_Play(NPObj_VidPly* obj,const NPVariant* args, uint32_t argCount, NPVariant* result)
{
	int cmd_result;
	int len;
	int cmd;
	TRACEINFO;


	//debug( "################# VIDPLY_Invoke_Play argCount %d\n", argCount);
	//debug( "################# VIDPLY_Invoke_Play type %d\n", args[0].type);
	
	cmd = NPVARIANT_TO_DOUBLE(args[0]);

	debug( "################# VIDPLY_Invoke_Play cmd %d\n", cmd);

	if (cmd == 0)
	{
		cmd_result = send_cmd(CONTROL_PAUSE_PLAYING, NULL);
	}
	else if (cmd == 1)
	{
		if (get_playstate() == PLAYSTATE_PAUSED)
		{
			cmd_result = send_cmd(CONTROL_RESUME_PLAYING, NULL);
		}
		else
		{
			int count = 0;
			playtime = position = 0;
			cmd_result = send_cmd(CONTROL_OPENURL, NULL);
		}
	}
				
	BOOLEAN_TO_NPVARIANT(1, *result);

    return;    
}

void		VIDPLY_setsize(NPObj_VidPly* obj,int32_t witdh, int32_t height)
{
	debug("VIDPLY_setsize\n");
}

