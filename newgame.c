#include "newgame.h"

void convert_graph_to_sdl(Vertex * vertex, Vertex * vertex2) {
  vertex->x = vertex->x + X_MID;
  vertex->y = Y_MID - vertex->y;
  vertex2->x = vertex2->x + X_MID;
  vertex2->y = Y_MID - vertex2->y;
}

void scale(const Player * player, Vertex * vertex, Vertex * vertex2) {
  vertex->x = vertex->x / (vertex->z / X_MID);
  vertex->y = vertex->y / (vertex->z / Y_MID);
  vertex2->x = vertex2->x / (vertex2->z / X_MID);
  vertex2->y = vertex2->y / (vertex2->z / Y_MID);
}

void draw_line(FILE * logfile, SDL_Renderer * renderer, const Player * player, const Vertex * vertex, const Vertex * vertex2, const Color * color) {
  fprintf(logfile, "x %g y %g z %g x2 %g y2 %g z2 %g\n", vertex->x, vertex->y, vertex->z, vertex2->x, vertex2->y, vertex2->z);
  if(!(vertex->z <= 0 && vertex2->z <= 0)) {
    Vertex svertex = *vertex, svertex2 = *vertex2;
    if(svertex.z < 1) {
      if(svertex.x == svertex2.x) {
        svertex.z = 1;
      } else if(svertex.x > svertex2.x) {
        float slope = (svertex.z - svertex2.z) / (svertex.x - svertex2.x);
        float b = svertex.z - (slope * svertex.x);
        float x = -1 * (b / slope) + (1 / slope);
        svertex.x = x;
        svertex.z = 1;
      } else {
        float slope = (svertex2.z - svertex.z) / (svertex2.x - svertex.x);
        float b = svertex2.z - (slope * svertex2.x);
        float x = -1 * (b / slope) + (1 / slope);
        svertex.x = x;
        svertex.z = 1;
      }
    }
    if(svertex2.z < 1) {
      if(svertex.x == svertex2.x) {
        svertex2.z = 1;
      } else if(svertex.x > svertex2.x) {
        float slope = (svertex.z - svertex2.z) / (svertex.x - svertex2.x);
        float b = svertex.z - (slope * svertex.x);
        float x = -1 * (b / slope) + (1 / slope);
        svertex2.x = x;
        svertex2.z = 1;
      } else {
        float slope = (svertex2.z - svertex.z) / (svertex2.x - svertex.x);
        float b = svertex2.z - (slope * svertex2.x);
        float x = -1 * (b / slope) + (1 / slope);
        svertex2.x = x;
        svertex2.z = 1;
      }
    }

    svertex.y -= 750;
    svertex2.y -= 750;
    scale(player, &svertex, &svertex2);
    convert_graph_to_sdl(&svertex, &svertex2);
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, SDL_ALPHA_OPAQUE);
    fprintf(logfile, "x %g y %g x2 %g y2 %g\n", svertex.x, svertex.y, svertex2.x, svertex2.y);
    SDL_RenderDrawLine(renderer, svertex.x, svertex.y, svertex2.x, svertex2.y);
  }
}

void draw_polygon(FILE * logfile, SDL_Renderer * renderer, const Player * player, const Polygon * polygon) {
  int i;
  for(i = 0; i < polygon->num_vertices - 1; ++i) {
    draw_line(logfile, renderer, player, &polygon->vertices[i], &polygon->vertices[i+1], &polygon->color);
  }
}

void draw_object(FILE * logfile, SDL_Renderer * renderer, const Player * player, const Object * object) {
  int i;
  for(i = 0; i < object->num_polygons; ++i) {
    draw_polygon(logfile, renderer, player, &object->polygons[i]);
  }
}

void draw_objects(FILE * logfile, SDL_Renderer * renderer, const Player * player, Object objects[50], int num_objects) {
  int i;
  for(i = 0; i < num_objects; ++i) {
    draw_object(logfile, renderer, player, &objects[i]);
  }
}

void draw_background(SDL_Renderer * renderer, const Color * sky_color, const Color * ground_color) {
  SDL_SetRenderDrawColor(renderer, sky_color->r, sky_color->g, sky_color->b, SDL_ALPHA_OPAQUE);
  SDL_Rect sky = {0, 0, X_RESOLUTION, Y_MID};
  SDL_RenderFillRect(renderer, &sky);
  SDL_Rect ground = {0, 450, X_RESOLUTION, Y_MID};
  SDL_SetRenderDrawColor(renderer, ground_color->r, ground_color->g, ground_color->b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &ground);
}

