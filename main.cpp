#include "shell.h"

int main()
{
    Shell shell;
    shell.initialize();

    while (!shell.getQuit()) {
        shell.prompt();
    }

    return 0;
}