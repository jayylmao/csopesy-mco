#include "PrintCommand.h"
#include "DeclareCommand.h"
#include "AddCommand.h"
#include "ReadCommand.h"
#include "WriteCommand.h"
#include "SubtractCommand.h"
#include "SleepCommand.h"
#include "ForCommand.h"
#include "ReadCommand.h"
#include "WriteCommand.h"
#include "Process.h"

#include <iostream>
#include <ctime>
#include <random>
#include <iterator>
#include <stdexcept>
#include <sstream>
#include <chrono>

Process::Process(const std::string& name, int pid, int instructionCount, int mem, size_t pageSize, std::shared_ptr<DemandPagingAllocator> memoryManager)
	: name(name), pid(pid), totalInstructions(instructionCount), coreId(-1), finished(false), memory(mem), memoryManager(memoryManager)
{
	// store creation timestamp.
	auto now = std::chrono::system_clock::now();
	std::time_t raw_time = std::chrono::system_clock::to_time_t(now);
	std::tm* time_info = std::localtime(&raw_time);

	std::ostringstream oss;
	oss << std::put_time(time_info, "%m/%d/%Y, %I:%M:%S %p");
	creationTimestamp = oss.str();

	symbolTable["var1"] = 0;
	symbolTable["var2"] = 0;
	symbolTable["var3"] = 0;
	symbolTable["var4"] = 0;
	symbolTable["var5"] = 0;
	symbolTable["var6"] = 0;
	symbolTable["var7"] = 0;
	symbolTable["var8"] = 0;
	symbolTable["var9"] = 0;
	symbolTable["var10"] = 0;

	createInstructions();
}

/**
 * @brief Alternate constructor for Process where commands are specified. Used for screen -c.
 * @param name Name of process.
 * @param pid Unique process ID.
 * @param instructions Pointer to vector of unique pointers of commands.
 */
Process::Process(const std::string& name, int pid, std::vector<std::unique_ptr<ICommand>>&& instructions, size_t pageSize, std::shared_ptr<DemandPagingAllocator> memoryManager)
	: name(name), pid(pid), totalInstructions(instructions.size()), coreId(-1), finished(false), memory(64), memoryManager(memoryManager)
{
	// store creation timestamp.
	auto now = std::chrono::system_clock::now();
	std::time_t raw_time = std::chrono::system_clock::to_time_t(now);
	std::tm* time_info = std::localtime(&raw_time);

	std::ostringstream oss;
	oss << std::put_time(time_info, "%m/%d/%Y, %I:%M:%S %p");
	creationTimestamp = oss.str();

	symbolTable["var1"] = 0;
	symbolTable["var2"] = 0;
	symbolTable["var3"] = 0;
	symbolTable["var4"] = 0;
	symbolTable["var5"] = 0;
	symbolTable["var6"] = 0;
	symbolTable["var7"] = 0;
	symbolTable["var8"] = 0;
	symbolTable["var9"] = 0;
	symbolTable["var10"] = 0;

	for (auto& instruction : instructions) {
		instructionQueue.push(std::move(instruction));
	}
}

void Process::createInstructions()
{
	int remaining = totalInstructions;
	while (remaining > 0) {
		createFlatCommand(remaining, 0);
	}
}

void Process::createFlatCommand(int& remaining, int depth)
{
	const int NUM_TYPES = 5; // Only basic commands (no FOR as an instruction)
	if (remaining <= 0) return;

	// At depth >= 3, just make basic commands
	int type = rand() % (depth >= 3 ? NUM_TYPES : NUM_TYPES + 1); // NUM_TYPES + 1 allows chance of FOR at low depth
	
	// 0: PRINT, 1: DECLARE, 2: ADD, 3: SUBTRACT, 4: SLEEP, 5: FOR (if allowed)
	if (type < NUM_TYPES) {
		switch (type) {
			case ICommand::PRINT: {
				--remaining;
				std::string msg = "Hello world from " + this->name + "!";
				instructionQueue.push(std::make_unique<PrintCommand>(msg, logs));
				break;
			}
			case ICommand::DECLARE: {
				--remaining;
				auto it = symbolTable.begin();
				std::advance(it, rand() % symbolTable.size());
				std::string varName = it->first;
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<uint16_t> dist(0, std::numeric_limits<uint16_t>::max());
				uint16_t randomValue = dist(gen);
				instructionQueue.push(std::make_unique<DeclareCommand>(varName, randomValue));
				break;
			}
			case ICommand::ADD: {
				--remaining;
				std::vector<std::string> vars;
				for (const auto& kv : symbolTable) vars.push_back(kv.first);
				auto randomVar = [&]() { return vars[rand() % vars.size()]; };
				instructionQueue.push(std::make_unique<AddCommand>(randomVar(), randomVar(), randomVar()));
				break;
			}
			case ICommand::SUBTRACT: {
				--remaining;
				std::vector<std::string> vars;
				for (const auto& kv : symbolTable) vars.push_back(kv.first);
				auto randomVar = [&]() { return vars[rand() % vars.size()]; };
				instructionQueue.push(std::make_unique<SubtractCommand>(randomVar(), randomVar(), randomVar()));
				break;
			}
			case ICommand::SLEEP: {
				--remaining;
				std::random_device rd;
				std::mt19937 gen(rd());
				std::uniform_int_distribution<unsigned int> dist(1, std::numeric_limits<uint8_t>::max());
				uint8_t sleepTicks = static_cast<uint8_t>(dist(gen));
				instructionQueue.push(std::make_unique<SleepCommand>(sleepTicks));
				break;
			}
		}
	}
	else {
		// Flatten FOR: repeat a block of commands
		if (remaining < 2) return;
		int repeatCount = rand() % 3 + 1; // 1-3
		int bodyLen = std::min(remaining / repeatCount, 2); // limit the body size
		if (bodyLen == 0) bodyLen = 1;
		for (int r = 0; r < repeatCount && remaining > 0; ++r) {
			for (int i = 0; i < bodyLen && remaining > 0; ++i) {
				createFlatCommand(remaining, depth + 1);
			}
		}
	}
}

