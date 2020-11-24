
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

#  Directory holding the HVQM2 library (libhvqm2.a)
HVQMLIBDIR = ../libhvqm2/lib
#  Directory holding the HVQM2 include file
HVQMINCDIR = ../libhvqm2/lib

NUAUDIOLIB = -lnualsgi_n -lgn_audio

LCDEFS =	-DNU_DEBUG -DF3DEX_GBI_2
LCINCS =	-I. -I$(NUSYSINCDIR) -I$(HVQMLIBDIR) -I$(ROOT)/usr/include/PR
LCOPTS =	-G 0
LDFLAGS = $(MKDEPOPT) -L$(LIB) -L$(NUSYSLIBDIR) -L$(HVQMLIBDIR) $(NUAUDIOLIB) -lhvqm2 -lnusys -lgultra -L$(GCCDIR)/mipse/lib -lkmc

OPTIMIZER = -O2

APP =		jam.out

TARGETS =	jam.n64

HFILES =	main.h font.h stage00.h graphic.h gamemath.h segmentinfo.h terraintex.h map.h hitboxes.h portraittex.h protaggeo.h kaiju1.h kaiju2.h dialoguestage.h hvqm.h fmvstage.h 

CODEFILES   = 	main.c graphic.c gfxinit.c gamemath.c hvqmgfxinit.c hvqmmain.c hvqmaudio.c hvqmcopyframebuffer.c

CODEOBJECTS =	$(CODEFILES:.c=.o)  $(NUSYSLIBDIR)/nusys.o

DATAFILES   =	kaiju1.c kaiju2.c titlescreenstage.c hvqmwork.c hvqmaudiobuf.c hvqmvideobuf.c labbg.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

STAGEFILES  =	stage00.c map.c hitboxes.c portraittex.c protaggeo.c 

HQVMCODEFILES = fmvstage.c

DIALFILES   =	dialoguestage.c

DIALOBJ     =	$(DIALFILES:.c=.o)

STAGEOBJ    =	$(STAGEFILES:.c=.o)

HQVMOBJ    =	$(HQVMCODEFILES:.c=.o)

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS) $(STAGEOBJ) $(DIALOBJ) $(HQVMOBJ)

FORCELINK =	-u guOrtho \
			-u guPerspective \
			-u guLookAt \
			-u guRotate \
			-u guScale \
			-u guTranslate \
			-u guRandom \
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
			-u nuDebConCPuts \
			-u nuDebTaskPerfBar1EX2 \
			-u nuDebConDispEX2 \
			-u lerp \
			-u atan2bodyf \
			-u nu_atan2 \
			-u mat4x4_invert \
			-u fabs_d \
			-u cubic \
			-u catmullRom \
			-u bilinear \
			-u clamp \
			-u distanceSq \
			-u length \
			-u Q_rsqrt \
			-u dotProduct \
			-u sdSphere \
			-u sdOctahedron \
			-u sdBox \
			-u nuAuSeqPlayerStop \
			-u nuAuSeqPlayerSetNo \
			-u nuAuSeqPlayerPlay \
			-u nuAuSndPlayerPlay \
			-u nuAuSeqPlayerSetVol \
			-u n_alSndpSetPitch \
			-u hvqm2InitSP1 \
			-u hvqm2SetupSP1 \
			-u hvqm2DecodeSP1 \
			-u osPiStartDma \
			-u hvqmCopyFrameBuffer \
			-u guS2DInitBg \
			-u adpcmDecode \


default:        $(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS) Makefile
		$(LD) $(FORCELINK) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS):	$(OBJECTS)
		$(MAKEROM) -o spec -I$(NUSYSINCDIR) -r $(TARGETS) -e $(APP)
