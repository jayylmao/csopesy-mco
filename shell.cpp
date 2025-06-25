#include "shell.h"
#include "FCFSScheduler.h"
#include "RoundRobinScheduler.h"

#include <chrono>

Shell::Shell(int cores) :
    init(false),
    quit(false),
    focusedPID(0),
    cores(cores),
    scheduler(nullptr),
    batchProcessActive(false),  // Initialize batch processing as inactive
    batchFreq(1)                // Default frequency: 1 CPU cycle
{
}

Shell::~Shell() {
    schedulerStop();  // Ensure batch thread stops on exit
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

int Shell::getCores()
{
    return this->cores;
}

void Shell::initialize() {
    // Reload configuration every time
    std::cout << "[i] Loading configuration..." << std::endl;
    configManager.loadConfig("config.txt");

    // Print config if successfully loaded
    if (!configManager.getConfig().empty()) {
        configManager.printConfig();
    }
    else {
        std::cout << "[!] Config unable to be read. Check if file exists or is in out>build>x64-debug" << std::endl;
    }

    // Read and validate num-cores
    const auto& config = configManager.getConfig();
    if (config.find("num-cores") != config.end()) {
        try {
            cores = std::stoi(config.at("num-cores"));
            if (cores < 1 || cores > 16) {
                std::cerr << "[!] Invalid num-cores value ("
                    << cores << "). Must be between 1-16. Using default (4)." << std::endl;
                cores = 4;
            }
        }
        catch (...) {
            std::cerr << "[!] Invalid num-cores value. Using default (4)." << std::endl;
            cores = 4;
        }
    }

    if (!init) {
        init = true;

        // Create scheduler based on config
        std::string schedulerType;
        bool validScheduler = false;

        // Check if scheduler type is specified in config
        if (config.find("scheduler") != config.end()) {
            schedulerType = config.at("scheduler");


            if (schedulerType == "fcfs" || schedulerType == "FCFS") {
                std::cout << "[i] Using FCFS scheduler with " << cores << " cores" << std::endl;
                scheduler = std::make_unique<FCFSScheduler>(cores);
                validScheduler = true;
            }
            else if (schedulerType == "rr" || schedulerType == "RR") {
                int quantum = 5; // Default time quantum
                bool validQuantum = true;

                // Check for time quantum configuration
                if (config.find("time_quantum") != config.end()) {
                    try {
                        quantum = std::stoi(config.at("time_quantum"));
                        if (quantum <= 0) {
                            std::cerr << "[!] Invalid time_quantum value ("
                                << quantum << "). Must be positive integer. Using default (5)" << std::endl;
                            validQuantum = false;
                        }
                    }
                    catch (...) {
                        std::cerr << "[!] Invalid time_quantum value. Using default (5)" << std::endl;
                        validQuantum = false;
                    }
                }

                if (validQuantum) {
                    std::cout << "[i] Using Round Robin scheduler with " << cores
                        << " cores (quantum=" << quantum << ")" << std::endl;
                }
                else {
                    std::cout << "[i] Using Round Robin scheduler with " << cores
                        << " cores and default quantum (5)" << std::endl;
                }

                scheduler = std::make_unique<RoundRobinScheduler>(cores, quantum);
                validScheduler = true;
            }
        }

        // Handle invalid/missing scheduler configuration
        if (!validScheduler) {
            if (schedulerType.empty()) {
                std::cerr << "[!] No scheduler type specified in config. Initialize failed." << std::endl;
            }
            else {
                std::cerr << "[!] Invalid scheduler type '" << schedulerType
                    << "' specified. Initialize failed." << std::endl;
            }

            // Initialize failed.
            init = !init;
        }

        // Load batch-process-freq from config with validation
        if (config.find("batch-process-freq") != config.end()) {
            try {
                batchFreq = std::stoi(config.at("batch-process-freq"));
                if (batchFreq < 1 || batchFreq > 232) {
                    std::cerr << "[!] Invalid batch-process-freq value ("
                        << batchFreq << "). Must be between 1-232. Using default (1)." << std::endl;
                    batchFreq = 1;
                }
            }
            catch (...) {
                std::cerr << "[!] Invalid batch-process-freq value. Using default (1)." << std::endl;
                batchFreq = 1;
            }
        }

        // Start the scheduler
        if (init) {
            std::thread schedulerThread([this]() {
                if (scheduler) {
                    scheduler->runScheduler();
                }
                else {
                    std::cerr << "[!] Scheduler initialization failed!" << std::endl;
                }
                });
            schedulerThread.detach();
        }

    }
    else {
        std::cout << "[i] Configuration reloaded." << std::endl;
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
        std::string processName = args[1];

        //simulated total lines
        int totalLines = 100;
        processManager.createProcess(processName, totalLines);

        std::shared_ptr<Process> ptr = processManager.getSharedProcess(processManager.getNextPID() - 1);

        // Use the scheduler interface instead of concrete class
        if (scheduler) {
            scheduler->addProcess(ptr);
        }
        else {
            std::cerr << "[!] Scheduler not initialized! Run 'initialize' first." << std::endl;
            return;
        }

        auto screen = std::make_shared<ScreenS>(processName, totalLines, processManager.getNextPID() - 1);
        ConsoleManager::getInstance()->addConsole(processName, screen);
        screen->onEnabled();
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
        std::shared_ptr<AConsole> console = ConsoleManager::getInstance()->getConsole(processName);

        if (console) {
            std::system("cls");
            console->onEnabled();
            std::system("cls");
            printHeader();
        }
        else {
            std::cout << "[*] No process with name '" << processName << "' found.\n";
        }
        return;
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
    outputProcessList(std::cout);
}

void Shell::outputProcessList(std::ostream& out)
{
    std::vector<Process*> processes = processManager.listProcesses();

    int totalCores = cores;
    int usedCores = 0;

    // Count used cores
    for (const auto& proc : processes) {
        if (!proc->hasFinished() && proc->getCoreId() != -1) {
            usedCores++;
        }
    }

    int availableCores = totalCores - usedCores;
    if (availableCores < 0) availableCores = 0;

    double utilization = (static_cast<double>(usedCores) / totalCores) * 100.0;

    out << "Core utilization: " << static_cast<int>(utilization) << "%" << std::endl;
    out << "Cores used: " << usedCores << std::endl;
    out << "Cores available: " << availableCores << std::endl;
    out << "--------------------" << std::endl;

    if (processes.empty()) {
        out << "[!] No processes are running." << std::endl;
        return;
    }

    out << "Running processes" << std::endl;
    for (const auto& proc : processes) {
        if (!(proc->hasFinished())) {
            std::string coreDisplay = (proc->getCoreId() == -1) ? "Pending" : std::to_string(proc->getCoreId());
            out << proc->getName() << "\t(" << proc->getCreationTimestamp()
                << ")\tCore: " << coreDisplay << "\t" << proc->getCurrentLine() << "/" << proc->getTotalLines()
                << std::endl;
        }
    }

    out << "--------------------" << std::endl
        << "Finished processes" << std::endl;
    for (const auto& proc : processes) {
        if (proc->hasFinished()) {
            std::string coreDisplay = (proc->getCoreId() == -1) ? "N/A" : std::to_string(proc->getCoreId());
            out << proc->getName() << "\t(" << proc->getCreationTimestamp()
                << ")\tFinished " << proc->getCurrentLine() << "/" << proc->getTotalLines()
                << std::endl;
        }
    }
    out << "--------------------" << std::endl;
}


// shell.cpp - Modified schedulerStart() function
void Shell::schedulerStart()
{
    if (!init) {
        std::cout << "[!] The system has not been initialized. Please run 'initialize' first." << std::endl;
        return;
    }

    if (batchProcessActive) {
        std::cout << "[i] Batch processing is already active." << std::endl;
        return;
    }

    // Start batch processing
    batchProcessActive = true;
    batchThread = std::thread([this]() {
        int counter = 0;
        while (batchProcessActive) {
            // Create new batch process
            std::string name = "batch_" + std::to_string(counter++);
            int totalLines = 100; // Default instruction count

            processManager.createProcess(name, totalLines);
            std::shared_ptr<Process> proc = processManager.getSharedProcess(processManager.getNextPID() - 1);

            if (scheduler) {
                scheduler->addProcess(proc);
                // Removed notification to prevent CLI flooding
            }

            // CORRECTED: Generate batchFreq processes per CPU cycle
            // Each instruction = 100ms, so time between processes = 100ms / batchFreq
            int sleep_time = 100 / batchFreq;
            if (sleep_time <= 0) sleep_time = 1; // Ensure minimum sleep time

            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
        });

    std::cout << "[i] Batch processing started (frequency: "
        << batchFreq << " processes per CPU cycle)" << std::endl;
}

void Shell::schedulerStop()
{
    if (!batchProcessActive) {
        std::cout << "[i] Batch processing is not currently active." << std::endl;
        return;
    }

    // Stop batch processing
    batchProcessActive = false;
    if (batchThread.joinable()) {
        batchThread.join();
    }
    std::cout << "[i] Batch processing stopped." << std::endl;
}



void Shell::reportUtil()
{
    std::ofstream outFile("csopesy-log.txt");
    if (!outFile) {
        std::cerr << "[!] Failed to open csopesy-log.txt for writing." << std::endl;
        return;
    }
    outFile 
        << " _______ ___  ___  ___ ___ __ __\n"
        "/ __(_-</ _ \\/ _ \\/ -_|_-</ // /\n"
        "\\__/___/\\___/ .__/\\__/___/\\_, / \n"
        "           /_/           /___/  \n";

    outFile << "Welcome to the CSOPESY Emulator!\n\n"
        "Developers: \nCarlos, Jay Michael \n"
        "Diego, Karl Nico \n"
        "Reyes, Janica Megan \n"
        "Santos, Emmanuel Gabriel\n\n";

    auto now = std::chrono::system_clock::now();
    std::time_t raw_time = std::chrono::system_clock::to_time_t(now);

    outFile << "Last updated: "
        << std::put_time(std::localtime(&raw_time), "%m/%d/%Y %I:%M:%S %p");

    outputProcessList(outFile);
    std::cout << "Report generated." << std::endl;
}

void Shell::clear()
{
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

void Shell::splitString(std::string const& string, char const delim, std::vector<std::string>& tokens)
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

    std::cout << "\nuser ~ > " << std::flush;
    std::getline(std::cin, input);

    // don't perform string split when input is empty.
    if (input.empty()) {
        return;
    }

    // split string to get command and parameters as vector.
    std::vector<std::string> input_tokens;
    splitString(input, delimiter, input_tokens);

    // Handle initialize command
    if (input_tokens[0] == "initialize") {
        initialize();
        return;
    }

    // Block other commands if not initialized
    if (!init) {
        if (input_tokens[0] == "help" || input_tokens[0] == "exit") {
            // Allow these commands
        }
        else {
            std::cout << "[!] The system has not been initialized. Please run 'initialize' first." << std::endl;
            return;
        }
    }

    if (input_tokens[0] == "screen") {
        std::vector<std::string> sliced_input_tokens(input_tokens.begin() + 1, input_tokens.end());
        screen(sliced_input_tokens);
    }
    else if (input_tokens[0] == "scheduler-start") {
        schedulerStart();
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
            << "    - screen -s <args>\n"
            << "    - screen -r <args>\n"
            << "    - screen -ls\n"
            << "  scheduler-start  Start generating batch processes\n"  // Updated help text
            << "  scheduler-stop   Stop generating batch processes\n"   // Updated help text
            << "  report-util\n"
            << "  clear\n"
            << "  exit\n" << std::endl;
    }
    else if (!input_tokens[0].empty()) { // error on unknown, non-empty command.
        std::cout << "[*] Unknown command. Type 'help' to get a list of commands." << std::endl;
    }
}