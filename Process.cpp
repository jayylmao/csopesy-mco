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
	const int NUM_TYPES = 6; // Adjust as needed: PRINT, DECLARE, ADD, etc. // 0-5
	int randVal;
	int remainingInstructions = totalInstructions;

	while (remainingInstructions > 0) {
		auto cmd = createCommand(remainingInstructions, 0);
		
		if (cmd) {
			instructionQueue.push(std::move(cmd));
		}
		else {
			break;
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
		}

		if (instructionQueue.empty()) {
			finished = true;
		}
	}
}

std::unique_ptr<ICommand> Process::createCommand(int& remaining, int depth)
{
	const int NUM_TYPES = 6;
	if (remaining <= 0) return nullptr;

	int type;

	if (depth >= 3) {
		type = rand() % (NUM_TYPES - 1);
	}
	else {
		type = rand() % NUM_TYPES;
	}

	switch (type) {
	case ICommand::PRINT: {
		--remaining;
		std::string msg = "Hello world from " + this->name + "!";
		return std::make_unique<PrintCommand>(msg);
	}
	case ICommand::DECLARE:{
		--remaining;
		auto it = symbolTable.begin();
		std::advance(it, rand() % symbolTable.size());

		std::string varName = it->first;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint16_t> dist(0, std::numeric_limits<uint16_t>::max());

		uint16_t randomValue = dist(gen);

		return std::make_unique<DeclareCommand>(varName, randomValue);
	}
	case ICommand::ADD:
	case ICommand::SUBTRACT: {
		--remaining;
		std::vector<std::string> vars;
		for (const auto& kv : symbolTable) vars.push_back(kv.first);
		auto randomVar = [&]() { return vars[rand() % vars.size()]; };

		if (type == ICommand::ADD)
			return std::make_unique<AddCommand>(randomVar(), randomVar(), randomVar());
		else
			return std::make_unique<SubtractCommand>(randomVar(), randomVar(), randomVar());
	}
	case ICommand::SLEEP:{
		--remaining;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<unsigned int> dist(1, std::numeric_limits<uint8_t>::max());
		
		uint8_t sleepTicks = static_cast<uint8_t>(dist(gen));
		return std::make_unique<SleepCommand>(sleepTicks);
	}
	case ICommand::FOR: {
		if (remaining < 2) {
			return createCommand(remaining, depth);
		}
		
		--remaining;

		int repeatCount = rand() % 3 + 1;
		std::vector<std::unique_ptr<ICommand>> body;

		// Try to add as many subcommands as remaining allows
		int maxBody = std::min(remaining, 3); // conservative
		int numBodyCmds = std::min(remaining, rand() % 3 + 1);

		for (int i = 0; i < numBodyCmds; ++i) {
			auto subCmd = createCommand(remaining, depth + 1);
			if (!subCmd) break;
			body.push_back(std::move(subCmd));
		}

		return std::make_unique<ForCommand>(std::move(body), repeatCount);
	}
	default:
		throw std::invalid_argument("Unknown command type");
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