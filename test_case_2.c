#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>


void raw_enable() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void raw_disable() {
    struct termios original;
    tcgetattr(STDIN_FILENO, &original);
    original.c_lflag |= (ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}

int main() {
    struct termios original;
    tcgetattr(STDIN_FILENO, &original);

    raw_enable();
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    assert((original.c_lflag & (ECHO | ICANON)) == (ICANON | ECHO));
       
    raw_disable();
    struct termios restored;
    tcgetattr(STDIN_FILENO, &restored);
    assert(memcmp(&original, &restored, sizeof(struct termios)) == 0); 
    printf("Raw mode test passed!\n");
    return 0;
}
