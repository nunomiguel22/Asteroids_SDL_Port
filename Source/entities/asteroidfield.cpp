#include <stdlib.h>

#include "asteroidfield.h"

void ast_spawn(asteroid asteroid_field[], player *player1) {
  /* Resets all asteroid death timers */
  for (unsigned int i = 0; i < MAX_ASTEROIDS; i++) {
    asteroid_field[i].death_timer = 0;
    asteroid_field[i].death_frame = 0;
  }

  /* Initiates a number of asteroids based on round with random movement*/
  for (int i = 0; i < player1->round; i++) {
    /* Randomizes size based on round */
    int random_size = rand() % (100 - 1) + 1;
    if (random_size < (100 - ((player1->round - STARTING_ASTEROIDS) *
                              SMALL_ASTEROID_CHANGE_INCREASE_RATE)))
      asteroid_field[i].size = LARGE;
    else
      asteroid_field[i].size = MEDIUM;

    asteroid_field[i].position.x = (float)(rand() % math_h_positive_bound);
    asteroid_field[i].position.y = (float)(rand() % math_v_positive_bound);

    /* Randomizes velocity/direction, smaller asteroids are always faster */
    if (asteroid_field[i].size == MEDIUM) {
      asteroid_field[i].velocity.setX(
          (float)(rand() % (MEDIUM_ASTEROID_MAX_VELOCITY -
                            MEDIUM_ASTEROID_MIN_VELOCITY) +
                  MEDIUM_ASTEROID_MIN_VELOCITY));
      asteroid_field[i].velocity.setY(
          (float)(rand() % (MEDIUM_ASTEROID_MAX_VELOCITY -
                            MEDIUM_ASTEROID_MIN_VELOCITY) +
                  MEDIUM_ASTEROID_MIN_VELOCITY));
    } else {
      asteroid_field[i].velocity.setX(
          (float)(rand() % (LARGE_ASTEROID_MAX_VELOCITY -
                            LARGE_ASTEROID_MIN_VELOCITY) +
                  LARGE_ASTEROID_MIN_VELOCITY));
      asteroid_field[i].velocity.setY(
          (float)(rand() % (LARGE_ASTEROID_MAX_VELOCITY -
                            LARGE_ASTEROID_MIN_VELOCITY) +
                  LARGE_ASTEROID_MIN_VELOCITY));
    }

    mvector2d random((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
    asteroid_field[i].velocity += random;

    /* Randomizes position x/y sign*/
    int random_xsign = rand() % 10;
    int random_ysign = rand() % 10;
    if (random_xsign >= 5) asteroid_field[i].position.x *= -1;
    if (random_ysign >= 5) asteroid_field[i].position.y *= -1;

    /* Randomizes velocity x/y sign*/
    random_xsign = rand() % 10;
    random_ysign = rand() % 10;
    if (random_xsign >= 5)
      asteroid_field[i].velocity.setX(asteroid_field[i].velocity.getX() * -1);
    if (random_ysign >= 5)
      asteroid_field[i].velocity.setY(asteroid_field[i].velocity.getY() * -1);

    /* Asteroid hit radius based on size */
    if (asteroid_field[i].size == MEDIUM)
      asteroid_field[i].hit_radius = MEDIUM_ASTEROID_HITRADIUS;
    else
      asteroid_field[i].hit_radius = LARGE_ASTEROID_HITRADIUS;

    /* Activates asteroid and initiates graphical orientation*/
    asteroid_field[i].active = true;
    asteroid_field[i].degrees = 0;
  }

  /* Makes all not needed asteroids inactive */
  for (int i = player1->round; i < MAX_ASTEROIDS; i++) {
    asteroid_field[i].active = false;
  }
}

int ast_collision(asteroid asteroid_field[], player *player1) {
  bool collision = false;

  for (unsigned int i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroid_field[i].active) {
      /* Player laser to asteroid Collision */
      for (unsigned int j = 0; j < AMMO; j++) {
        if (player1->lasers[j].is_active()) {
          mpoint2d laserpos = player1->lasers[j].get_position();
          mvector2d v_ast_laser(laserpos, asteroid_field[i].position);
          if (v_ast_laser.magnitude() <= asteroid_field[i].hit_radius) {
            player1->status |= BIT(2);
            collision = true;
            asteroid_field[i].active = false;
            player1->lasers[j].deactivate();
            asteroid_field[i].death_timer =
                (unsigned int)(ASTEROID_DEATH_DURATION);

            if (asteroid_field[i].size == LARGE) {
              player1->score += 50;
              ast_fragment(asteroid_field, i);
            } else {
              player1->score += 100;
            }
          }
        }
      }
      /* Player ship to asteroid Collision */
      mvector2d v_ast_ship(player1->pivot, asteroid_field[i].position);
      float total_radius = player1->hit_radius + asteroid_field[i].hit_radius;
      if (total_radius > v_ast_ship.magnitude()) {
        collision = true;
        asteroid_field[i].active = false;
        if (asteroid_field[i].size == LARGE) {
          if (!(player1->status & BIT(3))) {
            player1->hp -= 30;
            player1->score += 50;
          }
        } else {
          if (!(player1->status & BIT(3))) {
            player1->hp -= 15;
            player1->score += 100;
          }
        }
        asteroid_field[i].death_timer = (unsigned int)(ASTEROID_DEATH_DURATION);
      }
    }
  }
  return collision;
}

bool ast_update(asteroid asteroid_field[]) {
  bool liveasteroids = false;

  /* Updates asteroid position and warps asteroids to the other edge of the
   * screen */
  for (unsigned int i = 0; i < MAX_ASTEROIDS; i++) {
    if (asteroid_field[i].active) {
      liveasteroids = true;
      asteroid_field[i].position.x += asteroid_field[i].velocity.getX();
      asteroid_field[i].position.y += asteroid_field[i].velocity.getY();

      if (asteroid_field[i].position.x > math_h_positive_bound)
        asteroid_field[i].position.x -= hres;
      else if (asteroid_field[i].position.x < math_h_negative_bound)
        asteroid_field[i].position.x += hres;

      if (asteroid_field[i].position.y > math_v_positive_bound)
        asteroid_field[i].position.y -= vres;
      else if (asteroid_field[i].position.y < math_v_negative_bound)
        asteroid_field[i].position.y += vres;

      if (asteroid_field[i].degrees >= 360)
        asteroid_field[i].degrees -= 360;
      else
        asteroid_field[i].degrees++;
    }
    if (asteroid_field[i].death_timer > 0) liveasteroids = true;
  }
  return liveasteroids;
}

void ast_fragment(asteroid asteroid_field[], int ast_index) {
  /* Fragments large asteroids into two smaller asteroids with random movement
   */
  int frag_counter = 0;
  float x = asteroid_field[ast_index].position.x;
  float y = asteroid_field[ast_index].position.y;

  for (int i = 0; i < MAX_ASTEROIDS; i++) {
    if (!asteroid_field[i].active && asteroid_field[i].death_timer == 0) {
      ++frag_counter;
      asteroid_field[i].size = MEDIUM;

      asteroid_field[i].position.x = x;
      asteroid_field[i].position.y = y;

      asteroid_field[i].velocity.setX(
          (float)(rand() % (MEDIUM_ASTEROID_MAX_VELOCITY -
                            MEDIUM_ASTEROID_MIN_VELOCITY) +
                  MEDIUM_ASTEROID_MIN_VELOCITY));
      asteroid_field[i].velocity.setY(
          (float)(rand() % (MEDIUM_ASTEROID_MAX_VELOCITY -
                            MEDIUM_ASTEROID_MIN_VELOCITY) +
                  MEDIUM_ASTEROID_MIN_VELOCITY));

      mvector2d random((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
      asteroid_field[i].velocity += random;

      int random_xsign = rand() % 10;
      int random_ysign = rand() % 10;
      if (random_xsign >= 5)
        asteroid_field[i].velocity.setX(asteroid_field[i].velocity.getX() * -1);
      if (random_ysign >= 5)
        asteroid_field[i].velocity.setY(asteroid_field[i].velocity.getY() * -1);

      asteroid_field[i].hit_radius = MEDIUM_ASTEROID_HITRADIUS;
      asteroid_field[i].active = true;
      asteroid_field[i].degrees = 0;
    }

    if (frag_counter == 2) break;
  }
}
