#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios default_termios;

void die(const char *s){
    perror(s);
    exit(1);
}

void disableRawMode(){
    if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &default_termios) == -1) die("tcsetattr");
}

void enableRawMode(){
    if (tcgetattr(STDERR_FILENO, &default_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = default_termios;
    raw.c_iflag &= ~(ICRNL|IXON);  // ICRNL = ctrl-m, IXON = ctrl-s & ctrl-q 
    raw.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);  // ECHO = console echo, ISIG = ctrl-z & ctrl-c, ICANON = read bytes, IEXTEN = ctrl-v
    raw.c_oflag &= ~(OPOST);  // OPOST = cursor reposition on new line
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    raw.c_iflag &= ~(BRKINT|INPCK|ISTRIP);  // Legacy
    raw.c_cflag |= ~(CS8);  // Legacy
    if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int main(){
    enableRawMode();
    while(1){
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if(iscntrl(c)) printf("%d\r\n", c);
        else printf("%d ('%c')\r\n", c, c);
        if (c=='q') break;
    }
    return 0;
}