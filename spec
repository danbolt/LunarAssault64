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
	name "kaiju1"
	flags OBJECT
	after "stage"
	include "kaiju1.o"
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

beginwave
	name	"gameplay_wave"
	include	"code"
	include	"stage"
	include	"kaiju1"
	include "moon_geo"
	include "level1_terrain"
	include "level1_topography"
endwave

beginwave
	name	"dialogue_wave"
	include	"code"
	include	"dialogue"
endwave
