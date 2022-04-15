
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
#include "hbbtvbrowserpluginapi.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#define MAIN_W 720
#define MAIN_H 576
#define SUB_W 1280
#define SUB_H 720

void OnNoFullscreenSetWindow(int x, int y, int width, int height)
{

	char cmd[256];
	int fd;
	char data[256];
	int _x, _y, _w, _h;
	int _x1, _y1, _w1, _h1;
	
    debug( "OnNoFullscreenSetWindow %d %d %d %d\n", x, y, width, height);

	if (width == 0 || height == 0)
	{
		debug( "OnNoFullscreenSetWindow error width %d height %d\n", width, height);
		return;
	}

	if ((x == 0) && ( y == 0) && (width == 1280) && ( height == 720))
	{
		OnVIDBRC_SetFullScreen(0);
		return;
	}

    _x = x * MAIN_W / SUB_W;
	_w = width * MAIN_W / SUB_W;
	_y = y * MAIN_H / SUB_H;
	_h = height * MAIN_H / SUB_H;

	fd = open("/proc/stb/vmpeg/0/dst_left", O_RDONLY);
	memset(data, 0, 256);
	read (fd, data, 16);
	_x1 = strtol(data, NULL, 16);
	close(fd);

	fd = open("/proc/stb/vmpeg/0/dst_top", O_RDONLY);
	memset(data, 0, 256);
	read (fd, data, 16);
	_y1 = strtol(data, NULL, 16);
	close(fd);

	fd = open("/proc/stb/vmpeg/0/dst_width", O_RDONLY);
	memset(data, 0, 256);
	read (fd, data, 16);
	_w1 = strtol(data, NULL, 16);
	close(fd);

	fd = open("/proc/stb/vmpeg/0/dst_height", O_RDONLY);
	memset(data, 0, 256);
	read (fd, data, 16);
	_h1 = strtol(data, NULL, 16);
	close(fd);

	if ( _x == _x1 && _y==_y1 && _w == _w1 && _h == _h1)
	{
		debug( "OnNoFullscreenSetWindow same size %d %d %d %d\n", x, y, width, height);
		return;
	}
	
	//memset (cmd , 0, 256);
	//sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_apply", 0);
	//system(cmd);	
	memset (cmd , 0, 256);
	sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_left", _x);
	system(cmd);
	memset (cmd , 0, 256);
	sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_top", _y);
	system(cmd);
	memset (cmd , 0, 256);
	sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_height", _h);
	system(cmd);
	memset (cmd , 0, 256);
	sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_width", _w);
	system(cmd);
	memset (cmd , 0, 256);
	sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_apply", 1);
	system(cmd);

}

void OnAPPLICATION_Show()
{

}

void OnAPPLICATION_Hide()
{
}

void OnVIDBRC_SetFullScreen(int fullscreenparam)
{
	char cmd[256];
	int fd;
	char data[256];
	int _x, _y, _w, _h;
	int _x1, _y1, _w1, _h1;

    debug( "OnVIDBRC_SetFullScreen\n");

	fd = open("/proc/stb/vmpeg/0/dst_left", O_RDONLY);
	memset(data, 0, 256);
	read (fd, data, 16);
	_x1 = strtol(data, NULL, 16);
	close(fd);

	fd = open("/proc/stb/vmpeg/0/dst_top", O_RDONLY);
	memset(data, 0, 256);
	read (fd, data, 16);
	_y1 = strtol(data, NULL, 16);
	close(fd);

	fd = open("/proc/stb/vmpeg/0/dst_width", O_RDONLY);
	memset(data, 0, 256);
	read (fd, data, 16);
	_w1 = strtol(data, NULL, 16);
	close(fd);

	fd = open("/proc/stb/vmpeg/0/dst_height", O_RDONLY);
	memset(data, 0, 256);
	read (fd, data, 16);
	_h1 = strtol(data, NULL, 16);
	close(fd);

	if (_x1 == 0 && _y1 == 0 && _w1 == 720 && _h1==576)
	{
		debug( "OnVIDBRC_SetFullScreen same size \n");
		return;
	}

	//memset (cmd , 0, 256);
	//sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_apply", 0);
	//system(cmd);
	memset (cmd , 0, 256);
	sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_left", 0);
	system(cmd);
	memset (cmd , 0, 256);
	sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_top", 0);
	system(cmd);
	memset (cmd , 0, 256);
	sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_height", 576);
	system(cmd);
	memset (cmd , 0, 256);
	sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_width", 720);
	system(cmd);
	memset (cmd , 0, 256);
	sprintf(cmd, "echo %x > /proc/stb/vmpeg/0/dst_apply", 1);
	system(cmd);
}

void OnKEYSET_SetValue(double param)
{
}

void OnKEYSET_Allocate()
{
}
