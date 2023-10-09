#include<ctype.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios t_o;

void die(const char *g) {
  perror(g);
  exit(1);
}

void raw_disable() {
 if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &t_o) == -1) 
    die("tcsetattr");
}

void raw_enable() {
  
  if(tcgetattr(STDIN_FILENO, &t_o) == -1) die("tcgetattr");
  atexit(raw_disable);

  struct termios raw = t_o;
  raw.c_iflag &= ~(BRKINT|INPCK|ISTRIP|IXON|ICRNL);
  raw.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char rk_editor() {
  int i;
  char c;
  while ((i = read(STDIN_FILENO, &c, 1)) != 1) {
    if (i == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

void r_e_s() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
}
/*** input ***/
void k_p_e() {
  char c = rk_editor();
  switch (c) {
    case ('q' & 0x1F):
      exit(0);
      break;
  }
}


int main() {
  raw_enable();
  
  while(1){
  r_e_s(); 
  k_p_e();
  }
  return 0;
}
