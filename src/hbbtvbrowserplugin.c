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
/*
** HbbTV Plugin PC
*/

#if 0
#include <gdk/gdk.h>
#ifdef XP_UNIX
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#endif
#include <gtk/gtk.h>
#endif

#include "hbbtvbrowserplugin.h"
#include "oipfapplicationmanager.h"
#include "oipfconfiguration.h"
#include "videobroadcast.h"
#include "videoplayer.h"

#include "hbbtvcommonutil.h"

#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>


#if 0
#define HBBTV_MIME_DESC "application/vnd.dvb.ait+xml:aitx:XmlAit;"\
						"application/ce-html+xml:cex:XmlCEHtml;"\
						"application/x-ce-html+xml:cex:XmlXCEHtml" \
						"application/vnd.hbbtv.xhtml+xml:hbbx:XmlXHBBHtml" \
						"application/vnd.dvb.ait+xml:aitx:XmlAit;"\
						"application/oipfApplicationManager:bsc:ApplicationManager;"\
						"application/oipfConfiguration:bsc:Configuration;"\
						"application/oipfCapabilities:bsc:OipfCapabilities;"\
						"application/oipfSearchManager:bsc:SearchManager;"\
						"application/oipfParentalControlManager:bsc:ParentalControlManager;"\
						"video/broadcast:bsc:VideoBroadcast;"\
						"video/mpeg:bsc:VideoMpeg;"\
						"video/mp4:bsc:VideoMP4;"\
						"video/mpeg4:bsc:VideoMP4;"\
						"audio/mpeg:bsc:AudioMpeg;"\
						"audio/mp4:bsc:AudioMP4;"\
						"application/oipfRecordingScheduler:bsc:OipfRecordingScheduler;"\
						"application/oipfDrmAgent"
#else
#define HBBTV_MIME_DESC "application/vnd.dvb.ait+xml:aitx:XmlAit;"\
						"application/ce-html+xml:cex:XmlCEHtml;"\
						"application/x-ce-html+xml:cex:XmlXCEHtml" \
						"application/vnd.hbbtv.xhtml+xml:hbbx:XmlXHBBHtml" \
						"application/vnd.dvb.ait+xml:aitx:XmlAit;"\
						"application/oipfApplicationManager:bsc:ApplicationManager;"\
						"application/oipfConfiguration:bsc:Configuration;"\
						"application/oipfSearchManager:bsc:SearchManager;"\
						"application/oipfParentalControlManager:bsc:ParentalControlManager;"\
						"video/broadcast:bsc:VideoBroadcast;"\
						"video/mpeg:bsc:VideoMpeg;"\
						"video/mp4:bsc:VideoMP4;"\
						"video/mpeg4:bsc:VideoMP4;"\
						"audio/mpeg:bsc:AudioMpeg;"\
						"audio/mp4:bsc:AudioMP4;"\
						"application/oipfRecordingScheduler:bsc:OipfRecordingScheduler;"\
						"application/oipfDrmAgent"
#endif

#define PLUGIN_NAME			"hbbtvbrowserplugin"
#define PLUGIN_DESCRIPTION	"HbbTV browser plugin "
#define PLUGIN_VERSION		"0.20110704"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))


static void
fillPluginFunctionTable(NPPluginFuncs* pFuncs)
{
  TRACEINFO;
  pFuncs->version = 11;
  pFuncs->size = sizeof(*pFuncs);
  pFuncs->newp = OIPF_NPP_New;
  pFuncs->destroy = OIPF_NPP_Destroy;
  pFuncs->setwindow = OIPF_NPP_SetWindow;
  pFuncs->newstream = OIPF_NPP_NewStream;
  pFuncs->destroystream = OIPF_NPP_DestroyStream;
//  pFuncs->asfile = NPP_StreamAsFile;
//  pFuncs->writeready = NPP_WriteReady;
//  pFuncs->write = NPP_Write;
//  pFuncs->print = NPP_Print;
  pFuncs->event = OIPF_NPP_HandleEvent;

  pFuncs->urlnotify = OIPF_NPP_URLNotify;
  pFuncs->urlredirectnotify = OIPF_NPP_URLRedirectNotify;

  pFuncs->getvalue = OIPF_NPP_GetValue;
  pFuncs->setvalue = OIPF_NPP_SetValue;
}

