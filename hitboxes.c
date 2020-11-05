#include "hitboxes.h"

KaijuHitbox hitboxes[NUMBER_OF_KAIJU_HITBOXES];

void parentHitboxes(KaijuHitbox* child, KaijuHitbox* newParent) {
  if (newParent->numberOfChildren == MAX_CHILDREN_PER_HITBOX) {
    return;
  }

  newParent->children[newParent->numberOfChildren++] = (struct KaijuHitbox*)(child);
  child->parent = (struct KaijuHitbox*)(newParent);
}

void appendHitboxDL(KaijuHitbox* hitbox, DisplayData* dynamicp) {
  int i;

  if (!(hitbox->alive)) {
    return;
  }

  guMtxF2L(hitbox->computedTransform.data, &(dynamicp->hitboxTransform[dynamicp->hitboxTransformCount]));
  gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->hitboxTransform[dynamicp->hitboxTransformCount])), G_MTX_MODELVIEW | G_MTX_PUSH);
  gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(hitbox->displayCommands));
  dynamicp->hitboxTransformCount++;

  for (i = 0; i < hitbox->numberOfChildren; i++) {
    appendHitboxDL(hitbox->children[i], dynamicp);
  }

  gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
}