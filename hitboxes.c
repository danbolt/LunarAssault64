#include "hitboxes.h"

KaijuHitbox hitboxes[NUMBER_OF_KAIJU_HITBOXES];

void parentHitboxes(KaijuHitbox* child, KaijuHitbox* newParent) {
  if (newParent->numberOfChildren == MAX_CHILDREN_PER_HITBOX) {
    return;
  }

  newParent->children[newParent->numberOfChildren++] = (struct KaijuHitbox*)(child);
  child->parent = (struct KaijuHitbox*)(newParent);
}