int scale_polygons(FILE * logfile, const Object * walls, float zs[X_RESOLUTION]) {
  int i;
  for(i = 0; i < walls->num_polygons; ++i) {
    float x, x2, z, z2, y, y2;
    int j;
    const Polygon * c = &walls->polygons[i];
    x = c->vertices[0].x;
    z = c->vertices[0].z;
    for(j = 0; j < c->num_vertices; ++j) {
      if(c->vertices[j].x != x)
        x2 = c->vertices[j].x;

      if(c->vertices[j].z != z)
        z2 = c->vertices[j].z;
    }

    float ox = x;
    float ox2 = x2;
    x = (x / (z / X_MID)) + X_MID;
    x2 = (x2 / (z2 / X_MID)) + X_MID;

    float slope = (z2 - z) / (ox2 - ox);
    float b = z / (slope * ox);

    if(x > x2) {
      int k;
      int bcount = x2;
      float nz;
      for(k = ox2; k < ox; ++k) {
        if(bcount >= 0 && bcount < 1600) {
          nz = slope * k + b;
          if(zs[bcount]) {
            if(zs[bcount] > nz) {
              zs[bcount] = nz;
            }
          } else {
            zs[bcount] = nz;
          }
        }
        ++bcount;
      }
    } else {
      int k;
      int bcount = x;
      float nz;
      for(k = ox; k < ox2; ++k) {
        if(bcount >= 0 && bcount < 1600) {
          nz = slope * k + b;
          fprintf(logfile, "bcount: %d slope: %g k: %d b: %g nz: %g\n", bcount, slope, k, b, nz);
          if(zs[bcount] && nz) {
            if(zs[bcount] > nz) {
              zs[bcount] = nz;
            }
          } else {
            zs[bcount] = nz;
          }
        }
        ++bcount;
      }
    }
  }
}

draw_walls(FILE * logfile, SDL_Renderer * renderer, const Object * walls) {
  fprintf(logfile, "asdf\n");
  float zs[X_RESOLUTION] = {0};
  scale_polygons(logfile, walls, zs);
  fprintf(logfile, "ffffffffffffffffffffffffffff\n");
  int i;
  for(i = 0; i < X_RESOLUTION; ++i) {
    float y = Y_MID - ((5000 - 750) / (zs[i] / Y_MID));
    float y2 = Y_MID - (-750 / (zs[i] / Y_MID));
    fprintf(logfile, "z: %g\n", zs[i]);
    SDL_SetRenderDrawColor(renderer, 250, 5, 200, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, i, y, i, y2);
  }
}

void draw(Game * g) {
  SDL_RenderClear(g->renderer);
  draw_background(g->renderer, &g->sky_color, &g->ground_color);
  draw_walls(g->logfile, g->renderer, &g->walls);
  /* draw_objects(g->logfile, g->renderer, &g->player, g->objects, g->num_objects); */
  SDL_RenderPresent(g->renderer);
}

void rotate_vertex_clockwise_y(Vertex * vertex, float degrees) {
  float angle_radians = degrees * (M_PI/180);
  float sin_angle = sin(angle_radians);
  float cos_angle = cos(angle_radians);
  float tx = vertex->x;
  float tz = vertex->z;

  float txc = tx * cos_angle;
  float tzs = tz * sin_angle;
  float txs = tx * sin_angle;
  float tzc = tz * cos_angle;

  tx = tzs + txc;
  tz = tzc - txs;

  vertex->x = tx;
  vertex->z = tz;
}

void turn(Game * game, float degrees) {
  int i, j, k;
  for(i = 0; i < game->num_objects; ++i) {
    Object * o = &game->objects[i];
    for(j = 0; j < o->num_polygons; ++j) {
      Polygon * p = &o->polygons[j];
      for(k = 0; k < p->num_vertices; ++k) {
        rotate_vertex_clockwise_y(&p->vertices[k], degrees);
      }
    }
  }
}

void move_left(Game * g) {
  int o, p, v;
  for(o = 0; o < g->num_objects; ++o) {
    Object * ob = &g->objects[o];
    for(p = 0; p < ob->num_polygons; ++p) {
      Polygon * po = &ob->polygons[p];
      for(v = 0; v < po->num_vertices; ++v)
        po->vertices[v].x += 50;
    }
  }
}

void move_right(Game * g) {
  int o, p, v;
  for(o = 0; o < g->num_objects; ++o) {
    Object * ob = &g->objects[o];
    for(p = 0; p < ob->num_polygons; ++p) {
      Polygon * po = &ob->polygons[p];
      for(v = 0; v < po->num_vertices; ++v)
        po->vertices[v].x -= 50;
    }
  }
}

