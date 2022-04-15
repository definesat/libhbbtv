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

#include <mqueue.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "hbbtvcommonutil.h"
#include "currentchannelclass.h"

static unsigned char url[1024];
static int playstate = PLAYSTATE_STOPPED;
static time_t last_seconds = 0;

int get_playstate(void)
{
	return playstate;
}

void set_playstate(int state)
{
	playstate = state;
}

int set_url(char *_url, int len)
{
	int res = 0;
	if (strncmp(url, _url, len) || (strlen(url) != len))
	{
		if ((playstate > PLAYSTATE_STOPPED) &&  (playstate <  PLAYSTATE_FINISHED))
			res = 1;
	}
	memset(url, 0, 1024);
	snprintf(url, len, "%s",_url);
	debug("set_url : %s\n", url);

	return res;
}

int clear_url(void)
{
	memset(url, 0, 1024);
	return 0;
}

int check_url(void)
{
	return strlen(url);
}

int send_cmd(int cmd, void *out_data)
{
	int cmd_result = 0;
	int sockfd;
	struct sockaddr_un servaddr;
	int data[1024];
	char *c_data = (char *)data;
	int len, ret;
	
	if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
	{
			debug("socket_error\n");
			return cmd_result;
	}

	memset(&servaddr,0, sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, "/tmp/.browser.support");

	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
			debug("connect_error\n");
			close(sockfd);
			return cmd_result;
	}

	memset(data,0,1024);

	switch (cmd)
	{
		case CONTROL_PAUSE_PLAYING:
			data[0] = cmd;
			ret = write(sockfd,data,16);
			ret = read(sockfd,data,1024);
			debug("read data op %d result %d len %d\n", data[0], data[1], data[2]);
			cmd_result = data[1];
			playstate = PLAYSTATE_PAUSED;
			break;
		case CONTROL_RESUME_PLAYING:
			debug("!!!!!!!!!!!!!!!!!!!!!! CONTROL_RESUME_PLAYING\n");
			data[0] = cmd;
			ret = write(sockfd,data,16);
			ret = read(sockfd,data,1024);
			debug("read data op %d result %d len %d\n", data[0], data[1], data[2]);
			cmd_result = data[1];
#ifdef PLAY_STATE_PATCH
			playstate = PLAYSTATE_CONNECTING;
#else
			playstate = PLAYSTATE_PLAYING;
#endif
			break;	
		case CONTROL_SKIP:
		{
			int *tmp_data = (int *)out_data;

			time_t cur_seconds = 0;
			time (&cur_seconds);
			
			if (cur_seconds - last_seconds < 1)
			{
				close(sockfd);
				return 0;
			}

			last_seconds = cur_seconds;
			debug("last_seconds %d\n", last_seconds);
		
			data[0] = cmd;
			data[2] = 12 + 1;
			data[3] = tmp_data[0];
			ret = write(sockfd,data,16);
			ret = read(sockfd,data,1024);
			debug("read data op %d result %d len %d\n", data[0], data[1], data[2]);
			cmd_result = data[1];
			
			break;
		}
		case CONTROL_STOP :
			debug("!!!!!!!!!!!!!!!!!!!!!! CONTROL_STOP\n");
			if (playstate == PLAYSTATE_PAUSED || playstate == PLAYSTATE_PLAYING)
			{
				data[0] = cmd;
				ret = write(sockfd,data,16);
				ret = read(sockfd,data,1024);
				debug("read data op %d result %d len %d\n", data[0], data[1], data[2]);
				cmd_result = data[1];
			}
			
			playstate = PLAYSTATE_STOPPED;
			{
				mqd_t mq;
			    char buffer[QUEUE_MAX_SIZE];
			    /* open the mail queue */
			    mq = mq_open(QUEUE_NAME, O_WRONLY);
			    snprintf(buffer, sizeof(buffer), "PLAYSTOP");
			    mq_send(mq, buffer, QUEUE_MAX_SIZE, 0);
			    mq_close(mq);
			    sleep(1);
			}
			break;
		case CONTROL_OPENURL :
			debug("!!!!!!!!!!!!!!!!!!!!!! CONTROL_OPENURL\n");
			data[0] = cmd;
			len = strlen(url);

			data[2] = len + 12;
			sprintf(&c_data[12],"%s",url);
			ret = write(sockfd,data,len+12);
			ret = read(sockfd,data,1024);
			debug("read data op %d result %d len %d\n", data[0], data[1], data[2]);
			cmd_result = data[1];

#ifdef PLAY_STATE_PATCH
			playstate = PLAYSTATE_CONNECTING;
#else
			playstate = PLAYSTATE_PLAYING;
#endif
			break;
		case CONTROL_GET_POS_LEN :
		{
			int *tmp_data = (int *)out_data;
			data[0] = cmd;
			ret = write(sockfd,data,20);
			ret = read(sockfd,data,1024);
			debug("read data op %d result %d len %d pos %d playtime %d state %d\n", data[0], data[1], data[2], data[4], data[3], data[5]);
			cmd_result = data[1];
			
			tmp_data[1] = data[3];
			tmp_data[0] = data[4];
			tmp_data[2] = data[5];

			break;
		}
		case CONTROL_GET_PLAYSTATE:
		{
			int *tmp_data = (int *)out_data;
			data[0] = cmd;
			ret = write(sockfd,data,16);
			ret = read(sockfd,data,1024);
			debug("read data op %d result %d len %d mediastate %d \n", data[0], data[1], data[2], data[3]);
			cmd_result = data[1];
			tmp_data[0] = data[3];

			break;
		}		
		case OOIF_BROADCAST_CHECK :
			data[0] = cmd; /*OOIF_BROADCAST_CHECK*/
			ret = write(sockfd,data,16);
			ret = read(sockfd,data,1024);
			debug("read data op %d result %d len %d\n", data[0], data[1], data[2]);
			cmd_result = data[1];
			break;
		case OOIF_BROADCAST_STOP :
			debug("!!!!!!!!!!!!!!!!!!!!!! OOIF_BROADCAST_STOP\n");
			data[0] = cmd; /*OOIF_BROADCAST_STOP*/
			ret = write(sockfd,data,16);
			ret = read(sockfd,data,1024);
			debug("read data op %d result %d len %d\n", data[0], data[1], data[2]);
			cmd_result = data[1];
			break;

		case OOIF_BROADCAST_PLAY :
			debug("!!!!!!!!!!!!!!!!!!!!!! OOIF_BROADCAST_PLAY\n");
			data[0] = cmd; /*OOIF_BROADCAST_STOP*/
			ret = write(sockfd,data,16);
			ret = read(sockfd,data,1024);
			debug("read data op %d result %d len %d\n", data[0], data[1], data[2]);
			cmd_result = data[1];
			break;


		case OOIF_GET_CURRENT_CHANNEL :
		{
			NPObj_CurrentChannel* newcurrentchannel = (NPObj_CurrentChannel*) out_data;
			debug("!!!!!!!!!!!!!!!!!!!!!! OOIF_GET_CURRENT_CHANNEL\n");
			data[0] = cmd; /*OOIF_GET_CURRENT_CHANNEL*/
			ret = write(sockfd,data,16);
			ret = read(sockfd,data,1024);
#if 1
			newcurrentchannel->sid = data[4];
			newcurrentchannel->onid = data[5];
			newcurrentchannel->tsid = data[6];
#else
			newcurrentchannel->sid = 128;
			newcurrentchannel->onid = 8468;
			newcurrentchannel->tsid = 2561;
#endif
			/*default*/
			newcurrentchannel->channelType = TYPE_TV;
			newcurrentchannel->idType =	ID_DVB_S2;

#if 1
			snprintf(newcurrentchannel->name, data[7], "%s", &c_data[8*4]);
			debug("%s\n", newcurrentchannel->name);
#else
			sprintf(newcurrentchannel->name, "Das Erste");
#endif

			break;

			
		}
		case HBBTV_GETURLAIT:
		{
			int *tmp_data = (int *)out_data;
			char *tmp_c_data = (char *)out_data;
			debug("!!!!!!!!!!!!!!!!!!!!!! HBBTV_GETURLAIT\n");
			data[0] = cmd; /*HBBTV_GETURLAIT*/
			data[2] = 12 + 8;
			
			data[3] = tmp_data[0];
			data[4] = tmp_data[1];

			ret = write(sockfd,data,20);
			ret = read(sockfd,data,1024);
			debug("read data op %d result %d len %d\n", data[0], data[1], data[2]);
			debug("url %s\n", &c_data[12]);
			if (data[1] == 1)
			{
				cmd_result = 1;
				memcpy(tmp_c_data, &c_data[12], strlen(&c_data[12]));
			}
			else
			{
				cmd_result = 0;
			}
			break;
		}
		default:
			break;

	}
	
	close(sockfd);

	return cmd_result;

}