void Process::accessCurrentPage() {
	size_t numPages = (memory + memoryManager->getPageSize() - 1) / memoryManager->getPageSize();
	size_t page = executedInstructions % numPages;
	memoryManager->accessPage(pid, page);
}

void Process::executeInstruction()
{
	//std::lock_guard<std::mutex> lock(mtx);
	if (!instructionQueue.empty()) {
		accessCurrentPage();
		auto cmd = std::move(instructionQueue.front());
		instructionQueue.pop();

		if (cmd) {
			cmd->execute(*this);
		}

		if (instructionQueue.empty()) {
			finished = true;
		}
	}
}

bool Process::hasFinished() const
{
	std::lock_guard<std::mutex> lock(mtx);
	return finished;
}

void Process::setFinished(bool value)
{
	std::lock_guard<std::mutex> lock(mtx);
	finished = value;
}

int Process::getCurrentLine()
{
	std::lock_guard<std::mutex> lock(mtx);
	return this->totalInstructions - this->instructionQueue.size();
}

int Process::getPID()
{
	return this->pid;
}

std::string Process::getName()
{
	return this->name;
}

std::string Process::getCreationTimestamp() {
	
	return this->creationTimestamp;
}

void Process::setCoreId(int id)
{
	std::lock_guard<std::mutex> lock(mtx);
	coreId = id;
}

int Process::getCoreId() const
{
	std::lock_guard<std::mutex> lock(mtx);
	return coreId;
}

int Process::getTotalLines() const
{
	return totalInstructions;
}

uint16_t Process::getVar(std::string name)
{
	auto var = symbolTable.find(name);

	if (var == symbolTable.end()) {
		return 0;
	}
	else {
		return var->second;
	}
}

void Process::setVar(std::string name, uint16_t val)
{
	symbolTable[name] = val;
}

int Process::getMemory() const
{
	return this->memory;
}

void Process::setParsedInstructions(const std::vector<std::vector<std::string>>& instructions) {
	
	// Convert parsed instructions into ICommand objects
	for (const auto& instrParts : instructions) {
		if (instrParts.empty()) continue;

		const std::string& command = instrParts[0];

		// --- Command Factory Logic ---
		if (command == "DECLARE" && instrParts.size() == 3) {
			std::string varName = instrParts[1];
			uint16_t value;
			try {
				value = static_cast<uint16_t>(std::stoi(instrParts[2]));
			}
			catch (...) {
				throw std::runtime_error("Invalid DECLARE value: " + instrParts[2]);
			}

			// Save to symbol table immediately (like in createFlatCommand)
			symbolTable[varName] = value;
			instructionQueue.push(std::make_unique<DeclareCommand>(varName, value ));
		}

		else if (command == "PRINT") {
			if (instrParts.size() >= 2) {
				std::vector<std::string> elements;
				std::string content = instrParts[1];

				// Improved parsing logic
				size_t start = 0;
				bool inQuotes = false;
				int bracketDepth = 0;

				for (size_t i = 0; i <= content.size(); ++i) {
					char c = (i < content.size()) ? content[i] : '\0';

					// Handle quotes (considering escaped quotes)
					if (c == '"' && (i == 0 || content[i - 1] != '\\')) {
						inQuotes = !inQuotes;
					}
					// Handle parentheses (if needed for future extensions)
					else if (!inQuotes) {
						if (c == '(') bracketDepth++;
						else if (c == ')') bracketDepth--;
					}

					// Split at + operator when not in quotes and outside brackets
					if (!inQuotes && bracketDepth == 0) {
						if (c == '+' || i == content.size()) {
							// Extract element between start and current position
							if (i > start) {
								std::string element = content.substr(start, i - start);

								// Trim whitespace only outside quotes
								size_t first = element.find_first_not_of(" \t");
								size_t last = element.find_last_not_of(" \t");

								if (first != std::string::npos) {
									element = element.substr(first, (last - first + 1));

									// Validate variables
									if (!element.empty()) {
										if (element.front() != '"' &&
											symbolTable.find(element) == symbolTable.end()) {
											throw std::runtime_error("Undefined variable: " + element);
										}
										elements.push_back(element);
									}
								}
							}
							start = i + 1;  // Move start to next character
						}
					}
				}

				if (!elements.empty()) {
					instructionQueue.push(std::make_unique<PrintCommand>(elements, logs));
				}
			}
		}
		else if (command == "ADD" && instrParts.size() == 4) {
			instructionQueue.push(std::make_unique<AddCommand>(instrParts[1], instrParts[2], instrParts[3]));
		}
		else if (command == "WRITE" && instrParts.size() == 3) {
			instructionQueue.push(std::make_unique<WriteCommand>(instrParts[1], instrParts[2]));
		}
		else if (command == "READ" && instrParts.size() == 3) {
			instructionQueue.push(std::make_unique<ReadCommand>(instrParts[1], instrParts[2]));
		}
		else {
			throw std::runtime_error("Invalid command or arguments: " + command);
		}
	}

	totalInstructions = instructionQueue.size();
}

void Process::setMemoryAccessViolation(const std::string& time, const std::string& address) {
	memoryAccessViolation = true;
	violationTime = time;
	violationAddress = address;
}

bool Process::hasMemoryAccessViolation() const {
	return memoryAccessViolation;
}

std::string Process::getViolationTime() const {
	return violationTime;
}

std::string Process::getViolationAddress() const {
	return violationAddress;
}