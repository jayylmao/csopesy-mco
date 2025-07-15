#include "PrintCommand.h"
#include "DeclareCommand.h"
#include "AddCommand.h"
#include "SubtractCommand.h"
#include "SleepCommand.h"
#include "ForCommand.h"
#include "Process.h"

#include <ctime>
#include <random>
#include <iterator>
#include <stdexcept>
#include <sstream>
#include <chrono>

Process::Process(const std::string& name, int pid, int instructionCount, int memory)
	: name(name), pid(pid), totalInstructions(instructionCount), coreId(-1), finished(false), memory(memory)
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
			instructionQueue.push(std::make_unique<PrintCommand>(msg));
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

void Process::executeInstruction()
{
	//std::lock_guard<std::mutex> lock(mtx);
	if (!instructionQueue.empty()) {
		auto cmd = std::move(instructionQueue.front());
		instructionQueue.pop();

		if (cmd) {
			cmd->execute(*this);
			//incrementExecutedInstructions();
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