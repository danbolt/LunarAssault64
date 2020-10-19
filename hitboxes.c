#include "hitboxes.h"

void parentHitboxes(KaijuHitbox* child, KaijuHitbox* newParent) {
  if (newParent->numberOfChildren == MAX_CHILDREN_PER_HITBOX) {
    return;
  }

  newParent->children[newParent->numberOfChildren++] = (struct KaijuHitbox*)(child);
  child->parent = (struct KaijuHitbox*)(newParent);
}