void move_forward(Game * g) {
  int o, p, v;
  for(o = 0; o < g->num_objects; ++o) {
    Object * ob = &g->objects[o];
    for(p = 0; p < ob->num_polygons; ++p) {
      Polygon * po = &ob->polygons[p];
      for(v = 0; v < po->num_vertices; ++v)
        po->vertices[v].z -= 50;
    }
  }
}

void move_backward(Game * g) {
  int o, p, v;
  for(o = 0; o < g->num_objects; ++o) {
    Object * ob = &g->objects[o];
    for(p = 0; p < ob->num_polygons; ++p) {
      Polygon * po = &ob->polygons[p];
      for(v = 0; v < po->num_vertices; ++v)
        po->vertices[v].z += 50;
    }
  }
}

void handle_events(Game * g) {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_QUIT:
      g->playing = SDL_FALSE;
      break;
    case SDL_KEYDOWN:
      switch(event.key.keysym.sym) {
      case SDLK_ESCAPE:
        g->playing = SDL_FALSE;
        break;
      case SDLK_a:
      case SDLK_LEFT:
        move_left(g);
        break;
      case SDLK_d:
      case SDLK_RIGHT:
        move_right(g);
        break;
      case SDLK_w:
      case SDLK_UP:
        move_forward(g);
        break;
      case SDLK_s:
      case SDLK_DOWN:
        move_backward(g);
        break;
      case SDLK_q:
        turn(g, 1.5);
        break;
      case SDLK_e:
        turn(g, -1.5);
        break;
      }
      break;
    }
  }
}

void loop(Game * g) {
  while(g->playing) {
    draw(g);
    handle_events(g);
  }
}

Polygon make_wall(float x, float z, float x2, float z2) {
  Polygon wall = {
    {
      {x, 5000, z},
      {x2, 5000, z2},
      {x2, 0, z2},
      {x, 0, z},
      {x, 5000, z}
    },
    5,
    {250, 5, 200}
  };
  return wall;
}

int main() {

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("New Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, X_RESOLUTION, Y_RESOLUTION, SDL_WINDOW_OPENGL);
  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  Polygon wall = make_wall(-10000, 10000, 10000, 10000);
  Polygon wall2 = make_wall(-10000, 10000, -10000, -10000);
  Polygon wall3 = make_wall(-10000, -10000, 10000, -10000);
  Polygon wall4 = make_wall(10000, -10000, 10000, -5000);
  Polygon wall5 = make_wall(10000, 10000, 10000, 5000);
  Polygon wall6 = make_wall(10000, 5000, 20000, 5000);
  Polygon wall7 = make_wall(20000, 5000, 20000, 10000);
  Polygon wall8 = make_wall(20000, 10000, 40000, 10000);
  Polygon wall9 = make_wall(40000, 10000, 40000, -10000);
  Polygon wall10 = make_wall(40000, -10000, 35000, -10000);
  Polygon wall11 = make_wall(10000, -5000, 20000, -5000);
  Polygon wall12 = make_wall(20000, -5000, 20000, -10000);
  Polygon wall13 = make_wall(20000, -10000, 25000, -10000);
  Polygon wall14 = make_wall(25000, -10000, 25000, -20000);
  Polygon wall15 = make_wall(25000, -20000, 20000, -20000);
  Polygon wall16 = make_wall(20000, -20000, 20000, -40000);
  Polygon wall17 = make_wall(20000, -40000, 40000, -40000);
  Polygon wall18 = make_wall(40000, -40000, 40000, -20000);
  Polygon wall19 = make_wall(40000, -20000, 35000, -20000);
  Polygon wall20 = make_wall(35000, -20000, 35000, -10000);

  Game g = {
    {
      {0, 0, 0},
      180
    },
    {
      {wall, wall2, wall3, wall4, wall5, wall6, wall7, wall8, wall9, wall10, wall11, wall12, wall13, wall14, wall15, wall16, wall17, wall18, wall19, wall20},
      20
    },
    {},
    1,
    renderer,
    {135, 206, 250},
    {91, 84, 74},
    SDL_TRUE,
    fopen("./newgame.log", "w+")
  };

  loop(&g);

  if(renderer)
    SDL_DestroyRenderer(renderer);
  if(window)
    SDL_DestroyWindow(window);

  fclose(g.logfile);

  SDL_Quit();
  return 0;

}
