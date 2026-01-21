#define _POSIX_C_SOURCE  200809L
#include <stdlib.h>
#include <stdio.h>
#include "my_export.h"
#include "../../utils.h"

extern struct env *env;

int my_export(char **command){
  if(command[0]==NULL){
    fprintf(stderr, "Error: my_export: no args ");
    return 2;
  }
  if(strchr(command[0],'=')){

    int i=0;
    while(command[0][i]!='='){
      i++;
    }
    command[0][i]=0;
    if(!is_valid_name(command[0])){
        fprintf(stderr, "Error: my_export: not valid Variable Name");
        return 2;
    }

    char* value=malloc(sizeof(char)* strlen(command[0]+i+1)-1);// pointeur apres le = dans strlen
    int j=i+1;
    int k=0;
    
    while(command[0][j]){
      value[k++]=command[0][j++];
    }
    value[k]=0;


    bool updated=0;
    hash_map_insert(env->variables,strdup(command[0]), value,&updated);

  }
  export_add_variable(env->export_variables,strdup(command[0]));

  return 0;

}
