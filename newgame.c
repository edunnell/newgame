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
  if(!(vertex->z <= 0 && vertex2->z <= 0)) {
    Vertex svertex = *vertex, svertex2 = *vertex2;
    svertex.z = svertex.z < 0.1 ? 0.1 : svertex.z;
    svertex2.z = svertex2.z < 0.1 ? 0.1 : svertex2.z;
    svertex.y -= 750;
    svertex2.y -= 750;
    scale(player, &svertex, &svertex2);
    convert_graph_to_sdl(&svertex, &svertex2);
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, SDL_ALPHA_OPAQUE);
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

void draw(Game * g) {
  SDL_RenderClear(g->renderer);
  draw_background(g->renderer, &g->sky_color, &g->ground_color);
  draw_objects(g->logfile, g->renderer, &g->player, g->objects, g->num_objects);
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

  Object walls = {
    {wall, wall2, wall3, wall4, wall5, wall6, wall7, wall8, wall9, wall10, wall11, wall12, wall13, wall14, wall15, wall16, wall17, wall18, wall19, wall20},
    20
  };

  Game g = {
    {
      {0, 0, 0},
      180
    },
    {walls},
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
