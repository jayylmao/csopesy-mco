#include "shell.h"

Shell::Shell()
{
    init = false;
    quit = false;
    focusedPID = 0;
}

bool Shell::getQuit() const
{
    return quit;
}

void Shell::setQuit()
{
    quit = true;
    std::exit(EXIT_SUCCESS);
}

bool Shell::getInit() const
{
    return init;
}

void Shell::setInit()
{
    init = true;
}

int Shell::getFocusedPID() const
{
    return focusedPID;
}

void Shell::setFocusedPID(const int pid)
{
    focusedPID = pid;
}

void Shell::initialize()
{
    if (!init) {
        init = true;
        printHeader();
        prompt();
    }
    else {
        std::cout << "[i] initialize command recognized. Doing something." << std::endl;
    }
}

void Shell::screen(std::vector<std::string> args)
{
    // no argument given to screen.
    if (args.size() < 1) {
        std::cout << "[*] The screen command needs an argument. -s to create a new process, -r to view a process, and -ls to list the running processes." << std::endl;
        return;
    } // switch to list screen command.
    else if (args[0] == "-ls") { //SCREEN -LS
        screenList();
        return;
    } // too many arguments given to screen.
  

    else if (args[0] == "-s") { //SCREEN - S
        if (args.size() < 2) {
            std::cout << "[*] You must provide a process name. Usage: screen -s <processname>" << std::endl;
            return;
        }
        std::system("cls");
        static int nextPID = 1; //first process ID
        std::string processName = args[1];
        int pid = nextPID++;

        //simulated total lines
        int totalLines = 50;

        auto newConsole = std::make_unique<ScreenS>(processName, totalLines, pid);
        newConsole->onEnabled(); // runs child console (displays and waits for "exit")

        // Save the process
        consoles.push_back(std::move(newConsole));

        //clear();
        std::system("cls");
        printHeader();

    }


    else if (args[0] == "-r") { // SCREEN -R
        if (args.size() < 2) {
            std::cout << "[*] You must provide a process name. Usage: screen -r <processname>" << std::endl;
            return;
        }

        std::string processName = args[1];
        bool found = false;

        for (const auto& console : consoles) {
            if (console->getName() == processName) {
                std::system("cls");
                console->onEnabled(); // Enter interactive loop
                found = true;
                break;
            }
        }

        if (!found) {
            std::cout << "[*] No process with name '" << processName << "' found.\n";
        }
        else {
            // After exiting child process, return to main screen
            std::system("cls");
            printHeader(); // Re-show your main screen
        }
    }


    else if (args.size() > 2) {
        std::cout << "[*] Too many arguments given. -s to create a new process, -r to redraw the screen and create a new process, and -ls to list the running processes." << std::endl;
        return;
    }
    else if (args[0] == "-s") {
        std::cout << "[*] Creating process..." << std::endl;
        return;
    }
    else if (args[0] == "-r") {
        std::cout << "[*] Redrawing and creating a new process..." << std::endl;
        return;
    }
    else {
        std::cout << "[*] " << args[0] << " is an invalid argument." << std::endl;
        return;
    }
}

void Shell::screenList()
{
    //std::cout << "[i] list all running processes." << std::endl;
    ScreenList listView(consoles);
    listView.display();
}

void Shell::schedulerTest()
{
    std::cout << "[i] scheduler-test command recognized. Doing something." << std::endl;
}

void Shell::schedulerStop()
{
    std::cout << "[i] scheduler-stop command recognized. Doing something." << std::endl;
}

void Shell::reportUtil()
{
    std::cout << "[i] report-util command recognized. Doing something." << std::endl;
}

void Shell::clear()
{
    std::cout << "[i] clear command recognized. Doing something." << std::endl;
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
    
    printHeader();
}

void Shell::exit()
{
    if (getFocusedPID() == 0) {
        setQuit();
    }
    else { // TODO: exit process. for now, this just sets the focused pid and nothing else.
        setFocusedPID(0);
    }
}

void Shell::printHeader()
{
    std::cout << "\033[1;36m" // Bright cyan
        << " _______ ___  ___  ___ ___ __ __\n"
        "/ __(_-</ _ \\/ _ \\/ -_|_-</ // /\n"
        "\\__/___/\\___/ .__/\\__/___/\\_, / \n"
        "           /_/           /___/  \n"
        << "\033[1;32m" // Bright green
        << "Welcome to the CSOPESY command line. Type 'help' for a list of commands.\n"
        << "\033[0m"; // Reset color
}

void Shell::splitString(std::string const &string, char const delim, std::vector<std::string> &tokens)
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

void Shell::prompt()
{
    std::string input;
    constexpr char delimiter = ' '; // command arguments are separated by a space.

    std::cout << "user ~ > " << std::flush;
    std::getline(std::cin, input);

    // don't perform string split when input is empty.
    if (input.empty()) {
        return;
    }

    // split string to get command and parameters as vector.
    std::vector<std::string> input_tokens;
    splitString(input, delimiter, input_tokens);

    if (input_tokens[0] == "initialize") {
        initialize();
    } 
    else if (input_tokens[0] == "screen") {
        std::vector<std::string> sliced_input_tokens(input_tokens.begin() + 1, input_tokens.end());
        screen(sliced_input_tokens);
    }
    else if (input_tokens[0] == "scheduler-test") {
        schedulerTest();
    }
    else if (input_tokens[0] == "scheduler-stop") {
        schedulerStop();
    }
    else if (input_tokens[0] == "report-util") {
        reportUtil();
    }
    else if (input_tokens[0] == "clear") {
        clear();
    }
    else if (input_tokens[0] == "exit") {
        setQuit();
    }
    else if (input_tokens[0] == "help") { //update for more commands soon.
        std::cout << "[*] Available commands:\n\n"
            << "  initialize\n"
            << "  screen\n"
            << "    - screen -s\n"
            << "    - screen -r\n"
            << "    - screen -ls\n"
            << "  scheduler-test\n"
            << "  scheduler-stop\n"
            << "  report-util\n"
            << "  clear\n"
            << "  exit\n" << std::endl;
    }
    else if (!input_tokens[0].empty()) { // error on unknown, non-empty command.
        std::cout << "[*] Unknown command. Type 'help' to get a list of commands." << std::endl;
    }
}
