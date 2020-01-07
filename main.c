#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bomber.h" //This is like a link to a  library with functions defined in bomber.c

int main(int argc, char* argv[]) {
  //This section just checks if debug mode is on, you can get into it by typing ./game debug
  int DebugOn = 0;

  if(argv[1] == NULL){
    argv[1] = "none";
  }

  if((strcmp(argv[1],"debug")) == 0){
    DebugOn = 1;
  }

  mainMenu(DebugOn);
  return 0;
}