#include "newgame.h"

void convert_graph_to_sdl(Vertex * vertex, Vertex * vertex2) {
  vertex->x = vertex->x + X_MID;
  vertex->y = Y_MID - vertex->y;
  vertex2->x = vertex2->x + X_MID;
  vertex2->y = Y_MID - vertex2->y;
}

void scale(const Player * player, Vertex * vertex, Vertex * vertex2) {
  float z_distance, z_distance2;
  if(vertex->z > player->position.z) {
    z_distance = vertex->z - player->position.z;
  } else {
    z_distance = player->position.z - vertex->z;
  }
  if(vertex2->z > player->position.z) {
    z_distance2 = vertex2->z - player->position.z;
  } else {
    z_distance2 = player->position.z - vertex2->z;
  }

  vertex->x = vertex->x / (X_MID / z_distance);
  vertex->y = vertex->y / (Y_MID / z_distance);
  
  vertex2->x = vertex2->x / (X_MID / z_distance2);
  vertex2->y = vertex2->y / (Y_MID / z_distance2);
}

void draw_line(SDL_Renderer * renderer, const Player * player, const Vertex * vertex, const Vertex * vertex2, const Color * color) {
  Vertex svertex = *vertex, svertex2 = *vertex2;
  scale(player, &svertex, &svertex2);
  convert_graph_to_sdl(&svertex, &svertex2);
  SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer, svertex.x, svertex.y, svertex2.x, svertex2.y);
}

void draw_polygon(SDL_Renderer * renderer, const Player * player, const Polygon * polygon) {
  int i;
  for(i = 0; i < polygon->num_vertices - 1; ++i) {
    draw_line(renderer, player, &polygon->vertices[i], &polygon->vertices[i+1], &polygon->color);
  }
}

void draw_object(SDL_Renderer * renderer, const Player * player, const Object * object) {
  int i;
  for(i = 0; i < object->num_polygons; ++i) {
    draw_polygon(renderer, player, &object->polygons[i]);
  }
}

 void draw_objects(SDL_Renderer * renderer, const Player * player, Object objects[50], int num_objects) {
  int i;
  for(i = 0; i < num_objects; ++i) {
    draw_object(renderer, player, &objects[i]);
  }
}

void draw(Game * g) {
  SDL_SetRenderDrawColor(g->renderer, g->bg_color.r, g->bg_color.g, g->bg_color.b, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(g->renderer);
  draw_objects(g->renderer, &g->player, g->objects, g->num_objects);
  SDL_RenderPresent(g->renderer);
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
        break;
      case SDLK_d:
      case SDLK_RIGHT:
        break;
      case SDLK_w:
      case SDLK_UP:
        break;
      case SDLK_s:
      case SDLK_DOWN:
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

  Polygon ground_p = {
    {
      {-200, 0, -200},
      {200, 0, -200},
      {200, 0, -100},
      {-200, 0, -100},
      {-200, 0, -200}
    },
    5,
    {225, 125, 50}
  };

  Polygon ground_p2 = {
    {
      {-800, -500, -900},
      {800, -500, -900},
      {800, -500, -100},
      {-800, -500, -100},
      {-800, -500, -900}
    },
    5,
    {100, 125, 250}
  };

  Polygon ground_p3 = {
    {
      {-100, -100, -1500},
      {-100, 100, -1500},
      {100, 100, -1500},
      {100, -100, -1500},
      {-100, -100, -1500}
    },
    5,
    {250, 5, 200}
  };


  Object ground = {
    {ground_p, ground_p2, ground_p3},
    3
  };

  Game g = {
    {
      {0, 0, 0},
      180
    },
    {ground},
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

  SDL_Quit();
  return 0;

}
