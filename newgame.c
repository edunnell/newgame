#include "newgame.h"

void convert_graph_to_sdl(Vertex * vertex, Vertex * vertex2) {
  vertex->x = vertex->x + X_MID;
  vertex->y = Y_MID - vertex->y;
  vertex2->x = vertex2->x + X_MID;
  vertex2->y = Y_MID - vertex2->y;
}

void scale(const Player * player, Vertex * vertex, Vertex * vertex2) {
  vertex->x = vertex->x / vertex->z;
  vertex->y = vertex->y / vertex->z;
  vertex2->x = vertex2->x / vertex2->z;
  vertex2->y = vertex2->y / vertex2->z;
}

void draw_line(FILE * logfile, SDL_Renderer * renderer, const Player * player, const Vertex * vertex, const Vertex * vertex2, const Color * color) {
  if(!(vertex->z <= 0 && vertex2->z <= 0)) {
    Vertex svertex = *vertex, svertex2 = *vertex2;
    if(svertex.z <= 0)
      svertex.z = 0.5;
    if(svertex2.z <= 0)
      svertex2.z = 0.5;
    scale(player, &svertex, &svertex2);
    convert_graph_to_sdl(&svertex, &svertex2);
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, SDL_ALPHA_OPAQUE);
    fprintf(logfile, "x: %g  y: %g x2: %g  y2: %g\n", svertex.x, svertex.y, svertex2.x, svertex2.y);
    SDL_RenderDrawLine(renderer, svertex.x, svertex.y, svertex2.x, svertex2.y);
  }
}

void draw_polygon(FILE * logfile, SDL_Renderer * renderer, const Player * player, const Polygon * polygon) {
  int i;
  for(i = 0; i < polygon->num_vertices - 1; ++i) {
    if(i == 2)
      fprintf(logfile, "zzzzzzzzzzzzzzzzzzzzzz: %g", polygon->vertices[i].z);
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

void draw(Game * g) {
  SDL_SetRenderDrawColor(g->renderer, g->bg_color.r, g->bg_color.g, g->bg_color.b, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(g->renderer);
  draw_objects(g->logfile, g->renderer, &g->player, g->objects, g->num_objects);
  SDL_RenderPresent(g->renderer);
}

void move_left(Game * g) {
  int o, p, v;
  for(o = 0; o < g->num_objects; ++o) {
    Object * ob = &g->objects[o];
    for(p = 0; p < ob->num_polygons; ++p) {
      Polygon * po = &ob->polygons[p];
      for(v = 0; v < po->num_vertices; ++v)
        po->vertices[v].x += 20;
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
        po->vertices[v].x -= 20;
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
        po->vertices[v].z -= 0.5;
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
        po->vertices[v].z += 0.5;
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
        break;
      case SDLK_e:
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
    SDL_Delay(25);
  }
}

int main() {

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("New Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, X_RESOLUTION, Y_RESOLUTION, SDL_WINDOW_OPENGL);
  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  Polygon wall = {
    {
      {1200, -900, 100},
      {1200, 450, 100},
      {1200, 450, 1},
      {1200, -900, 1},
      {1200, -900, 100}
    },
    5,
    {250, 5, 200}
  };

  Polygon wall2 = {
    {
      {-1200, -900, 100},
      {-1200, 450, 100},
      {-1200, 450, 1},
      {-1200, -900, 1},
      {-1200, -900, 100}
    },
    5,
    {225, 125, 50}
  };

  Polygon wall3 = {
    {
      {-1200, -900, 100},
      {-1200, 450, 100},
      {1200, 450, 100},
      {1200, -900, 100},
      {-1200, -900, 100}
    },
    5,
    {100, 125, 250}
  };

  Object walls = {
    {wall, wall2, wall3},
    3
  };

  Game g = {
    {
      {0, 0, 0},
      180
    },
    {walls},
    1,
    renderer,
    {110, 220, 20},
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
