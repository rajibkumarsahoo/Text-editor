#include<stdio.h>
#include<assert.h>
#include<unistd.h>
#include<errno.h>

char rk_editor() {
  int i;
  char c;
  while(1){
  i = read(STDIN_FILENO, &c, 1);
  assert(i == 1);
  if (i == -1){
  perror("read");
  } else{
  return c;}
  } 
}

int main() {
    char input;
    while (1) {
        input = rk_editor();
    }
    return 0;
}