char* booltostr(bool data)
{
    char *result;
    if (data)
        result = "true";
    else
        result = "false";
    return result;
}

/*
 void drawWindow (HBBTVPluginData* instanceData, GdkDrawable* gdkWindow)
{
  TRACEINFO;
  NPWindow window = *(instanceData->window);
  int x = window.x;
  int y = window.y;
  int width = window.width;
  int height = window.height;
  debug("%p, x = %i  y=%i, width = %i height = %i\n",gdkWindow, x ,y,width,height);

  NPP npp = instanceData->npp;
  if (!npp)
    return;

  const char* uaString = "DemoPlug"; // sBrowserFuncs->uagent(npp);
  if (!uaString)
    return;

  GdkGC* gdkContext = gdk_gc_new(gdkWindow);

  // draw a grey background for the plugin frame
  GdkColor grey;
  grey.red = 24000; grey.blue = 24000; grey.green = 24000;
  gdk_gc_set_rgb_fg_color(gdkContext, &grey);
  gdk_draw_rectangle(gdkWindow, gdkContext, TRUE, x, y, width, height);

  // draw a 3-pixel-thick black frame around the plugin
  GdkColor black;
  black.red = black.green = black.blue = 0;
  gdk_gc_set_rgb_fg_color(gdkContext, &black);
  gdk_gc_set_line_attributes(gdkContext, 3, GDK_LINE_SOLID, GDK_CAP_NOT_LAST, GDK_JOIN_MITER);
  gdk_draw_rectangle(gdkWindow, gdkContext, FALSE, x + 1, y + 1,
                     width - 3, height - 3);

  // paint the UA string
  PangoContext* pangoContext = gdk_pango_context_get();
  PangoLayout* pangoTextLayout = pango_layout_new(pangoContext);
  pango_layout_set_width(pangoTextLayout, (width - 10) * PANGO_SCALE);
  pango_layout_set_text(pangoTextLayout, uaString, -1);
  gdk_draw_layout(gdkWindow, gdkContext, x + 5, y + 5, pangoTextLayout);
  g_object_unref(pangoTextLayout);

  g_object_unref(gdkContext);
}
*/
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <getopt.h>
#include <fcntl.h>


static int brc_x, brc_y , brc_w, brc_h;

bool            v_bVIDPlyInitialized = false;

static timer_t timerID;

static int timer_SetWindow(void)
{

	//debug("timer_SetWindow %d %d %d %d\n", brc_x, brc_y, brc_w, brc_h);

	OnNoFullscreenSetWindow(brc_x,
							brc_y,
							brc_w,
							brc_h);
	

}

int Internal_Timer_Create(timer_t *timerID)
{

    struct sigevent         te;  
    struct itimerspec       its;  
    struct sigaction        sa;  
    int                     sigNo = SIGRTMIN;  
   
    /* Set up signal handler. */  
    sa.sa_flags = SA_SIGINFO;  
    sa.sa_sigaction = timer_SetWindow;     // 展戚袴 硲窒獣 硲窒拝 敗呪 
    sigemptyset(&sa.sa_mask);  
  
    if (sigaction(sigNo, &sa, NULL) == -1)  
    {  
        printf("sigaction error\n");
        return -1;  
    }  


    /* Set and enable alarm */  
    te.sigev_notify = SIGEV_SIGNAL;  
    te.sigev_signo = sigNo;  
    te.sigev_value.sival_ptr = timerID;  
    timer_create(CLOCK_REALTIME, &te, timerID);  

	return 0;

}

int Internal_setTimer(timer_t *timerID)
{
    struct itimerspec       its;  
	struct itimerspec		oldits; 

	//debug("Internal_setTimer\n");

	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = 10 * 1000000;

	{
	its.it_interval.tv_sec = 0;
	its.it_interval.tv_nsec = 0; 

	}
	if (timer_gettime(*timerID, &oldits) != 0)
	{
		//debug ("timer_gettime fail\n");
		timer_settime(*timerID, 0, &its, NULL);
	}
	else
	{
		//debug ("timer_gettime OK its.it_value.tv_sec %d its.it_value.tv_msec %d \n", oldits.it_value.tv_sec, oldits.it_value.tv_nsec/1000000);
		timer_settime(*timerID, 0, &its, &oldits);
	}

	return 0;

}


