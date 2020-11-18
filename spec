/*
	ROM spec file

	Main memory map

  	0x80000000  exception vectors, ...
  	0x80000400  zbuffer (size 320*240*2)
  	0x80025c00  codesegment
	      :  
  	0x8038F800  cfb 16b 3buffer (size 320*240*2*3)

        Copyright (C) 1997-1999, NINTENDO Co,Ltd.
*/

#include <nusys.h>
#include "hvqm.h"

/* Use all graphic micro codes */
beginseg
	name	"code"
	flags	BOOT OBJECT
	entry 	nuBoot
	address NU_SPEC_BOOT_ADDR
        stack   NU_SPEC_BOOT_STACK
	include "codesegment.o"
	include "$(ROOT)/usr/lib/PR/rspboot.o"
	include "$(ROOT)/usr/lib/PR/gspF3DEX2.fifo.o"
	include "$(ROOT)/usr/lib/PR/gspF3DLX2.Rej.fifo.o"
	include "$(ROOT)/usr/lib/PR/n_aspMain.o"
endseg

beginseg
	name "stage"
	flags OBJECT
	after "code"
	align 32
	include "stage00.o"
	include "hitboxes.o"
	include "map.o"
	include "portraittex.o"
	include "protaggeo.o"
endseg

beginseg
	name "dialogue"
	flags OBJECT
	after "code"
	align 32
	include "dialoguestage.o"
endseg

beginseg
	name "titlescreen"
	flags OBJECT
	after "code"
	align 32
	include "titlescreenstage.o"
endseg

beginseg
	name "kaiju1"
	flags OBJECT
	after "stage"
	include "kaiju1.o"
endseg

beginseg
	name "kaiju2"
	flags OBJECT
	after "stage"
	include "kaiju2.o"
endseg

beginseg
	name "moon_geo"
	flags "RAW"
	include "geotex/moon_geo.bin"
endseg

beginseg
	name "level1_terrain"
	flags "RAW"
	include "terrain/level1.bin"
endseg

beginseg
	name "level1_topography"
	flags "RAW"
	include "topography/level1.bin"
endseg

beginseg
	name "audiobank_table"
	flags "RAW"
	include "audio/bank.tbl"
endseg

beginseg
	name "audiobank_control"
	flags "RAW"
	include "audio/bank.ctl"
endseg

beginseg
	name "songs"
	flags "RAW"
	include "audio/songs.sbk"
endseg

beginseg
	name "fmv"
	flags OBJECT
	after "code"
	include "fmvstage.o"
	include "$(ROOT)/usr/lib/PR/gspF3DEX2.fifo.o"
	include "$(ROOT)/usr/lib/PR/gspS2DEX2.fifo.o"
#if HVQM_CFB_FORMAT == 1
	include "../libhvqm2/rspcode/hvqm2sp1.o"/* HVQM2 microcode */
/*	include "$(ROOT)/usr/lib/PR/hvqm2sp1.o"	/* HVQM2 microcode */
#else /* HVQM_CFB_FORMAT */
	include "../libhvqm2/rspcode/hvqm2sp2.o"/* HVQM2 microcode */
/*	include "$(ROOT)/usr/lib/PR/hvqm2sp2.o"	/* HVQM2 microcode */
#endif /* HVQM_CFB_FORMAT */
endseg

beginseg
	name	"hvqmvideobuf"
	flags	OBJECT
	address 0x80180000	/* 16byte alignment */
	include "hvqmvideobuf.o"
endseg

beginseg
	name	"hvqmaudiobuf"
	flags	OBJECT
	address 0x80200000	/* 16byte alignment */
	include "hvqmaudiobuf.o"
endseg

beginseg
	name	"hvqwork"
	flags	OBJECT
	address 0x80300000
	include "hvqmwork.o"
endseg

beginseg
	name	"hvqmdata"
	flags	RAW
	include "../sample/sample2.hvqm"
endseg

beginwave
	name	"gameplay_wave_stage1"
	include	"code"
	include	"stage"
	include	"kaiju1"
endwave

beginwave
	name	"gameplay_wave_stage2"
	include	"code"
	include	"stage"
	include	"kaiju2"
endwave

beginwave
	name	"dialogue_wave"
	include	"code"
	include	"dialogue"
endwave

beginwave
	name	"titlescreen_wave"
	include	"code"
	include	"titlescreen"
endwave

beginwave
	name	"fmv_wave"
	include	"code"
	include	"fmv"
 	include  "hvqmdata"
 	include	"hvqmvideobuf"
 	include	"hvqmaudiobuf"
 	include	"hvqwork"
endwave

