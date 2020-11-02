
include $(ROOT)/usr/include/make/PRdefs

N64KITDIR    = c:\nintendo\n64kit
NUSYSINCDIR  = $(N64KITDIR)/nusys/include
NUSYSLIBDIR  = $(N64KITDIR)/nusys/lib

LIB = $(ROOT)/usr/lib
LPR = $(LIB)/PR
INC = $(ROOT)/usr/include
CC  = gcc
LD  = ld
MAKEROM = mild

LCDEFS =	-DNU_DEBUG -DF3DEX_GBI_2
LCINCS =	-I. -I$(NUSYSINCDIR) -I$(ROOT)/usr/include/PR
LCOPTS =	-G 0
LDFLAGS = $(MKDEPOPT) -L$(LIB) -L$(NUSYSLIBDIR) -lnusys_d -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc

OPTIMIZER =	-g

APP =		jam.out

TARGETS =	jam.n64

HFILES =	main.h font.h stage00.h graphic.h gamemath.h segmentinfo.h terraintex.h map.h hitboxes.h portraittex.h protaggeo.h kaiju1.h dialoguestage.h

CODEFILES   = 	main.c graphic.c gfxinit.c kaiju1.c

CODEOBJECTS =	$(CODEFILES:.c=.o)  $(NUSYSLIBDIR)/nusys.o

DATAFILES   =	stage00.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

STAGEFILES  =	stage00.c gamemath.c map.c hitboxes.c portraittex.c protaggeo.c 

DIALFILES   =	dialoguestage.c

DIALOBJ     =	$(DIALFILES:.c=.o)

STAGEOBJ    =	$(STAGEFILES:.c=.o)

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS) $(STAGEOBJ) $(DIALOBJ)

FORCELINK =	-u guOrtho \
			-u guPerspective \
			-u guLookAt \
			-u guRotate \
			-u guScale \
			-u guTranslate \
			-u guMtxIdent \
			-u guTranslateF \
			-u guRotateF \
			-u guScaleF \
			-u guRotateRPYF \
			-u guMtxCatF \
			-u guMtxXFMF \
			-u cosf \
			-u sinf \
			-u __cmpdi2 \
			-u __floatdisf \
			-u nuDebPerfMarkSet \
			-u nuPiReadRom \
			-u nuContDataGetEx \
			-u nuDebConTextPos \
			-u sprintf \
			-u gfxRCPInit \
			-u gfxClearCfb \

default:        $(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS) Makefile
		$(LD) $(FORCELINK) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS):	$(OBJECTS)
		$(MAKEROM) -o spec -I$(NUSYSINCDIR) -r $(TARGETS) -e $(APP)
