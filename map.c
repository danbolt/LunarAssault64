
#include "map.h"

u8 GroundMapping[MAP_LENGTH];
u8 GroundTexIndex[MAP_LENGTH];

SectionData sections[SECTIONS_PER_MAP];
SectionData lowDetailSections[SECTIONS_PER_MAP];

u8 sampleHeight(u16 x, u16 y) {
  return GroundMapping[((x + MAP_WIDTH) % MAP_WIDTH) + (((y + MAP_HEIGHT) % MAP_HEIGHT) * MAP_WIDTH)];
}
u8 sampleTileTex(u16 x, u16 y) {
  return GroundTexIndex[((x + MAP_WIDTH) % MAP_WIDTH) + (((y + MAP_HEIGHT) % MAP_HEIGHT) * MAP_WIDTH)];
}

void generateDLPerSection(SectionData* section, int originX, int originY) {
  int i;
  int j;
  Vtx* verts = section->verts;
  Gfx* commands = section->commands;

  section->centroid = (vec3){ originX + (SECTION_SIZE * 0.5f), originY + (SECTION_SIZE * 0.5f), 0.f };
  guTranslate(&(section->transform), originX * 100, originY * 100, 0.f);

  for (i = 0; i < SECTION_SIZE; i++) {
    for (j = 0; j < SECTION_SIZE; j++) {
      const int texIndex = sampleTileTex(originX+i, originY+j);
      const int mid = sampleHeight(originX+i, originY+j);
      const int n = sampleHeight(originX+i, originY+j-1);
      const int s = sampleHeight(originX+i, originY+j+1);
      const int e = sampleHeight(originX+i+1, originY+j);
      const int w = sampleHeight(originX+i-1, originY+j);
      const int ne = sampleHeight(originX+i+1, originY+j-1);
      const int se = sampleHeight(originX+i+1, originY+j+1);
      const int nw = sampleHeight(originX+i-1, originY+j-1);
      const int sw = sampleHeight(originX+i-1, originY+j+1);
      const float heightA = (((nw + n + mid + w) * 0.25f) / 256.f) * 50.f * 100.f;
      const float heightB = (((n + ne + e + mid) * 0.25f) / 256.f) * 50.f * 100.f;
      const float heightC = (((mid + e + se + s) * 0.25f) / 256.f) * 50.f * 100.f;
      const float heightD = (((w + mid + s + sw) * 0.25f) / 256.f) * 50.f * 100.f;

      if (i == 1 && j == 1) {
        section->centroid.z = heightA / 100.f;
      }

      *(verts++) = (Vtx){(i * 100) -   5, (j * 100) -   5, heightA, 0, ((texIndex % 4) * 8) << 6,     (((texIndex / 4) % 4) * 8) << 6, 0x11, (u8)(0xff * heightA / 2000.f), 0x13, 0xff };
      *(verts++) = (Vtx){(i * 100) + 105, (j * 100) -   5, heightB, 0, ((texIndex % 4) * 8 + 7) << 6, (((texIndex / 4) % 4) * 8) << 6, 0x11, (u8)(0xff * heightB / 2000.f), 0x13, 0xff };
      *(verts++) = (Vtx){(i * 100) + 105, (j * 100) + 105, heightC, 0, ((texIndex % 4) * 8 + 7) << 6, (((texIndex / 4) % 4) * 8 + 7) << 6, 0x11, (u8)(0xff * heightC / 2000.f), 0x13, 0xff };
      *(verts++) = (Vtx){(i * 100) -   5, (j * 100) + 105, heightD, 0, ((texIndex % 4) * 8) << 6,     (((texIndex / 4) % 4) * 8 + 7) << 6, 0x11, (u8)(0xff * heightD / 2000.f), 0x13, 0xff };
    }
  }

  gSPMatrix(commands++, OS_K0_TO_PHYSICAL(&(section->transform)), G_MTX_MODELVIEW | G_MTX_PUSH);
  gSPVertex(commands++, section->verts, VERTS_PER_SECTION, 0);
  for (i = 0; i < 64; i += 4) {
    gSP2Triangles(commands++, i + 0, i + 1, i + 2, 0, i + 0, i + 2, i + 3, 0);
  }
  gSPPopMatrix(commands++, G_MTX_MODELVIEW);
  gSPEndDisplayList(commands++);
}

void generateLowDetailDLPerSection(SectionData* section, int originX, int originY) {
  int i;
  int j;
  int texIndex;
  float heightA;
  float heightB;
  float heightC;
  float heightD;
  Vtx* verts = section->verts;
  Gfx* commands = section->commands;

  section->centroid = (vec3){ originX + (SECTION_SIZE / 2), originY + (SECTION_SIZE / 2), 0.f };
  guTranslate(&(section->transform), originX * 100, originY * 100, 0.f);

  texIndex = sampleTileTex(originX, originY);
  heightA = (sampleHeight(originX - 1 , originY - 1) / 256.f) * 50.f * 100.f;
  heightB = (sampleHeight(originX + 4, originY - 1) / 256.f) * 50.f * 100.f;
  heightC = (sampleHeight(originX + 4, originY + 4) / 256.f) * 50.f * 100.f;
  heightD = (sampleHeight(originX - 1 , originY + 4) / 256.f) * 50.f * 100.f;

  *(verts++) = (Vtx){(originX) -   100, (originY) -   100, heightA - 1.f, 0, ((texIndex % 4) * 8) << 6,     (((texIndex / 4) % 4) * 8) << 6, 0x11, (u8)(0xff * heightA / 2000.f), 0x13, 0xff };
  *(verts++) = (Vtx){(originX) + 500, (originY) -   100, heightB - 1.f, 0, ((texIndex % 4) * 8 + 7) << 6, (((texIndex / 4) % 4) * 8) << 6, 0x11, (u8)(0xff * heightB / 2000.f), 0x13, 0xff };
  *(verts++) = (Vtx){(originX) + 500, (originY) + 500, heightC - 1.f, 0, ((texIndex % 4) * 8 + 7) << 6, (((texIndex / 4) % 4) * 8 + 7) << 6, 0x11, (u8)(0xff * heightC / 2000.f), 0x13, 0xff };
  *(verts++) = (Vtx){(originX) -   100, (originY) + 500, heightD - 1.f, 0, ((texIndex % 4) * 8) << 6,     (((texIndex / 4) % 4) * 8 + 7) << 6, 0x11, (u8)(0xff * heightD / 2000.f), 0x13, 0xff };

  gSPMatrix(commands++, OS_K0_TO_PHYSICAL(&(section->transform)), G_MTX_MODELVIEW | G_MTX_PUSH);
  gSPVertex(commands++, section->verts, 4, 0);
  gSP2Triangles(commands++, 0, 1, 2, 0, 0, 2, 3, 0);
  gSPPopMatrix(commands++, G_MTX_MODELVIEW);
  gSPEndDisplayList(commands++);
}

void generateSectionDLs() {
  int i;

  for (i = 0; i < SECTIONS_PER_MAP; i++) {
    int xOrigin = (i % (MAP_WIDTH / SECTION_SIZE)) * SECTION_SIZE;
    int yOrigin = (i / (MAP_WIDTH / SECTION_SIZE)) * SECTION_SIZE;

    generateDLPerSection(&(sections[i]), xOrigin, yOrigin);
    generateLowDetailDLPerSection(&(lowDetailSections[i]), xOrigin, yOrigin);
  }
}
