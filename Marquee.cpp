#include "Marquee.h"

std::string currentInput = "";
std::mutex MarqueeMutex;
std::vector<std::string> commandHistory;

Marquee::Marquee()
{
    init = false;
    quit = false;
}

bool Marquee::getQuit() const
{
    return quit;
}

void Marquee::setQuit()
{
    quit = true;
}

void Marquee::initialize()
{
    if (!init) {
        init = true;
        printHeader();
        // Start marquee animation in a separate thread
        std::thread marqueeThread(&Marquee::runMarquee, this);

        // Start input handling in main thread
        while (!getQuit()) {
            prompt();
        }

        marqueeThread.join(); // Wait for the marquee thread to finish (on quit)
    }
    else {
        std::cout << "[i] initialize command recognized. Doing something." << std::endl;
    }
}

void Marquee::clear()
{
    std::cout << "[i] clear command recognized. Doing something." << std::endl;
#ifdef _WIN32
    std::vector<std::string>().swap(commandHistory); //clears vector and free ups memory
    std::system("cls");

#else
    std::system("clear");
#endif

    printHeader();
}


void Marquee::printHeader()
{
    std::cout << "**********************************************\n"
        << "* Displaying a marquee console!             *\n"
        << "**********************************************\n";
}

void Marquee::runMarquee() {
    const std::string text = "Hello world in marquee!";
    const int delayMs = 16; //1000/60 (Ms/fps)

    // console handle
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // console size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // Header takes 3 lines and Prompt takes 6 lines
    const int headerHeight = 3;
    int promptHeight = 6;
    int marqueeBottomLimit = height - promptHeight;

    // Initial position and direction
    int x = 0, y = headerHeight;
    int dx = 1, dy = 1;

    int prevWidth = width, prevHeight = height;

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    while (!quit) {
        {
            std::lock_guard<std::mutex> lock(MarqueeMutex);  //mutex lock

            // Accommodates window resizing
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

            if (width != prevWidth || height != prevHeight) {
                prevWidth = width;
                prevHeight = height;
                marqueeBottomLimit = height - promptHeight;


                // Snap x to max allowed horizontal position
                if (x + static_cast<int>(text.length()) >= width) {
                    x = width - static_cast<int>(text.length()) - 1;
                    if (x < 0) x = 0;
                }

                // Snap y to bottom limit
                if (y >= marqueeBottomLimit) {
                    y = marqueeBottomLimit - 1;
                    if (y < headerHeight) y = headerHeight;
                }
            }

            system("cls");
            printHeader();

            // Update position
            x += dx;
            y += dy;

            // Bounce horizontally
            if (x <= 0 || x + text.length() >= width) dx = -dx;
            // Bounce vertically (after header)
            if (y <= headerHeight || y >= marqueeBottomLimit - 1) dy = -dy;

            // Draw marquee
            COORD newPos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
            SetConsoleCursorPosition(hConsole, newPos);
            std::cout << text << std::flush;

            // Prompt area
            COORD promptLine = { 0, static_cast<SHORT>(height - promptHeight) };
            SetConsoleCursorPosition(hConsole, promptLine);
            std::cout << "Enter a command for MARQUEE_CONSOLE: " << currentInput << std::endl;

            SHORT line = height;
            for (const auto& command : commandHistory) {
                COORD historyPos = { 0, line++ };
                SetConsoleCursorPosition(hConsole, historyPos);
                std::cout << command << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
}

void Marquee::splitString(std::string const& string, char const delim, std::vector<std::string>& tokens)
{
    // push an empty string and return to avoid going through the splitting process if input is empty.
    if (string.empty()) {
        tokens.push_back("");
        return;
    }

    size_t start;
    size_t end = 0;

    // find start and end of non-delimiter substrings and push them to a vector.
    while ((start = string.find_first_not_of(delim, end)) != std::string::npos) {
        end = string.find(delim, start);
        tokens.push_back(string.substr(start, end - start));
    }
}

void Marquee::prompt()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    char ch;
    currentInput.clear();

    while (true) {
        if (_kbhit()) {
            ch = _getch();

            if (ch == '\r') {  // Enter key
                {
                    std::lock_guard<std::mutex> lock(MarqueeMutex);

                    if (!currentInput.empty()) {
                        commandHistory.push_back("Command Processed in MARQUEE_CONSOLE : " + currentInput);
                    }
                }

                std::vector<std::string> tokens;
                splitString(currentInput, ' ', tokens);

                std::string cmd = tokens.empty() ? "" : tokens[0];

                if (cmd == "clear") {
                    clear();
                }
                else if (cmd == "exit") {
                    setQuit();
                }
                else if (!cmd.empty()) {
                }

                currentInput.clear();
                break;
            }
            else if (ch == '\b') {  // Backspace
                if (!currentInput.empty()) {
                    currentInput.pop_back();
                }
            }
            else if (isprint(ch)) {
                currentInput += ch;
            }
        }

        // Let marquee thread update in background
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }
}

