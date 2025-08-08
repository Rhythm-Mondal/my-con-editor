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
    raw.c_iflag &= ~(ICRNL|IXON);  // ICRNL = ctrl-m, IXON = ctrl-s & ctrl-q 
    raw.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);  // ECHO = console echo, ISIG = ctrl-z & ctrl-c, ICANON = read bytes, IEXTEN = ctrl-v
    raw.c_oflag &= ~(OPOST);  // OPOST = cursor reposition on new line
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    raw.c_iflag &= ~(BRKINT|INPCK|ISTRIP);  // Legacy
    raw.c_cflag |= ~(CS8);  // Legacy
    tcsetattr(STDERR_FILENO, TCSAFLUSH, &raw);
}

int main(){
    enableRawMode();
    printf("write somethign doc!\n");
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
        if iscntrl(c) printf("%d\r\n", c);
        else printf("%d ('%c')\r\n", c, c);
    }
    return 0;
}