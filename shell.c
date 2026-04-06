#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "utils.h"

#define MAX_HISTORY 100

ssize_t prompt_and_get_input(const char* prompt,
                            char **line,
                            size_t *len) {
  fputs(prompt, stderr);
  return getline(line, len, stdin);
}


void close_ALL_the_pipes(int n_pipes, int (*pipes)[2]) {
  for (int i = 0; i < n_pipes; ++i) {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }
}


int exec_with_redir(cmd_struct* command, int n_pipes, int (*pipes)[2]) {
  int fd = -1;
  if ((fd = command->redirect[0]) != -1) {
    dup2(fd, STDIN_FILENO);
  }
  if ((fd = command->redirect[1]) != -1) {
    dup2(fd, STDOUT_FILENO);
  }
  close_ALL_the_pipes(n_pipes, pipes);
  return execvp(command->progname, command->args);
}


pid_t run_with_redir(cmd_struct* command, int n_pipes, int (*pipes)[2]) {
  pid_t child_pid = fork();

  if (child_pid) {
    switch(child_pid) {
      case -1:
        fprintf(stderr, "Oh dear.\n");
        return -1;
      default:
        return child_pid;
    }
  } else {
    exec_with_redir(command, n_pipes, pipes);
    perror("GRESIT!");
    return 0;
  }
}

char* expand_home_path(char* path){
  if(path == NULL){
    return NULL;
  }

  if(strcmp(path, "~") == 0){
    char* home = getenv("HOME");
    return home != NULL ? home : path;
  }

  if(strncmp(path, "~/", 2) == 0){
    char* home = getenv("HOME");
    if(home == NULL){
      return path;
    }

    size_t len = strlen(home) + strlen(path) - 1 + 1;
    char* expanded = malloc(len);
    if(expanded == NULL){
      return path;
    }

    snprintf(expanded, len, "%s/%s", home, path+2);
    return expanded; 
  }
  return path;
}

char* expand_env_arg(char* arg){
  if(arg == NULL){
    return NULL;
  }
  if(arg[0] == '$' && strlen(arg)>1){
    char* value = getenv(arg+1);
    if(value != NULL){
      return value;
    }
    return "";
  }

  return arg;
}

void expand_env_vars_in_command(cmd_struct* command){
  for(int i=0; command->args[i] != NULL; i++){
    command->args[i] = expand_env_arg(command->args[i]);
  }
}


int main() {
  char *line = NULL;
  size_t len = 0;
  char cwd[1024];
  char prompt[1200];
  char *history[MAX_HISTORY] = {0};
  int history_count = 0;

  while(1) {
    if(getcwd(cwd, sizeof(cwd)) != NULL){
      snprintf(prompt, sizeof(prompt), "mini-shell:%s$ ", cwd);
    }else{
      snprintf(prompt, sizeof(prompt), "mini-shell$ ");
    }
    if(prompt_and_get_input(prompt, &line, &len) <= 0){
      break;
    }

    int background = 0;

    line[strcspn(line, "\n")] = '\0';

    size_t line_len = strlen(line);
    if(line_len > 0 && line[line_len - 1] == '&'){
      background = 1;
      line[line_len - 1] = '\0';

      while(line_len > 1 && line[line_len - 2] == ' '){
        line[line_len - 2] = '\0';
        line_len --;
      }
    }

    if(strcmp(line, "") != 0){
      if(history_count < MAX_HISTORY){
        history[history_count] = strdup(line);
        history_count++;
      }
    }

    if(strcmp(line, "") == 0){
      continue;
    }

    if(strcmp(line, "help") == 0){
      printf("Built-in commands:\n");
      printf(" exit - exit shell\n");
      printf(" cd [dir] - change directory\n");
      printf(" help - help message\n");
      printf(" history - command history\n");
      continue;
    }

    if(strcmp(line, "exit")==0){
    break;
    }

    if(strcmp(line, "pwd") == 0){
      if(getcwd(cwd, sizeof(cwd)) != NULL){
        printf("%s\n", cwd);
      }else{
        perror("pwd");
      }
      continue;
    }


    if(strcmp(line, "cd") == 0){
      char *home = getenv("HOME");
      if(home == NULL){
      fprintf(stderr, "cd : HOME not set\n");
      }else if(chdir(home)!=0){
       perror("cd");
      }
      continue;
    }

    if(strcmp(line, "cd ~") == 0){
      char *home = getenv("HOME");
      if(home == NULL){
        fprintf(stderr, "cd: HOME not set\n");
      }else if(chdir(home) != 0){
        perror("cd");
      }
      continue;
    }

    if(strncmp(line, "cd ", 3) == 0){
      char *path = line+3;
      path = expand_env_arg(path);

      char* expanded_path = expand_home_path(path);
      if(chdir(expanded_path) != 0){
       perror("cd");
      }

      if(expanded_path != path){
        free(expanded_path);
      }
      continue;
    }

    if(strcmp(line, "history\n") == 0 || strcmp(line, "history") == 0){
      for(int i=0;i<history_count;i++){
        printf("%d %s", i+1, history[i]);
	if(history[i][strlen(history[i]) - 1] != '\n'){
          printf("\n");
        }
      }
      continue;
    }

    pipeline_struct* pipeline = parse_pipeline(line);

    for(int i=0;i<pipeline->n_cmds;++i){
      expand_env_vars_in_command(pipeline->cmds[i]);
    }

    int n_pipes = pipeline->n_cmds - 1;



    int (*pipes)[2] = calloc(sizeof(int[2]), n_pipes);

    for (int i = 1; i < pipeline->n_cmds; ++i) {
      pipe(pipes[i-1]);
      pipeline->cmds[i]->redirect[STDIN_FILENO] = pipes[i-1][0];
      pipeline->cmds[i-1]->redirect[STDOUT_FILENO] = pipes[i-1][1];
    }

    for (int i = 0; i < pipeline->n_cmds; ++i) {
      run_with_redir(pipeline->cmds[i], n_pipes, pipes);
    }

    close_ALL_the_pipes(n_pipes, pipes);

    if(!background){
        for (int i = 0; i < pipeline->n_cmds; ++i) {
          wait(NULL);
        }
      }
    }
  fputs("\n", stderr);
  return 0;
}
