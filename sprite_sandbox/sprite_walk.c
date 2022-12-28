#include "app.h"

#include "sprite_walk.h"
#include "sprite_sandbox_icons.h"

SpriteWalk* sprite_walk_alloc() {
    SpriteWalk* sprite_walk = malloc(sizeof(SpriteWalk));
    sprite_walk->direction = SpriteWalkDown;
    sprite_walk->anim_up = icon_animation_alloc(&A_PokemonPlayerUp_16);
    sprite_walk->anim_down = icon_animation_alloc(&A_PokemonPlayerDown_16);
    sprite_walk->anim_left = icon_animation_alloc(&A_PokemonPlayerLeft_16);
    sprite_walk->anim_right = icon_animation_alloc(&A_PokemonPlayerRight_16);
    sprite_walk->anim_active = sprite_walk->anim_down;
    return sprite_walk;
}

void sprite_walk_free(SpriteWalk* sprite_walk) {
    icon_animation_free(sprite_walk->anim_up);
    icon_animation_free(sprite_walk->anim_down);
    icon_animation_free(sprite_walk->anim_left);
    icon_animation_free(sprite_walk->anim_right);
    free(sprite_walk);
}

void sprite_walk_set_direction(SpriteWalk* sprite_walk, SpriteWalkDirection direction) {
    if(direction != sprite_walk->direction) {
        icon_animation_stop(sprite_walk->anim_active);
    }
    switch(direction) {
    case SpriteWalkUp:
        sprite_walk->anim_active = sprite_walk->anim_up;
        break;
    case SpriteWalkDown:
        sprite_walk->anim_active = sprite_walk->anim_down;
        break;
    case SpriteWalkLeft:
        sprite_walk->anim_active = sprite_walk->anim_left;
        break;
    case SpriteWalkRight:
        sprite_walk->anim_active = sprite_walk->anim_right;
        break;
    }
    sprite_walk->direction = direction;
}

void sprite_walk_animation_start(SpriteWalk* sprite_walk) {
    icon_animation_start(sprite_walk->anim_active);
}

void sprite_walk_animation_stop(SpriteWalk* sprite_walk) {
    icon_animation_stop(sprite_walk->anim_active);
}

void sprite_walk_draw(Canvas* const canvas, uint8_t x, uint8_t y, SpriteWalk* sprite_walk) {
    canvas_draw_icon_animation(canvas, x, y, sprite_walk->anim_active);
}
