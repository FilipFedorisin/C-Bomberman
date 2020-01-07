struct progress {
  int glitch;
  int WonTheGame;
  int AtLevel;
  int AvailableLevels;
}; 
struct progress user = {0,0,1,0};

struct arguments { // argumenty prikazoveho riadku
  int DebugOn; // if(argv[1] == "debug") 
};
struct arguments arg = {0};

struct playerData {
  int dead;
  int kills;
  char input;
  int pos[2];
};
struct playerData player = {0,0,'o',{0,0}};

struct bombData {
  int pos[2];
};
struct bombData bomb = {{20,30}};

struct enemies {
  int count;
  int buffer;
  int pos[10][2]; // enemy.pos[*enemy_index*][*position_x**position_y*]
};
struct enemies enemy = {0,0,{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}};