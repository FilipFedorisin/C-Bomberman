#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <time.h>
#include <unistd.h>
#include "bomber.h"
#include "structs.h"

void mainMenu(const int DebugOn){ //mainMenu
  // rewriting argument struct from main
  if(DebugOn == 1) arg.DebugOn = 1;

  int buffer = 0; 
  int itemCount = 4;
  char selection = 'd';
  char mainMenu[4][30] = {"      NEW GAME      ","     ABOUT GAME     ","    SCREEN SAVER    ","        EXIT        "};
  //char mainItem[4];
  
  initGraphics();
  drawLogo();
  while( selection != 'f'){
    selection = getch();
    if( selection == 's' && buffer < 3){
      buffer++;
    } else if( selection == 'w' && buffer > 0){
      buffer--;
    }
    for(int i = 0; i < itemCount; i++){ //Initializes item menu list
      if( i == buffer){ //Higlight of the first item
        wattron(stdscr, A_STANDOUT);
      } else {
        wattroff(stdscr, A_STANDOUT);
      }
      //sprintf(mainItem, "%-7s", mainMenu[isOn]);
      mvprintw(LINES/2-4+i, COLS/2-10, "%s", mainMenu[i]);
    }
    refresh();
  }

  switch(buffer){
    case 0: initLevel(); break; // New GAME
    case 1: aboutSection(); break;  // About
    case 2: initMap();
            scanMap();
            while(player.input != 'q' && player.input != 'Q'){
              user.glitch = rand() % 10;
              initMap();
              usleep(200000);
              player.input = getch();
              initEnemy();
            }
    break;  // Scresaver
    case 3: break;  // exit
  }

  endwin();
}
void drawLogo(){
  //BOMBERMAN logo
  mvprintw(LINES/2-11, COLS/2-30, " ____   ____  __  __ ____  ______ _____  __  __          _   _ ");
  mvprintw(LINES/2-10, COLS/2-30, "|  _ \\ / __ \\|  \\/  |  _ \\|  ____|  __ \\|  \\/  |   /\\   | \\ | |");
  mvprintw(LINES/2-9, COLS/2-30, "| |_) | |  | | \\  / | |_) | |__  | |__) | \\  / |  /  \\  |  \\| |");
  mvprintw(LINES/2-8, COLS/2-30, "|  _ <| |  | | |\\/| |  _ <|  __| |  _  /| |\\/| | / /\\ \\ | . ` |");
  mvprintw(LINES/2-7, COLS/2-30, "| |_) | |__| | |  | | |_) | |____| | \\ \\| |  | |/ ____ \\| |\\  |");
  mvprintw(LINES/2-6, COLS/2-30, "|____/ \\____/|_|  |_|____/|______|_|  \\_\\_|  |_/_/    \\_\\_| \\_|");
  refresh();
}
void initGraphics(){
  srand(time(NULL));
  // initialize the library
  initscr();
  // set implicit modes
  cbreak();
  keypad(stdscr,TRUE);
  // invisible cursor, visibility of cursor (0,1,2)
  curs_set(FALSE);
  // getch() will be non-blocking
  nodelay(stdscr, TRUE);
  noecho();
  refresh();
}
void initLevel(){
  clear();
  
  int enemyTimer = 0;
  int bombAvailableTimer = 0;
  int bombAvailable = 1;
  int bombDetonatorTimer = 0;
  int bombDetonator = 0;
  int levelLoader = 1;

  while(player.input != 'q' && player.input != 'Q') { // MAIN Timer LOOP
    usleep(5); // Refresh rate
    DEBUG();

    player.input = getch();

    if(levelLoader == user.AtLevel){
      clear();
      refresh();
      initMap();
      scanMap();
      refresh();
      levelLoader++;
    }

    while(player.input == 'p' || player.input == 'P'){
      mvprintw(1, COLS/2 - 4, "PAUSE!");
      refresh();
      usleep(5);
      player.input = getch();
      if(player.input == 'p' || player.input == 'P'){
        mvprintw(1, COLS/2 - 4, "      ");
        break;
      } else {
        player.input = 'p';
      }
    }

    //Bomb!!! Cover 
    if((player.input == 'f' || player.input == 'F') && bombAvailable == 1) {
      bomb.pos[1] = player.pos[1];
      bomb.pos[0] = player.pos[0];
      bombON();
      mvprintw(bomb.pos[1],bomb.pos[0], "*");
      bombAvailable = 0;
      bombDetonatorTimer = 0;
      bombDetonator = 1;
    }

    //Enemy Handling Setup
    enemyTimer++;
    if(enemyTimer == 8000){
      initEnemy();
      enemyTimer = 0;
    }

    //Player Handling Setup
    playerCollisionMovement();

    //Bomb check went off
    if(bombDetonator == 1){
      mvprintw(bomb.pos[1],bomb.pos[0], "*");
      bombAvailable = 0;
      bombDetonatorTimer++;
      if(bombDetonatorTimer >= 20000){
        bombOFF();
        bombAvailableTimer++;
        if(bombAvailableTimer == 2000){
          bombAvailable = 1;
          mvprintw(bomb.pos[1],bomb.pos[0], " ");
          bombON();
          bombDetonator = 0;
          bombDetonatorTimer = 0;
          bombAvailableTimer = 0;
        }
      }
    }

    //Checking if player alive after DA BLAST!!!
    if(player.dead == 1){
      loseScreen();
      break;
    }

    //Checking if enemy alive after dem bom
    for(int i = 0; i < 10; i++){
      if(mvinch(enemy.pos[i][1], enemy.pos[i][0]) == 'X'){
        enemy.pos[i][1] = -5;
        enemy.pos[i][0] = -5;
        player.kills++;
      }
    }
    //Checking if player has won!!
    if(player.kills >= enemy.count){
      player.kills = 0;
      user.AtLevel++;
      bombAvailable = 1;
      mvprintw(bomb.pos[1],bomb.pos[0], " ");
      bombON();
      bombDetonator = 0;
      bombDetonatorTimer = 0;
      bombAvailableTimer = 0;
    }
    if(user.AtLevel == user.AvailableLevels + 1){
      winScreen();
      break;
    }
  }
  refresh();
}
void initMap(){
  char ch = 'i';
  char map[12] = {"level_X.txt\0"};
  int mapIndex = user.AtLevel + 48;
  for(int i = 0; i < 12; i++){
    if(map[i] == 'X') map[i] = mapIndex;
  }
  int row = 0;
  int col = 2;
  FILE *level;
  level = fopen(map, "r");

  bool endOfRow = false , endOfMap = false ;
  int rowBuffer = 0, endBuffer = 0;

  while(!endOfMap){
    ch = fgetc(level);
    
    if(endOfRow){
      endOfRow = false;
      row = row + 1 + user.glitch;
      col = 0 - user.glitch;
    }else if(!endOfMap){
      if(user.glitch == 0){
        mvprintw(LINES/2-7+row+user.glitch, COLS/2-16+col - 3-user.glitch, "%c", ch);
      } else {
        mvprintw(LINES/100-7+row+user.glitch, COLS/100-16+col - 3-user.glitch, "%c", ch);
      }
    }

    if(ch == '|' || ch == '+'){
      rowBuffer++;
      if(rowBuffer == 2){
        rowBuffer = 0;
        endOfRow = true;
      }
    }

    if(ch == '+'){
      endBuffer++;
      if(endBuffer == 4){
        endOfMap = true;
      }
    }

    col = col + 1 + user.glitch;
    refresh();
  }
  fclose(level);
  user.AvailableLevels++;
  refresh();
}
void scanMap(){
  //Scans for enemies and writes their location ... Maximum of 10 enemies !!!!!
  enemy.buffer = 0;
  enemy.count = 0;
  for(int x=0; x <= LINES; x++){
    for(int y=0; y <= COLS; y++){
      if(mvinch(x,y) == '@'){
        enemy.count++;
        enemy.pos[enemy.buffer][0] = y;
        enemy.pos[enemy.buffer][1] = x;
        enemy.buffer++;
      }
      if(enemy.buffer == 10){
        x = x + COLS;
        y = y + LINES;
      }
    }
  }
  for(int x=0; x <= LINES; x++){
    for(int y=0; y <= COLS; y++){
      if(mvinch(x,y) == 'O'){
        player.pos[0] = y;
        player.pos[1] = x;
        x = x + COLS;
        y = y + LINES;
      }
    }
  }
  
}
void playerCollisionMovement(){
  switch(player.input){
    case 'w':
    case 'W':
      if(mvinch(player.pos[1] - 1, player.pos[0]) == ' '){
        mvprintw(player.pos[1], player.pos[0], " ");
        player.pos[1] = player.pos[1] - 1;
        mvprintw(player.pos[1], player.pos[0], "O");
      }
    break;
    case 's':
    case 'S':
      if(mvinch(player.pos[1] + 1, player.pos[0]) == ' '){
        mvprintw(player.pos[1], player.pos[0], " ");
        player.pos[1] = player.pos[1] + 1;
        mvprintw(player.pos[1], player.pos[0], "O");
      }
    break;
    case 'a':
    case 'A':
      if(mvinch(player.pos[1], player.pos[0] - 1) == ' '){
        mvprintw(player.pos[1], player.pos[0], " ");
        player.pos[0] = player.pos[0] - 1;
        mvprintw(player.pos[1], player.pos[0], "O");
      }
    break;
    case 'd':
    case 'D':
      if(mvinch(player.pos[1], player.pos[0] + 1) == ' '){
        mvprintw(player.pos[1], player.pos[0], " ");
        player.pos[0] = player.pos[0] + 1;
        mvprintw(player.pos[1], player.pos[0], "O");
      }
    break;
  }
}
void bombON(){
  if(mvinch(bomb.pos[1], bomb.pos[0] - 1) == 'X') {
    mvprintw(bomb.pos[1], bomb.pos[0] - 1, " ");
    if(mvinch(bomb.pos[1], bomb.pos[0] - 2) == 'X') {
      mvprintw(bomb.pos[1], bomb.pos[0] - 2, " ");
      if(mvinch(bomb.pos[1], bomb.pos[0] - 3) == 'X') {
        mvprintw(bomb.pos[1], bomb.pos[0] - 3, " ");
      }
    }
  }
  if(mvinch(bomb.pos[1], bomb.pos[0] + 1) == 'X') {
    mvprintw(bomb.pos[1], bomb.pos[0] + 1, " ");
    if(mvinch(bomb.pos[1], bomb.pos[0] + 2) == 'X') {
      mvprintw(bomb.pos[1], bomb.pos[0] + 2, " ");
      if(mvinch(bomb.pos[1], bomb.pos[0] + 3) == 'X') {
        mvprintw(bomb.pos[1], bomb.pos[0] + 3, " ");
      }
    }
  }
  if(mvinch(bomb.pos[1] - 1, bomb.pos[0] ) == 'X') {
    mvprintw(bomb.pos[1] - 1, bomb.pos[0], " ");
    if(mvinch(bomb.pos[1] - 2, bomb.pos[0] ) == 'X') {
      mvprintw(bomb.pos[1] - 2, bomb.pos[0], " ");
      if(mvinch(bomb.pos[1] - 3, bomb.pos[0] ) == 'X') {
        mvprintw(bomb.pos[1] - 3, bomb.pos[0], " ");
      }
    }
  }
  if(mvinch(bomb.pos[1] + 1, bomb.pos[0]) == 'X') {
    mvprintw(bomb.pos[1] + 1, bomb.pos[0], " ");
    if(mvinch(bomb.pos[1] + 2, bomb.pos[0]) == 'X') {
      mvprintw(bomb.pos[1] + 2, bomb.pos[0], " ");
      if(mvinch(bomb.pos[1] + 3, bomb.pos[0]) == 'X') {
        mvprintw(bomb.pos[1] + 3, bomb.pos[0], " ");
      }
    }
  }
}
void bombOFF(){
  if(mvinch(bomb.pos[1], bomb.pos[0] - 1) == 'O') player.dead = 1;
  if(mvinch(bomb.pos[1], bomb.pos[0] - 1) == ' ' || mvinch(bomb.pos[1], bomb.pos[0] - 1) == 'c' || mvinch(bomb.pos[1], bomb.pos[0] - 1) == '@'){
    mvprintw(bomb.pos[1], bomb.pos[0] - 1, "X");
    if(mvinch(bomb.pos[1], bomb.pos[0] - 2) == 'O') player.dead = 1;
    if(mvinch(bomb.pos[1], bomb.pos[0] - 2) == ' ' || mvinch(bomb.pos[1], bomb.pos[0] - 2) == 'c' || mvinch(bomb.pos[1], bomb.pos[0] - 2) == '@'){
      mvprintw(bomb.pos[1], bomb.pos[0] - 2, "X");
      if(mvinch(bomb.pos[1], bomb.pos[0] - 3) == 'O') player.dead = 1;
      if(mvinch(bomb.pos[1], bomb.pos[0] - 3) == ' ' || mvinch(bomb.pos[1], bomb.pos[0] - 3) == 'c' || mvinch(bomb.pos[1], bomb.pos[0] - 3) == '@') {
        mvprintw(bomb.pos[1], bomb.pos[0] - 3, "X");
      }
    }
  }
  if(mvinch(bomb.pos[1], bomb.pos[0] + 1) == 'O') player.dead = 1;
  if(mvinch(bomb.pos[1], bomb.pos[0] + 1) == ' ' || mvinch(bomb.pos[1], bomb.pos[0] + 1) == 'c' || mvinch(bomb.pos[1], bomb.pos[0] + 1) == '@') {
    mvprintw(bomb.pos[1], bomb.pos[0] + 1, "X");
    if(mvinch(bomb.pos[1], bomb.pos[0] + 2) == 'O') player.dead = 1;
    if(mvinch(bomb.pos[1], bomb.pos[0] + 2) == ' ' || mvinch(bomb.pos[1], bomb.pos[0] + 2) == 'c' || mvinch(bomb.pos[1], bomb.pos[0] + 2) == '@'){
      mvprintw(bomb.pos[1], bomb.pos[0] + 2, "X");
      if(mvinch(bomb.pos[1], bomb.pos[0] + 3) == 'O') player.dead = 1;
      if(mvinch(bomb.pos[1], bomb.pos[0] + 3) == ' ' || mvinch(bomb.pos[1], bomb.pos[0] + 3) == 'c' || mvinch(bomb.pos[1], bomb.pos[0] + 3) == '@') {
        mvprintw(bomb.pos[1], bomb.pos[0] + 3, "X");
      }
    }
  }
  if(mvinch(bomb.pos[1] - 1, bomb.pos[0]) == 'O') player.dead = 1;
  if(mvinch(bomb.pos[1] - 1, bomb.pos[0] ) == ' ' || mvinch(bomb.pos[1] - 1, bomb.pos[0]) == 'c' || mvinch(bomb.pos[1] - 1, bomb.pos[0]) == '@' ) {
    mvprintw(bomb.pos[1] - 1, bomb.pos[0], "X");
    if(mvinch(bomb.pos[1] - 2, bomb.pos[0]) == 'O') player.dead = 1;
    if(mvinch(bomb.pos[1] - 2, bomb.pos[0] ) == ' ' || mvinch(bomb.pos[1] - 2, bomb.pos[0]) == 'c' || mvinch(bomb.pos[1] - 2, bomb.pos[0]) == '@') {
      mvprintw(bomb.pos[1] - 2, bomb.pos[0], "X");
      if(mvinch(bomb.pos[1] - 3, bomb.pos[0]) == 'O') player.dead = 1;
      if(mvinch(bomb.pos[1] - 3, bomb.pos[0] ) == ' ' || mvinch(bomb.pos[1] - 3, bomb.pos[0]) == 'c' || mvinch(bomb.pos[1] - 3, bomb.pos[0]) == '@' ) {
        mvprintw(bomb.pos[1] - 3, bomb.pos[0], "X");
      }
    }
  }
  if(mvinch(bomb.pos[1] + 1, bomb.pos[0]) == 'O') player.dead = 1;
  if(mvinch(bomb.pos[1] + 1, bomb.pos[0]) == ' ' || mvinch(bomb.pos[1] + 1, bomb.pos[0]) == 'c' || mvinch(bomb.pos[1] + 1, bomb.pos[0]) == '@') {
    mvprintw(bomb.pos[1] + 1, bomb.pos[0], "X");
    if(mvinch(bomb.pos[1] + 2, bomb.pos[0]) == 'O') player.dead = 1;
    if(mvinch(bomb.pos[1] + 2, bomb.pos[0]) == ' ' || mvinch(bomb.pos[1] + 2, bomb.pos[0]) == 'c' || mvinch(bomb.pos[1] + 2, bomb.pos[0]) == '@') {
      mvprintw(bomb.pos[1] + 2, bomb.pos[0], "X");
      if(mvinch(bomb.pos[1] + 3, bomb.pos[0]) == 'O') player.dead = 1;
      if(mvinch(bomb.pos[1] + 3, bomb.pos[0]) == ' ' || mvinch(bomb.pos[1] + 3, bomb.pos[0]) == 'c' || mvinch(bomb.pos[1] + 3, bomb.pos[0]) == '@' ) {
        mvprintw(bomb.pos[1] + 3, bomb.pos[0], "X");
      }
    }
  }
}
void initEnemy(){
  int random = 0;
  for(int i = 0; i < 10; i++){
    random = rand() % 4;
    if(random == 0){
      if(mvinch(enemy.pos[i][1] - 1, enemy.pos[i][0]) == 'O') player.dead = 1;
      if(mvinch(enemy.pos[i][1] - 1, enemy.pos[i][0]) == ' '){
        mvprintw(enemy.pos[i][1], enemy.pos[i][0], " ");
        enemy.pos[i][1] = enemy.pos[i][1] - 1;
        mvprintw(enemy.pos[i][1], enemy.pos[i][0], "@");
      }
    }
    if(random == 1){
      if(mvinch(enemy.pos[i][1] + 1, enemy.pos[i][0]) == 'O') player.dead = 1;
      if(mvinch(enemy.pos[i][1] + 1, enemy.pos[i][0]) == ' '){
        mvprintw(enemy.pos[i][1], enemy.pos[i][0], " ");
        enemy.pos[i][1] = enemy.pos[i][1] + 1;
        mvprintw(enemy.pos[i][1], enemy.pos[i][0], "@");
      }
    }
    if(random == 2){
      if(mvinch(enemy.pos[i][1], enemy.pos[i][0] - 1) == 'O') player.dead = 1;
      if(mvinch(enemy.pos[i][1], enemy.pos[i][0] - 1) == ' '){
        mvprintw(enemy.pos[i][1], enemy.pos[i][0], " ");
        enemy.pos[i][0] = enemy.pos[i][0] - 1;
        mvprintw(enemy.pos[i][1], enemy.pos[i][0], "@");
      }
    }
    if(random == 3){
      if(mvinch(enemy.pos[i][1], enemy.pos[i][0] + 1) == 'O') player.dead = 1;
      if(mvinch(enemy.pos[i][1], enemy.pos[i][0] + 1) == ' '){
        mvprintw(enemy.pos[i][1], enemy.pos[i][0], " ");
        enemy.pos[i][0] = enemy.pos[i][0] + 1;
        mvprintw(enemy.pos[i][1], enemy.pos[i][0], "@");
      }
    }
  }
}
void DEBUG(){
  if(arg.DebugOn == 1){
    mvprintw(0,0,"Enemy 0 posX:[%4d] posY:[%4d]" ,enemy.pos[0][0],enemy.pos[0][1]);
    mvprintw(1,0,"Enemy 1 posX:[%4d] posY:[%4d]" ,enemy.pos[1][0],enemy.pos[1][1]);
    mvprintw(2,0,"Enemy 2 posX:[%4d] posY:[%4d]" ,enemy.pos[2][0],enemy.pos[2][1]);
    mvprintw(3,0,"Enemy 3 posX:[%4d] posY:[%4d]" ,enemy.pos[3][0],enemy.pos[3][1]);
    mvprintw(4,0,"Enemy 4 posX:[%4d] posY:[%4d]" ,enemy.pos[4][0],enemy.pos[4][1]);
    mvprintw(5,0,"Enemy 5 posX:[%4d] posY:[%4d]" ,enemy.pos[5][0],enemy.pos[5][1]);
    mvprintw(6,0,"Enemy 6 posX:[%4d] posY:[%4d]" ,enemy.pos[6][0],enemy.pos[6][1]);
    mvprintw(7,0,"Enemy 7 posX:[%4d] posY:[%4d]" ,enemy.pos[7][0],enemy.pos[7][1]);
    mvprintw(8,0,"Enemy 8 posX:[%4d] posY:[%4d]" ,enemy.pos[8][0],enemy.pos[8][1]);
    mvprintw(9,0,"Enemy 9 posX:[%4d] posY:[%4d]" ,enemy.pos[9][0],enemy.pos[9][1]);

    mvprintw(9,0,"Player  posX:[%4d] posY:[%4d]" ,player.pos[0],player.pos[1]);
    mvprintw(10,0,"Kills :[%4d]" ,player.kills);
  }
}
void winScreen(){
  clear();
  refresh();
  nodelay(stdscr, FALSE);
  echo();
  mvprintw(LINES/2, COLS/2-4, "YOU WON!");
  getch();
}
void loseScreen(){
  clear();
  refresh();
  nodelay(stdscr, FALSE);
  echo();
  mvprintw(LINES/2, COLS/2-4, "YOU LOST");
  getch();
}
void aboutSection(){
  clear();
  refresh();
  mvprintw(LINES/2  , COLS/2-21, "Movement:WASD,F-Bomb/Enter,Q-Quit,P-Pause   ");
  mvprintw(LINES/2+1, COLS/2-21, "VERSION 1                                   ");
  mvprintw(LINES/2+2, COLS/2-21, "Author: Filip Fedorisin TUKE FEI AES KEM E3 ");
  mvprintw(LINES/2+4, COLS/2-21, "Game inspired by original Bomberman         ");
  mvprintw(LINES/2+5, COLS/2-21, "all rights belongs to original authors      ");
  refresh();
  echo();
  nodelay(stdscr, FALSE);
  getch();
}