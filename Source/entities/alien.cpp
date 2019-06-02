#include <stdlib.h>

#include "../general/mvector.h"
#include "alien.h"
#include "weapon.h"

/* ALIEN SHIP */

void alien_spawn(player *p) {
  /*Initiates alien ships values, gives it random movement */
  p->force.setX((float)rand() / RAND_MAX);
  p->force.setY((float)rand() / RAND_MAX);

  int random_xsign = rand() % 10;
  int random_ysign = rand() % 10;

  if (random_xsign >= 5) p->force.setX(p->force.getX() * -1);

  if (random_ysign >= 5) p->force.setY(p->force.getY() * -1);

  p->pivot.x = (float)(rand() % math_h_positive_bound);
  p->pivot.y = (float)(rand() % math_v_positive_bound);

  p->cannon.x = p->pivot.x;
  p->cannon.y = p->pivot.y + 40;

  p->port.setX(-1);
  p->port.setY(0);

  p->starboard.setX(1);
  p->starboard.setY(0);

  p->hp = ALIEN_MAX_HEALTH;
  p->hit_radius = ALIEN_HIT_RADIUS;
  p->status = 0;
  p->status |= BIT(0);

  for (int i = 0; i < AMMO; i++) {
    p->lasers[i].deactivate();
  }
}

bool alien_update(player *alien, player *player1, game_timers *timers) {
  bool alien_fired = false;

  /* Updates alien ships position */
  alien->cannon.x += alien->force.getX();
  alien->cannon.y += alien->force.getY();
  alien->pivot.x += alien->force.getX();
  alien->pivot.y += alien->force.getY();

  /* Warps alien ship to the other end when crossing bounds */
  ship_warp(alien);

  /* Rotates ship, so it tracks the player's ship */
  mvector2d vcannon(alien->pivot, alien->cannon);
  mvector2d vships(alien->pivot, player1->pivot);

  float ship_degrees = vships.angle() - vcannon.angle();
  if (ship_degrees < 0) vcannon.rotate(-ALIEN_ROTATION_SPEED);
  if (ship_degrees > 0) vcannon.rotate(+ALIEN_ROTATION_SPEED);

  vcannon.limit(40);
  alien->cannon.x = alien->pivot.x + vcannon.getX();
  alien->cannon.y = alien->pivot.y + vcannon.getY();

  /* Fires lasers when available */
  if (timers->alien_weapon_timer >= (60 / ALIEN_FIRE_RATE)) {
    ship_fire_laser(alien, &timers->alien_weapon_timer);
    alien_fired = true;
  }

  /* Destroys out of bounds lasers */
  for (unsigned int i = 0; i < AMMO; ++i) {
    if (alien->lasers[i].is_active()) alien->lasers[i].update();
  }
  return alien_fired;
}

void alien_collision(player *alien, player *player1, game_timers *timers) {
  /* Player laser to alien ship collision */
  for (unsigned int j = 0; j < AMMO; j++) {
    if (player1->lasers[j].is_active()) {
      mpoint2d laserpos = player1->lasers[j].get_position();
      mvector2d v_ast_laser(laserpos, alien->pivot);

      if (v_ast_laser.magnitude() <= alien->hit_radius) {
        player1->status |= BIT(2);
        alien->hp -= PLAYER_LASER_DAMAGE;
        player1->lasers[j].deactivate();
        if (alien->hp <= 0) {
          timers->alien_death_timer = (unsigned int)(ALIEN_DEATH_DURATION * 60);
          alien->status &= ~BIT(0);
          player1->score += 400;
        }
      }
    }
  }

  /* Alien laser to player ship collision */
  for (unsigned int j = 0; j < AMMO; j++) {
    if (alien->lasers[j].is_active()) {
      mpoint2d laserpos = alien->lasers[j].get_position();
      mvector2d v_ast_laser(laserpos, player1->pivot);

      if (v_ast_laser.magnitude() <= player1->hit_radius) {
        player1->hp -= 20;
        alien->lasers[j].deactivate();
      }
    }
  }

  /* Alien ship to player ship collision, this kills the player ship */
  mvector2d v_ast_ship(player1->pivot, alien->pivot);
  float total_radius = player1->hit_radius + alien->hit_radius;
  if (total_radius > v_ast_ship.magnitude() && !(player1->status & BIT(3)))
    player1->hp -= player1->hp;
}
