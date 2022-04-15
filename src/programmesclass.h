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
#ifndef    __HBBTVCURRENTCHANNEL_H__
#define    __HBBTVCURRENTCHANNEL_H__

#include "hbbtvbrowserplugin.h"

#if 0
#define TYPE_TV 					0 		   /* Used in the channelType property to indicate a TV channel. */
#define TYPE_RADIO 				1  /* Used in the channelType property to indicate a radio channel. */
#define TYPE_OTHER 				2  /* Used in the channelType property to indicate that the type of the channel is unknown or known but not of type TV or radio. */

#define ID_ANALOG 				0   /* Used in the idType property to indicate an analogue channel identified by the property freq and optionally cni or name . */
#define ID_DVB_C				  10  /* Used in the idType property to indicate a DVB-C channel identified by the three properties onid , tsid , sid . */
#define ID_DVB_S 				  11  /* Used in the idType property to indicate a DVB-S channel identified by the three properties onid , tsid , sid . */
#define ID_DVB_T 				  12  /* Used in the idType property to indicate a DVB-T channel identified by the three properties onid , tsid , sid . */
#define ID_DVB_SI_DIRECT 13 /* Used in the idType property to indicate a channel that is identified through its delivery system descriptor as defined by DVB-SI [EN 300 468] section 6.2.13. */
#define ID_DVB_C2 			  14 /* Used in the idType property to indicate a DVB-C or DVB-C2 channel identified by the three properties onid , tsid , sid .*/
#define ID_DVB_S2 			  15 /* Used in the idType property to indicate a DVB-S or DVB-S2 channel identified by the three properties onid , tsid , sid . */
#define ID_DVB_T2 			  16 /* Used in the idType property to indicate a DVB-T or DVB-T2 channel identified by the three properties onid , tsid , sid .*/
#define ID_ISDB_C 			  20  /* Used in the idType property to indicate an ISDB-C channel identified by the three properties: onid , tsid , sid .*/
#define ID_ISDB_S 			  21  /* Used in the idType property to indicate an ISDB-S channel identified by the three properties onid , tsid , sid .*/
#define ID_ISDB_T 				  22  /* Used in the idType property to indicate an ISDB-T channel identified by the three properties onid , tsid , sid .*/
#define ID_ATSC_T 			  30 /* Used in the idType property to indicate a terrestrial ATSC channel identified by the property sourceID .*/
#define ID_IPTV_SDS 			  40 /* Used in the idType property to indicate an IP broadcast channel identified through SD&S by a DVB textual service identifier specified in the format
														“ServiceName.DomainName” as value for property ipBroadcastID with ServiceName and DomainName as defined in [DVB-IPTV]. This idType
														SHALL be used to indicate Scheduled content service defined by [OIPF_PROT2] */
#define ID_IPTV_URI 			  41 /* Used in the idType property to indicate an IP broadcast channel identified by a DVB MCAST URI (e.g. i.e. dvb-mcast://), as value for property
ipBroadcastID . */
#endif
typedef struct
{
	NPObject header;
	NPP npp;
#if 0
#if 0
	double* value;                  /* readonly Integer */
	double* maximumValue;  /* readonly Integer */
#else
	double channelType;
    double idType;
    double ccid;
    double tunerID;
	double onid;
    double tsid;
    double sid;
    double sourceID;
    double freq;
    double cni;
    char name[2048];
#endif
#endif
} NPObj_Programmes;

NPClass* fillPROGRAMMESclass(void);

NPObject *  PROGRAMMES_Allocate(NPP npp, NPClass *aClass);
void PROGRAMMES_Deallocate(NPObject *obj);
void PROGRAMMES_Invalidate(NPObject *obj);
bool PROGRAMMES_HasMethod(NPObject *obj, NPIdentifier name);
bool PROGRAMMES_Invoke(NPObject *obj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool PROGRAMMES_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool PROGRAMMES_HasProperty(NPObject *obj, NPIdentifier name);
bool PROGRAMMES_GetProperty(NPObject *obj, NPIdentifier name, NPVariant *result);
bool PROGRAMMES_SetProperty(NPObject *obj, NPIdentifier name, const NPVariant *value);
bool PROGRAMMES_RemoveProperty(NPObject *npobj, NPIdentifier name);
bool PROGRAMMES_Enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count);
void  PROGRAMMES_Invoke_SetValue(NPObject* obj,const NPVariant* args, uint32_t argCount, NPVariant *result);

#endif
