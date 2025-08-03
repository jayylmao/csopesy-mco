#include "shell.h"
#include <windows.h>
#include <thread>     
#include <chrono> 
#include <cstdint>

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

    const int delayPerCycleMs = 100; //change here
    Shell shell(delayPerCycleMs);

    shell.printHeader();  // Print header immediately

    //uint32_t CPUcycles = 0;
    


    while (!shell.getQuit()) {
        shell.prompt();
        
       // CPUcycles++;
        std::this_thread::sleep_for(std::chrono::milliseconds(delayPerCycleMs));
    }

    return 0;
}