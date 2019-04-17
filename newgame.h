#include <SDL2/SDL.h>
#include <math.h>
#define X_RESOLUTION 1600
#define Y_RESOLUTION 900
#define X_MID X_RESOLUTION/2
#define Y_MID Y_RESOLUTION/2

typedef struct Vertex {
  float x;
  float y;
  float z;
} Vertex;

typedef struct Color {
  int r;
  int g;
  int b;
} Color;

typedef struct Polygon {
  Vertex vertices[50];
  int num_vertices;
  Color color;
} Polygon;

typedef struct Object {
  Polygon polygons[50];
  int num_polygons;
} Object;

typedef struct Player {
  Vertex position;
  int xz_angle;
} Player;

typedef struct Game {
  Player player;
  Object walls;
  Object objects[50];
  int num_objects;
  SDL_Renderer * renderer;
  Color sky_color;
  Color ground_color;
  SDL_bool playing;
  FILE * logfile;
} Game;
