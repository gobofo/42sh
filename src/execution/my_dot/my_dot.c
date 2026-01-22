#include "my_dot.h"

#include <string.h>


int my_dot(char **command){
  if(command[0]==NULL || command[1]==NULL){
    fprintf(stderr, "Error: my_dot: no args\n");
    return 2;
  }
  return my_42sh(2,command);
}
