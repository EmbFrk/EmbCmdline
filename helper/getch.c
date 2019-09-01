/*********************************************************
* linux substituide to get one character in the terminal *
*********************************************************/

#if LINUX_GETCH
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

int getch () {
    int ch;
    struct termios tc_attrib;
    if (tcgetattr(STDIN_FILENO, &tc_attrib))
        return -1;

    tcflag_t lflag = tc_attrib.c_lflag;
    tc_attrib.c_lflag &= ~ICANON & ~ECHO;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &tc_attrib))
        return -1;

    ch = getchar();

    tc_attrib.c_lflag = lflag;
    tcsetattr (STDIN_FILENO, TCSANOW, &tc_attrib);
    return ch;
}
#endif
