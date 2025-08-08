#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios default_termios;

void disableRawMode(){
    tcsetattr(STDERR_FILENO, TCSAFLUSH, &default_termios);
}

void enableRawMode(){
    tcgetattr(STDERR_FILENO, &default_termios);
    atexit(disableRawMode);

    struct termios raw = default_termios;
    raw.c_iflag &= ~(IXON);
    raw.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);
    tcsetattr(STDERR_FILENO, TCSAFLUSH, &raw);
}

int main(){
    enableRawMode();
    printf("write somethign doc!\n");
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
        if iscntrl(c) printf("%d\n", c);
        else printf("%d ('%c')\n", c, c);
    }
    return 0;
}