NPError NP_Initialize(NPNetscapeFuncs* bFuncs, NPPluginFuncs* pFuncs)
{
	TRACEINFO;
	sBrowserFuncs = bFuncs;
	fillPluginFunctionTable(pFuncs);
	return NPERR_NO_ERROR;

}

char* NP_GetPluginVersion()
{
    TRACEINFO;
	return PLUGIN_VERSION;
}

NPError NP_Shutdown()
{
    TRACEINFO;
	return NPERR_NO_ERROR;
}

char* NP_GetMIMEDescription()
{
    TRACEINFO;
#if 0    
	return MIMETYPEDESCRIPTION;
#else	
	return HBBTV_MIME_DESC;
#endif	
}

NPError NP_GetValue(void *instance, NPPVariable variable, void *value)
{
    TRACEINFO;
	switch (variable)
	{
		case NPPVpluginNameString :
			*(char**)value = PLUGIN_NAME;
			break;
		case NPPVpluginDescriptionString :
			*(char**)value = PLUGIN_DESCRIPTION;
			break;
		default :
			break;
	}
	return NPERR_NO_ERROR;
}

NPError OIPF_NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char *argn[], char *argv[], NPSavedData *saved)
{
	TRACEINFO;
	int i = 0;

	NPBool browserSupportsWindowless = false;
	sBrowserFuncs->getvalue(instance, NPNVSupportsWindowless, &browserSupportsWindowless);
	if (!browserSupportsWindowless)
	{
		return NPERR_GENERIC_ERROR;
	}
	sBrowserFuncs->setvalue(instance, NPPVpluginWindowBool, (void*)false);
	sBrowserFuncs->setvalue(instance, NPPVpluginTransparentBool, (void*)false);


    NPObject* obj;
    instance->pdata = NULL;

    for(i = 0 ; i<argc ; i++ ){
		if((strcmp(argn[i],"type")) == 0){

			debug( "OIPF_NPP_New %s:%s [%d/%d]\n", argn[i], argv[i], i, argc);
			instance->pdata = (HBBTVPluginData*)MEMALLOC(sizeof(HBBTVPluginData));
			HBBTVPluginData* pdata = ((HBBTVPluginData*)instance->pdata);

			memset(pdata, 0, sizeof(HBBTVPluginData));
			if (strcmp(argv[i], "application/oipfApplicationManager") == 0)
			{
				obj = sBrowserFuncs->createobject(instance, fillOAMpclass());
			}
			else if (strcmp(argv[i], "application/oipfConfiguration") == 0)
			{
				obj = sBrowserFuncs->createobject(instance, fillOCFGpclass());
			}
#if 0
			else if (strcmp(argv[i], "application/oipfCapabilities") == 0)
			{
				obj = sBrowserFuncs->createobject(instance, fillCAPABILITYpclass());
			}
#endif
			else if (strcmp(argv[i], "video/broadcast") == 0)
			{
				obj = sBrowserFuncs->createobject(instance, fillVIDBRCpclass());					
			}
			else if ((strcmp(argv[i], "video/mp4") == 0) || 
				(strcmp(argv[i], "video/mpeg4")  == 0) || 
				(strcmp(argv[i], "video/mpeg") == 0) || 
				(strcmp(argv[i], "audio/mp4") == 0) || 
				(strcmp(argv[i], "audio/mpeg") == 0))										
			{
				if (!v_bVIDPlyInitialized)
				{	
					obj = sBrowserFuncs->createobject(instance, fillVIDPLYpclass());
					v_bVIDPlyInitialized = true;
				}						
			}			
			else
			{
				obj = NULL;
			}
			pdata->type = (char*)MEMALLOC(strlen(argv[i]));
			memset(pdata->type, 0, strlen(argv[i]));
			strcpy(pdata->type, argv[i] );

			if((strcmp(argn[0],"id")) == 0)
			{
				strncpy(pdata->id, argv[0], MIN(strlen(argv[0]), sizeof(pdata->id)));
			}

			for (++i; i<argc ; i++)
			{
	           	debug("################### [%s] %s w: [%d/%d]\n", argn[i], argv[i],  i, argc);

				if((strcmp(argn[i],"width")) == 0)
				{
					pdata->width = atoi(argv[i]);
				}
				else if((strcmp(argn[i],"height")) == 0)
				{
					pdata->height = atoi(argv[i]);
				}
				else if((strcmp(argn[i],"style")) == 0)
				{
					strncpy(pdata->style, argv[i], MIN(strlen(argv[i]), sizeof(pdata->style)));
				}
				/* by Jin-Chul */
				else if ((strcmp(argn[i],"data")) == 0 || (strcmp(argn[i],"src")) == 0)
				{
					set_url((char *)argv[i], MIN(strlen(argv[i])+1, 1024));
				}

				
			}
            debug("################### [%s] %s w:%d h:%d\n", pdata->type, pdata->id, pdata->width, pdata->height);
#if 0
            if (obj == NULL)
            {
            	return NPERR_GENERIC_ERROR;
            }
            else
#endif
            {
            	pdata->obj = obj;
            	pdata->npp = instance;
            }
		}
	}

	return NPERR_NO_ERROR;
}

