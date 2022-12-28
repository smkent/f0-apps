#pragma once

typedef enum {
    SpriteWalkUp,
    SpriteWalkDown,
    SpriteWalkLeft,
    SpriteWalkRight,
} SpriteWalkDirection;

typedef struct {
    SpriteWalkDirection direction;
    IconAnimation* anim_active;
    IconAnimation* anim_up;
    IconAnimation* anim_down;
    IconAnimation* anim_left;
    IconAnimation* anim_right;
} SpriteWalk;

SpriteWalk* sprite_walk_alloc();
void sprite_walk_free(SpriteWalk* sprite_walk);

void sprite_walk_set_direction(SpriteWalk* sprite_walk, SpriteWalkDirection direction);
void sprite_walk_animation_start(SpriteWalk* sprite_walk);
void sprite_walk_animation_stop(SpriteWalk* sprite_walk);

void sprite_walk_draw(Canvas* const canvas, uint8_t x, uint8_t y, SpriteWalk* sprite_walk);
