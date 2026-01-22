#define _POSIX_C_SOURCE  200809L
#include <stdlib.h>
#include <stdio.h>
#include "my_export.h"
#include "../../utils.h"

extern struct env *env;

int my_export(char **command){
  if(command[0]==NULL){
    fprintf(stderr, "Error: my_export: no args\n");
    return 2;
  }
  for(int j = 0; command[j] != NULL; j++){
    if(strchr(command[j],'=')){
      int i=0;
      while(command[j][i]!='='){
        i++;
      }
      command[j][i]=0;
      if(!is_valid_name(command[j])){
        fprintf(stderr, "Error: my_export: not valid Variable Name\n");
        return 2;
      }
      char* value=malloc(sizeof(char)* (strlen(&command[j][i+1])+1));
      int pos=i+1;
      int k=0;
      
      while(command[j][pos]){
        value[k++]=command[j][pos++];
      }
      value[k]=0;
      bool updated=0;
      hash_map_insert(env->variables,command[j], value,&updated);
      free(value);
    }
    export_add_variable(env->export_variables,strdup(command[j]));
  }
  return 0;
}
