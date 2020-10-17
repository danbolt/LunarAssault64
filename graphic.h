#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include "hitboxes.h"

/* The screen size  */
#define SCREEN_HT        240
#define SCREEN_WD        320

/* The maximum length of the display list of one task  */
#define GFX_GLIST_LEN     2048


typedef struct {
  Mtx     camera;
  Mtx     projection;

  Mtx     playerTranslation;
  Mtx     playerScale;
  Mtx     playerRotation;

  Mtx     mapScale;

  Mtx     hitboxTransform[NUMBER_OF_KAIJU_HITBOXES];

  Mtx     divineLineTransform;
  Mtx     divineLineScale;

  Mtx     orthoHudProjection;
  Mtx     orthoHudModelling;
} DisplayData;

/*-------------------------------- parameter---------------------------------*/
extern DisplayData gfx_dynamic[];
extern Gfx* glistp;
extern Gfx gfx_glist[][GFX_GLIST_LEN];
extern u32 gfx_gtask_no;

/*-------------------------------- function ---------------------------------*/
extern void gfxRCPInit(void);
extern void gfxClearCfb(void);

/*------------------------------- other extern define -----------------------*/
extern Gfx setup_rdpstate[];
extern Gfx setup_rspstate[];

#endif /* _GRAPHIC_H_ */



