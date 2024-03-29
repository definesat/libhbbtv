###############################################################################
####
#### MAKEFILE
####
#### Copyright Telecom Paristech 2011  
####
#### Author : Stanislas Selle
####
###############################################################################

###############################################################################
## Files and Path

PLUGINDIR = /usr/lib/mozilla/plugins
INSTALL_INCLUDEDIR = /usr/local/include/hbbtvbrowserplugin
INSTALL_LIBDIR = /usr/local/lib
INSTALL_PKGDIR = /usr/lib/pkgconfig

#PKGFILE = hbbtvbrowserplugin.pc

MAINTARGET = bin/libhbbtvbrowserplugin.so

SRCDIR = src
HEADERSDIR = src

OBJ = 	obj/hbbtvbrowserplugin.o\
		obj/hbbtvbrowserpluginapi.o \
		obj/oipfapplicationmanager.o\
		obj/applicationclass.o\
		obj/applicationprivatedataclass.o\
		obj/oipfconfiguration.o\
		obj/configurationclass.o\
		obj/capabilityclass.o\
		obj/currentchannelclass.o\
		obj/oipfdownloadmanager.o\
		obj/oipfdownloadtrigger.o\
		obj/downloadclass.o\
		obj/downloadcollectionclass.o\
		obj/drmcontrolinfocollectionclass.o\
		obj/drmcontrolinformationclass.o\
		obj/videobroadcast.o\
        obj/videoplayer.o\
		obj/keysetclass.o\
		obj/hbbtvcommonutil.o

EXPORTHEADERS = hbbtvbrowserpluginapi.h

#TESTURL = file:///home/selle/ressources/HbbTVapps/TPT/app1/index.html
TESTURL = http://aquila.enst.fr:8080/subwebsite/hbbtvtest/test0003/index.php
TESTLOG = /tmp/test-err.txt
###############################################################################
## Programs

COMPILER	= ${CC}
#COMPILER	= arm-linux-g++

DELETER		= rm -f
BROWSERTEST	= firefox


###############################################################################
## Options, Flags and LinkS

CFLAGS = -Wall -DXP_UNIX=1 -DQ_WS_WIN=1 -DMOZ_X11=1 -fPIC -g

INCLUDEFLAGS = -Isrc -Isrc/webkit-plugin-header
LIBRARYFLAGS =

#FROMPKG = libxul

#PKGFLAGS = `pkg-config --cflags $(FROMPKG) `
#PKGLIBS = `pkg-config --libs $(FROMPKG) `

INCLUDEFLAGS += $(PKGFLAGS)
LIBRARYFLAGS += $(PKGLIBS)

###############################################################################
## Rules

all : $(MAINTARGET)

prepare:
	mkdir bin
	mkdir obj

$(MAINTARGET) : prepare $(OBJ)
	@echo "====> Compiling $(MAINTARGET)"
	$(COMPILER) $(CFLAGS) $(LIBRARYFLAGS) -shared $(OBJ) -o $(MAINTARGET)

obj/%.o: $(SRCDIR)/%.c $(HEADERSDIR)/%.h
	@echo "====> Compiling $< "
	$(COMPILER) $(CFLAGS) -c $< $(INCLUDEFLAGS) -o $@

test : all
	$(BROWSERTEST) $(TESTURL)  2> $(TESTLOG); cat $(TESTLOG)

install : all
	@echo "====> Installing"
	cp $(MAINTARGET) $(PLUGINDIR)
	@if test -d $(INSTALL_INCLUDEDIR);\
		then echo " ===> $(INSTALL_INCLUDEDIR) existing already" ; \
	else mkdir $(INSTALL_INCLUDEDIR); fi;
	cp $(HEADERSDIR)/$(EXPORTHEADERS) $(INSTALL_INCLUDEDIR)
	cp $(PKGFILE) $(INSTALL_PKGDIR)
	@echo "Done."

uninstall :
	@echo "====> Uninstalling"
	$(DELETER) $(INSTALL_PKGDIR)/$(PKGFILE) $(INSTALL_INCLUDEDIR)/$(EXPORTHEADERS) $(INSTALL_INCLUDEDIR)/$(MAINTARGET)
	if test -d $(INSTALL_INCLUDEDIR);\
		then rmdir $(INSTALL_INCLUDEDIR); fi;

clean :
	@echo "====> Cleaning"
	@$(DELETER) $(OBJ)
	@echo "Deleting objects"
	@$(DELETER) $(MAINTARGET)
	@echo "Deleting $(MAINTARGET)"

###############################################################################
###############################################################################