NPError OIPF_NPP_Destroy(NPP instance, NPSavedData **save)
{
    TRACEINFO;
    HBBTVPluginData* pdata = (HBBTVPluginData*)instance->pdata;
    if (pdata)
    {
        sBrowserFuncs->releaseobject(pdata->obj);
        if (strlen(pdata->type))
        {
            if ((strcmp(pdata->type, "video/mp4") == 0) ||
                (strcmp(pdata->type, "video/mpeg4")  == 0) ||
                (strcmp(pdata->type, "video/mpeg") == 0) ||
                (strcmp(pdata->type, "audio/mp4") == 0) ||
                (strcmp(pdata->type, "audio/mpeg") == 0))
            {
                v_bVIDPlyInitialized = false;
            }
        }
        MEMFREE(pdata->type);
        MEMFREE(pdata);
    }
    return NPERR_NO_ERROR;
}


NPError OIPF_NPP_SetWindow(NPP instance, NPWindow *window)
{
//	TRACEINFO;
	HBBTVPluginData* pdata = (HBBTVPluginData*)instance->pdata;

//    debug( "OIPF_NPP_SetWindow x:%d y:%d w:%d h:%d [%s]\n",
//		window->x, window->y, window->width, window->height, pdata->type);

	if(pdata)
	{
		pdata->window = window;
		if (pdata->type)
		{
			if (strcmp(pdata->type, "video/broadcast") == 0)
			{
				debug( "%s found \n", pdata->type);
				if (pdata->obj)
				{
					NPObj_VidBrc* vidbrcobj  = (NPObj_VidBrc*)pdata->obj;
					//if (!(vidbrcobj->fullscreen))
 					{

						//debug( "OIPF_NPP_SetWindow BROADCAST x:%d y:%d w:%d h:%d [%s]\n",
						//		window->x, window->y, window->width, window->height, pdata->type);
						{

							brc_x=pdata->window->x;
							brc_y=pdata->window->y;
							brc_w=pdata->window->width;
							brc_h=pdata->window->height;
#if 0						
							OnNoFullscreenSetWindow(pdata->window->x,
													pdata->window->y,
													pdata->window->width,
													pdata->window->height);
							vidbrcobj->size_flag = 0;
#endif
						}
						Internal_setTimer(&timerID);

					}
					
				}
			}
			else if ((strcmp(pdata->type, "video/mp4") == 0) ||
                (strcmp(pdata->type, "video/mpeg4")  == 0) ||
                (strcmp(pdata->type, "video/mpeg") == 0))
            {

				debug( "%s found \n", pdata->type);
				if (pdata->obj)
				{
					NPObj_VidPly* vidplayerobj  = (NPObj_VidPly*)pdata->obj;
					//if (!(vidbrcobj->fullscreen))
					if (check_url() > 0)
					{
						//debug( "OIPF_NPP_SetWindow VIDEO x:%d y:%d w:%d h:%d [%s]\n",
						//		window->x, window->y, window->width, window->height, pdata->type);
						{
							brc_x=pdata->window->x;
							brc_y=pdata->window->y;
							brc_w=pdata->window->width;
							brc_h=pdata->window->height;

#if 0							
							OnNoFullscreenSetWindow(pdata->window->x,
													pdata->window->y,
													pdata->window->width,
													pdata->window->height);
						    vidplayerobj->size_flag = 0;	
#endif
						}
						Internal_setTimer(&timerID);



					}
					
				}
            }
	

			
		}
		/*debug( "\tWindow found : %p\n", window);
		debug( "\twindow = %p ws_info %p \n\tx = %d, y= %d\n\tclipRect.left = %d clipRect.top= %d\n\twidth=%d,  height= %d\n",
			pdata->window->window,
			pdata->window->ws_info,
			pdata->window->x,
			pdata->window->y,
			pdata->window->clipRect.left,
			pdata->window->clipRect.top,
			pdata->window->width,
			pdata->window->height);*/
	}

	return NPERR_NO_ERROR;
}

