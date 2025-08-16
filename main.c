#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)

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

char editorReadKey(){
    int nread;
    char c;
    while((nread = read(STDERR_FILENO, &c, 1) != 1)){
        if(nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

void editorProcessKeypress(){
    char c = editorReadKey();
    
    switch (c)
    {
        case CTRL_KEY('q'):
            exit(0);
            break;
    }
}

int main(){
    enableRawMode();
    while(1){
        editorProcessKeypress();
    }
    return 0;
}