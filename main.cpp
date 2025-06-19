#include "shell.h"
#include <windows.h>

void enableVirtualTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

int main()
{
    enableVirtualTerminal();
    Shell shell(4);
    shell.printHeader();  // Print header immediately

    while (!shell.getQuit()) {
        shell.prompt();
    }

    return 0;
}