NPError OIPF_NPP_HandleEvent(NPP instance, void* event)
{
	 TRACEINFO; // frequent event, avoid over log...
#ifndef Q_WS_WIN
	XEvent *nativeEvent = (XEvent*)event;
	if (nativeEvent->type == GraphicsExpose)
	{
		XGraphicsExposeEvent *expose = &nativeEvent->xgraphicsexpose;
		GC gc = XCreateGC(expose->display, expose->drawable, 0, 0);
		XFillRectangle(expose->display, expose->drawable, gc, expose->x, expose->y,expose->width, expose->height);
		XFreeGC(expose->display, gc);
		return true;
	}
#endif

	return false;
}

void OIPF_NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
	HBBTVPluginData* pdata = (HBBTVPluginData*)instance->pdata;
    debug("NPP_URLNotify[%s]:%s\n", pdata->type, url);
}

void OIPF_NPP_URLRedirectNotify(NPP instance, const char *url, int32_t status, void *notifyData)
{
	HBBTVPluginData* pdata = (HBBTVPluginData*)instance->pdata;
    debug("NPP_URLRedirectNotify[%s]:%s\n", pdata->type, url);
}

NPError OIPF_NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    //TRACEINFO;

    HBBTVPluginData* pdata = (HBBTVPluginData*)instance->pdata;
    debug("OIPF_NPP_GetValue[%s]:%d\n", pdata->type, variable);

    if ( variable == NPPVpluginScriptableNPObject )
    {
        void ** v = (void **)value;
        if(pdata)
        {
			if (strcmp(pdata->type, "application/oipfCapabilities"))
			{
						NPObject* obj = ((HBBTVPluginData*)instance->pdata)->obj;
						if(obj == NULL){
						    debug("ERR] no java script object for %s\n", pdata->type);
						    return NPERR_GENERIC_ERROR;
						}else{

						    debug("retain java script object for %s\n", pdata->id);
							sBrowserFuncs->retainobject(obj);
						}
						*v = obj;
						return NPERR_NO_ERROR;
			}
			else
			{
				debug("SKIPPPPPPPPPPPPPPPPPPPPPPPPP OIPF_NPP_GetValue NPPVpluginScriptableNPObject %s\n", pdata->type);
			}
		}

    }
#if 1
    else if ( variable == NPPVpluginNeedsXEmbed)
    {
    	//*((PRBool*) val) = PR_FALSE;
    	//*((NPBool*)value) = false;
    	*((bool*)value) = false;
    	//*(static_cast<NPBool*>(value)) = true;
    	return NPERR_NO_ERROR;
    }
#endif
	return NPERR_GENERIC_ERROR;
}


NPError OIPF_NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    TRACEINFO;
	return NPERR_GENERIC_ERROR;
}


NPError OIPF_NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
	int len;
	int cmd_result;

    TRACEINFO;
	HBBTVPluginData* pdata = (HBBTVPluginData*)instance->pdata;
	debug( "OIPF_NPP_NewStream[%s] url:%s\n", pdata->type, stream->url);
#if 0
	if ((strcmp(pdata->type, "video/mp4") == 0) ||
					(strcmp(pdata->type, "video/mpeg4")  == 0) ||
					(strcmp(pdata->type, "video/mpeg") == 0))
	{
		set_url(stream->url, strlen(stream->url));
		cmd_result = send_cmd(CONTROL_OPENURL, NULL);
	}
#endif	
	return NPERR_GENERIC_ERROR;
}

NPError OIPF_NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    TRACEINFO;
	NOTIMPLEMENTED;
	return NPERR_GENERIC_ERROR;
}
