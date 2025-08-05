#include "shell.h"
#include "ScreenCommands.h"
#include "Marquee.h"
#include "FCFSScheduler.h"
#include "RoundRobinScheduler.h"
#include "IScheduler.h"
#include "Process.h"
#include "DemandPagingAllocator.h"

#include <chrono>
#include <iostream>
#include <fstream>
#include <random>
#include <memory>

Shell::Shell(int delayPerCycleMs) :
    delayPerCycleMs(delayPerCycleMs),
    init(false),
    quit(false),
    focusedPID(0),
    cores(4),
    scheduler(nullptr),
    delayPerExec(100),
    memPerFrame(16),
    maxMem(16384),
    minMemPerProc(64),
    maxMemPerProc(16384),
    batchProcessActive(false),  // Initialize batch processing as inactive
    batchFreq(1)                // Default frequency: 1 CPU cycle
{
    srand(time(0));
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

    const auto& config = configManager.getConfig();
    // Sets minIns and Max Ins
	if (config.find("min-ins") != config.end()) 
    {
		minIns = std::stoi(config.at("min-ins"));
	}
	if (config.find("max-ins") != config.end()) 
    {
		maxIns = std::stoi(config.at("max-ins"));
	}
    // Swaps if min greater than max
	if (minIns > maxIns) std::swap(minIns, maxIns); 


	// Read and validate num-cores
    if (config.find("num-cpu") != config.end()) {
        try {
            std::cout << "core count: " << std::stoi(config.at("num-cpu")) << std::endl;
            cores = std::stoi(config.at("num-cpu"));
            if (cores < 1 || cores > 128) {
                std::cerr << "[!] Invalid num-cpu value ("
                    << cores << "). Must be between 1-128. Using default (4)." << std::endl;
                cores = 4;
            }
        }
        catch (...) {
            std::cerr << "[!] Invalid num-cores value. Using default (4)." << std::endl;
            cores = 4;
        }
    }

    if (config.find("delay-per-exec") != config.end()) {
        try {
            std::cout << "delay per instruction execution: " << std::stoi(config.at("delay-per-exec")) << std::endl;
            delayPerExec = std::stoi(config.at("delay-per-exec"));
            if (delayPerExec < 0 || delayPerExec > 4294967296) {
                std::cerr << "[!] Invalid delay-per-exec value (" << delayPerExec << "). Using default (100)." << std::endl;
                delayPerExec = 100;
            }
        }
        catch (...) {
            std::cerr << "[!] Invalid delay-per-exec value. Using default ()." << std::endl;
            delayPerExec = 100;
        }
    }
    
    maxMem = 16384;
    if (config.find("max-overall-mem") != config.end()) {
        try {
            maxMem = std::stoi(config.at("max-overall-mem"));
            std::cout << "maximum memory: " << maxMem << " kb" << std::endl;
            
            if (maxMem < 64 || maxMem > 65536) {
                std::cerr << "[!] Invalid max-overall-mem value (" << maxMem << "). Using default (16384)." << std::endl;
            }
        }
        catch (...) {
            std::cerr << "[!] Invalid max-overall-mem value (" << maxMem << "). Using default (16384)." << std::endl;
        }
    }

    memPerFrame = 16;
    if (config.find("mem-per-frame") != config.end()) {
        try {
            memPerFrame = std::stoi(config.at("mem-per-frame"));
            std::cout << "memory per frame: " << memPerFrame << " kb" << std::endl;

            if (memPerFrame < 16 || memPerFrame > maxMem) {
                std::cerr << "[!] Invalid mem-per-frame value (" << memPerFrame << "). Using default (16)." << std::endl;
            }
        }
        catch (...) {
            std::cerr << "[!] Invalid mem-per-frame value (" << memPerFrame << "). Using default (16)." << std::endl;
        }
    }

    memoryManager = std::make_shared<DemandPagingAllocator>(maxMem, memPerFrame);
    processManager = std::make_shared<ProcessManager>(memoryManager);

    minMemPerProc = 64;
    if (config.find("min-mem-per-proc") != config.end()) {
        try {
            minMemPerProc = std::stoi(config.at("min-mem-per-proc"));
            std::cout << "minimum memory per process: " << minMemPerProc << " kb" << std::endl;

            if (minMemPerProc < 64 || minMemPerProc > maxMem) {
                std::cerr << "[!] Invalid min-mem-per-proc value (" << minMemPerProc << "). Using default (64)." << std::endl;
            }
        }
        catch (...) {
            std::cerr << "[!] Invalid min-mem-per-proc value (" << minMemPerProc << "). Using default (64)." << std::endl;
        }

        processManager->setMinMemPerProc(minMemPerProc);
    }
    
    maxMemPerProc = 65536;
    if (config.find("max-mem-per-proc") != config.end()) {
        try {
            maxMemPerProc = std::stoi(config.at("max-mem-per-proc"));
            std::cout << "maximum memory per process: " << maxMemPerProc << " kb" << std::endl;

            if (maxMemPerProc < 64 || maxMemPerProc > maxMem) {
                std::cerr << "[!] Invalid min-mem-per-proc value (" << maxMemPerProc << "). Using default (65536)." << std::endl;
            }
        }
        catch (...) {
            std::cerr << "[!] Invalid min-mem-per-proc value (" << maxMemPerProc << "). Using default (65536)." << std::endl;
        }

        processManager->setMaxMemPerProc(maxMemPerProc);
    }

    if (!init) {
        init = true;
        // Create scheduler based on config
        std::string schedulerType;
        bool validScheduler = false;
        
        std::cout << "allocate " << maxMem << std::endl;

        // Check if scheduler type is specified in config
        if (config.find("scheduler") != config.end()) {
            schedulerType = config.at("scheduler");

            if (schedulerType == "fcfs" || schedulerType == "FCFS") {
                std::cout << "[i] Using FCFS scheduler with " << cores << " cores" << std::endl;
                scheduler = std::make_unique<FCFSScheduler>(cores, memoryManager, processManager);
                validScheduler = true;
            }
            else if (schedulerType == "rr" || schedulerType == "RR") {
                int quantum = 5; // Default time quantum
				int snapshotInterval = 5; // Default snapshot value
                bool validQuantum = true;

                // Check for time quantum configuration
                if (config.find("quantum-cycles") != config.end()) {
                    try {
                        quantum = std::stoi(config.at("quantum-cycles"));
                        snapshotInterval = quantum;
                        if (quantum <= 0) {
                            std::cerr << "[!] Invalid quantum-cycles value ("
                                << quantum << "). Must be positive integer. Using default (5)" << std::endl;
                            validQuantum = false;
                        }
                    }
                    catch (...) {
                        std::cerr << "[!] Invalid quantum-cycles value. Using default (5)" << std::endl;
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

                scheduler = std::make_unique<RoundRobinScheduler>(cores, quantum, snapshotInterval, memoryManager, processManager);
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
        std::cout << "[i] Configuration loaded." << std::endl;
    }
}

void Shell::screen(std::vector<std::string> args)
{
    // no argument given to screen.
    if (args.size() < 1) {
        std::cout << "[*] The screen command needs an argument. -s to create a new process, -r to view a process, and -ls to list the running processes." << std::endl;
        return;
    } // switch to list screen command.
    //SCREEN -LS
    else if (args[0] == "-ls") { 
screenList();
        return;
    } // too many arguments given to screen.
    //SCREEN - S
    else if (args[0] == "-s") { 
        if (args.size() < 3) {
            std::cout << "[*] You must provide a process name. Usage: screen -s <processname> <memorysize>" << std::endl;
            return;
        }
        std::system("cls");
        std::string processName = args[1];
        int memory = stoi(args[2]);

        processManager->createProcess(processName, randomNumber(), memory, memPerFrame);

        std::shared_ptr<Process> ptr = processManager->getSharedProcess(processName);

        // Use the scheduler interface instead of concrete class
        if (scheduler) {
            scheduler->addProcess(ptr);
        }
        else {
            std::cerr << "[!] Scheduler not initialized! Run 'initialize' first." << std::endl;
            return;
        }

        int pid = processManager->getNextPID() - 1;
        auto screen = std::make_shared<ScreenS>(processName, pid, processManager.get(), memoryManager);
        screen->onEnabled();
        clear();
    }
    // SCREEN -R
    else if (args[0] == "-r") {
        if (args.size() < 2) {
            std::cout << "[*] You must provide a process name. Usage: screen -r <processname>" << std::endl;
            return;
        }

        std::string processName = args[1];
        std::vector<Process*> processes = processManager->listProcesses();
        Process* found = nullptr;
        int pid = -1, totalLines = 0;

        for (auto* p : processes) {
            if (p->getName() == processName) {
                found = p;
                pid = p->getPID();
                totalLines = p->getTotalLines();
                break;
            }
        }

        if (found) {
            // Check if process terminated due to memory access violation
            if (found->hasMemoryAccessViolation()) {
                std::cout << "Process " << processName << " shut down due to memory access violation error that occurred at "
                    << found->getViolationTime() << ". " << found->getViolationAddress() << " invalid" << std::endl;
                return;
            }

            std::system("cls");
            auto screenConsole = std::make_shared<ScreenS>(processName, pid, processManager.get(), memoryManager);
            screenConsole->onEnabled();
            clear();
        }
        else {
            std::cout << "[*] Process not found: " << processName << std::endl;
        }
        return;
    }
    // SCREEN -C
    else if (args[0] == "-c") {
        // Validate argument count
        if (args.size() < 4) {
            std::cerr << "[*] Usage: screen -c <process_name> <memory_size> \"<1-50 semicolon-separated instructions>\"\n";
            std::cerr << "[*] Example: screen -c process2 256 \"DECLARE varA 10; PRINT(\"Result: \" + varA + \" units\")\"\n";
            return;
        }

        const std::string& processName = args[1];
        const std::string& memSizeStr = args[2];

        // Reconstruct instruction string with proper quote handling
        std::string instructionStr;
        for (size_t i = 3; i < args.size(); ++i) {
            if (!instructionStr.empty()) instructionStr += " ";
            instructionStr += args[i];
        }

        // Remove ONLY outer quotes while preserving inner quotes
        if (instructionStr.size() >= 2 &&
            instructionStr.front() == '"' &&
            instructionStr.back() == '"') {
            instructionStr = instructionStr.substr(1, instructionStr.size() - 2);
        }

        // Enhanced instruction parsing
        std::vector<std::vector<std::string>> parsedInstructions;
        size_t start = 0;
        bool inQuotes = false;
        int parenDepth = 0;

        for (size_t i = 0; i <= instructionStr.size(); ++i) {
            char c = (i < instructionStr.size()) ? instructionStr[i] : ';';

            // Track quotes (ignoring escaped quotes)
            if (c == '"' && (i == 0 || instructionStr[i - 1] != '\\')) {
                inQuotes = !inQuotes;
            }
            // Track parentheses outside quotes
            else if (!inQuotes && c == '(') parenDepth++;
            else if (!inQuotes && c == ')') parenDepth--;

            // Split at semicolons only when not in quotes or parentheses
            if (c == ';' && !inQuotes && parenDepth == 0) {
                std::string instruction = instructionStr.substr(start, i - start);
                start = i + 1;

                // Trim whitespace
                instruction.erase(0, instruction.find_first_not_of(" \t\n\r\f\v"));
                instruction.erase(instruction.find_last_not_of(" \t\n\r\f\v") + 1);

                if (!instruction.empty()) {
                    std::vector<std::string> parts;

                    // Special handling for PRINT commands
                    if (instruction.find("PRINT(") == 0 && instruction.back() == ')') {
                        parts.push_back("PRINT");
                        // Extract the entire content inside parentheses as a single string
                        std::string content = instruction.substr(6, instruction.size() - 7);
                        parts.push_back(content);  // Push the entire expression as one argument
                    }
                    else {
                        // Normal command parsing
                        std::istringstream iss(instruction);
                        std::string part;
                        while (iss >> std::quoted(part)) {
                            parts.push_back(part);
                        }
                    }

                    if (!parts.empty()) {
                        parsedInstructions.push_back(parts);
                    }
                }
            }
        }

        // Validate instruction count
        if (parsedInstructions.empty() || parsedInstructions.size() > 50) {
            std::cerr << "[!] Invalid command: Requires 1-50 instructions (got "
                << parsedInstructions.size() << ")\n";
            return;
        }

        // Validate memory size
        int memorySize;
        try {
            memorySize = std::stoi(memSizeStr);
        }
        catch (...) {
            std::cerr << "[!] Invalid memory size: must be integer\n";
            return;
        }

        // Check memory range and power of 2
        if (memorySize < 64 || memorySize > 65536 || (memorySize & (memorySize - 1)) != 0) {
            std::cerr << "[!] Memory must be 64-65536 bytes and a power of 2\n";
            return;
        }

        // Create process
        try {
            processManager->createProcess(processName, parsedInstructions.size(), memorySize, memPerFrame);
            auto proc = processManager->getSharedProcess(processName);
            proc->setParsedInstructions(parsedInstructions);  // Store for execution

            if (scheduler) {
                scheduler->addProcess(proc);
                std::cout << "[+] Created process '" << processName
                    << "' with " << parsedInstructions.size()
                    << " instructions\n";
            }
            else {
                throw std::runtime_error("Scheduler not initialized");
            }
        }
        catch (const std::exception& e) {
            std::cerr << "[!] Failed to create process: " << e.what() << "\n";
        }
    }

    // <SCREEN size>2 argument
    else if (args.size() > 2)
    {
        std::cout << "[*] Too many arguments given. -s to create a new process, -r to redraw the screen and create a new process, and -ls to list the running processes." << std::endl;
        return;
    }
    // Log fillers
    else if (args[0] == "-s" || args[0] == "-c")
    {
        std::cout << "[*] Creating process..." << std::endl;
        return;
    }
    else if (args[0] == "-r")
    {
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
    std::vector<Process*> processes = processManager->listProcesses();

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
            std::string coreDisplay;
            if (proc->getCoreId() == -1) {
                coreDisplay = "Pending";
            }
            else {
                coreDisplay = std::to_string(proc->getCoreId());
                // Pad with spaces to match "Pending" (7 chars)
                while (coreDisplay.length() < 7)
                    coreDisplay += ' ';
            }
            out << proc->getName() << " \t(" << proc->getCreationTimestamp()
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

void Shell::marquee()
{
    Marquee marquee;
    
    marquee.initialize();

    Shell::clear();
}

int Shell::randomNumber()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(minIns, maxIns);
    return dist(gen);
}

void Shell::schedulerStart()
{
    ICommand::PER_INSTRUCTION_DELAY_MS = delayPerExec;

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
            int totalLines = randomNumber(); //random per process

            processManager->createProcess(name, totalLines, minMemPerProc, memPerFrame); // NOTE: hardcoded to use the minimum memory per process.
            std::shared_ptr<Process> proc = processManager->getSharedProcess(name);

            if (scheduler) {
                scheduler->addProcess(proc);
                // Removed notification to prevent CLI flooding
            }

            // CORRECTED: Generate batchFreq processes per CPU cycle
            int sleep_time = delayPerCycleMs * batchFreq;
            if (sleep_time <= 0) sleep_time = 1; // Ensure minimum sleep time
           

            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
    });

    std::cout << "[i] Batch processing started (frequency: "
        << batchFreq << " CPU cycles per process)" << std::endl;
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
    std::cout << "[i] Report generated." << std::endl;
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

void Shell::processSMI()
{
    std::cout << memoryManager->displayMemory() << std::endl;
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
        if (input_tokens[0] == "help" || input_tokens[0] == "exit" || input_tokens[0] == "clear") {
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
    else if (input_tokens[0] == "marquee") {
        marquee();
    }
    else if (input_tokens[0] == "clear") {
        clear();
    }
    else if (input_tokens[0] == "exit") {
        setQuit();
    }
    else if (input_tokens[0] == "process-smi") {
        processSMI();
    }
    else if (input_tokens[0] == "help") {
        std::cout << "[*] Available commands:\n\n"
            << "  initialize\n"
            << "  screen\n"
            << "    - screen -s <args>\n"
            << "    - screen -c <args>\n" 
            << "    - screen -r <args>\n"
            << "    - screen -ls\n"
            << "  scheduler-start\n"
            << "  scheduler-stop\n"
            << "  report-util\n"
            << "  clear\n"
            << "  exit\n" << std::endl;
    }
    else if (!input_tokens[0].empty()) { // error on unknown, non-empty command.
        std::cout << "[*] Unknown command. Type 'help' to get a list of commands." << std::endl;
    }
}