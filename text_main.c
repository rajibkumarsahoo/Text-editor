#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)
#define KILO_VERSION "0.0.1"


struct editorConfig{
    int cx, cy;
    int total_rows;// total number of rows in the editor
    int total_cols; // total number of cols in the editor
    struct termios t_o;  
};

struct editorConfig E;
// E.total_rows = 24;
// E.total_cols = 80;


enum editorKey {
  ARROW_LEFT = 1000,
  ARROW_RIGHT ,
  ARROW_UP ,
  ARROW_DOWN,
  PAGE_UP,
  PAGE_DOWN
};


void editorMoveCursor(int key) {
  switch (key) {
    case ARROW_LEFT:
      if(E.cx !=0){
        E.cx--;
      } 
      break;
    case ARROW_RIGHT:
      if(E.cx != E.total_cols-1){
        E.cx++;
      }  
      break;
    case ARROW_UP:
      if(E.cy != 0){
        E.cy--;
      }
      break;
    case ARROW_DOWN:
    if(E.cy != E.total_rows-1){
      E.cy++;
    }
      break;
  }
}


void die(const char *g) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO,"\x1b[H",3);    
  perror(g);
  exit(1);
}

int rk_editor() {//editor read key
  int i;
  char c;
  while ((i = read(STDIN_FILENO, &c, 1)) != 1) {
    if (i == -1 && errno != EAGAIN) die("read");
  }
  if (c == '\x1b') {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
    if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
          }
        }
      } 
      else {
      switch (seq[1]) {
        case 'A': return ARROW_UP;
        case 'B': return ARROW_DOWN;
        case 'C': return ARROW_RIGHT;
        case 'D': return ARROW_LEFT;
      }
    }
    }
    return '\x1b';
  } else {
    return c;
  }
}

int getCursorPosition(int *rows, int *cols) {
  char buf[32];
  unsigned int i = 0;
  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';
  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
  return 0;
}

int getWindowSize(int *rows, int *cols) {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    return getCursorPosition(rows, cols);
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/*** append buffer ***/
struct abuf {
  char *b;
  int len;
};

#define ABUF_INIT {NULL, 0}


void abAppend(struct abuf *ab, const char *s, int len) {
  char *new = realloc(ab->b, ab->len + len);
  if (new == NULL) return;
  memcpy(&new[ab->len], s, len);
  ab->b = new;
  ab->len += len;
}
void abFree(struct abuf *ab) {
  free(ab->b);
}


void editorDrawRows(struct abuf *ab){
    int i;
    for(i=0;i<E.total_rows;i++){
        

        if (i == E.total_rows / 3) {
          char welcome[80];
          int welcomelen = snprintf(welcome, sizeof(welcome),
            "Kilo editor -- version %s", KILO_VERSION);
          if (welcomelen > E.total_cols) {
            welcomelen = E.total_cols;
          } 

        int padding = (E.total_cols - welcomelen) / 2;
        if (padding) {
          abAppend(ab, "~", 1);
          padding--;
        }
        while (padding--) {
          abAppend(ab, " ", 1);
        }

          abAppend(ab, welcome, welcomelen);
        } else {
          abAppend(ab, "~", 1);
        }

        abAppend(ab, "\x1b[K", 3);
        if(i<E.total_rows-1){
            abAppend(ab, "\r\n", 2);
        }
    }
}



/** initialization of the editor **/
void initEditor(){
    E.cx = 0;
    E.cy = 0;
    if(getWindowSize(&E.total_rows,&E.total_cols)==-1)die("getWindowSize");
}









void raw_disable() {
 if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.t_o) == -1) 
    die("tcsetattr");
}

void raw_enable() {
  
  if(tcgetattr(STDIN_FILENO, &E.t_o) == -1) die("tcgetattr");
  atexit(raw_disable);

  struct termios raw = E.t_o;
  raw.c_iflag &= ~(BRKINT|INPCK|ISTRIP|IXON|ICRNL);
  raw.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw) == -1) die("tcsetattr");
}



void r_e_s() {// editor refresh screen

  struct abuf ab = ABUF_INIT;

  abAppend(&ab, "\x1b[?25l", 6);

  // abAppend(&ab, "\x1b[2J", 4);
  abAppend(&ab, "\x1b[H", 3);
  editorDrawRows(&ab);

  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1, E.cx + 1);
  abAppend(&ab, buf, strlen(buf));

  abAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

/*** input ***/
void k_p_e() {//editorprocesskeyword
  int c = rk_editor();
  switch (c) {
    case CTRL_KEY('q'):
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO,"\x1b[H",3);
      exit(0);
      break;

    case PAGE_UP:
    case PAGE_DOWN:
    {
      int time=E.total_rows;
      while(time--){
        editorMoveCursor(c==PAGE_UP ? ARROW_UP : ARROW_DOWN);
      }
    }
    break;  

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
      editorMoveCursor(c);
      break;



  }
}


int main() {
  raw_enable();
  initEditor();
  
  while(1){
  r_e_s(); 
  k_p_e();
  }
  return 0;
}