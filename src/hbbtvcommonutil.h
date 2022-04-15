#ifndef _HBBTVCOMMONTUIL_H_
#define _HBBTVCOMMONTUIL_H_

#if 0
#define PLAYSTATE_UNSTARTED 	-1	
#define PLAYSTATE_ENDED 		0	
#define PLAYSTATE_PLAYING 		1	
#define PLAYSTATE_PAUSE 		2	
#define PLAYSTATE_BUFFERING 	3	
#define PLAYSTATE_VIDEOCUED 	4	
#else
#define PLAYSTATE_UNSTARTED 	-1	
#define PLAYSTATE_STOPPED 		0	
#define PLAYSTATE_PLAYING 		1	
#define PLAYSTATE_PAUSED 		2	
#define PLAYSTATE_CONNECTING 	3	
#define PLAYSTATE_BUFFERING 	4	
#define PLAYSTATE_FINISHED 		5	
#define PLAYSTATE_ERROR 		6

#define ERRORSTATE_0 0 /*- A/V format not supported.*/
#define ERRORSTATE_1 1 /*- cannot connect to server or connection lost.*/
#define ERRORSTATE_2 2 /*- unidentified error.*/
#define ERRORSTATE_3 3 /*- insufficient resources.*/
#define ERRORSTATE_4 4 /* content corrupt or invalid.*/
#define ERRORSTATE_5 5 /* content not available.*/
#define ERRORSTATE_6 6 /* content not available at given position.*/

#endif

#define CONTROL_BACK 			0
#define CONTROL_FORWARD 		1
#define CONTROL_STOP			2
#define CONTROL_RELOAD			3
#define CONTROL_OPENURL			4
#define CONTROL_EXIT			5
#define CONTROL_TITLE			6
#define CONTROL_OK				7
#define CONTROL_OUT_OF_MEMORY	8
#define CONTROL_INVALIDATE		9
#define CONTROL_GET_FBSIZE		10
#define CONTROL_GET_VOLUME		11
#define CONTROL_SET_VOLUME		12
#define CONTROL_VOLUME_UP		13
#define CONTROL_VOLUME_DOWN		14
#define HBBTV_LOADAIT			15
#define HBBTV_OPENURL			16
#define YOUTUBETV_OPENURL		17
#define BROWSER_OPENURL			18
#define BROWSER_MENU_OPEN		19
#define BROWSER_MENU_CLOSE		20
#define BROWSER_VKB_OPEN		21
#define BROWSER_VKB_CLOSE		22
#define OOIF_GET_CURRENT_CHANNEL	23
#define OOIF_BROADCAST_PLAY			24
#define OOIF_BROADCAST_STOP			25
#define OOIF_BROADCAST_CHECK		26
#define CONTROL_RELOAD_KEYMAP		27
#define GET_TIME_OFFSET				28
#define CONTROL_CHANNEL_UP			29
#define CONTROL_CHANNEL_DOWN		30
#define CONTROL_GET_POS_LEN			31
#define CONTROL_PAUSE_PLAYING		32
#define CONTROL_RESUME_PLAYING		33
#define CONTROL_SKIP				34
#define CONTROL_GET_PLAYSTATE		35
#define HBBTV_GETURLAIT			36

#define OPCODE_END					37


#define QUEUE_NAME  "/hbbtv_queue"
#define QUEUE_MAX_SIZE    256
#define MSG_STOP "exit"

#define PLAY_STATE_PATCH 

int get_playstate(void);
void set_playstate(int state);

int set_url(char *_url, int len);
int clear_url(void);
int check_url(void);

int send_cmd(int cmd, void *data);
int check_security(void);


#endif

